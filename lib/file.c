// vim: noai:ts=2:sw=2

#include "file.h"

#define N_ALLOC 2048 
#if GZ
#define GZ_BUFF 10485760 //10MB for each string line (in txt files)
#endif

#define FILE_NAME 1024

/* Returns the file extension
*/
const char *get_filename_ext(const char *filename) {

  char *dot = strrchr(filename, '.');

  if(!dot || dot == filename) return "";

  return dot + 1; 
}   

/*******************************************************************/
/* Returns gz file extension
*/
const char *get_gz_ext(const char *filename) {

  char *dot = strrchr(filename, '.');

  char *tmp = dot;
  *tmp = '#';

  dot = strrchr(filename, '.');
  *tmp = '.';

  if(!dot || dot == filename) return "";

  return dot + 1; 
}   

/*******************************************************************/

char *filename_without_path(char *filename) {

  char *dot = strrchr(filename, '/');

  if(!dot || dot == filename) return filename;

  return dot+1; 
}   

/*******************************************************************/

char *filename_without_ext(char *filename) {

  char *dot = strrchr(filename, '.');

  if(!dot || dot == filename) return "";                                                            
  *dot = '\0';

  return filename; 
}   

/*******************************************************************/

/* Changes to a working directory, where everything will be read
 * from and written to
 */ 
int file_chdir(char* dir){

  char* oldwd = getcwd(NULL,0);
  if (!oldwd) die(__func__);
  if (chdir(dir) == -1) die(__func__);

  free(oldwd);
  return 0;
}

/*******************************************************************/

//Open a file and returns a pointer
FILE* file_open(char *c_file, const char * c_mode){

  FILE* f_in;

  f_in = fopen(c_file, c_mode);
  if(!f_in){
    fprintf(stderr, "%s\n", c_file);
    perror ("file_open");
  }

  fseek(f_in, 0, SEEK_SET);

  return f_in;
}


int file_close(FILE* f_in){

  fclose(f_in);
  if (!f_in) perror ("file_close");

  return 0;
}

/*******************************************************************/

size_t file_size(FILE* f_in){

  fseek(f_in, 0, SEEK_END);
  size_t length = ftell(f_in);
  rewind(f_in);

  return length;
}


/*******************************************************************/

char* file_load(FILE* f_in) {

  size_t len = 0;
  ssize_t size = 0;
  char *c_aux = NULL;

  /*Read one line*/
  size = getline(&c_aux, &len, f_in);
  if (size == -1) perror("file_load");

  /*Copy line to c_buffer*/
  char *c_buffer = (char*) malloc((size+1)*sizeof(char));
  strncpy(c_buffer, c_aux, size);

  c_buffer[size-1] = separator;
  c_buffer[size] = terminator;

  free(c_aux);

  return c_buffer;
}

/*******************************************************************/

// read line by line
char** load_multiple_txt(char *c_file, int *k, size_t *n) {

  FILE* f_in = file_open(c_file, "rb");
  if(!f_in){
    fprintf (stderr, "file_open of '%s' failed: %s.\n", c_file, strerror (errno));
    exit (EXIT_FAILURE);
  }

  int n_alloc = N_ALLOC;
  char **c_buffer = (char**) malloc(n_alloc*sizeof(char*));

  int i;
  for(i=0; i<*k; i++){

    size_t len = 0; c_buffer[i] = NULL;		
    ssize_t size = getline(&c_buffer[i], &len, f_in);
    if (size == -1){
      *k = i;
      break;		
    }
    c_buffer[i][size-1] = 0;

    (*n) += size;

    if(i==n_alloc-1){
      n_alloc+=N_ALLOC;
      c_buffer = (char**) realloc(c_buffer, n_alloc*sizeof(char*));
    }
  }
  fclose(f_in);

  return c_buffer;
}

/*******************************************************************/

