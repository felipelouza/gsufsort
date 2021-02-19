#include "document_array.hpp"

#ifndef SDV
#define SDV   0
#endif

int document_array(unsigned char *str, uint_t *SA, int_da *DA, int_t n, int_t d){

  int_t ones=0;//counts the number of separators

#if SDV
  printf("sparse bitvector:\n");
  sd_vector_builder builder(n,d);
#else
  bit_vector b = bit_vector(n, 0);
#endif

  //1. computes B[i]
  for(size_t i=0; i < (size_t) n; i++){
    if(str[i]==1){
      ones++;
#if SDV
      builder.set(i);
#else
      b[i] = 1; //separator==1
#endif
    }
  }

  //2. free T
  //free(str);

  //3. compute rank_structure
#if SDV
  sd_vector<> sdv(builder);
  rank_support_sd<1> b_rank(&sdv);
#else
  rank_support_v<1> b_rank(&b);
#endif

  //4. compute DA
  for(int_t i=0; i < n; i++)
    DA[i] = b_rank(SA[i]);//TODO: stream directly to disk


  return 1;
}


int document_array_wrapper(unsigned char *str, uint_t *SA, int_da *DA, int_t n, int_t d){

  document_array(str, SA, DA, n, d);

  return 0;
}
