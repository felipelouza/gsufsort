// vim: noai:ts=2:sw=2

/*
 * gsufsort 
 *
 * Authors: Felipe A. Louza
 * contact: louza@usp.br
 * 18/09/2018
 *
 */

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <getopt.h>

#include "lib/utils.h"
#include "lib/file.h"
#include "external/gsacak.h"
#include "lib/rankbv.h"

#define SIGMA 256

#ifndef DEBUG
  #define DEBUG 0 
#endif

#ifndef LAST_END
  #define LAST_END 1
#endif

#define WORD (size_t)(pow(256,sizeof(int_t))/2.0)

int store_to_disk(unsigned char *T, int_da *DA, rankbv_t* rbv, int_t *SA,  int_t *LCP, size_t n, char* c_file, char *ext, int wsize1, int wsize2, int wsize3); 
size_t load_from_disk(unsigned char **T, int_da **DA, int_t **SA, int_t **LCP, char* c_file, char *ext, int wsize1, int wsize2, int wsize3);

/******************************************************************************/

void usage(char *name){
  printf("\n\tUsage: %s FILENAME [options]\n\n",name);
  puts("Extensions supported: txt (raw files) fasta and fastq");
  puts("Available options:");

  puts("\t--build               (default)");
  puts("\t--load                load from disk FILENAME[.sa][.da][.lcp][.gsa][.str]");
  puts("\t--sa    [w]           compute SA (default) using w (def 4) bytes (FILENAME.w.sa)");
  puts("\t--isa   [w]           compute ISA (FILENAME.w.isa)");
  puts("\t--lcp   [w]           compute LCP (FILENAME.w.lcp)");
  puts("\t--da    [w]           compute DA  (FILENAME.w.da)");
  puts("\t--gsa   [w1][w2]      compute GSA=(text, suff) using pairs of (w1, w2) bytes (FILENAME.w1.w2.gsa)");
  puts("\t--light               run lightweight algorithm to compute DA (also GSA)");
  puts("\t--gesa  [w1][w2][w3]  compute GESA=(GSA.text, GSA.suff, LCP, BWT) (FILENAME.w1.w2.w3.1.gesa)");
  puts("\t--bwt                 compute BWT using 1 byte per symbol (FILENAME.bwt)");
  puts("\t--ibwt                invert the BWT, given FILENAME[.bwt]");
  puts("\t--qs                  output (only for fastq) QS permuted according to the BWT (FILENAME.bwt.qs)");
  puts("\t--str                 output concatenated input T^{cat} (FILENAME.1.str)");
  puts("\t--docs    d           number of strings (def all FILENAME)");
  puts("\t--print   p           print arrays (stdout) A[1,p]");
  puts("\t--lcp_max             output maximum LCP");
  puts("\t--lcp_max_text        output maximum LCP (text)");
  puts("\t--lcp_avg             output average LCP");
  puts("\t--trlcp   k           output k-truncated LCP array (FILENAME.w.lcp)");
  puts("\t--lower               convert input symbols to lowercase");
  puts("\t--upper               convert input symbols to uppercase");
  puts("\t--time                output time (seconds)");
  puts("\t--txt                 handle input (FILENAME) as raw file (one string per line)");
  puts("\t--fasta               handle input (FILENAME) as FASTA");
  puts("\t--fastq               handle input (FILENAME) as FASTQ");
  puts("\t--dir                 handle multiple files in directory (DIR) as input");
  puts("\t--output  outfile     rename output file");
  puts("\t--verbose             verbose output");
  puts("\t--help                this help message");
  exit(EXIT_FAILURE);
}

/******************************************************************************/

int build=1; 
int load=0;
int ibwt=0;
int light=0; 