// read sequences separeted by '@' line
char** load_multiple_fastq(char *c_file, int *k, size_t *n){

  FILE* f_in = file_open(c_file, "rb");
  if(!f_in){
    fprintf (stderr, "file_open of '%s' failed: %s.\n", c_file, strerror (errno));
    exit (EXIT_FAILURE);
  }

  int n_alloc = N_ALLOC;
  char **c_buffer = (char**) malloc(n_alloc*sizeof(char*));

  size_t len = 0;
  char *buf = NULL;
  int i;
  for(i=0; i<*k; i++){

    len = 0; buf = NULL;
    ssize_t size = getline(&buf, &len, f_in); // @'s line
    free(buf);
    if (size <= 1){
      *k = i;
      break;		
    }

    len = 0; c_buffer[i] = NULL;
    size = getline(&c_buffer[i], &len, f_in); // read line
    c_buffer[i][size-1] = 0;

    (*n) += size;

    len = 0; buf = NULL;
    size = getline(&buf, &len, f_in); // +'s line
    free(buf);
    len = 0; buf = NULL;
    size = getline(&buf, &len, f_in); // @'s line
    free(buf);

    if(i==n_alloc-1){
      n_alloc+=N_ALLOC;
      c_buffer = (char**) realloc(c_buffer, n_alloc*sizeof(char*));
    }
  }
  fclose(f_in);

  return c_buffer;
}

/*******************************************************************/
char** load_multiple_qs(char *c_file, int *k, size_t *n){

  FILE* f_in = file_open(c_file, "rb");
  if(!f_in){
    fprintf (stderr, "file_open of '%s' failed: %s.\n", c_file, strerror (errno));
    exit (EXIT_FAILURE);
  }

  int n_alloc = N_ALLOC;
  char **c_buffer = (char**) malloc(n_alloc*sizeof(char*));

  size_t len = 0;
  char *buf = NULL;
  int i;
  for(i=0; i<*k; i++){

    len = 0; buf = NULL;
    ssize_t size = getline(&buf, &len, f_in); // @'s line
    free(buf);
    if (size <= 1){
      *k = i;
      break;		
    }

    len = 0; buf = NULL;
    size = getline(&buf, &len, f_in); // read line
    free(buf);
    len = 0; buf = NULL;
    size = getline(&buf, &len, f_in); // +'s line
    free(buf);

    len = 0; c_buffer[i] = NULL;
    size = getline(&c_buffer[i], &len, f_in); // @'s line
    c_buffer[i][size-1] = 0;

    (*n) += size;

    if(i==n_alloc-1){
      n_alloc+=N_ALLOC;
      c_buffer = (char**) realloc(c_buffer, n_alloc*sizeof(char*));
    }
  }
  fclose(f_in);

  return c_buffer;
}

/*******************************************************************/

// read sequences separeted by '>' line
char** load_multiple_fasta(char *c_file, int *k, size_t *n){

  FILE* f_in = file_open(c_file, "rb");
  if(!f_in){
    fprintf (stderr, "file_open of '%s' failed: %s.\n", c_file, strerror (errno));
    exit (EXIT_FAILURE);
  }

  int n_alloc = N_ALLOC;
  char **c_buffer = (char**) malloc(n_alloc*sizeof(char*));

  char *buf = NULL;
  size_t len = 0;

  ssize_t size = getline(&buf, &len, f_in);// first sequence
  if (size == -1) perror("file_load");
  free(buf);

  int count=0;
  int i;
  for(i=0; i<*k; i++){

    if(i!=count){
      *k = i;
      break;		
    }

    int c_alloc = 128;
    c_buffer[i] = (char*) malloc(c_alloc*sizeof(char));

    size_t p=0;
    len = 0; buf=NULL;
    while(getline(&buf, &len, f_in)!=-1){

      if(buf[0] == '>'){
        count++;
        break;
      }

      if(p+len>c_alloc){
        c_alloc += len+128;
        c_buffer[i] = (char*) realloc(c_buffer[i], sizeof(char) * c_alloc);
      }

      strcpy(&c_buffer[i][p], buf);
      p+=strlen(buf)-1;

      free(buf);
      buf=NULL; len=0;
    }

    free(buf);
    c_buffer[i][p] = 0;
    (*n) += p+1;

    if(i==n_alloc-1){
      n_alloc+=N_ALLOC;
      c_buffer = (char**) realloc(c_buffer, n_alloc*sizeof(char*));
    }
  }
  fclose(f_in);

  return c_buffer;
}

