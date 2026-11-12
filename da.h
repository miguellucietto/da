#ifndef _DA__H_
#define _DA__H_


#ifndef DA_ALLOC
#define DA_ALLOC malloc
#endif

#ifndef DA_REALLOC
#define DA_REALLOC realloc
#endif

#ifndef DA_FREE
#define DA_FREE free
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

typedef struct DA_Header {
  size_t len;
  size_t cap;
} DA_Header;

// Array Declaration: any *arr = NULL;
#define da_header(a) ((DA_Header*)(a) - 1)

#define da_len(a) ((a) ? (da_header(a)->len) : 0)
#define da_cap(a) ((a) ? (da_header(a)->cap) : 0)

#define da_grow(a) (da_grow_impl((void**)&(a), sizeof(*(a)), da_cap(a) * 2))
#define da_maybegrow(a) (da_maybegrow_impl((void**)&(a), sizeof(*(a))))

#define da_clear(a) ((a) ? (da_header(a)->len = 0, true) : false)
#define da_free(a) ((a) ? (DA_FREE(da_header(a)), (a) = NULL, true) : false)

#define da_swap(a, n1, n2) \
do { \
    __typeof__((a)[0]) tmp = (a)[n1]; \
    (a)[n1] = (a)[n2]; \
    (a)[n2] = tmp; \
} while (0)


#define da_append(a, value)    (da_maybegrow(a) ? ((a)[da_header(a)->len++] = (value), true) : false)
#define da_insert(a, value, n)                                                 \
  (da_maybegrow(a) && (size_t)(n) <= da_len(a)                                 \
       ? (memmove(&(a)[(n) + 1], &(a)[n], sizeof(*(a)) * (da_len(a) - (n))),   \
          (a)[n] = (value), da_header(a)->len++, true) : false)

#define da_pop(a, n)                                                           \
  do {                                                                         \
    if (!(a) || da_len(a) == 0) {                                              \
      break;                                                                   \
    }                                                                          \
    if ((int)(n) == -1) {                                                      \
      da_header(a)->len--;                                                     \
    } else if ((int)(n) >= 0 && (size_t)(n) < da_len(a)) {                 \
      memmove(&(a)[n], &(a)[(n) + 1], sizeof(*(a)) * (da_len(a) - (n) - 1));   \
      da_header(a)->len--;                                                     \
    } else {                                                                   \
      assert(0 && "da_pop: invalid index");                                    \
    }                                                                          \
  } while (0)

#define da_sort(a, fn) \
do { \
    if (a) \
        qsort((a), da_len(a), sizeof(*(a)), (fn)); \
} while (0)

bool da_grow_impl(void **arr, size_t esize, size_t ncap);
bool da_maybegrow_impl(void **arr, size_t esize);

#endif

#ifdef DA_IMPLEMENTATION

bool da_grow_impl(void **arr, size_t esize, size_t ncap) {
  DA_Header *h = *arr ? (DA_Header *)*arr - 1 : NULL;
  
  if (ncap > (SIZE_MAX - sizeof(DA_Header)) / esize) {
    return false;
  }

  size_t ts = ncap * esize + sizeof(DA_Header);
  DA_Header *nh = (DA_Header*)DA_REALLOC(h, ts);
  if (!nh) return false;

  if (!h)
    nh->len = 0;
  
  nh->cap = ncap;
  *arr = nh + 1;

  return true;
}

bool da_maybegrow_impl(void **arr, size_t esize) {
    DA_Header *h = *arr ? (DA_Header *)*arr - 1 : NULL;

    if (!h || h->len >= h->cap) {
        size_t new_cap = h ? h->cap * 2 : 8;

        if (new_cap < 8)
            new_cap = 8;

        return da_grow_impl(arr, esize, new_cap);
    }

    return true;
}


#endif
