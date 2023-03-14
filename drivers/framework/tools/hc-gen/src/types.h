/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_TYPES_H
#define HC_GEN_TYPES_H

#define ALIGN_SIZE        4
#define TAB               "    "
#define TAB_SIZE          4
#define OPCODE_BYTE_WIDTH 1

#define BYTE_SIZE  1
#define WORD_SIZE  2
#define DWORD_SIZE 4
#define QWORD_SIZE 8

#define UNIX_SEPARATOR '/'
#define WIN_SEPARATOR  '\\'

#ifdef OS_WIN
#define OS_SEPARATOR WIN_SEPARATOR
#else
#define OS_SEPARATOR UNIX_SEPARATOR
#endif

enum HcsErrorNo {
    NOERR = 0,     /* No error */
    EFAIL,         /* Process fail */
    EOOM,          /* Out of memory */
    EOPTION,       /* Option error */
    EREOPENF,      /* Reopen argument */
    EINVALF,       /* Invalid file */
    EINVALARG,     /* Invalid argument */
    EDECOMP,       /* Decompile error */
    EOUTPUT,       /* Output error */
    EASTWALKBREAK, /* Break ast walk */
};

#endif // HC_GEN_TYPES_H