/*******************************************************************/
//loads fasta and fastq compressed files (gz)
#if GZ
char** load_multiple_gz(char *c_file, int *k, size_t *n, int in_type){

  int n_alloc=N_ALLOC;
  char **c_buffer = (char**) malloc(n_alloc*sizeof(char*));

  gzFile fp; 
  int i, l;

  fp = gzopen(c_file, "r"); // STEP 2: open the file handler
  if(!fp){
    fprintf (stderr, "gzopen of '%s' failed: %s.\n", c_file, strerror (errno));
    exit (EXIT_FAILURE);
  }

  const char *type = get_gz_ext(c_file);

  if(in_type==1 || strcmp(type, "txt.gz")==0){

    char *buf = (char*) malloc(GZ_BUFF*sizeof(char));

    for(i=0; i<*k; i++){

      if(gzgets(fp, buf, GZ_BUFF-1)==Z_NULL){//EOF
        *k=i;
        break;
      }
      size_t len = strlen(buf);

      c_buffer[i] = (char*) malloc((len+1)*sizeof(char));
      strcpy(&c_buffer[i][0], buf);
      (*n) += len+1;

      c_buffer[i][len-1] = 0;

      if(i==n_alloc-1){
        n_alloc+=N_ALLOC;
        c_buffer = (char**) realloc(c_buffer, n_alloc*sizeof(char*));
      }
    }

    free(buf);

  }
  else{ //fasta and fastq

    kseq_t *seq = kseq_init(fp); // STEP 3: initialize seq  
    for(i=0; i<*k; i++){

      if((l = kseq_read(seq)) < 0){
        *k = i;
        break;
      }
      int len = strlen(seq->seq.s);  
      c_buffer[i] = (char*) malloc((len+1)*sizeof(char));

      strcpy(&c_buffer[i][0], seq->seq.s);

      c_buffer[i][len] = 0;
      (*n) += len+1;

      if(i==n_alloc-1){
        n_alloc+=N_ALLOC;
        c_buffer = (char**) realloc(c_buffer, n_alloc*sizeof(char*));
      }
    }
    kseq_destroy(seq); // STEP 5: destroy seq
  }
  gzclose(fp); // STEP 6: close the file handler  

  return c_buffer;
}

/*******************************************************************/

char** load_multiple_gz_qs(char *c_file, int *k, size_t *n){

  int n_alloc=N_ALLOC;
  char **c_buffer = (char**) malloc(n_alloc*sizeof(char*));

  gzFile fp; 
  int i, l;

  fp = gzopen(c_file, "r"); // STEP 2: open the file handler
  if(!fp){
    fprintf (stderr, "gzopen of '%s' failed: %s.\n", c_file, strerror (errno));
    exit (EXIT_FAILURE);
  }

  kseq_t *seq = kseq_init(fp); // STEP 3: initialize seq  
  for(i=0; i<*k; i++){

    if((l = kseq_read(seq)) < 0){
      *k = i;
      break;
    }
    int len = strlen(seq->qual.s);  
    c_buffer[i] = (char*) malloc((len+1)*sizeof(char));

    strcpy(&c_buffer[i][0], seq->qual.s);
    c_buffer[i][len] = 0;
    (*n) += len+1;

    if(i==n_alloc-1){
      n_alloc+=N_ALLOC;
      c_buffer = (char**) realloc(c_buffer, n_alloc*sizeof(char*));
    }
  }
  kseq_destroy(seq); // STEP 5: destroy seq

  gzclose(fp); // STEP 6: close the file handler  

  return c_buffer;
}
#endif

/*******************************************************************/

