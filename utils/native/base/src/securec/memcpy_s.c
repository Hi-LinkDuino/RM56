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
/* [Standardize-exceptions] Use unsafe function: Portability
 * [reason] Use unsafe function to implement security function to maintain platform compatibility.
 *          And sufficient input validation is performed before calling
 */

#include "securecutil.h"

#ifndef SECUREC_MEMCOPY_WITH_PERFORMANCE
#define SECUREC_MEMCOPY_WITH_PERFORMANCE 0
#endif

#if SECUREC_WITH_PERFORMANCE_ADDONS || SECUREC_MEMCOPY_WITH_PERFORMANCE
#ifndef SECUREC_MEMCOPY_THRESHOLD_SIZE
#define SECUREC_MEMCOPY_THRESHOLD_SIZE (64UL)
#endif

#define SECUREC_COPY_STRUCT(num) case num:*(SecStrBuf##num *)dest=*(const SecStrBuf##num *) src;break;

#define SECUREC_SMALL_MEM_COPY do { \
        if (SECUREC_ADDR_ALIGNED_8(dest) && SECUREC_ADDR_ALIGNED_8(src)) { \
            /* use struct assignment */ \
            switch (count) { \
                SECUREC_COPY_STRUCT(1) \
                SECUREC_COPY_STRUCT(2) \
                SECUREC_COPY_STRUCT(3) \
                SECUREC_COPY_STRUCT(4) \
                SECUREC_COPY_STRUCT(5) \
                SECUREC_COPY_STRUCT(6) \
                SECUREC_COPY_STRUCT(7) \
                SECUREC_COPY_STRUCT(8) \
                SECUREC_COPY_STRUCT(9) \
                SECUREC_COPY_STRUCT(10) \
                SECUREC_COPY_STRUCT(11) \
                SECUREC_COPY_STRUCT(12) \
                SECUREC_COPY_STRUCT(13) \
                SECUREC_COPY_STRUCT(14) \
                SECUREC_COPY_STRUCT(15) \
                SECUREC_COPY_STRUCT(16) \
                SECUREC_COPY_STRUCT(17) \
                SECUREC_COPY_STRUCT(18) \
                SECUREC_COPY_STRUCT(19) \
                SECUREC_COPY_STRUCT(20) \
                SECUREC_COPY_STRUCT(21) \
                SECUREC_COPY_STRUCT(22) \
                SECUREC_COPY_STRUCT(23) \
                SECUREC_COPY_STRUCT(24) \
                SECUREC_COPY_STRUCT(25) \
                SECUREC_COPY_STRUCT(26) \
                SECUREC_COPY_STRUCT(27) \
                SECUREC_COPY_STRUCT(28) \
                SECUREC_COPY_STRUCT(29) \
                SECUREC_COPY_STRUCT(30) \
                SECUREC_COPY_STRUCT(31) \
                SECUREC_COPY_STRUCT(32) \
                SECUREC_COPY_STRUCT(33) \
                SECUREC_COPY_STRUCT(34) \
                SECUREC_COPY_STRUCT(35) \
                SECUREC_COPY_STRUCT(36) \
                SECUREC_COPY_STRUCT(37) \
                SECUREC_COPY_STRUCT(38) \
                SECUREC_COPY_STRUCT(39) \
                SECUREC_COPY_STRUCT(40) \
                SECUREC_COPY_STRUCT(41) \
                SECUREC_COPY_STRUCT(42) \
                SECUREC_COPY_STRUCT(43) \
                SECUREC_COPY_STRUCT(44) \
                SECUREC_COPY_STRUCT(45) \
                SECUREC_COPY_STRUCT(46) \
                SECUREC_COPY_STRUCT(47) \
                SECUREC_COPY_STRUCT(48) \
                SECUREC_COPY_STRUCT(49) \
                SECUREC_COPY_STRUCT(50) \
                SECUREC_COPY_STRUCT(51) \
                SECUREC_COPY_STRUCT(52) \
                SECUREC_COPY_STRUCT(53) \
                SECUREC_COPY_STRUCT(54) \
                SECUREC_COPY_STRUCT(55) \
                SECUREC_COPY_STRUCT(56) \
                SECUREC_COPY_STRUCT(57) \
                SECUREC_COPY_STRUCT(58) \
                SECUREC_COPY_STRUCT(59) \
                SECUREC_COPY_STRUCT(60) \
                SECUREC_COPY_STRUCT(61) \
                SECUREC_COPY_STRUCT(62) \
                SECUREC_COPY_STRUCT(63) \
                SECUREC_COPY_STRUCT(64) \
                default:break; \
            } /* END switch */ \
        } else { \
            char *tmpDest = (char *)dest; \
            const char *tmpSrc =  (const char *)src; \
            switch (count) { \
                case 64: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 63: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 62: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 61: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 60: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 59: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 58: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 57: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 56: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 55: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 54: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 53: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 52: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 51: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 50: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 49: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 48: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 47: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 46: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 45: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 44: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 43: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 42: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 41: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 40: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 39: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 38: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 37: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 36: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 35: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 34: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 33: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 32: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 31: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 30: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 29: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 28: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 27: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 26: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 25: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 24: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 23: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 22: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 21: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 20: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 19: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 18: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 17: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 16: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 15: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 14: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 13: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 12: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 11: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 10: *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 9:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 8:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 7:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 6:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 5:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 4:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 3:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 2:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                case 1:  *tmpDest++ = *tmpSrc++; /* fall-through */ /* FALLTHRU */ \
                default:break; \
            } \
        } \
    } SECUREC_WHILE_ZERO
#endif

static errno_t SecMemcpyError(void *dest, size_t destMax, const void *src, size_t count)
{
    if (destMax == 0 || destMax > SECUREC_MEM_MAX_LEN) {
        SECUREC_ERROR_INVALID_RANGE("memcpy_s");
        return ERANGE;
    }
    if (dest == NULL || src == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("memcpy_s");
        if (dest != NULL) {
            (void)memset(dest, 0, destMax);
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    }
    if (count > destMax) {
        (void)memset(dest, 0, destMax);
        SECUREC_ERROR_INVALID_RANGE("memcpy_s");
        return ERANGE_AND_RESET;
    }
    if (dest == src) {
        return EOK;
    }
    if ((dest > src && dest < (const void *)((const unsigned char *)src + count)) ||
        (src > dest && src < (void *)((unsigned char *)dest + count))) {
        (void)memset(dest, 0, destMax);
        SECUREC_ERROR_BUFFER_OVERLAP("memcpy_s");
        return EOVERLAP_AND_RESET;
    }
    /* count == 0 also return EOK */
    return EOK;
}

#if SECUREC_WITH_PERFORMANCE_ADDONS || SECUREC_MEMCOPY_WITH_PERFORMANCE
static void SecDoMemcpyOpt(void *dest, const void *src, size_t count)
{
    if (count > SECUREC_MEMCOPY_THRESHOLD_SIZE) {
/*
 * if SECUREC_USE_ASM macro is enabled, it will call assembly language function to improve performance.
 */
#ifdef SECUREC_USE_ASM
        (void)memcpy_opt(dest, src, count);
#else
        /* large enough, let system API do it */
        (void)memcpy(dest, src, count);
#endif

    } else {
        SECUREC_SMALL_MEM_COPY;
    }
    return;
}
#endif

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The memcpy_s function copies n characters from the object pointed to by src into the object pointed to by dest
 *
 * <INPUT PARAMETERS>
 *    dest                      Destination buffer.
 *    destMax                   Size of the destination buffer.
 *    src                       Buffer to copy from.
 *    count                     Number of characters to copy
 *
 * <OUTPUT PARAMETERS>
 *    dest buffer               is updated.
 *
 * <RETURN VALUE>
 *    EOK                      Success
 *    EINVAL                   dest is  NULL and destMax != 0 and destMax <= SECUREC_MEM_MAX_LEN
 *    EINVAL_AND_RESET         dest != NULL and src is NULL and destMax != 0 and destMax <= SECUREC_MEM_MAX_LEN
 *    ERANGE                   destMax > SECUREC_MEM_MAX_LEN or destMax is 0
 *    ERANGE_AND_RESET         count > destMax and destMax != 0 and destMax <= SECUREC_MEM_MAX_LEN
 *                             and dest  !=  NULL  and src != NULL
 *    EOVERLAP_AND_RESET       dest buffer and source buffer are overlapped and
 *                             count <= destMax destMax != 0 and destMax <= SECUREC_MEM_MAX_LEN and dest  !=  NULL
 *                             and src != NULL  and dest != src
 *
 *    if an error occurred, dest will be filled with 0.
 *    If the source and destination overlap, the behavior of memcpy_s is undefined.
 *    Use memmove_s to handle overlapping regions.
 *******************************************************************************
 */
errno_t memcpy_s(void *dest, size_t destMax, const void *src, size_t count)
{
#if defined(SECUREC_COMPATIBLE_WIN_FORMAT)
    /* fread API in windows will call memcpy_s and pass 0xffffffff to destMax.
     * To avoid the failure of fread, we don't check desMax limit.
     */
    if (SECUREC_LIKELY(count <= destMax && dest != NULL && src != NULL &&
                       count > 0 &&
                       ((dest > src && (void *)((const unsigned char *)src + count) <= dest) ||
                        (src > dest && (void *)((unsigned char *)dest + count) <= src)))) {
#else
    if (SECUREC_LIKELY(count <= destMax && dest != NULL && src != NULL &&
                       destMax <= SECUREC_MEM_MAX_LEN &&
                       count > 0 &&
                       ((dest > src && (const void *)((const unsigned char *)src + count) <= dest) ||
                        (src > dest && (void *)((unsigned char *)dest + count) <= src)))) {
#endif
#if SECUREC_MEMCOPY_WITH_PERFORMANCE
        SecDoMemcpyOpt(dest, src, count);
#else
#ifdef SECUREC_USE_ASM
        (void)memcpy_opt(dest, src, count);
#else
        (void)memcpy(dest, src, count);
#endif
#endif
        return EOK;
    }

    /* meet some runtime violation, return error code */
    return SecMemcpyError(dest, destMax, src, count);

}

#if SECUREC_IN_KERNEL
EXPORT_SYMBOL(memcpy_s);
#endif

#if SECUREC_WITH_PERFORMANCE_ADDONS

errno_t memcpy_sOptAsm(void *dest, size_t destMax, const void *src, size_t count)
{
    if (SECUREC_LIKELY(count <= destMax && dest != NULL && src != NULL &&
                       destMax <= SECUREC_MEM_MAX_LEN &&
                       count > 0 &&
                       ((dest > src && (const void *)((const unsigned char *)src + count) <= dest) ||
                        (src > dest && (const void *)((const unsigned char *)dest + count) <= src)))) {
        SecDoMemcpyOpt(dest, src, count);
        return EOK;
    }
    /* meet some runtime violation, return error code */
    return SecMemcpyError(dest, destMax, src, count);
}

/* trim judgement on "destMax <= SECUREC_MEM_MAX_LEN" */
errno_t memcpy_sOptTc(void *dest, size_t destMax, const void *src, size_t count)
{
    if (SECUREC_LIKELY(count <= destMax && dest != NULL && src != NULL &&
                       count > 0 &&
                       ((dest > src && (const void *)((const unsigned char *)src + count) <= dest) ||
                        (src > dest && (void *)((unsigned char *)dest + count) <= src)))) {
        SecDoMemcpyOpt(dest, src, count);
        return EOK;
    }
    /* meet some runtime violation, return error code */
    return SecMemcpyError(dest, destMax, src, count);
}
#endif

