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

#ifndef DEBUG
	#define DEBUG 0 
#endif

int store_to_disk(unsigned char *str, int_t *A, int_t *B, size_t n, char* c_file, char *ext, int wsize1, int wsize2);
size_t load_from_disk(unsigned char **str, int_t **A, int_t **B, char* c_file, char *ext, int wsize1, int wsize2);

/******************************************************************************/

void usage(char *name){
  printf("\n\tUsage: %s FILE [options]\n\n",name);
  puts("Extensions supported: .txt .fasta .fastq");
  puts("Available options:");

  puts("\t--build		(default)");
  puts("\t--load		load from disk FILE[.sa][.da][.lcp][.gsa][.bin]");

  puts("\t--sa  [w]	computes SA  using w (def 4) bytes (FILE.w.sa)");
  puts("\t--lcp [w]	computes LCP using w bytes (FILE.w.lcp)");
  puts("\t--da  [w]	computes DA  using w bytes (FILE.w.da)");
  puts("\t--gsa [w1][w2]	computes GSA using pairs of (w1, w2) bytes (FILE.w1.w1.gsa)");
  puts("\t--bwt		computes BWT using 1 byte (FILE.1.bwt)");
  puts("\t--bin		computes T^{cat} using 1 byte (FILE.1.bin)");

  puts("\t--docs   d	number of strings (def all FILE)");
  puts("\t--print  p	print arrays (stdout) A[1,p]");
  puts("\t--output out	renames output file");
  puts("\t--verbose	verbose output");
  puts("\t--time	output time (sec.)");
  puts("\t--help		this help message");
  exit(EXIT_FAILURE);
}

/******************************************************************************/

