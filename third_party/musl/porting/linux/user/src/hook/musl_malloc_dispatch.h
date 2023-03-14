#ifndef _MUSL_MALLOC_DISPATCH_H
#define _MUSL_MALLOC_DISPATCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void* (*MallocMallocType)(size_t);
typedef void* (*MallocReallocType)(void*, size_t);
typedef void* (*MallocCallocType)(size_t, size_t);
typedef void* (*MallocVallocType)(size_t);
typedef void (*MallocFreeType)(void*);
typedef void* (*MallocMemalignType)(size_t, size_t);
typedef size_t (*MallocMallocUsableSizeType)(void*);
typedef bool (*GetHookFlagType)();
typedef bool (*SetHookFlagType)(bool);

struct MallocDispatchType {
	MallocMallocType malloc;
	MallocCallocType calloc;
	MallocReallocType realloc;
	MallocVallocType valloc;
	MallocFreeType free;
	MallocMemalignType memalign;
	MallocMallocUsableSizeType malloc_usable_size;
	GetHookFlagType get_hook_flag;
	SetHookFlagType set_hook_flag;
};
#ifdef __cplusplus
}
#endif

#endif
