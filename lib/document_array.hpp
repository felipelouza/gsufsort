#ifndef DAH
#define DAH

#ifdef __cplusplus

#include <cstring>
#include <cstdio>
#include <climits>
#include <iostream>

#include <sdsl/bit_vectors.hpp>
#include <sdsl/sd_vector.hpp>

using namespace std;
using namespace sdsl;

#else

#include <string.h>
#include <stdio.h>
#include <limits.h>

#endif

#include "utils.h"


//int document_array(unsigned char*, int*, int*, int);
#ifdef __cplusplus
extern "C" { /* for inclusion from C++ */
#endif

  int document_array_wrapper(unsigned char *str, uint_t *SA, int_da *DA, int_t n, int_t d);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
