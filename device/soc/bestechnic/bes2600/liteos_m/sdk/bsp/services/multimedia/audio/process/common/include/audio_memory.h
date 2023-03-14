#ifndef AUDIO_MEMORY_H
#define AUDIO_MEMORY_H

#include "string.h"
#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VQE_SIMULATE

#include "heap_api.h"

#if defined(AUDIO_PROCESS_IN_CP) && !defined(__NuttX__)
#include "cp_server.h"
#include "hal_location.h"
#endif

#if !defined(AUDIO_PROCESS_IN_CP) || defined(__NuttX__)
extern heap_handle_t g_audio_heap;
#endif

__STATIC_FORCEINLINE void audio_heap_init(void *begin_addr, size_t size)
{
#if defined(AUDIO_PROCESS_IN_CP) && !defined(__NuttX__)
    return;
#else
    memset(begin_addr, 0, size);
    g_audio_heap = heap_register(begin_addr, size);
#endif
}

__STATIC_FORCEINLINE void *audio_malloc(size_t size)
{
#if defined(AUDIO_PROCESS_IN_CP) && !defined(__NuttX__)
    return cp_heap_malloc(size);
#else
    return heap_malloc(g_audio_heap, size);
#endif
}

__STATIC_FORCEINLINE void audio_free(void *p)
{
#if defined(AUDIO_PROCESS_IN_CP) && !defined(__NuttX__)
    cp_heap_free(p);
#else
    heap_free(g_audio_heap, p);
#endif
}

__STATIC_FORCEINLINE void *audio_calloc(size_t nmemb, size_t size)
{
#if defined(AUDIO_PROCESS_IN_CP) && !defined(__NuttX__)
    return cp_heap_calloc(nmemb, size);
#else
    void *ptr = audio_malloc(nmemb * size);
    if (ptr != NULL)
        memset(ptr, 0, nmemb * size);
    return ptr;
#endif
}

__STATIC_FORCEINLINE void *audio_realloc(void *ptr, size_t size)
{
#if defined(AUDIO_PROCESS_IN_CP) && !defined(__NuttX__)
    return cp_heap_realloc(ptr, size);
#else
    return heap_realloc(g_audio_heap, ptr, size);
#endif
}

__STATIC_FORCEINLINE void audio_memory_info(size_t *total,
                    size_t *used,
                    size_t *max_used)
{
#if defined(AUDIO_PROCESS_IN_CP) && !defined(__NuttX__)
    cp_heap_memory_info(total, used, max_used);
#else
    heap_memory_info(g_audio_heap,total,used,max_used);
#endif
}
#else
#include <stddef.h>
#include <stdlib.h>

void audio_heap_init(void *begin_addr, size_t size);
void *audio_malloc(size_t size);
void audio_free(void *p);
void *audio_calloc(size_t nmemb, size_t size);
void *audio_realloc(void *ptr, size_t size);
void audio_memory_info(size_t *total,
                    size_t *used,
                    size_t *max_used);
#endif

#ifdef __cplusplus
}
#endif

#endif