char** file_load_multiple_dir(char* c_file, int *k, size_t *n, int in_type, int verbose) {

  char **c_buffer = NULL; // = (char**) malloc(k*sizeof(char*));
  if(*k==0) *k=INT_MAX;
  int tmp=*k;

  int total=0;
  size_t sum=0;

  char *c_dir = (char*) malloc(strlen(c_file)+2);
  if(c_file[strlen(c_file)-1]=='/')
    sprintf(c_dir, "%s", c_file);
  else
    sprintf(c_dir, "%s%s", c_file, "/");

  /**/
  struct dirent **namelist;
  int num_files = scandir(c_dir, &namelist, NULL, alphasort);

  if(num_files<0){
    printf("ERROR: Could not open directory: %s\n", c_dir); 
    exit (EXIT_FAILURE);
  }
  else {
    int i=0;
    for (i=0; i<num_files; i++) {
      if(namelist[i]->d_type==DT_REG){

        char *c_in = (char*) malloc(strlen(c_dir)+strlen(namelist[i]->d_name)+1);
        sprintf(c_in, "%s%s", c_dir, namelist[i]->d_name);
        char **c_tmp = file_load_multiple(c_in, k, n, in_type, 1);//ignores non allowed extensions

        total+=(*k);
        sum+=(*n);

        c_buffer = (char**) realloc(c_buffer, (total+1)*sizeof(char*));
        memcpy(c_buffer+((total)-(*k)), c_tmp, (*k)*sizeof(char*));

        free(c_tmp);

        if(verbose){
          printf("%s\n", c_in); 
          if(*k==0) printf("## ignored ##\n");
          else{
            printf("N = %zu\n", *n);
            printf("d = %d\n", *k);
          }
        }

#if DEBUG
        printf("%d\t%d\t%d\n", total, (total)-(*k), *k);
#endif

        /**/
        if(tmp==INT_MAX) *k=tmp;
        else{
          *k = tmp-total;
          if(*k<=0) break;
        }
        *n=0;
        /**/

        free(c_in);
      }
      free(namelist[i]);
    }
    free(namelist);

    /**/

    *k = total;
    *n = sum;
  }

  free(c_dir);

  return c_buffer;
}

/*******************************************************************/

char** file_load_multiple(char* c_file, int *k, size_t *n, int in_type, int ignore) {

  /* .ext
   * .txt   - strings per line
   * .fasta - strings separated by '>' line
   * .fastq - strings separated by four lines
   * .gz    - use kseq parser to extract sequences
   */

  char **c_buffer = NULL; // = (char**) malloc(k*sizeof(char*));
  if(*k==0) *k=INT_MAX;

  const char *type = get_filename_ext(c_file);

  if(in_type){

#if GZ
    if(strcmp(type,"gz") == 0){
      c_buffer = load_multiple_gz(c_file, k, n, in_type);
      return c_buffer;
    }
#endif

    if(in_type==1) c_buffer = load_multiple_txt(c_file, k, n);
    else if(in_type==2) c_buffer = load_multiple_fasta(c_file, k, n);
    else if(in_type==3)	c_buffer = load_multiple_fastq(c_file, k, n);
    else{
      exit (EXIT_FAILURE);
    }

  }
  else{

    if(strcmp(type,"txt") == 0)
      c_buffer = load_multiple_txt(c_file, k, n);

    else if(strcmp(type,"fasta") == 0 || strcmp(type,"fa") == 0 || strcmp(type,"fna") == 0)
      c_buffer = load_multiple_fasta(c_file, k, n);

    else if(strcmp(type,"fastq") == 0 || strcmp(type,"fq") == 0)
      c_buffer = load_multiple_fastq(c_file, k, n);

#if GZ
    else if(strcmp(type,"gz") == 0)
      c_buffer = load_multiple_gz(c_file, k, n, in_type);
#endif

    else{
      if(ignore){
        *k=0;
        *n=0;
        return c_buffer;
      }
      printf("%s\n", c_file);
      printf("ERROR: file not recognized (.txt, .fasta, .fa, .fna, .fastq, .fq)\n");
      exit (EXIT_FAILURE);
    }
  }


  return c_buffer;
}

/*******************************************************************/

