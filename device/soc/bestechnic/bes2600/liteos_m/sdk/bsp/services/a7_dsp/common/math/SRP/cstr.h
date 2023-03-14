/*
	Portions Copyright (C) 2007-2014 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef _CSTR_H_
#define _CSTR_H_

/* A general-purpose string "class" for C */

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Arguments to allocator methods ordered this way for compatibility */
typedef struct cstr_alloc_st {
  // APPLE MODIFICATION: Use the size_t variant for all platforms.
  void * (_CDECL * alloc)(size_t n, void * heap);
  void (_CDECL * free)(void * p, void * heap);
  void * heap;
} cstr_allocator;

typedef struct cstr_st {
  char * data;	/* Okay to access data and length fields directly */
  int length;
  int cap;
  int ref;	/* Simple reference counter */
  cstr_allocator * allocator;
} cstr;

_TYPE( void ) cstr_set_allocator P((cstr_allocator * alloc));

_TYPE( cstr * ) cstr_new P((void));
_TYPE( cstr * ) cstr_new_alloc P((cstr_allocator * alloc));
_TYPE( cstr * ) cstr_dup P((const cstr * str));
_TYPE( cstr * ) cstr_dup_alloc P((const cstr * str, cstr_allocator * alloc));
_TYPE( cstr * ) cstr_create P((const char * s));
_TYPE( cstr * ) cstr_createn P((const char * s, int len));

_TYPE( void ) cstr_detach(cstr * str, uint8_t ** outPtr, size_t * outLen);
_TYPE( void ) cstr_free P((cstr * str));
_TYPE( void ) cstr_clear_free P((cstr * str));
_TYPE( void ) cstr_use P((cstr * str));
_TYPE( void ) cstr_empty P((cstr * str));
_TYPE( int ) cstr_copy P((cstr * dst, const cstr * src));
_TYPE( int ) cstr_set P((cstr * str, const char * s));
_TYPE( int ) cstr_setn P((cstr * str, const char * s, int len));
_TYPE( int ) cstr_set_length P((cstr * str, int len));
_TYPE( int ) cstr_append P((cstr * str, const char * s));
_TYPE( int ) cstr_appendn P((cstr * str, const char * s, int len));
_TYPE( int ) cstr_append_str P((cstr * dst, const cstr * src));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CSTR_H_ */
