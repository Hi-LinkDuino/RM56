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

#ifndef __SEC_INPUT_H_E950DA2C_902F_4B15_BECD_948E99090D9C
#define __SEC_INPUT_H_E950DA2C_902F_4B15_BECD_948E99090D9C
#include "securecutil.h"

#define SECUREC_SCANF_EINVAL (-1)
#define SECUREC_SCANF_ERROR_PARA (-2)

/* for internal stream flag */
#define SECUREC_MEM_STR_FLAG 0X01
#define SECUREC_FILE_STREAM_FLAG 0X02
#define SECUREC_FROM_STDIN_FLAG 0X04
#define SECUREC_LOAD_FILE_TO_MEM_FLAG 0X08

#define SECUREC_UNINITIALIZED_FILE_POS (-1)
#define SECUREC_BOM_HEADER_SIZE (2)
#define SECUREC_UTF8_BOM_HEADER_SIZE (3)

typedef struct {
    int count;                  /* the size of buffered string in bytes */
    const char *cur;            /* the pointer to next read position */
    char *base;                 /* the pointer to the header of buffered string */
    unsigned int flag;          /* mark the properties of input stream */
    FILE *pf;                   /* the file pointer */
    int fileRealRead;
    long oriFilePos;            /* the original position of file offset when fscanf is called */
#if defined(SECUREC_NO_STD_UNGETC)
    unsigned int lastChar;      /* the char code of last input */
    int fUnget;                 /* the boolean flag of pushing a char back to read stream */
#endif
} SecFileStream;

#if defined(SECUREC_NO_STD_UNGETC)
#define SECUREC_INIT_SEC_FILE_STREAM {0, NULL, NULL, 0, NULL, 0, 0, 0, 0}
#else
#define SECUREC_INIT_SEC_FILE_STREAM {0, NULL, NULL, 0, NULL, 0, 0}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    extern int SecInputS(SecFileStream *stream, const char *format, va_list arglist);
    extern int SecInputSW(SecFileStream *stream, const wchar_t *format, va_list arglist);
    extern void SecClearDestBuf(const char *buffer, const char *cformat, va_list arglist);
    extern void SecClearDestBufW(const wchar_t *buffer, const wchar_t *cformat, va_list arglist);

/* 20150105 For software and hardware decoupling,such as UMG */
#ifdef SECUREC_SYSAPI4VXWORKS
#ifdef feof
#undef feof
#endif
    extern int feof(FILE *stream);

#ifndef isspace
#define isspace(c)      (((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n'))
#endif
#ifndef isascii
#define isascii(c)       (((unsigned char)(c))<=0x7f)
#endif
#ifndef isupper
#define isupper(c)       ((c) >= 'A' && (c) <= 'Z')
#endif
#ifndef islower
#define islower(c)       ((c) >= 'a' && (c) <= 'z')
#endif
#ifndef isalpha
#define isalpha(c)       (isupper(c) || (islower(c)))
#endif
#ifndef isdigit
#define isdigit(c)       ((c) >= '0' && (c) <= '9')
#endif
#ifndef isxdigit
#define isxupper(c)      ((c) >= 'A' && (c) <= 'F')
#define isxlower(c)      ((c) >= 'a' && (c) <= 'f')
#define isxdigit(c)      (isdigit(c) || isxupper(c) ||isxlower(c))
#endif
#endif

#ifdef __cplusplus
}
#endif
/* Reserved file operation macro interface */
#define SECUREC_LOCK_FILE(s)
#define SECUREC_UNLOCK_FILE(s)
#define SECUREC_LOCK_STDIN(i, s)
#define SECUREC_UNLOCK_STDIN(i, s)
#endif