char** file_load_multiple_qs(char* c_file, int *k, size_t *n, int in_type) {

  /* .ext
   * .fastq - strings separated by four lines
   * .gz    - use kseq parser to extract sequences
   */

  const char *type = get_filename_ext(c_file);

  char **c_buffer = NULL; // = (char**) malloc(k*sizeof(char*));

  if(*k==0) *k=INT_MAX;

  if(in_type){
#if GZ
    if(strcmp(type,"gz") == 0){
      c_buffer = load_multiple_gz_qs(c_file, k, n);
      return c_buffer;
    }
#endif
    if(in_type==3) c_buffer = load_multiple_qs(c_file, k, n);
    else{
      printf("Error: file not recognized (fastq)\n");
      exit (EXIT_FAILURE);
    }
  }
  else{
    if(strcmp(type,"fastq") == 0 || strcmp(type,"fq") == 0)
      c_buffer = load_multiple_qs(c_file, k, n);

#if GZ
    else if(strcmp(type,"gz") == 0)
      c_buffer = load_multiple_gz_qs(c_file, k, n);
#endif

    else{
      printf("Error: file not recognized (.fastq or .fq)\n");
      exit (EXIT_FAILURE);
    }
  }


  return c_buffer;
}

/**********************************************************************/

void mkdir(const char* c_file){

  char c_aux[FILE_NAME];

  strcpy (c_aux,"mkdir -p ");
  strcat (c_aux, c_file);

  if(system(c_aux) == -1)
    perror ("mkdir");

}

/**********************************************************************/

int file_text_write(unsigned char *str, size_t n, char* c_file, const char* ext){

  FILE *f_out;
  char *c_out = (char*) malloc((strlen(c_file)+strlen(ext)+3)*sizeof(char));

  sprintf(c_out, "%s.%s", c_file, ext);
  f_out = file_open(c_out, "wb");

  size_t i;
  for(i=0; i<n;i++) if(str[i]) str[i]--;

  fwrite(str, sizeof(unsigned char), n, f_out);

  file_close(f_out);
  free(c_out);

  return 1;
}

int file_text_int_write(int_t *str, size_t n, char* c_file, const char* ext){

  FILE *f_out;
  char *c_out = (char*) malloc((strlen(c_file)+strlen(ext))*sizeof(char));

  sprintf(c_out, "%s.%s", c_file, ext);
  f_out = file_open(c_out, "wb");

  fwrite(str, sizeof(int_t), n, f_out);

  file_close(f_out);
  free(c_out);

  return 1;
}

/*******************************************************************/

int_t file_text_read(unsigned char** str, char* c_file, const char* ext){

  FILE *f_in;
  char *c_in = (char*) malloc((strlen(c_file)+strlen(ext))*sizeof(char));

  sprintf(c_in, "%s.%s", c_file, ext);
  f_in = file_open(c_in, "rb");

  fseek(f_in, 0L, SEEK_END);
  size_t size = ftell(f_in);
  rewind(f_in);

  int_t n = size/sizeof(unsigned char);

  *str = (unsigned char*) malloc(n*sizeof(unsigned char));
  if(fread(*str, sizeof(unsigned char), n, f_in)==EOF)
    perror("file_text_read");

  file_close(f_in);
  free(c_in);

  return n;
}

int_t file_text_int_read(int_t** str_int, char* c_file, const char* ext){

  FILE *f_in;
  char *c_in = (char*) malloc((strlen(c_file)+strlen(ext))*sizeof(char));

  sprintf(c_in, "%s.%s", c_file, ext);
  f_in = file_open(c_in, "rb");

  fseek(f_in, 0L, SEEK_END);
  size_t size = ftell(f_in);
  rewind(f_in);

  int_t n = size/sizeof(int_t);

  *str_int = (int_t*) malloc(n*sizeof(int_t));
  if(fread(*str_int, sizeof(int_t), n, f_in)==EOF)
    perror("file_text_int_read");

  file_close(f_in);
  free(c_in);

  return n;
}

/*******************************************************************/
