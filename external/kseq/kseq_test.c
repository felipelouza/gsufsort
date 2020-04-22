#include <zlib.h>
#include <stdio.h>
#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

int main(int argc, char *argv[]){

	gzFile fp;
	kseq_t *seq;
  int l;
  if (argc == 1) {
    fprintf(stderr, "Usage: %s <in.seq>\n", argv[0]);
    return 1;
  }

  fp = gzopen(argv[1], "r"); // STEP 2: open the file handler
  seq = kseq_init(fp); // STEP 3: initialize seq

  while ((l = kseq_read(seq)) >= 0) { // STEP 4: read sequence  
    printf("name: %s\n", seq->name.s);  
    if (seq->comment.l) printf("comment: %s\n", seq->comment.s);  
    printf("seq: %s\n", seq->seq.s);  
    if (seq->qual.l) printf("qual: %s\n", seq->qual.s);  
  }  

  /*
	while (kseq_read(seq) >= 0)
		++n, slen += seq->seq.l, qlen += seq->qual.l;
	printf("%d\t%d\t%d\n", n, slen, qlen);
  */
	kseq_destroy(seq);
	gzclose(fp);
	return 0;
}
