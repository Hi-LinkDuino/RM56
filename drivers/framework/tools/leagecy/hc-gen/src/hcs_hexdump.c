/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include <hcs_file.h>
#include <hcs_log.h>
#include <hcs_option.h>

#define HCS_HEXDUMP_ENTRY_SYMBOL "hdfConfigEntrySymbol"
#define HCS_HEXDUMP_FILE_SUFFIX "_hex.c"
#define PRINT_SKIP_STEP 2
#define NUMS_PER_LINE 16

int32_t HcsHexdumpOutput(FILE *in, FILE *out)
{
    const char *prefix = HcsOptGetSymbolNamePrefix();
    if (prefix == NULL) {
        prefix = "";
    }
    if (fprintf(out, "static const unsigned char g_%s%s[] = {\n", prefix, HCS_HEXDUMP_ENTRY_SYMBOL) < 0) {
        return EOUTPUT;
    }
    uint32_t writeCount = 0;
    int32_t byte;
    while ((byte = getc(in)) != EOF) {
        if (fprintf(out, "%s0x%02x", (writeCount % NUMS_PER_LINE) ? ", " : &",\n    "[PRINT_SKIP_STEP * !writeCount],
            byte) < 0) {
            return EOUTPUT;
        }
        writeCount++;
    }
    if (fprintf(out, "\n};\n") < 0) {
        return EOUTPUT;
    }

    if (fprintf(out, "static const unsigned int g_%sLen = %u;\n", HCS_HEXDUMP_ENTRY_SYMBOL, writeCount) < 0) {
        return EOUTPUT;
    }
    if (fprintf(out,
        "void HdfGetBuildInConfigData(const unsigned char** data, unsigned int* size)\n"
        "{\n"
        "    *data = g_%s%s;\n"
        "    *size = g_%s%sLen;\n"
        "}",
        prefix, HCS_HEXDUMP_ENTRY_SYMBOL,  prefix, HCS_HEXDUMP_ENTRY_SYMBOL) < 0) {
        return EOUTPUT;
    }
    return NOERR;
}

int32_t HcsBinaryToHexdump(const char *inputFileName)
{
    struct HcsFile *source = NULL;
    if (HcsOpenSourceFile(inputFileName, &source, "rb")) {
        HCS_ERROR("can not open %s", inputFileName);
        return EINVALF;
    }

    struct HcsFile *out = HcsOpenOutputFile(HCS_HEXDUMP_FILE_SUFFIX);
    if (out == NULL) {
        HCS_ERROR("can not open %s", HcsGetOutPutFilePath());
        HcsCloseFile(source);
        return EINVALF;
    }

    int32_t ret = HcsHexdumpOutput(source->file, out->file);
    if (ret) {
        HCS_ERROR("failed to gen bytecode hexdump in C style");
    }
    HcsCloseFile(source);
    HcsCloseFile(out);
    return ret;
}
