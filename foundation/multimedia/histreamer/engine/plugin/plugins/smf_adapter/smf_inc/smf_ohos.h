#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

typedef void* (*CbFOpen)(const char* __filename, const char* __modes);
typedef int (*CbFClose)(void* __stream);
typedef size_t(*CbFRead)(void* __ptr, size_t __size, size_t __n, void* __stream);
typedef size_t(*CbFWrite)(const void* ptr, size_t size, size_t nmemb, void* __stream);
typedef int (*CbFSeek)(void* __stream, long int __off, int __whence);
typedef long (*CbFTell)(void* __stream);
typedef long (*CbFLength)(void* __stream);
typedef struct {
	CbFOpen open;
	CbFClose close;
	CbFRead read;
	CbFWrite write;
	CbFSeek seek;
	CbFTell tell;
	CbFLength getsize;
}ohos_file_ops;

EXTERNC void* smf_ohos_create_io(void *ctx);

EXTERNC void smf_ohos_fs_callback_register(const ohos_file_ops &ops);