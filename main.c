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
//#include "lib/document_array.hpp"

#ifndef DEBUG
  #define DEBUG 0 
#endif

#ifndef LAST_END
  #define LAST_END 0
#endif

#ifndef LIGHT 
  #define LIGHT 0
#endif

#define WORD (size_t)(pow(256,sizeof(int_t))/2.0)

int store_to_disk(unsigned char *str, int_da *DA, rankbv_t* rbv, int_t *SA,  int_t *LCP, size_t n, char* c_file, char *ext, int wsize1, int wsize2, int wsize3); 
size_t load_from_disk(unsigned char **str, int_da **DA, int_t **SA, int_t **LCP, char* c_file, char *ext, int wsize1, int wsize2, int wsize3);

//int document_array_wrapper(unsigned char *str, uint_t *SA, int_da *DA, int_t n);

/******************************************************************************/

void usage(char *name){
  printf("\n\tUsage: %s FILE [options]\n\n",name);
  puts("Extensions supported: .txt .fasta .fastq");
  puts("Available options:");

  puts("\t--build               (default)");
  puts("\t--load                load from disk FILE[.sa][.da][.lcp][.gsa][.bin]");
  puts("\t--sa    [w]           computes SA  using w (def 4) bytes (FILE.w.sa)");
  puts("\t--lcp   [w]           computes LCP (FILE.w.lcp)");
  puts("\t--da    [w]           computes DA  (FILE.w.da)");
  puts("\t--gsa   [w1][w2]      computes GSA=(text, suff) using pairs of (w1, w2) bytes (FILE.w1.w2.gsa)");
  puts("\t--gesa  [w1][w2][w3]  computes GESA=(GSA.text, GSA.suff, LCP, BWT) (FILE.w1.w2.w3.1.gesa)");
  puts("\t--bwt                 computes BWT using 1 byte (FILE.1.bwt)");
  puts("\t--bin                 computes T^{cat} (FILE.1.bin)");
  puts("\t--docs    d           number of strings (def all FILE)");
  puts("\t--print   p           print arrays (stdout) A[1,p]");
  puts("\t--output  outfile     renames output file");
  puts("\t--verbose             verbose output");
  puts("\t--lcp_max             outputs maximum LCP");
  puts("\t--lcp_avg             outputs average LCP");
  puts("\t--trlcp   k           outputs k-truncated LCP array (FILE.w.lcp)");
  puts("\t--time                output time (seconds)");
  puts("\t--light               runs lightweight algorithm to compute DA");
  puts("\t--help                this help message");
  exit(EXIT_FAILURE);
}

/******************************************************************************/

