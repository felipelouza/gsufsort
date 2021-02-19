#include "rankbv.h"

#include "string.h"


inline void rankbv_setbit(rankbv_t* rbv,size_t i)
{
    size_t bs = i/rbv->s;
    size_t block = bs + i/RBVW + 1;
    rbv->S[block] |= (1LL<<(i%RBVW));
}


inline size_t
rankbv_length(rankbv_t* rbv)
{
    return rbv->n;
}
inline uint32_t rankbv_bits(size_t n)
{
    uint32_t b = 0;
    while (n) {
        b++;
        n >>= 1;
    }
    return b;
}

inline uint32_t
rankbv_popcount8(const uint32_t x)
{
    return __builtin_popcount(x&0xff);
}

inline size_t
rankbv_numsblocks(rankbv_t* rbv)
{
    return rbv->n/rbv->s+1;
}

inline uint64_t*
rankbv_getdata(rankbv_t* rbv)
{
    size_t num_sblocks = rankbv_numsblocks(rbv);
    return (uint64_t*)(((char*)rbv) + sizeof(rankbv_t) +
                       (sizeof(uint64_t)*num_sblocks));
}

inline int
rankbv_getbit(rankbv_t* rbv,size_t i)
{
    size_t bs = i/rbv->s;
    size_t block = bs + i/RBVW + 1;
    return ((rbv->S[block] >> (i%RBVW)) & 1LL);
}

rankbv_t*
rankbv_create(size_t n,uint32_t f)
{
    if (!f) f = rankbv_bits(n); /* lg(n) */
    size_t s = RBVW*f;
    size_t num_sblocks = n/s+1;
    size_t ints = n/RBVW+1;

    rankbv_t* rbv = (rankbv_t*) rankbv_safecalloc(sizeof(rankbv_t)
                    + (num_sblocks*sizeof(uint64_t)) /*S[]*/
                    + (ints*sizeof(uint64_t)));      /*A[]*/

    rbv->n = n;
    rbv->factor = f;
    rbv->s = s;

    return rbv;
}

rankbv_t*
rankbv_create_A(uint64_t* A,size_t n,uint32_t f)
{
    rankbv_t* rbv = rankbv_create(n,f);
    size_t ints = n/RBVW+1, i;
    for (i=0; i<ints; i++) {
        size_t block = i/rbv->factor;
        rbv->S[(block+1)+(block*rbv->factor)+(i%rbv->factor)] = A[i];
        /*rbv->S[i+(i/rbv->factor)+1] = A[i];*/
    }
    rankbv_build(rbv);
    return rbv;
}

void
rankbv_free(rankbv_t* rbv)
{
    if (rbv) {
        free(rbv);
    }
}

void
rankbv_build(rankbv_t* rbv)
{
    size_t i,j,start,stop;
    uint64_t tmp;
    size_t num_sblocks = rankbv_numsblocks(rbv);
    rbv->S[0] = 0;
    for (i=1; i<num_sblocks; i++) {
        rbv->S[i*rbv->factor+i] = rbv->S[(i-1)*rbv->factor+(i-1)];
        tmp = 0;
        start = (i-1)*rbv->factor+i; stop = start+rbv->factor;
        for (j=start; j<stop; j++) tmp += __builtin_popcountll(rbv->S[j]);
        rbv->S[i*rbv->factor+i] += tmp;
    }
    rbv->ones = rankbv_rank1(rbv,rbv->n-1);
}

int
rankbv_access(rankbv_t* rbv,size_t i)
{
    return rankbv_getbit(rbv,i);
}

size_t
rankbv_rank1(rankbv_t* rbv,size_t i)
{
    size_t j;
    i++;
    uint64_t bs = i/rbv->s;
    uint64_t SBlock = bs*rbv->factor+bs;
    uint64_t resp = rbv->S[SBlock];
    size_t start = SBlock+1;
    size_t stop = start+(i%rbv->s)/RBVW;
    uint64_t* S = (uint64_t*) rbv->S;
    for (j=start; j<stop; j++) resp+=__builtin_popcountll(S[j]);
    resp += __builtin_popcountll(S[stop]&((1LL<<(i &rankbv_mask63))-1));
    return resp;
}


size_t
rankbv_ones(rankbv_t* rbv)
{
    return rbv->ones;
}

void
rankbv_print(rankbv_t* rbv)
{
    size_t i;
    for (i = 0; i < rbv->n; ++i) {
        if (rankbv_getbit(rbv,i)) printf("1");
        else printf("0");
    }
    printf("\n");
}