int main(int argc, char** argv){

	time_t t_total=0;clock_t c_total=0;
	time_t t_start=0;clock_t c_start=0;
	int_t i;

	int sa=0, lcp=0, da=0, bwt=0, bin=0, gsa=0, time=0; //txt
	int sa_bytes=sizeof(int_t);
	int lcp_bytes=sizeof(int_t);
	int da_bytes=sizeof(int_t);
	int t_bytes=sizeof(int_t), s_bytes=sizeof(int_t);

	extern char *optarg;
	extern int optind;
	int c;
	char *c_dir=NULL, *c_file=NULL, *c_input=NULL, *c_output=NULL;

	int verbose=0, print=0;
	int d=0; //number of string
	int output=0; //output

	int build=1; 
	int load=0;

/********/
  //int digit_optind = 0;

	while (1) {
		//int this_option_optind = optind ? optind : 1;
		int option_index = 0;

		static struct option long_options[] = {
    	{"print",		required_argument, 0, 'p'},
			{"sa",			optional_argument, 0, 'S'},
			{"lcp",			optional_argument, 0, 'L'},
			{"da",			optional_argument, 0,	'D'},
			{"gsa",			optional_argument, 0, 'G'},
			{"bwt",			no_argument,			 0, 'B'},
			{"bin",			no_argument,			 0, 'T'},
    	{"docs",		required_argument, 0, 'd'},
    	{"verbose", no_argument,       0, 'v'},
    	{"time",		no_argument,       0, 't'},
    	{"help",		no_argument,       0, 'h'},
    	{"output",  required_argument, 0, 'o'},
    	{"build",		no_argument,       0, '1'},
    	{"load",		no_argument,       0, '2'},
    	{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "S:vtp:d:L:D:GB:Tho:", long_options, &option_index);

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
			case 't':
				time++; break;
			case 'p':
				print = (int) atoi(optarg); break;
			case 'S':
				if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') sa_bytes	= (int) atoi(argv[optind++]);
				sa=1; break;
			case 'L':
				if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') lcp_bytes	= (int) atoi(argv[optind++]);
				lcp=1; break;
			case 'D':
				if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') da_bytes	= (int) atoi(argv[optind++]);
				da=1; break;
			case 'G':
				if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') t_bytes	= (int) atoi(argv[optind++]);
				if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') s_bytes	= (int) atoi(argv[optind++]);
				gsa=1; break;
			case 'B':
				bwt=1; break;
			case 'T':
				bin=1; break;
			case 'd':
				d = (int) atoi(optarg); break;
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

	if(!bwt && !bin && !sa && !lcp && !da && !gsa && !bwt){
		puts("####");
		die("choose at least one option  [--sa] [--lcp] [--da] [--gsa] [--bwt] [--bin]");
		exit(EXIT_FAILURE);
	}

	c_file= strrchr(c_input, '/');
	if(c_file==NULL){
	 	c_file = c_input;
		c_dir = "./";
	}
	else {
		c_file++;
		c_dir = strndup(c_input, strlen(c_input)-strlen(c_file));
	}

	if(!output){
		c_output = (char*) malloc(strlen(c_input)+5);
	  sprintf(c_output, "%s", c_input);
	}

	if(build){

		unsigned char **R;
		size_t n=0;
	
		//file_chdir(c_dir);
		//disk access
		R = (unsigned char**) file_load_multiple(c_input, &d, &n);
	
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
		for(i=0; i<n; i++) SA[i]=0;
	
		int_t *LCP = NULL;	
		if(lcp){
			LCP = (int_t*) malloc(n*sizeof(int_t));
			assert(LCP);
			for(i=0; i<n; i++) LCP[i]=0;
		}
	
		int_t *DA = NULL;	
		if(da || gsa){
			DA = (int_t*) malloc(n*sizeof(int_t));
			assert(DA);
			for(i=0; i<n; i++) DA[i]=-1;
		}
	
		if(verbose){
			printf("########\n");
			printf("INPUT = %s%s\n", c_dir,c_file);
			printf("OUTPUT = %s", c_output);
			if(sa) printf("[.sa]");
			if(lcp)	printf("[.lcp]");
			if(da)	printf("[.da]");
			if(gsa)	printf("[.gsa]");
			if(bwt)	printf("[.bwt]");
			printf("\n");
			printf("N = %zu\n", n);
			printf("d = %d\n", d);
			printf("sizeof(int) = %zu bytes\n", sizeof(int_t));
			printf("########\n");
		}
	
	/********/
	
		if(time){
		 	time_start(&t_start, &c_start);
			printf("## gsufsort ##\n");
		}
	
		gsacak(str, (uint_t*)SA, LCP, DA, n);
	
		if(time){
			double d_time = time_stop(t_start, c_start);
			if(verbose) fprintf(stderr,"%.6lf\n", d_time);
		}
	
	/********/
	
		if(time){
		 	time_start(&t_start, &c_start);
		}
		printf("## store_to_disk ##\n");
	
		//store to disk
		if(bin)	store_to_disk(str,  NULL,	NULL,	n, c_output, "bin",	sizeof(char), 0);
		if(sa)	store_to_disk(NULL, SA,		NULL,	n, c_output, "sa",	sa_bytes,	 0);
		if(lcp) store_to_disk(NULL, LCP,	NULL,	n, c_output, "lcp",	lcp_bytes, 0);
		if(da)	store_to_disk(NULL, DA,		NULL,	n, c_output, "da",	da_bytes,	 0);
		if(gsa) store_to_disk(NULL,	DA,		SA,		n, c_output, "gsa", t_bytes, s_bytes);
		if(bwt) store_to_disk(str,	SA,		NULL,	n, c_output, "bwt", sizeof(char), 0);
	
		if(time){
			double d_time = time_stop(t_start, c_start);
			if(verbose) fprintf(stderr,"%.6lf\n", d_time);
		}
	
	/********/
	
		if(print){
			printf("## print ##\n");
			print_array(str, SA, LCP, DA, bin=1, sa, da, bwt, gsa, n, min(n,print));
		}

		//free memory
		free(str);
		free(SA);
		if(lcp) free(LCP);
		if(da || gsa) free(DA);

	}//if BUILD
	else if(load){
		printf("## load_from_disk ##\n");

		if(verbose){
			printf("########\n");
			if(sa)	printf("SA  = %d bytes\n", sa_bytes);
			if(lcp)	printf("LCP = %d bytes\n", lcp_bytes);
			if(da)	printf("DA  = %d bytes\n", da_bytes);
			if(gsa)	printf("GSA = (%d, %d) bytes\n", t_bytes, s_bytes);
			printf("########\n");
		}
	
		unsigned char *str = NULL;
		unsigned char *BWT = NULL;
		int_t *SA = NULL;
		int_t *LCP = NULL;
		int_t *DA = NULL;
		int_t *GSA_text = NULL;
		int_t *GSA_suff = NULL;
	
		size_t n=0;
		if(bin)	n = load_from_disk(&str, NULL, NULL, c_output, "bin", 1, 0);
		if(sa)	n = load_from_disk(NULL, &SA,	 NULL, c_output, "sa", sa_bytes, 0);
		if(lcp)	n = load_from_disk(NULL, &LCP, NULL, c_output, "lcp", lcp_bytes, 0);
		if(da)	n = load_from_disk(NULL, &DA,	 NULL, c_output, "da", da_bytes, 0);
		if(gsa)	n = load_from_disk(NULL, &GSA_text, &GSA_suff, c_output, "gsa", t_bytes, s_bytes);
		if(bwt)	n = load_from_disk(&BWT, NULL,	NULL, c_output, "bwt", 1, 0);
	
		if(print){
			// output
			printf("i\t");
			if(sa)  printf("SA\t");
			if(da)  printf("DA\t");
			if(LCP) printf("LCP\t");
			if(gsa) printf("GSA\t\t");
			if(bwt) printf("BWT\t");
			if(bin)	printf("suffixes");
			printf("\n");

			for(i = 0; i < min(n,print); ++i) {
	  	  printf("%d\t",i);
			  if(sa)  printf("%" PRIdN "\t",SA[i]);
				if(da)  printf("%" PRIdN "\t",DA[i]);
				if(LCP) printf("%" PRIdN "\t",LCP[i]);
				if(gsa) printf("(%" PRIdN ", %" PRIdN ")   \t", GSA_text[i], GSA_suff[i]);
				if(bwt) printf("%c\t",BWT[i]);
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

int store_to_disk(unsigned char *str, int_t *A, int_t *B, size_t n, char* c_file, char *ext, int wsize1, int wsize2){

	char c_out[256];
	if(strcmp(ext, "gsa")==0)
	  sprintf(c_out, "%s.%d.%d.%s", c_file, wsize1, wsize2, ext);
	else
		sprintf(c_out, "%s.%d.%s", c_file, wsize1, ext);
  FILE *f_out = file_open(c_out, "wb");

	printf("%s\t%zu bytes (n = %zu)\n", c_out, n*(wsize1+wsize2), n);

	size_t i;
	if(strcmp(ext, "gsa")==0){
		for(i=0; i<n; i++){
			int_t value = (A[i]==0)?B[i]:B[i]-B[A[i]]-1; //DA==A, SA==B
			fwrite(&A[i], wsize1, 1, f_out);
			fwrite(&value, wsize2, 1, f_out);
		}
	}
	else if(strcmp(ext, "bwt")==0){
		for(i=0; i<n; i++){
			char c = (A[i])? str[A[i]-1]-1:'#'; //SA==A
			if(c==0) c = '$';
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
	else{
		for(i=0; i<n; i++){
			fwrite(&A[i], wsize1, 1, f_out);
		}
	}

  file_close(f_out);

return 0;
}

/******************************************************************************/

size_t load_from_disk(unsigned char **str, int_t **A, int_t **B, char* c_file, char *ext, int wsize1, int wsize2){

	char c_in[256];
	if(strcmp(ext, "gsa")==0)
		sprintf(c_in, "%s.%d.%d.%s", c_file, wsize1, wsize2, ext);
	else
		sprintf(c_in, "%s.%d.%s", c_file, wsize1, ext);

  FILE *f_in = file_open(c_in, "rb");
  size_t size = file_size(f_in);
                                                            
  size_t n = size/(wsize1+wsize2);

	printf("%s\t%zu bytes (n = %zu)\n", c_in, size, n);

	if(strcmp(ext, "bin")==0 || strcmp(ext, "bwt")==0){
		*str = (unsigned char*) malloc(n*sizeof(unsigned char));
		fread(*str, wsize1, n, f_in);
	}
	else{
                                       
		*A = (int_t*) malloc(n*sizeof(int_t));
		if(B!=NULL){
			*B = (int_t*) malloc(n*sizeof(int_t));
		}
		size_t i;
		for(i=0; i<n; i++){
			int_t value=0;
			fread(&value, wsize1, 1, f_in);
		  (*A)[i] = (int_t) value;

			if(B){
				fread(&value, wsize1, 1, f_in);
				(*B)[i] = (int_t) value;
			}
		}
	}

  file_close(f_in);

return n;
}

/******************************************************************************/
