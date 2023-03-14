/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <unistd.h>
#include "hcs_file.h"
#include "hcs_compiler.h"
#include "hcs_log.h"

#define ARG_COUNT_MIN 2
#define USAGE(option, info) HCS_PRINT("  %-12s%s\n", option, info)

static bool g_genTextConfigOutput = false;
static bool g_genByteCodeConfigOutput = true;
static bool g_genByteCodeHexdump = false;
static bool g_verbosePrint = false;
static bool g_decompile = false;
static bool g_shouldAlign = false;
static const char *g_symbolPrefix = NULL;

bool HcsOptShouldAlign(void)
{
    return g_shouldAlign;
}

void HcsOptSetAlign(bool align)
{
    g_shouldAlign = align;
}

bool HcsOptShouldGenTextConfig(void)
{
    return g_genTextConfigOutput;
}

bool HcsOptShouldGenByteCodeConfig(void)
{
    return g_genByteCodeConfigOutput;
}

bool HcsOptDecompile(void)
{
    return g_decompile;
}

const char *HcsOptGetSymbolNamePrefix(void)
{
    return g_symbolPrefix;
}

bool HcsOptShouldGenHexdump(void)
{
    return g_genByteCodeHexdump;
}

void ShowUsage()
{
    HCS_PRINT("Usage: hc-gen [Options] [File]\n");
    HCS_PRINT("options:\n");
    USAGE("-o <file>", "output file name, default same as input");
    USAGE("-a", "hcb align with four bytes");
    USAGE("-b", "output binary output, default enable");
    USAGE("-t", "output config in C language source file style");
    USAGE("-i", "output binary hex dump in C language source file style");
    USAGE("-p <prefix>", "prefix of generated symbol name");
    USAGE("-d", "decompile hcb to hcs");
    USAGE("-V", "show verbose info");
    USAGE("-v", "show version");
    USAGE("-h", "show this help message");
}

#define HCS_SUPPORT_ARGS "o:ap:bditvVh"

void ShowVersionInfo()
{
    HCS_PRINT("Hcs compiler v%u.%u\n", HCS_COMPILER_VERSION_MAJOR, HCS_COMPILER_VERSION_MINOR);
    HCS_PRINT("Copyright (c) 2020-2021 Huawei Device Co., Ltd.\n");
}

static int32_t ProcessOption(int32_t argc, char *argv[])
{
    int32_t op = 0;
    while (op != HCS_OPTION_END) {
        op = getopt(argc, argv, HCS_SUPPORT_ARGS);
        switch (op) {
            case 'o':
                if (HcsSetOutPutName(optarg)) {
                    return EOPTION;
                }
                break;
            case 'a':
                g_shouldAlign = true;
                break;
            case 'b':
                g_genByteCodeConfigOutput = true;
                break;
            case 't':
                g_genTextConfigOutput = true;
                g_genByteCodeConfigOutput = false;
                break;
            case 'p':
                g_symbolPrefix = optarg;
                break;
            case 'i':
                g_genByteCodeHexdump = true;
                break;
            case 'V':
                g_verbosePrint = true;
                break;
            case 'd':
                g_decompile = true;
                break;
            case 'v':
                ShowVersionInfo();
                return EFAIL;
            case 'h': /* fall-through */
            case '?':
                ShowUsage();
                return EOPTION;
            default:
                break;
        }
    }

    return NOERR;
}

int32_t DoOption(int32_t argc, char *argv[])
{
    if (argc < ARG_COUNT_MIN) {
        ShowUsage();
        return EOPTION;
    }

    int32_t ret = ProcessOption(argc, argv);
    if (ret) {
        return ret;
    }

    if (optind >= argc) {
        HCS_ERROR("Miss input file name");
        return EOPTION;
    } else {
        const char *inputFIlePath = argv[optind];
        HCS_DEBUG("opt info:input file name:%s\n", inputFIlePath);
        HcsSetInputFileName(inputFIlePath);
    }
    return 0;
}

bool HcsVerbosePrint(void)
{
    return g_verbosePrint;
}