int main(int argc, char** argv){

  time_t t_total=0;clock_t c_total=0;
  time_t t_start=0;clock_t c_start=0;
  int_t i;

  int sa=0, isa=0, lcp=0, da=0, bwt=0, q=0, str=0, gsa=0, gesa=0, lcp_max=0, lcp_max_text=0, lcp_avg=0, trlcp=0, time=0, upper=0; 
  int dir=0;
  int in_type=0;// (txt, 1), (fasta, 2) and (fastq, 3)
  int sa_bytes=sizeof(int_t);
  int isa_bytes=sizeof(int_t);
  int lcp_bytes=sizeof(int_t);
  int da_bytes=sizeof(int_t);
  int t_bytes=sizeof(int_t), s_bytes=sizeof(int_t);

  extern char *optarg;
  extern int optind;
  int c;
  char *c_input=NULL, *c_output=NULL;

  int d=0; //number of string
  int p=0; //print A[1,p]
  int output=0; //output
  int verbose=0, print=0;

/********/
  //int digit_optind = 0;

  while (1) {
    //int this_option_optind = optind ? optind : 1;
    int option_index = 0;

    static struct option long_options[] = {
      {"print",   optional_argument, 0, 'P'},
      {"sa",      optional_argument, 0, 'S'},
      {"isa",     optional_argument, 0, 'I'},
      {"lcp",     optional_argument, 0, 'L'},
      {"da",      optional_argument, 0, 'D'},
      {"gsa",     optional_argument, 0, 'g'},
      {"bwt",     no_argument,       0, 'B'},
      {"qs",      no_argument,       0, 'Q'},
      {"str",     no_argument,       0, 'b'},
      {"dir",     no_argument,       0, 'm'},
      {"docs",    required_argument, 0, 'd'},
      {"verbose", no_argument,       0, 'v'},
      {"time",    no_argument,       0, 'T'},
      {"lower",   no_argument,       0, 'u'},
      {"upper",   no_argument,       0, 'U'},
      {"help",    no_argument,       0, 'h'},
      {"output",  required_argument, 0, 'o'},
      {"txt",     no_argument,       0, 't'},
      {"fasta",   no_argument,       0, 'f'},
      {"fastq",   no_argument,       0, 'q'},
      {"build",   no_argument,       0, '1'},
      {"load",    no_argument,       0, '2'},
      {"ibwt",    no_argument,       0, '3'},
      {"light",   no_argument,       0, 'l'},
      {"gesa",    optional_argument, 0, 'G'},
      {"lcp_max", optional_argument, 0, 'M'},
      {"lcp_max_text", optional_argument, 0, 'W'},
      {"lcp_avg", optional_argument, 0, 'A'},
      {"trlcp",   required_argument, 0, 'c'},
      {0,         0,                 0,  0 }
    };

    c = getopt_long(argc, argv, "S:vtP:d:L:D:g:lG:B:bhtfqo:ic:Qum", long_options, &option_index);

     if (c == -1) break;

    switch (c) {
       case 0:
        printf("option %s", long_options[option_index].name);
        if (optarg) printf(" with arg %s", optarg);
        printf("\n");
        break;
      case '1':
        build=1; break;
      case '2':
        load=1; build=0; break;
      case '3':
        load=0; build=0; ibwt=1;  break;
      case 'v':
        verbose++; break;
      case 'l':
        light++; break;
      case 'T':
        time++; break;
      case 'P':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') p = (int) atoi(argv[optind++]);
        print=1; break;
      case 'S':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') sa_bytes  = (int) atoi(argv[optind++]);
        sa=1; break;
      case 'I':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') isa_bytes  = (int) atoi(argv[optind++]);
        isa=1; break;
      case 'L':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') lcp_bytes  = (int) atoi(argv[optind++]);
        lcp=1; break;
      case 'D':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') da_bytes  = (int) atoi(argv[optind++]);
        da=1; break;
      case 'g':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') t_bytes  = (int) atoi(argv[optind++]);
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') s_bytes  = (int) atoi(argv[optind++]);
        gsa=1; break;
      case 'B':
        bwt=1; break;
      case 'Q':
        q=1; break;
      case 'b':
        str=1; break;
      case 'G':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') t_bytes  = (int) atoi(argv[optind++]);
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') s_bytes  = (int) atoi(argv[optind++]);
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') lcp_bytes  = (int) atoi(argv[optind++]);
        gesa=1; break;
      case 'd':
        d = (int) atoi(optarg); break;
      case 'M':
        lcp_max=1; break;
      case 'W':
        lcp_max_text=1; break;
      case 'A':
        lcp_avg=1; break;
      case 'c':
        trlcp = (int) atoi(optarg);
        lcp=1; break;
      case 'u':
        upper=1; break; //lowercase
      case 'U':
        upper=2; break; //uppercase
      case 't':
        in_type=1; break; //txt
      case 'f':
        in_type=2; break; //fasta
      case 'q':
        in_type=3; break; //fastq
      case 'm':
        dir=1; break; //multiple files in directory FILENAME 
      case 'h':
        usage(argv[0]); break;      
      case 'o':
        c_output=optarg; output=1; break;
      case '?':
        exit(EXIT_FAILURE);
    }
  }

/********/

  if(optind+1==argc) {
    c_input=argv[optind++];
  }
  else  usage(argv[0]);


  if(time){
     time_start(&t_total, &c_total);
  }

  if(lcp_max || lcp_avg || lcp_max_text) lcp=1;

  if(!bwt && !str && !sa && !isa && !lcp && !da && !gsa && !bwt && !gesa && !ibwt){
    //die("choose at least one option  [--sa] [--lcp] [--da] [--gsa] [--gesa] [--bwt] [--str]");
    sa=1;//default
  }

  if(!output){
    c_output = (char*) malloc(strlen(c_input)+5);
    if(dir)
      sprintf(c_output, "all");
    else
      sprintf(c_output, "%s", filename_without_path(c_input));
  }
  else{
    if(c_output[strlen(c_output)-1]=='/'){
      char* c_tmp = (char*) malloc(strlen(c_output)+strlen(c_input)+1);
      if(dir){
        mkdir(c_output);
        sprintf(c_tmp, "%s%s", c_output, "all");
      }
      else
        sprintf(c_tmp, "%s%s", c_output, filename_without_path(c_input));
      c_output = c_tmp;
      output=0;
    }
  }

  if(build){

    unsigned char **R;
    size_t n=0;
  
    //disk access
    if(dir){
      R = (unsigned char**) file_load_multiple_dir(c_input, &d, &n, in_type, verbose);
    }
    else{
      R = (unsigned char**) file_load_multiple(c_input, &d, &n, in_type, 0);
    }

    if(n>pow(2,30) && (sizeof(int_t)<8)){
      fprintf(stderr, "####\n");
      fprintf(stderr, "ERROR: INPUT LARGER THAN %.1lf GB (%.1lf GB)\n", WORD/pow(2,30), n/pow(2,30));
      fprintf(stderr, "PLEASE USE %s-64\n", argv[0]);
      fprintf(stderr, "####\n");
      exit(EXIT_FAILURE);
    }
  
    //concatenate all string
    unsigned char *T = cat_all(R, d, &n, verbose, upper);
  
    #if DEBUG
      printf("R:\n");
      for(i=0; i<d && i<10; i++)
        printf("%" PRIdN ") %s (%zu)\n", i, R[i], strlen((char*)R[i]));
      printf("####\n");
      for(i=0; i<n; i++) printf("[%d]", T[i]);
      printf("\n");
    #endif

    //quality score sequence
    unsigned char **QS; 
    unsigned char *qs = NULL;
    if(q){

      size_t n=0;
      QS = (unsigned char**) file_load_multiple_qs(c_input, &d, &n, in_type);
      //concatenate all string
      qs = cat_all(QS, d, &n, 0, 0);

      #if DEBUG
        printf("QS:\n");
        for(i=0; i<d && i<10; i++)
          printf("%" PRIdN ") %s (%zu)\n", i, QS[i], strlen((char*)QS[i]));
        printf("####\n");
        for(i=0; i<n; i++) printf("[%d]", qs[i]);
        printf("\n");
      #endif

      //free memory
      for(i=0; i<d; i++)
        free(QS[i]);
      free(QS);
    }
  
    //free memory
    for(i=0; i<d; i++)
      free(R[i]);
    free(R);
  
    int_t *SA = NULL;
    SA = (int_t*) malloc(n*sizeof(int_t));
    assert(SA);
    int_t i;
    for(i=0; i<n; i++) SA[i]=0;

    int_t *ISA = NULL;  
    if(isa){
      ISA = (int_t*) malloc(n*sizeof(int_t));
      assert(ISA);
    }
  
    int_t *LCP = NULL;  
    if(lcp || gesa || lcp_max || lcp_avg || lcp_max_text){
      LCP = (int_t*) malloc(n*sizeof(int_t));
      assert(LCP);
      for(i=0; i<n; i++) LCP[i]=0;
    }
  
    int_da *DA = NULL;  
    rankbv_t* rbv = NULL;

    if(da || gsa || gesa){
      if(light==0){
        DA = (int_da*) malloc(n*sizeof(int_da));
        assert(DA);
        for(i=0; i<n; i++) DA[i]=0;
      }
    }
  
    if(verbose){
      printf("########\n");
      printf("INPUT = %s\n", c_input);
      printf("OUTPUT = %s", c_output);
      if(sa) printf("[.sa]");
      if(isa) printf("[.isa]");
      if(lcp)  printf("[.lcp]");
      if(da)  printf("[.da]");
      if(gsa)  printf("[.gsa]");
      if(gesa)  printf("[.gesa]");
      if(bwt)  printf("[.bwt]");
      if(q)  printf("[.qs]");
      printf("\n");
      printf("N = %zu\n", n);
      printf("d = %d\n", d);
      printf("sizeof(int) = %zu bytes\n", sizeof(int_t));
      if(da){
        if(light) printf("DA = lightweight\n");
        else printf("sizeof(int_da) = %zu bytes\n", sizeof(int_da));
      }
      #if LAST_END
        printf("LAST_END\n");
      #endif
      printf("########\n");
    }
  
  /********/
  
    printf("## gsufsort ##\n");

    if(time){
      time_start(&t_start, &c_start);
    }

    if(light){
      gsacak(T, (uint_t*)SA, LCP, NULL, n);

      //free LCP
      if(lcp){
        if(trlcp){
          for(i=0; i<n; i++) if(LCP[i]>trlcp) LCP[i]=trlcp;
        }
        printf("## store_to_disk ##\n");
        #if LAST_END  
          store_to_disk(NULL, NULL, NULL, NULL, LCP,  n, c_output,  "lcp",  0, 0, lcp_bytes);
        #else
          store_to_disk(NULL, NULL,  NULL, NULL,  LCP+1, n-1, c_output, "lcp",  0, 0, lcp_bytes);
        #endif
      }
      if(lcp && !gesa && !lcp_avg && !lcp_max && !lcp_max_text && !print) free(LCP);
      else if(verbose){ 
        printf("WARNING: LCP[1,N] not freed because of: ");
        if(gesa) printf("--gesa ");
        if(lcp_avg) printf("--lcp_avg ");
        if(lcp_max) printf("--lcp_max ");
        if(lcp_max_text) printf("--lcp_max_text ");
        if(print) printf("--print ");
        printf("\n");
      }
      /**/

      //compute DA
      if(da || gsa || gesa){
        rbv = rankbv_create(n,2);
        int_t i=0;
        for(;i<n-1;i++) if(T[i]==1) rankbv_setbit(rbv,i+1);
        rankbv_build(rbv);
      }
    }
    else{
      gsacak(T, (uint_t*)SA, LCP, DA, n);
    }

    //compute ISA
    if(isa){
      int_t i=0;
      for(;i<n;i++) ISA[SA[i]]=i; 
    }
  
    if(time){
      double d_time = time_stop(t_start, c_start);
      if(verbose) fprintf(stderr,"%.6lf\n", d_time);
    }
  
  /********/
  
    if(time){
       time_start(&t_start, &c_start);
    }
    if(light){
      if(!lcp)
        printf("## store_to_disk ##\n");
    }
    else
      printf("## store_to_disk ##\n");
  
    #if LAST_END  
      //store to disk
      if(str) store_to_disk(T,  NULL, NULL, NULL,  NULL, n, c_output, "str",  sizeof(char), 0, 0);
      if(sa)  store_to_disk(NULL, NULL, NULL, SA,   NULL, n, c_output,  "sa",   0, sa_bytes, 0);
      if(isa) store_to_disk(NULL, NULL, NULL, ISA,   NULL, n, c_output,  "isa", 0, isa_bytes, 0);
      if(light==0)
        if(lcp) store_to_disk(NULL, NULL, NULL, NULL, LCP,  n, c_output,  "lcp",  0, 0, lcp_bytes);
      if(da)  store_to_disk(NULL, DA,   rbv,  SA,   NULL, n, c_output,  "da",   da_bytes, 0, 0);
      if(gsa) store_to_disk(NULL, DA,   rbv,  SA,   NULL, n, c_output,  "gsa",  t_bytes, s_bytes, 0);
      if(gesa) store_to_disk(T, DA,   rbv,  SA,   LCP,  n, c_output,  "gesa", t_bytes, s_bytes, lcp_bytes);
      if(bwt) store_to_disk(T,  NULL, NULL, SA,   NULL, n, c_output,  "bwt",  sizeof(char), 0, 0);
      if(q)   store_to_disk(qs,  NULL, NULL, SA,   NULL, n, c_output,  "bwt.qs",  sizeof(char), 0, 0);
    #else
      //store to disk (+1 ignores last terminator)
      if(str) store_to_disk(T,  NULL,  NULL, NULL,  NULL,  n-1, c_output, "str",  sizeof(char), 0, 0);
      if(sa)  store_to_disk(NULL, NULL,  NULL, SA+1,  NULL,  n-1, c_output, "sa",   0, sa_bytes, 0);
      if(isa) store_to_disk(NULL, NULL,  NULL, ISA+1,  NULL,  n-1, c_output, "isa", 0, isa_bytes, 0);
      if(light==0)
        if(lcp) store_to_disk(NULL, NULL,  NULL, NULL,  LCP+1, n-1, c_output, "lcp",  0, 0, lcp_bytes);
      if(da)  store_to_disk(NULL, DA+1,  rbv,  SA+1,  NULL,  n-1, c_output, "da",   da_bytes, 0, 0);
      if(gsa) store_to_disk(NULL, DA+1,  rbv,  SA+1,  NULL,  n-1, c_output, "gsa",  t_bytes, s_bytes, 0);
      if(gesa) store_to_disk(T, DA+1,  rbv,  SA+1,  LCP+1, n-1, c_output, "gesa", t_bytes, s_bytes, lcp_bytes);
      if(bwt) store_to_disk(T,  NULL,  NULL, SA+1,  NULL,  n-1, c_output, "bwt", sizeof(char), 0, 0); 
      if(q)   store_to_disk(qs,  NULL,  NULL, SA+1,  NULL,  n-1, c_output, "bwt.qs", sizeof(char), 0, 0); 
    #endif
  
    if(time){
      double d_time = time_stop(t_start, c_start);
      if(verbose) fprintf(stderr,"%.6lf\n", d_time);
    }
  
  /********/
  
    if(print){
      printf("## print ##\n");
      if(!p)p=n;
      #if LAST_END
        print_array(T, DA, rbv, light, SA, ISA, LCP, str=1, da, sa, isa, lcp, bwt || gesa, gsa || gesa, gesa, n, min(n,p), 1);
      #else
        print_array(T, DA+1, rbv, light, SA+1, ISA+1, LCP+1, str=1, da, sa, isa, lcp, bwt || gesa, gsa || gesa, gesa, n-1, min(n-1,p), 0);
      #endif
    }

    if(lcp_max || lcp_avg || lcp_max_text){
      int_t max=0,total=n, j=0;
      double avg=0.0;
      #if LAST_END == 0
        total=n-1;
      #endif
      for(i=0; i<n; i++){
        if(LCP[i]>max){
          max=LCP[i];
          j=i;
        }
        avg+=(double)LCP[i]/(double)total;
      }
      if(lcp_max) printf("LCP max: %" PRIdN "\n", max);
      if(lcp_max_text){
        printf("Longest LCP: "); 
        for(i=SA[j]; i<SA[j]+max && T[i]!=1; i++){
          printf("%c", T[i]-1);
        }
        printf("\n");
      }
      if(lcp_avg) printf("LCP avg: %lf\n", avg);
    }

    //free memory
    free(T);
    if(q)
      free(qs);

    free(SA);
    if(isa)
      free(ISA);
    if(light){
      if(gesa || lcp_max || lcp_avg || lcp_max_text) free(LCP);
    }
    else{
      if(lcp || gesa || lcp_max || lcp_avg || lcp_max_text) free(LCP);
    }
    if(da || gsa || gesa){
      if(light)
        rankbv_free(rbv);
      else
        free(DA);
    }

  }//if BUILD
  else if(load){
    printf("## load_from_disk ##\n");

    if(verbose){
      printf("########\n");
      if(sa)  printf("SA  = %d bytes\n", sa_bytes);
      if(isa)  printf("ISA  = %d bytes\n", isa_bytes);
      if(lcp)  printf("LCP = %d bytes\n", lcp_bytes);
      if(da)  printf("DA  = %d bytes\n", da_bytes);
      if(gsa)  printf("GSA = (%d, %d) bytes\n", t_bytes, s_bytes);
      if(gesa)  printf("GESA = (%d, %d, %d, 1) bytes\n", t_bytes, s_bytes, lcp_bytes);
      printf("########\n");
    }
  
    unsigned char *T = NULL;
    unsigned char *BWT = NULL;
    int_t  *SA = NULL;
    int_t  *ISA = NULL;
    int_t  *LCP = NULL;
    int_da *DA = NULL;
    int_da *GSA_text = NULL;
    int_t  *GSA_suff = NULL;
  
    size_t n=0;
    if(str)  n = load_from_disk(&T, NULL, NULL, NULL, c_input, "str", 1, 0, 0);
    if(sa)   n = load_from_disk(NULL, NULL, &SA,  NULL, c_input, "sa", 0, sa_bytes,  0);
    if(isa)  n = load_from_disk(NULL, NULL, &ISA,  NULL, c_input, "isa", 0, isa_bytes,  0);
    if(lcp)  n = load_from_disk(NULL, NULL, NULL, &LCP, c_input, "lcp", 0, 0, lcp_bytes);
    if(da)   n = load_from_disk(NULL, &DA,  NULL, NULL, c_input, "da", da_bytes, 0, 0);
    if(gsa)  n = load_from_disk(NULL, &GSA_text, &GSA_suff, NULL, c_input, "gsa", t_bytes, s_bytes, 0);
    if(gesa) n = load_from_disk(&BWT, &GSA_text, &GSA_suff, &LCP, c_input, "gesa", t_bytes, s_bytes, lcp_bytes);
    if(bwt)  n = load_from_disk(&BWT, NULL,  NULL, NULL, c_input, "bwt", 1, 0, 0);
  
    if(trlcp){
      for(i=0; i<n; i++) if(LCP[i]>trlcp) LCP[i]=trlcp;
    }

    if(lcp_max || lcp_avg || lcp_max_text){
      int_t max=0, j=0;
      double avg=0.0;
      for(i=0; i<n; i++){
        if(LCP[i]>max){
          max=LCP[i];
          j=i;
        }
        avg+=(double)LCP[i]/(double)n;
      }
      if(lcp_max) printf("LCP max: %" PRIdN "\n", max);
      if(lcp_max_text){
        if(LCP && SA && T){
          printf("Longest LCP: "); 
          for(i=SA[j]; i<SA[j]+max && T[i]!=1; i++){
            printf("%c", T[i]);
          }
          printf("\n");
        }
        else{
          printf("ERROR: --lcp_max_text needs --str, --sa and --lcp.\n");
        }
      }
      if(lcp_avg) printf("LCP avg: %lf\n", avg);
    }

    if(print){
      printf("## print ##\n");
      // output
      printf("i\t");
      if(sa)  printf("SA\t");
      if(isa) printf("ISA\t");
      if(da)  printf("DA\t");
      if(gsa) printf("GSA\t\t");
      if(gesa) printf("GESA\t\t");
      if(LCP) printf("LCP\t");
      if(bwt || gesa) printf("BWT\t");
      if(str)  printf("suffixes");
      printf("\n");

      if(!p)p=n;
      for(i = 0; i < min(n,p); ++i) {
        printf("%" PRIdN "\t",i);
        if(sa)  printf("%" PRIdN "\t",SA[i]);
        if(isa) printf("%" PRIdN "\t",ISA[i]);
        if(da)  printf("%" PRIdA "\t",DA[i]);
        if(gsa || gesa) printf("(%" PRIdA ", %" PRIdN ")   \t", GSA_text[i], GSA_suff[i]);
        if(LCP) printf("%" PRIdN "\t",LCP[i]);
        if(bwt || gesa) printf("%c\t",BWT[i]);
        if(str && sa){
          int_t j=SA[i];
          while(j<n){
             if(T[j]=='\n'){ printf("$"); break;}
             else printf("%c", T[j]);
             j++;
           }
        }
        printf("\n");
      }
    }

    if(str) free(T);
    if(bwt) free(BWT);
    if(sa)  free(SA); 
    if(isa) free(ISA); 
    if(lcp) free(LCP);
    if(da)  free(DA); 
    if(gsa){
      free(GSA_text);
      free(GSA_suff);
    }
  
  }
  else if(ibwt){
    printf("## inverse_bwt ##\n");

    unsigned char *BWT = NULL;
    unsigned char *T = NULL;
  
    size_t n = load_from_disk(&BWT, NULL,  NULL, NULL, c_input, "ibwt", 1, 0, 0);

    //compute LF
    size_t i;
    int C[SIGMA];
    for(i=0; i<SIGMA; i++) C[i]=0;
    for(i=0; i<n; i++) C[BWT[i]]++;

    size_t sum=0;
    for(i=0; i<SIGMA; i++){
      sum+=C[i]; 
      C[i]=sum-C[i];
    }      

    char separator = 0, terminator = '\n';
    #if LAST_END
      separator = 1; 
    #endif

    size_t docs=0;
    int_t  *LF = (int_t*) malloc(n*sizeof(int_t));
    for(i=0; i<n; i++){
      LF[i] = C[BWT[i]]++;
      if(BWT[i]==separator)//separator
        docs++;
    }

    T = (unsigned char*) malloc(n*sizeof(unsigned char));

    //TODO --qs
    unsigned char *QS = NULL;
    unsigned char *qs = NULL;
  
    if(q){
      if(load_from_disk(&QS, NULL,  NULL, NULL, c_input, "qs", 1, 0, 0)!=n)
        exit(EXIT_FAILURE);
    
      qs = (unsigned char*) malloc(n*sizeof(unsigned char));
    }

    size_t j;
    #if LAST_END
      j=0;
      docs++;
    #else
      j=--docs;
    #endif

    for(i=2; i <n+1; i++){

      T[n-i]= (BWT[j]>separator)?BWT[j]:'\n';
      if(q) qs[n-i]=(QS[j]>separator)?QS[j]:'\n';

      //augmented LF-mapping
      if(BWT[j]==separator) j=--docs;
      else j=LF[j];
    }

    T[n-1]=terminator;
    if(q) qs[n-1]=terminator;

    if(print)
      printf("%s\n", T);

    printf("## store_to_disk ##\n");
  
    #if LAST_END
      store_to_disk(T,  NULL, NULL, NULL,  NULL, n-1, filename_without_ext(c_output), "ibwt",  sizeof(char), 0, 0);
      if(q)
        store_to_disk(qs,  NULL, NULL, NULL,  NULL, n-1, filename_without_ext(c_output), "iqs",  sizeof(char), 0, 0);
    #else
      store_to_disk(T,  NULL, NULL, NULL,  NULL, n, filename_without_ext(c_output), "ibwt",  sizeof(char), 0, 0);
      if(q)
        store_to_disk(qs,  NULL, NULL, NULL,  NULL, n, filename_without_ex(tc_output), "iqs",  sizeof(char), 0, 0);
    #endif

    free(BWT);     
    free(T);     
    free(LF);     
    if(q){
      free(QS);
      free(qs);
    }
  }

  if(time){
    printf("## total ##\n");
    double d_time = time_stop(t_total, c_total);
    if(verbose) fprintf(stderr,"%.6lf\n", d_time);
  }

  if(!output)
    free(c_output);

return 0;
}

