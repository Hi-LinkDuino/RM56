#ifndef STRING_S_H
#define STRING_S_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"
#include "plat_types.h"
#include "libc_rom_s_errno.h"

extern char *  strstr_s( const char *s,size_t s_l, const char *find,size_t find_l);
extern  char*  strsep_s(char **stringp,size_t stringp_l,char * delim,size_t delim_l);
extern char*  strtok_s(char *s,size_t s_l,const char *delim,size_t delim_l);
extern char*  strtok_r_s(char *s,size_t s_l, const char *delim,size_t delim_l ,char **lasts,size_t lasts_l);
extern char*  strncat_s(char *s1,size_t s1_l, const char *s2,size_t n);
extern int  strnicmp_s(const char *s1,size_t s1_l, const char *s2,size_t n);
extern int strcpy_s (char *dest, size_t destMax,const char *src);
extern char *strncpy_s (char *dest,size_t dstMax,const char *src,size_t n);
extern long  strtol_s( const char *nptr, size_t nptr_l,char **endptr, size_t endptr_l,int base);
extern unsigned long  strtoul_s( const char *nptr, size_t nptr_l,char **endptr, size_t endptr_l,int base);
extern int atoi_s(const char * s, size_t s_l);
extern size_t  qsort_s(void *baseP, size_t nElem, size_t width, int (*compar)(const void *, const void *));
extern void*  bsearch_s(const void *key, const void *base, size_t nmemb, size_t size,int (*compar)(const void *, const void *));  
extern char* strcat_s (char *dest,size_t dstMax, const char *src);
extern size_t  memcpy_s(void *dst,size_t dstMax,const void *src, size_t srcMax);
extern size_t  memmove_s(void *dst,size_t dstMax , const void *src, size_t srcMax);
extern size_t  memset_s(void *,size_t,int,size_t);
extern void * memset_s_f(void *,int,size_t);


#ifdef __cplusplus
}
#endif


#endif /* STRING_S_H */

