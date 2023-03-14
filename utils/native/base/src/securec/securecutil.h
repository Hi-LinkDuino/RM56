/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SECURECUTIL_H__46C86578_F8FF_4E49_8E64_9B175241761F
#define __SECURECUTIL_H__46C86578_F8FF_4E49_8E64_9B175241761F
#include "securec.h"

#ifndef SECUREC_HAVE_STRNLEN
#if (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 700) || (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L)
#if SECUREC_IN_KERNEL
#define SECUREC_HAVE_STRNLEN 0
#else
#if defined(__GLIBC__) && __GLIBC__ >= 2 && defined(__GLIBC_MINOR__) && __GLIBC_MINOR__ >= 10
#define SECUREC_HAVE_STRNLEN 1
#else
#define SECUREC_HAVE_STRNLEN 0
#endif
#endif
#else
#define SECUREC_HAVE_STRNLEN 0
#endif
#endif

#if SECUREC_IN_KERNEL == 0

#if (defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)) || defined(__ARMCC_VERSION)
typedef __int64 SecInt64;
typedef unsigned __int64 SecUnsignedInt64;
#if defined(__ARMCC_VERSION)
typedef int SecInt32;
typedef unsigned int SecUnsignedInt32;
#else
typedef __int32 SecInt32;
typedef unsigned __int32 SecUnsignedInt32;
#endif
#else
typedef int SecInt32;
typedef unsigned int SecUnsignedInt32;
typedef long long SecInt64;
typedef unsigned long long SecUnsignedInt64;
#endif

/* struct for performance */
typedef struct {
    unsigned char buf[1];
} SecStrBuf1;
typedef struct {
    unsigned char buf[2];
} SecStrBuf2;
typedef struct {
    unsigned char buf[3];
} SecStrBuf3;
typedef struct {
    unsigned char buf[4];
} SecStrBuf4;
typedef struct {
    unsigned char buf[5];
} SecStrBuf5;
typedef struct {
    unsigned char buf[6];
} SecStrBuf6;
typedef struct {
    unsigned char buf[7];
} SecStrBuf7;
typedef struct {
    unsigned char buf[8];
} SecStrBuf8;
typedef struct {
    unsigned char buf[9];
} SecStrBuf9;
typedef struct {
    unsigned char buf[10];
} SecStrBuf10;
typedef struct {
    unsigned char buf[11];
} SecStrBuf11;
typedef struct {
    unsigned char buf[12];
} SecStrBuf12;
typedef struct {
    unsigned char buf[13];
} SecStrBuf13;
typedef struct {
    unsigned char buf[14];
} SecStrBuf14;
typedef struct {
    unsigned char buf[15];
} SecStrBuf15;
typedef struct {
    unsigned char buf[16];
} SecStrBuf16;
typedef struct {
    unsigned char buf[17];
} SecStrBuf17;
typedef struct {
    unsigned char buf[18];
} SecStrBuf18;
typedef struct {
    unsigned char buf[19];
} SecStrBuf19;
typedef struct {
    unsigned char buf[20];
} SecStrBuf20;
typedef struct {
    unsigned char buf[21];
} SecStrBuf21;
typedef struct {
    unsigned char buf[22];
} SecStrBuf22;
typedef struct {
    unsigned char buf[23];
} SecStrBuf23;
typedef struct {
    unsigned char buf[24];
} SecStrBuf24;
typedef struct {
    unsigned char buf[25];
} SecStrBuf25;
typedef struct {
    unsigned char buf[26];
} SecStrBuf26;
typedef struct {
    unsigned char buf[27];
} SecStrBuf27;
typedef struct {
    unsigned char buf[28];
} SecStrBuf28;
typedef struct {
    unsigned char buf[29];
} SecStrBuf29;
typedef struct {
    unsigned char buf[30];
} SecStrBuf30;
typedef struct {
    unsigned char buf[31];
} SecStrBuf31;
typedef struct {
    unsigned char buf[32];
} SecStrBuf32;
typedef struct {
    unsigned char buf[33];
} SecStrBuf33;
typedef struct {
    unsigned char buf[34];
} SecStrBuf34;
typedef struct {
    unsigned char buf[35];
} SecStrBuf35;
typedef struct {
    unsigned char buf[36];
} SecStrBuf36;
typedef struct {
    unsigned char buf[37];
} SecStrBuf37;
typedef struct {
    unsigned char buf[38];
} SecStrBuf38;
typedef struct {
    unsigned char buf[39];
} SecStrBuf39;
typedef struct {
    unsigned char buf[40];
} SecStrBuf40;
typedef struct {
    unsigned char buf[41];
} SecStrBuf41;
typedef struct {
    unsigned char buf[42];
} SecStrBuf42;
typedef struct {
    unsigned char buf[43];
} SecStrBuf43;
typedef struct {
    unsigned char buf[44];
} SecStrBuf44;
typedef struct {
    unsigned char buf[45];
} SecStrBuf45;
typedef struct {
    unsigned char buf[46];
} SecStrBuf46;
typedef struct {
    unsigned char buf[47];
} SecStrBuf47;
typedef struct {
    unsigned char buf[48];
} SecStrBuf48;
typedef struct {
    unsigned char buf[49];
} SecStrBuf49;
typedef struct {
    unsigned char buf[50];
} SecStrBuf50;
typedef struct {
    unsigned char buf[51];
} SecStrBuf51;
typedef struct {
    unsigned char buf[52];
} SecStrBuf52;
typedef struct {
    unsigned char buf[53];
} SecStrBuf53;
typedef struct {
    unsigned char buf[54];
} SecStrBuf54;
typedef struct {
    unsigned char buf[55];
} SecStrBuf55;
typedef struct {
    unsigned char buf[56];
} SecStrBuf56;
typedef struct {
    unsigned char buf[57];
} SecStrBuf57;
typedef struct {
    unsigned char buf[58];
} SecStrBuf58;
typedef struct {
    unsigned char buf[59];
} SecStrBuf59;
typedef struct {
    unsigned char buf[60];
} SecStrBuf60;
typedef struct {
    unsigned char buf[61];
} SecStrBuf61;
typedef struct {
    unsigned char buf[62];
} SecStrBuf62;
typedef struct {
    unsigned char buf[63];
} SecStrBuf63;
typedef struct {
    unsigned char buf[64];
} SecStrBuf64;