/******************************************************************************/

int store_to_disk(unsigned char *T, int_da *DA, rankbv_t* rbv, int_t *SA, int_t *LCP, size_t n, char* c_file, char *ext, int wsize1, int wsize2, int wsize3){

  char c_out[256];
  if(strcmp(ext, "gsa")==0)
    sprintf(c_out, "%s.%d.%d.%s", c_file, wsize1, wsize2, ext);
  else if(strcmp(ext, "gesa")==0)
    sprintf(c_out, "%s.%d.%d.%d.1.%s", c_file, wsize1, wsize2, wsize3, ext);
  else if(strcmp(ext, "da")==0)
    sprintf(c_out, "%s.%d.%s", c_file, wsize1, ext);
  else if(strcmp(ext, "sa")==0 || strcmp(ext, "isa")==0)
    sprintf(c_out, "%s.%d.%s", c_file, wsize2, ext);
  else if(strcmp(ext, "lcp")==0)
    sprintf(c_out, "%s.%d.%s", c_file, wsize3, ext);
  else if(strcmp(ext, "bwt")==0 || strcmp(ext, "ibwt")==0 || strcmp(ext, "bwt.qs")==0|| strcmp(ext, "iqs")==0)
    sprintf(c_out, "%s.%s", c_file, ext);
  else
    sprintf(c_out, "%s.%d.%s", c_file, wsize1, ext);

  FILE *f_out = file_open(c_out, "wb");

  size_t wsize = wsize1+wsize2+wsize3;
  if(strcmp(ext, "gesa")==0) wsize++;

  printf("%s \t%zu bytes (n = %zu)\n", c_out, n*wsize, n);

  size_t i;
  if(strcmp(ext, "gsa")==0){
    for(i=0; i<n; i++){
      int_t da_value = (light)?rankbv_rank1(rbv,SA[i]):DA[i];
      fwrite(&da_value, wsize1, 1, f_out);

      #if LAST_END
        int_t value = (da_value==0)?SA[i]:SA[i]-SA[da_value]-1;
      #else
        int_t value = (da_value==0)?SA[i]:SA[i]-SA[da_value-1]-1;
      #endif
      fwrite(&value, wsize2, 1, f_out);
    }
  }
  else if(strcmp(ext, "gesa")==0){
    for(i=0; i<n; i++){
      //GSA
      int_t da_value = (light)?rankbv_rank1(rbv,SA[i]):DA[i];
      fwrite(&da_value, wsize1, 1, f_out);
      #if LAST_END
        int_t value = (da_value==0)?SA[i]:SA[i]-SA[da_value]-1;
      #else
        int_t value = (da_value==0)?SA[i]:SA[i]-SA[da_value-1]-1;
      #endif
      fwrite(&value, wsize2, 1, f_out);

      //LCP
      fwrite(&LCP[i], wsize3, 1, f_out); //LCP==C
      //BWT
      #if LAST_END
        char c = (SA[i])? T[SA[i]-1]:0; 
        c = (c==1)?c:c-1; //separators = 1, and terminator = 0
      #else
        char c = (SA[i])? T[SA[i]-1]-1:0; //separators = 0, no terminator
      #endif
      fwrite(&c, sizeof(char), 1, f_out);
    }
  }
  else if(strcmp(ext, "bwt")==0){
    for(i=0; i<n; i++){ //SA==B
      #if LAST_END
        char c = (SA[i])? T[SA[i]-1]:0; 
        c = (c>1)?c-1:c; //separators = 1, and terminator = 0
      #else
        char c = (SA[i])? T[SA[i]-1]-1:0; //separators = 0, no terminator
      #endif
      fwrite(&c, wsize1, 1, f_out); 
    }
  }
  else if(strcmp(ext, "bwt.qs")==0){
    for(i=0; i<n; i++){ //SA==B
      #if LAST_END
        char c = (SA[i])? T[SA[i]-1]:0; 
        c = (c>1)?c-1:c; //separators = 1, and terminator = 0
      #else
        char c = (SA[i])? T[SA[i]-1]-1:0; //separators = 0, no terminator
      #endif
      fwrite(&c, wsize1, 1, f_out); 
    }
  }
//TODO
  else if(strcmp(ext, "str")==0){
    for(i=0; i<n; i++){
      char c = (T[i]>0)?T[i]-1:'\n';
      if(c==0) c = '\n';
      fwrite(&c, wsize1, 1, f_out);
    }
  }
  else if(strcmp(ext, "da")==0){
    for(i=0; i<n; i++){
      int_t da_value = (light)?rankbv_rank1(rbv,SA[i]):DA[i];
      fwrite(&da_value, wsize1, 1, f_out);
    }
  }
  else if(strcmp(ext, "sa")==0){
    for(i=0; i<n; i++){
      fwrite(&SA[i], wsize2, 1, f_out);
    }
  }
  else if(strcmp(ext, "isa")==0){
    for(i=0; i<n; i++){
      fwrite(&SA[i], wsize2, 1, f_out);
    }
  }
  else if(strcmp(ext, "lcp")==0){
    for(i=0; i<n; i++){
      fwrite(&LCP[i], wsize3, 1, f_out);
    }
  }
  else if(strcmp(ext, "ibwt")==0 || strcmp(ext, "iqs")==0){
    fwrite(T, wsize1, n, f_out);
  }

  file_close(f_out);

return 0;
}

