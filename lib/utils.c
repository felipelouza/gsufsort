#include "utils.h"

/**********************************************************************/

void time_start(time_t *t_time, clock_t *c_clock){

  *t_time = time(NULL);
  *c_clock =  clock();
}

double time_stop(time_t t_time, clock_t c_clock){

  double aux1 = (clock() - c_clock) / (double)(CLOCKS_PER_SEC);
  double aux2 = difftime (time(NULL),t_time);

  printf("CLOCK = %lf TIME = %lf\n", aux1, aux2);

  return aux1;
}


/**********************************************************************/
void die(const char* where) {

  printf("Error at: %s: %s.\n",where,errno ? strerror(errno) : "errno not set");
  exit(errno?errno:1);
}

void dies(const char* where, char* format, ...) {

  int_t err = errno;
  va_list val;

  printf("Error at %s: %s.\n",where,errno ? strerror(errno) : "errno not set");
  va_start(val,format);
  vprintf(format,val);
  va_end(val);
  printf("\n");

  exit(err ? err : 1);
}
/**********************************************************************/
int_t print_char(char* A, int_t n){

  int_t i;
  for(i=0; i<n; i++)	
    printf("%" PRIdN ") %c\n", i, A[i]);

  printf("\n");

  return 0;
}
/**********************************************************************/
int_t print_int(int_t* A, size_t n){

  size_t i;
  for(i=0; i<n; i++)	
    printf("%zu) %" PRIdN "\n", i, A[i]);

  printf("\n");

  return 0;
}

/******************************************************************************/

unsigned char* cat(unsigned char* s1, unsigned char* s2, size_t *n){

  (*n) = (strlen((char*)s1)+1)+(strlen((char*)s2)+1)+1; //add 0 at the end

  size_t j, l=0;
  unsigned char *str = (unsigned char*) malloc((*n)*sizeof(unsigned char));

  /**/
  {
    int_t m = strlen((char*)s1);
    for(j=0; j<m; j++) if(s1[j]<255) str[l++] = s1[j]+1;
    str[l++] = 1; //add 1 as separator
  }

  {
    int_t m = strlen((char*)s2);
    for(j=0; j<m; j++) if(s2[j]<255) str[l++] = s2[j]+1;
    str[l++] = 1; //add 1 as separator
  }
  /**/

  str[l++]=0;
  if(*n>l){
    str = (unsigned char*) realloc(str, (l)*sizeof(unsigned char)); 
  }
  *n = l;

  return str;
}

/******************************************************************************/

unsigned char* cat_all(unsigned char** R, int k, size_t *n, int verbose, int upper){

  (*n)++; //add 0 at the end
  int_t i, j;
  size_t l=0;
  unsigned char *str = (unsigned char*) malloc((*n)*sizeof(unsigned char));

  int_t max=0;

  if(upper){

    int diff=0;
    char lb, rb;
    if(upper==1){//lowercase
      diff = 32;
      lb='A'; rb='Z';
    }
    if(upper==2){//uppercase
      diff = -32;
      lb='a'; rb='z';
    }

    for(i=0; i<k; i++){
      int_t m = strlen((char*)R[i]);
      if(m>max) max=m;
      for(j=0; j<m; j++){
        if(R[i][j]<255){
          str[l++] = R[i][j]+1;
          if(R[i][j]>=lb && R[i][j]<=rb) str[l-1]+=diff;
        }
      }
      if(m) str[l++] = 1; //add 1 as separator (ignores empty entries)
    }
  }
  else{
    for(i=0; i<k; i++){
      int_t m = strlen((char*)R[i]);
      if(m>max) max=m;
      for(j=0; j<m; j++){
        if(R[i][j]<255) str[l++] = R[i][j]+1;
      }
      if(m) str[l++] = 1; //add 1 as separator (ignores empty entries)
    }
  }

  str[l++]=0;
  if(*n>l){
    str = (unsigned char*) realloc(str, (l)*sizeof(unsigned char)); 
  }
  *n = l;

  if(verbose) printf("longest string = %" PRIdN "\n", max);

  return str;
}


/**********************************************************************/

int print_array(unsigned char *T, int_da *DA, rankbv_t* rbv, int light, int_t *SA, int_t *ISA, int_t *LCP, int str, int da, int sa, int isa, int lcp, int bwt, int gsa, int gesa, size_t n, size_t m, int last_end){

  size_t i,j;

  // output
  printf("i\t");
  if(sa)	printf("SA\t");
  if(isa)	printf("ISA\t");
  if(da)	printf("DA\t");
  if(gesa || lcp) printf("LCP\t");
  if(gsa)	printf("GSA\t\t");
  if(bwt) printf("BWT\t");
  if(str)	printf("suffixes");
  printf("\n");

  char terminal='$';

  if(last_end){
    terminal='#';
  }

  for(i=0; i < m; ++i) {

    printf("%zu\t",i);
    if(sa) 	printf("%" PRIdN "\t",SA[i]);
    if(isa) 	printf("%" PRIdN "\t",ISA[i]);
    int_da da_value = 0;
    if(da){
      da_value = (light)?rankbv_rank1(rbv,SA[i]):DA[i];
      printf("%" PRIdA "\t",da_value);
    }
    if(gesa || lcp)	printf("%" PRIdN "\t",LCP[i]);
    if(gsa){
      da_value = (light)?rankbv_rank1(rbv,SA[i]):DA[i];
      printf("(%" PRIdA ", ", da_value);

      int_t pos;
      if(last_end) pos=SA[da_value];
      else pos=SA[da_value-1];

      int_t value = (da_value==0)?SA[i]:SA[i]-pos-1;
      printf("%" PRIdN ")   \t", value);
    }
    if(bwt){
      char c = (SA[i])? T[SA[i]-1]-1:terminal;
      if(c==0) c = '$';
      printf("%c\t",c);
    }
    if(str){
      j=SA[i];
      while(j<n){
        if(T[j]==1){ printf("$"); break;}
        else if(T[j]==0) printf("#"); 
        else printf("%c", T[j]-1);
        j++;
      }
    }
    printf("\n");
  }

  return 0;
}


/**********************************************************************/