int main(int argc, char** argv){

  time_t t_total=0;clock_t c_total=0;
  time_t t_start=0;clock_t c_start=0;
  int_t i;

  int sa=0, lcp=0, da=0, bwt=0, bin=0, gsa=0, gesa=0, lcp_max=0, lcp_avg=0, trlcp=0,time=0; //txt
  int sa_bytes=sizeof(int_t);
  int lcp_bytes=sizeof(int_t);
  int da_bytes=sizeof(int_t);
  int t_bytes=sizeof(int_t), s_bytes=sizeof(int_t);

  extern char *optarg;
  extern int optind;
  int c;
  char *c_input=NULL, *c_output=NULL;

  int verbose=0, print=0;
  int d=0; //number of string
  int p=0; //print A[1,p]
  int light=0; //lightweight DA computation
  int output=0; //output

  int build=1; 
  int load=0;

/********/
  //int digit_optind = 0;

  while (1) {
    //int this_option_optind = optind ? optind : 1;
    int option_index = 0;

    static struct option long_options[] = {
      {"print",   optional_argument, 0, 'P'},
      {"sa",      optional_argument, 0, 'S'},
      {"lcp",     optional_argument, 0, 'L'},
      {"da",      optional_argument, 0, 'D'},
      {"gsa",     optional_argument, 0, 'g'},
      {"bwt",     no_argument,       0, 'B'},
      {"bin",     no_argument,       0, 'T'},
      {"docs",    required_argument, 0, 'd'},
      {"verbose", no_argument,       0, 'v'},
      {"time",    no_argument,       0, 't'},
      {"help",    no_argument,       0, 'h'},
      {"output",  required_argument, 0, 'o'},
      {"build",   no_argument,       0, '1'},
      {"load",    no_argument,       0, '2'},
      {"light",   no_argument,       0, 'l'},
      {"gesa",    optional_argument, 0, 'G'},
      {"lcp_max", optional_argument, 0, 'M'},
      {"lcp_avg", optional_argument, 0, 'A'},
      {"trlcp",   required_argument, 0, 'c'},
      {0,         0,                 0,  0 }
    };

    c = getopt_long(argc, argv, "S:vtP:d:L:D:g:lG:B:Tho:ic:", long_options, &option_index);

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
      case 'v':
        verbose++; break;
      case 'l':
        light++; break;
      case 't':
        time++; break;
      case 'P':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') p = (int) atoi(argv[optind++]);
        print=1; break;
      case 'S':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') sa_bytes  = (int) atoi(argv[optind++]);
        sa=1; break;
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
      case 'T':
        bin=1; break;
      case 'G':
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') t_bytes  = (int) atoi(argv[optind++]);
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') s_bytes  = (int) atoi(argv[optind++]);
        if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') lcp_bytes  = (int) atoi(argv[optind++]);
        gesa=1; break;
      case 'd':
        d = (int) atoi(optarg); break;
      case 'M':
        lcp_max=1; break;
      case 'A':
        lcp_avg=1; break;
      case 'c':
        trlcp = (int) atoi(optarg);
        lcp=1; break;
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

  if(!bwt && !bin && !sa && !lcp && !da && !gsa && !bwt && !gesa){
    puts("####");
    die("choose at least one option  [--sa] [--lcp] [--da] [--gsa] [--gesa] [--bwt] [--bin]");
    exit(EXIT_FAILURE);
  }

  if(!output){
    c_output = (char*) malloc(strlen(c_input)+5);
    sprintf(c_output, "%s", c_input);
  }

  if(build){

    unsigned char **R;
    size_t n=0;
  
    //disk access
    R = (unsigned char**) file_load_multiple(c_input, &d, &n);

    if(n>pow(2,30) && (sizeof(int_t)<8)){
      fprintf(stderr, "####\n");
      fprintf(stderr, "ERROR: INPUT LARGER THAN %.1lf GB (%.1lf GB)\n", WORD/pow(2,30), n/pow(2,30));
      fprintf(stderr, "PLEASE USE %s-64\n", argv[0]);
      fprintf(stderr, "####\n");
      exit(EXIT_FAILURE);
    }
  
    //concatenate all string
    unsigned char *str = cat_all(R, d, &n, verbose);
  
    #if DEBUG
      printf("R:\n");
      for(i=0; i<d && i<10; i++)
        printf("%" PRIdN ") %s (%zu)\n", i, R[i], strlen((char*)R[i]));
      printf("####\n");
      for(i=0; i<n; i++) printf("[%d]", str[i]);
      printf("\n");
    #endif
  
    //free memory
    for(i=0; i<d; i++)
      free(R[i]);
    free(R);
  
    int_t *SA = NULL;
    SA = (int_t*) malloc(n*sizeof(int_t));
    assert(SA);
    //for(i=0; i<n; i++) SA[i]=0;
  
    int_t *LCP = NULL;  
    if(lcp || gesa || lcp_max || lcp_avg){
      LCP = (int_t*) malloc(n*sizeof(int_t));
      assert(LCP);
      //for(i=0; i<n; i++) LCP[i]=0;
    }
  
    int_da *DA = NULL;  
    rankbv_t* rbv = NULL;

    if(da || gsa || gesa){
    #if LIGHT == 0
      DA = (int_da*) malloc(n*sizeof(int_da));
      assert(DA);
      //for(i=0; i<n; i++) DA[i]=-1;
    #endif
    }
  
    if(verbose){
      printf("########\n");
      printf("INPUT = %s\n", c_input);
      printf("OUTPUT = %s", c_output);
      if(sa) printf("[.sa]");
      if(lcp)  printf("[.lcp]");
      if(da)  printf("[.da]");
      if(gsa)  printf("[.gsa]");
      if(gesa)  printf("[.gesa]");
      if(bwt)  printf("[.bwt]");
      printf("\n");
      printf("N = %zu\n", n);
      printf("d = %d\n", d);
      printf("sizeof(int) = %zu bytes\n", sizeof(int_t));
      #if LIGHT == 0
        if(da) printf("sizeof(int_da) = %zu bytes\n", sizeof(int_da));
      #endif
      printf("########\n");
    }
  
  /********/
  
    if(time){
      time_start(&t_start, &c_start);
      printf("## gsufsort ##\n");
    }

    #if LIGHT
      gsacak(str, (uint_t*)SA, LCP, NULL, n);

      //free LCP
      if(trlcp){
        for(i=0; i<n; i++) if(LCP[i]>trlcp) LCP[i]=trlcp;
      }
      if(lcp){
        printf("## store_to_disk ##\n");
        #if LAST_END  
          store_to_disk(NULL, NULL, NULL, NULL, LCP,  n, c_output,  "lcp",  0, 0, lcp_bytes);
        #else
          store_to_disk(NULL, NULL,  NULL, NULL,  LCP+1, n-1, c_output, "lcp",  0, 0, lcp_bytes);
        #endif
      }
      if(lcp && !lcp_avg && !lcp_max) free(LCP);
      /**/

      //compute DA
      if(da || gsa || gesa){
        rbv = rankbv_create(n,0);
        int_t i=0;
        for(;i<n-1;i++) if(str[i]==1) rankbv_setbit(rbv,i+1);
        rankbv_build(rbv);
      }
    #else
      gsacak(str, (uint_t*)SA, LCP, DA, n);
    #endif
  
    if(time){
      double d_time = time_stop(t_start, c_start);
      if(verbose) fprintf(stderr,"%.6lf\n", d_time);
    }
  
  /********/
  
    if(time){
       time_start(&t_start, &c_start);
    }
    #if LIGHT
      if(!lcp)
    #endif
        printf("## store_to_disk ##\n");
  
    #if LAST_END  
      //store to disk
      if(bin) store_to_disk(str,  NULL, NULL, NULL,  NULL, n, c_output, "bin",  sizeof(char), 0, 0);
      if(sa)  store_to_disk(NULL, NULL, NULL, SA,   NULL, n, c_output,  "sa",   0, sa_bytes, 0);
      #if LIGHT == 0
        if(lcp) store_to_disk(NULL, NULL, NULL, NULL, LCP,  n, c_output,  "lcp",  0, 0, lcp_bytes);
      #endif
      if(da)  store_to_disk(NULL, DA,   rbv,  SA,   NULL, n, c_output,  "da",   da_bytes, 0, 0);
      if(gsa) store_to_disk(NULL, DA,   rbv,  SA,   NULL, n, c_output,  "gsa",  t_bytes, s_bytes, 0);
      if(gesa) store_to_disk(str, DA,   rbv,  SA,   LCP,  n, c_output,  "gesa", t_bytes, s_bytes, lcp_bytes);
      if(bwt) store_to_disk(str,  NULL, NULL, SA,   NULL, n, c_output,  "bwt",  sizeof(char), 0, 0);
    #else
      //store to disk (+1 ignores last terminator)
      if(bin) store_to_disk(str,  NULL,  NULL, NULL,  NULL,  n-1, c_output, "bin",  sizeof(char), 0, 0);
      if(sa)  store_to_disk(NULL, NULL,  NULL, SA+1,  NULL,  n-1, c_output, "sa",   0, sa_bytes, 0);
      #if LIGHT == 0
        if(lcp) store_to_disk(NULL, NULL,  NULL, NULL,  LCP+1, n-1, c_output, "lcp",  0, 0, lcp_bytes);
      #endif
      if(da)  store_to_disk(NULL, DA+1,  rbv,  SA+1,  NULL,  n-1, c_output, "da",   da_bytes, 0, 0);
      if(gsa) store_to_disk(NULL, DA+1,  rbv,  SA+1,  NULL,  n-1, c_output, "gsa",  t_bytes, s_bytes, 0);
      if(gesa) store_to_disk(str, DA+1,  rbv,  SA+1,  LCP+1, n-1, c_output, "gesa", t_bytes, s_bytes, lcp_bytes);
      if(bwt) store_to_disk(str,  NULL,  NULL, SA+1,  NULL,  n-1, c_output, "bwt", sizeof(char), 0, 0); 
    #endif
  
    if(time){
      double d_time = time_stop(t_start, c_start);
      if(verbose) fprintf(stderr,"%.6lf\n", d_time);
    }
  
  /********/
  
    if(print){
      printf("## print ##\n");
      if(!p)p=n;
      print_array(str, DA, SA, LCP, bin=1, da, sa, bwt || gesa, gsa || gesa, n, min(n,p));
    }

    if(lcp_max || lcp_avg){
      int_t max=0,total=n;
      double avg=0.0;
      #if LAST_END == 0
        total=n-1;
      #endif
      for(i=0; i<n; i++){
        if(LCP[i]>max) max=LCP[i];
        avg+=(double)LCP[i]/(double)total;
      }
      if(lcp_max) printf("LCP max: %" PRIdN "\n", max);
      if(lcp_avg) printf("LCP avg: %lf\n", avg);
    }

    //free memory
    free(str);
    free(SA);
    #if LIGHT
      if(gesa || lcp_max || lcp_avg) free(LCP);
    #else
      if(lcp || gesa || lcp_max || lcp_avg) free(LCP);
    #endif
    if(da || gsa || gesa){
      #if LIGHT
        rankbv_free(rbv);
      #else
        free(DA);
      #endif
    }

  }//if BUILD
  else if(load){
    printf("## load_from_disk ##\n");

    if(verbose){
      printf("########\n");
      if(sa)  printf("SA  = %d bytes\n", sa_bytes);
      if(lcp)  printf("LCP = %d bytes\n", lcp_bytes);
      if(da)  printf("DA  = %d bytes\n", da_bytes);
      if(gsa)  printf("GSA = (%d, %d) bytes\n", t_bytes, s_bytes);
      if(gesa)  printf("GESA = (%d, %d, %d, 1) bytes\n", t_bytes, s_bytes, lcp_bytes);
      printf("########\n");
    }
  
    unsigned char *str = NULL;
    unsigned char *BWT = NULL;
    int_t  *SA = NULL;
    int_t  *LCP = NULL;
    int_da *DA = NULL;
    int_da *GSA_text = NULL;
    int_t  *GSA_suff = NULL;
  
    size_t n=0;
    if(bin)  n = load_from_disk(&str, NULL, NULL, NULL, c_output, "bin", 1, 0, 0);
    if(sa)   n = load_from_disk(NULL, NULL, &SA,  NULL, c_output, "sa", 0, sa_bytes,  0);
    if(lcp)  n = load_from_disk(NULL, NULL, NULL, &LCP, c_output, "lcp", 0, 0, lcp_bytes);
    if(da)   n = load_from_disk(NULL, &DA,  NULL, NULL, c_output, "da", da_bytes, 0, 0);
    if(gsa)  n = load_from_disk(NULL, &GSA_text, &GSA_suff, NULL, c_output, "gsa", t_bytes, s_bytes, 0);
    if(gesa) n = load_from_disk(&BWT, &GSA_text, &GSA_suff, &LCP, c_output, "gesa", t_bytes, s_bytes, lcp_bytes);
    if(bwt)  n = load_from_disk(&BWT, NULL,  NULL, NULL, c_output, "bwt", 1, 0, 0);
  
    if(trlcp){
      for(i=0; i<n; i++) if(LCP[i]>trlcp) LCP[i]=trlcp;
    }

    if(lcp_max || lcp_avg){
      int_t max=0;
      double avg=0.0;
      for(i=0; i<n; i++){
        if(LCP[i]>max) max=LCP[i];
        avg+=(double)LCP[i]/(double)n;
      }
      if(lcp_max) printf("LCP max: %" PRIdN "\n", max);
      if(lcp_avg) printf("LCP avg: %lf\n", avg);
    }

    if(print){
      printf("## print ##\n");
      // output
      printf("i\t");
      if(sa)  printf("SA\t");
      if(da)  printf("DA\t");
      if(gsa) printf("GSA\t\t");
      if(gesa) printf("GESA\t\t");
      if(LCP) printf("LCP\t");
      if(bwt || gesa) printf("BWT\t");
      if(bin)  printf("suffixes");
      printf("\n");

      if(!p)p=n;
      for(i = 0; i < min(n,p); ++i) {
        printf("%" PRIdN "\t",i);
        if(sa)  printf("%" PRIdN "\t",SA[i]);
        if(da)  printf("%" PRIdA "\t",DA[i]);
        if(gsa || gesa) printf("(%" PRIdA ", %" PRIdN ")   \t", GSA_text[i], GSA_suff[i]);
        if(LCP) printf("%" PRIdN "\t",LCP[i]);
        if(bwt || gesa) printf("%c\t",BWT[i]);
        if(bin && sa){
          int_t j=SA[i];
          while(j<n){
             if(str[j]=='$'){ printf("$"); break;}
             else printf("%c", str[j]);
             j++;
           }
        }
        printf("\n");
      }
    }

    if(bin) free(str);
    if(bwt) free(BWT);
    if(sa)  free(SA); 
    if(lcp) free(LCP);
    if(da)  free(DA); 
    if(gsa){
      free(GSA_text);
      free(GSA_suff);
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

int store_to_disk(unsigned char *str, int_da *DA, rankbv_t* rbv, int_t *SA,  int_t *LCP, size_t n, char* c_file, char *ext, int wsize1, int wsize2, int wsize3){

  char c_out[256];
  if(strcmp(ext, "gsa")==0)
    sprintf(c_out, "%s.%d.%d.%s", c_file, wsize1, wsize2, ext);
  else if(strcmp(ext, "gesa")==0)
    sprintf(c_out, "%s.%d.%d.%d.1.%s", c_file, wsize1, wsize2, wsize3, ext);
  else if(strcmp(ext, "da")==0)
    sprintf(c_out, "%s.%d.%s", c_file, wsize1, ext);
  else if(strcmp(ext, "sa")==0)
    sprintf(c_out, "%s.%d.%s", c_file, wsize2, ext);
  else if(strcmp(ext, "lcp")==0)
    sprintf(c_out, "%s.%d.%s", c_file, wsize3, ext);
  else
    sprintf(c_out, "%s.%d.%s", c_file, wsize1, ext);

  FILE *f_out = file_open(c_out, "wb");

  size_t wsize = wsize1+wsize2+wsize3;
  if(strcmp(ext, "gesa")==0) wsize++;

  printf("%s\t%zu bytes (n = %zu)\n", c_out, n*wsize, n);

  size_t i;
  if(strcmp(ext, "gsa")==0){
    for(i=0; i<n; i++){
      #if LIGHT
        int_t da_value = rankbv_rank1(rbv,SA[i]);
      #else
        int_t da_value = DA[i];
      #endif
      fwrite(&da_value, wsize1, 1, f_out);
      int_t value = (da_value==0)?SA[i]:SA[i]-SA[da_value]-1;
      fwrite(&value, wsize2, 1, f_out);
    }
  }
  else if(strcmp(ext, "gesa")==0){
    for(i=0; i<n; i++){
      //GSA
      #if LIGHT
        int_t da_value = rankbv_rank1(rbv,SA[i]);
      #else
        int_t da_value = DA[i];
      #endif
      fwrite(&da_value, wsize1, 1, f_out);
      int_t value = (da_value==0)?SA[i]:SA[i]-SA[da_value]-1;
      fwrite(&value, wsize2, 1, f_out);
      //LCP
      fwrite(&LCP[i], wsize3, 1, f_out); //LCP==C
      //BWT
      #if LAST_END
        char c = (SA[i])? str[SA[i]-1]:0; 
        c = (c==1)?c:c-1; //separators = 1, and terminator = 0
      #else
        char c = (SA[i])? str[SA[i]-1]-1:0; //separators = 0, no terminator
      #endif
      fwrite(&c, sizeof(char), 1, f_out);
    }
  }
  else if(strcmp(ext, "bwt")==0){
    for(i=0; i<n; i++){ //SA==B
      #if LAST_END
        char c = (SA[i])? str[SA[i]-1]:0; 
        c = (c==1)?c:c-1; //separators = 1, and terminator = 0
      #else
        char c = (SA[i])? str[SA[i]-1]-1:0; //separators = 0, no terminator
      #endif
      fwrite(&c, wsize1, 1, f_out); 
    }
  }
  else if(strcmp(ext, "bin")==0){
    for(i=0; i<n; i++){
      char c = (str[i]>0)?str[i]-1:'#';
      if(c==0) c = '$';
      fwrite(&c, wsize1, 1, f_out);
    }
  }
  else if(strcmp(ext, "da")==0){
    for(i=0; i<n; i++){
      #if LIGHT
        int_t da_value = rankbv_rank1(rbv,SA[i]);
      #else
        int_t da_value = DA[i];
      #endif
      fwrite(&da_value, wsize1, 1, f_out);
    }
  }
  else if(strcmp(ext, "sa")==0){
    for(i=0; i<n; i++){
      fwrite(&SA[i], wsize2, 1, f_out);
    }
  }
  else if(strcmp(ext, "lcp")==0){
    for(i=0; i<n; i++){
      fwrite(&LCP[i], wsize3, 1, f_out);
    }
  }

  file_close(f_out);

return 0;
}

/******************************************************************************/

size_t load_from_disk(unsigned char **str, int_da **DA, int_t **SA, int_t **LCP, char* c_file, char *ext, int wsize1, int wsize2, int wsize3){

  char c_in[256];
  if(strcmp(ext, "gsa")==0)
    sprintf(c_in, "%s.%d.%d.%s", c_file, wsize1, wsize2, ext);
  else if(strcmp(ext, "gesa")==0)
    sprintf(c_in, "%s.%d.%d.%d.1.%s", c_file, wsize1, wsize2, wsize3, ext);
  else if(strcmp(ext, "da")==0)
    sprintf(c_in, "%s.%d.%s", c_file, wsize1, ext);
  else if(strcmp(ext, "sa")==0)
    sprintf(c_in, "%s.%d.%s", c_file, wsize2, ext);
  else if(strcmp(ext, "lcp")==0)
    sprintf(c_in, "%s.%d.%s", c_file, wsize3, ext);
  else
    sprintf(c_in, "%s.%d.%s", c_file, wsize1, ext);

  FILE *f_in = file_open(c_in, "rb");
  size_t size = file_size(f_in);
                                                            
  size_t wsize = wsize1+wsize2+wsize3;
  if(strcmp(ext, "gesa")==0) wsize++;

  size_t n = size/wsize;

  printf("%s\t%zu bytes (n = %zu)\n", c_in, size, n);

  if(strcmp(ext, "bin")==0 || strcmp(ext, "bwt")==0){
    *str = (unsigned char*) malloc(n*sizeof(unsigned char));
    fread(*str, wsize1, n, f_in);
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
      *str = (unsigned char*) malloc(n*sizeof(unsigned char));
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
        (*str)[i] = (char) value;
      }
    }
  }

  file_close(f_in);

return n;
}

/******************************************************************************/