size_t
rankbv_select0(rankbv_t* rbv,size_t x)
{
    if (x>rbv->n-rbv->ones) return (size_t)(-1);

    /* binary search over first level rank structure */
    if (x==0) return 0;

    size_t nsb = rankbv_numsblocks(rbv);
    size_t l=0, r=nsb-1;
    size_t mid=(l+r)/2;
    size_t sblock = mid*rbv->factor+mid;
    size_t rankmid = (mid*rbv->s) - rbv->S[sblock];
    /* binary search over first level rank structure */
    while (l<=r) {
        if (rankmid<x)
            l = mid+1;
        else
            r = mid-1;
        mid = (l+r)/2;
        sblock = mid*rbv->factor+mid;
        rankmid = (mid*rbv->s) - rbv->S[sblock];
    }

    /* sequential search using popcount over a int */
    x-=rankmid;
    sblock++;
    size_t zeros = RBVW - __builtin_popcountll(rbv->S[sblock]);
    size_t ints = nsb + rbv->n/RBVW+1;
    size_t skip = 0;
    while (zeros < x) {
        x-=zeros; sblock++;
        if (sblock > ints) return rbv->n;
        zeros = RBVW- __builtin_popcountll(rbv->S[sblock]);
        skip++;
    }

    //sequential search using popcount over a char
    /* binsearch over integer */
    uint64_t j = rbv->S[sblock];
    sblock=  mid*rbv->s + (skip*RBVW);
    rankmid = 32 - __builtin_popcount(j&0xFFFFFFFF);
    if (rankmid < x) {
        j=j>>32;
        x-=rankmid;
        sblock+=32;
        rankmid = 16 - __builtin_popcount(j&0x0000FFFF);
        if (rankmid < x) {
            j=j>>16;
            x-=rankmid;
            sblock+=16;
            rankmid = 8 - __builtin_popcount(j&0x000000FF);
            if (rankmid < x) {
                j=j>>8;
                x-=rankmid;
                sblock+=8;
            }
        }
    }

    // then sequential search bit a bit
    while (x>0) {
        if (j%2 == 0) x--;
        j=j>>1;
        sblock++;
    }
    sblock--;
    if (sblock > rbv->n)  return rbv->n;
    else return sblock;
}


size_t
rankbv_select1(rankbv_t* rbv,size_t x)
{
    if (x == 0) return (size_t)(-1);
    if (x > rbv->ones)  return (size_t)(-1);

    size_t nsb = rankbv_numsblocks(rbv);
    size_t l=0, r=nsb-1;
    size_t mid=(l+r)/2;
    size_t sblock = mid*rbv->factor+mid;
    size_t rankmid = rbv->S[sblock];
    /* binary search over first level rank structure */
    while (l<=r) {
        if (rankmid<x)
            l = mid+1;
        else
            r = mid-1;
        mid = (l+r)/2;
        sblock = mid*rbv->factor+mid;
        rankmid = rbv->S[sblock];
    }
    /* binary search over blocks */
    x-=rankmid;
    sblock++;
    size_t ones = __builtin_popcountll(rbv->S[sblock]);
    size_t ints = nsb + rbv->n/RBVW+1;
    size_t skip = 0;
    while (ones < x) {
        x-=ones; sblock++;
        if (sblock > ints) return rbv->n;
        ones = __builtin_popcountll(rbv->S[sblock]);
        skip++;
    }
    /* binsearch over integer */
    uint64_t j = rbv->S[sblock];
    sblock=  mid*rbv->s + (skip*RBVW);
    rankmid = __builtin_popcount(j&0xFFFFFFFF);
    if (rankmid < x) {
        j=j>>32;
        x-=rankmid;
        sblock+=32;
        rankmid = __builtin_popcount(j&0x0000FFFF);
        if (rankmid < x) {
            j=j>>16;
            x-=rankmid;
            sblock+=16;
            rankmid = __builtin_popcount(j&0x000000FF);
            if (rankmid < x) {
                j=j>>8;
                x-=rankmid;
                sblock+=8;
            }
        }
    }
    /* sequential search on bits */
    while (x>0) {
        if (j&1) x--;
        j=j>>1;
        sblock++;
    }
    return sblock-1;
}

size_t
rankbv_spaceusage(rankbv_t* rbv)
{
    size_t bytes;
    size_t num_sblocks = rankbv_numsblocks(rbv);
    bytes = sizeof(rankbv_t);
    bytes += sizeof(uint64_t)*(num_sblocks); /* S[] */
    bytes += sizeof(uint64_t)*(rbv->n/RBVW+1); /* A[] */
    return bytes;
}

rankbv_t*
rankbv_load(FILE* f)
{
    size_t bytes;
    /* read space */
    if(fread(&bytes,sizeof(size_t),1,f)==EOF)
    	perror("rankbv_load");

    /* read the bv */
    void* mem = rankbv_safecalloc(bytes);
    if(fread(mem,bytes,1,f)==EOF)
    	perror("rankbv_load");

    return (rankbv_t*)mem;
}

size_t
rankbv_save(rankbv_t* rbv,FILE* f)
{
    size_t ints = rbv->n/RBVW+1;
    size_t num_sblocks = rankbv_numsblocks(rbv);

    size_t bytes = sizeof(rankbv_t) + (sizeof(uint64_t)*num_sblocks) + (sizeof(uint64_t)*ints);

    fwrite(&bytes,sizeof(uint64_t),1,f);
    fwrite(rbv,sizeof(rankbv_t),1,f);
    fwrite(rbv->S,sizeof(uint64_t),num_sblocks,f);
    uint64_t* B = rankbv_getdata(rbv);
    fwrite(B,sizeof(uint64_t),ints,f);

    return bytes+sizeof(size_t);
}

void*
rankbv_safecalloc(size_t n)
{
    void* mem = calloc(n,1);
    if (!mem) {
        fprintf(stderr,"ERROR: rankbv_safecalloc()");
        exit(EXIT_FAILURE);
    }
    return mem;
}