#if (defined(_MSC_VER)) && (_MSC_VER >= 1400)
#define SECUREC_MASK_MSVC_CRT_WARNING __pragma(warning(push)) \
    __pragma(warning(disable:4996 4127))
#define SECUREC_END_MASK_MSVC_CRT_WARNING  __pragma(warning(pop))
#else
#define SECUREC_MASK_MSVC_CRT_WARNING
#define SECUREC_END_MASK_MSVC_CRT_WARNING
#endif

#define SECUREC_WHILE_ZERO SECUREC_MASK_MSVC_CRT_WARNING while (0) SECUREC_END_MASK_MSVC_CRT_WARNING

#define SECUREC_PRINTF_TRUNCATE (-2)
typedef struct {
    int count;
    char *cur;
} SecPrintfStream;

#endif

#define SECUREC_ADDR_ALIGNED_8(addr) (!((size_t)(addr) & 7))
#define SECUREC_IN_REGISTER register

/* User can change the error handler by modify the following definition,
 * such as logging the detail error in file.
 */
#if defined(_DEBUG) || defined(DEBUG)
#if defined(SECUREC_ERROR_HANDLER_BY_ASSERT)
#define SECUREC_ERROR_INVALID_PARAMTER(msg) assert( msg "invalid argument" == NULL)
#define SECUREC_ERROR_INVALID_RANGE(msg)    assert( msg "invalid dest buffer size" == NULL)
#define SECUREC_ERROR_BUFFER_OVERLAP(msg)   assert( msg "buffer overlap" == NULL)

#elif defined(SECUREC_ERROR_HANDLER_BY_PRINTF)
#if SECUREC_IN_KERNEL
#define SECUREC_ERROR_INVALID_PARAMTER(msg) printk( "%s invalid argument\n", msg)
#define SECUREC_ERROR_INVALID_RANGE(msg)    printk( "%s invalid dest buffer size\n", msg)
#define SECUREC_ERROR_BUFFER_OVERLAP(msg)   printk( "%s buffer overlap\n", msg)
#else
#define SECUREC_ERROR_INVALID_PARAMTER(msg) printf( "%s invalid argument\n", msg)
#define SECUREC_ERROR_INVALID_RANGE(msg)    printf( "%s invalid dest buffer size\n", msg)
#define SECUREC_ERROR_BUFFER_OVERLAP(msg)   printf( "%s buffer overlap\n", msg)
#endif
#elif defined(SECUREC_ERROR_HANDLER_BY_FILE_LOG)

#define SECUREC_ERROR_INVALID_PARAMTER(msg) LogSecureCRuntimeError(msg " EINVAL\n")
#define SECUREC_ERROR_INVALID_RANGE(msg)    LogSecureCRuntimeError(msg " ERANGE\n")
#define SECUREC_ERROR_BUFFER_OVERLAP(msg)   LogSecureCRuntimeError(msg " EOVERLAP\n")
#else

#define SECUREC_ERROR_INVALID_PARAMTER(msg) ((void)0)
#define SECUREC_ERROR_INVALID_RANGE(msg)    ((void)0)
#define SECUREC_ERROR_BUFFER_OVERLAP(msg)   ((void)0)
#endif

#else
#define SECUREC_ERROR_INVALID_PARAMTER(msg) ((void)0)
#define SECUREC_ERROR_INVALID_RANGE(msg)    ((void)0)
#define SECUREC_ERROR_BUFFER_OVERLAP(msg)   ((void)0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SECUREC_IN_KERNEL == 0
    extern int SecVsnprintfImpl(char *string, size_t count, const char *format, va_list arglist);
    extern int SecPutWcharStrEndingZero(SecPrintfStream *str, int zerosNum);
    extern void SecWriteMultiChar(char ch, int num, SecPrintfStream *f, int *pnumwritten);
    extern void SecWriteString(const char *string, int len, SecPrintfStream *f, int *pnumwritten);
#endif

/* assembly language memcpy memset for X86 or MIPS ... */
#ifdef SECUREC_USE_ASM
    extern void *memcpy_opt(void *, const void *, size_t);
    extern void *memset_opt(void *, int, size_t);
#endif

#if defined(SECUREC_VXWORKS_VERSION_5_4)
    extern int SecU64Div32(SecUnsignedInt32 dividendHigh, SecUnsignedInt32 dividendLow, SecUnsignedInt32 divisor,
                SecUnsignedInt32 *pQuotientHigh, SecUnsignedInt32 *pQuotientLow, SecUnsignedInt32 *puiRemainder);

#endif

#if defined(SECUREC_ERROR_HANDLER_BY_FILE_LOG)
    extern void LogSecureCRuntimeError(const char *errDetail);
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif


