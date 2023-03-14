#ifndef _MUSL_MALLOC_H
#define _MUSL_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOOK_ENABLE
#define MuslMalloc(func) __libc_ ## func
#else
#define MuslMalloc(func) func
#endif

void *__libc_malloc(size_t);
void *__libc_calloc(size_t, size_t);
void *__libc_realloc(void *, size_t);
void __libc_free(void *);
void *__libc_valloc(size_t);
void *__libc_memalign(size_t, size_t);
size_t __libc_malloc_usable_size(void *);

#ifdef __cplusplus
}
#endif

#endif