/******************************************************************************/

size_t load_from_disk(unsigned char **T, int_da **DA, int_t **SA, int_t **LCP, char* c_file, char *ext, int wsize1, int wsize2, int wsize3){

  char c_in[256];
  if(strcmp(ext, "gsa")==0)
    sprintf(c_in, "%s.%d.%d.%s", c_file, wsize1, wsize2, ext);
  else if(strcmp(ext, "gesa")==0)
    sprintf(c_in, "%s.%d.%d.%d.1.%s", c_file, wsize1, wsize2, wsize3, ext);
  else if(strcmp(ext, "da")==0)
    sprintf(c_in, "%s.%d.%s", c_file, wsize1, ext);
  else if(strcmp(ext, "sa")==0 || strcmp(ext, "isa")==0)
    sprintf(c_in, "%s.%d.%s", c_file, wsize2, ext);
  else if(strcmp(ext, "lcp")==0)
    sprintf(c_in, "%s.%d.%s", c_file, wsize3, ext);
  else if(strcmp(ext, "ibwt")==0)
    sprintf(c_in, "%s", c_file);
  else if(strcmp(ext, "bwt")==0 || strcmp(ext, "qs")==0)
    sprintf(c_in, "%s.%s", c_file, ext);
  else
    sprintf(c_in, "%s.%d.%s", c_file, wsize1, ext);

  FILE *f_in = file_open(c_in, "rb");
  if(f_in==NULL){
     exit(EXIT_FAILURE);
  }
  size_t size = file_size(f_in);
                                                            
  size_t wsize = wsize1+wsize2+wsize3;
  if(strcmp(ext, "gesa")==0) wsize++;

  size_t n = size/wsize;

  printf("%s\t%zu bytes (n = %zu)\n", c_in, size, n);

  if(strcmp(ext, "str")==0 || strcmp(ext, "bwt")==0 || strcmp(ext, "ibwt")==0 || strcmp(ext, "qs")==0){
    *T= (unsigned char*) malloc(n*sizeof(unsigned char));
    fread(*T, wsize1, n, f_in);
  }
  else{
                                       
    if(DA!=NULL){
      *DA = (int_da*) malloc(n*sizeof(int_da));
    }
    if(SA!=NULL){
      *SA = (int_t*) malloc(n*sizeof(int_t));
    }
    if(LCP!=NULL){
      *LCP = (int_t*) malloc(n*sizeof(int_t));
    }
    if(strcmp(ext, "gesa")==0){
      *T= (unsigned char*) malloc(n*sizeof(unsigned char));
    }
    size_t i;
    for(i=0; i<n; i++){

      if(DA){
        int_da da_value=0;
        fread(&da_value, wsize1, 1, f_in);
        (*DA)[i] = (int_da) da_value;
      }
      int_t value=0;
      if(SA){
        fread(&value, wsize2, 1, f_in);
        (*SA)[i] = (int_t) value;
      }
      //LCP (gesa)
      if(LCP){
        fread(&value, wsize3, 1, f_in);
        (*LCP)[i] = (int_t) value;
      }
      //BWT (gesa)
      if(strcmp(ext, "gesa")==0){
        fread(&value, sizeof(char), 1, f_in);
        (*T)[i] = (char) value;
      }
    }
  }

  file_close(f_in);

return n;
}

/******************************************************************************/
