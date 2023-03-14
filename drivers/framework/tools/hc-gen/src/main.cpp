/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "bytecode_gen.h"
#include "decompile.h"
#include "startup_cfg_gen.h"
#include "macro_gen.h"
#include "option.h"
#include "parser.h"
#include "text_gen.h"

using namespace OHOS::Hardware;

int main(int argc, char *argv[])
{
    auto option = Option::Instance().Parse(argc, argv);
    if (option.OptionError()) {
        return EFAIL;
    }

    if (option.ShouldShowUsage()) {
        option.ShowUsage();
        return option.OptionError();
    }

    if (option.ShouldShowVersion()) {
        option.ShowVersion();
        return 0;
    }

    if (option.ShouldDecompile()) {
        Decompile decompile(option.GetSourceName());
        return decompile.DoDecompile() ? 0 : EFAIL;
    }

    Parser parser;
    if (!parser.Parse()) {
        return EFAIL;
    }

    if (option.ShouldGenBinaryConfig()) {
        if (!ByteCodeGen(parser.GetAst()).Output()) {
            return EFAIL;
        }
        return 0;
    }

    if (option.ShouldGenTextConfig()) {
        if (!TextGen(parser.GetAst()).Output()) {
            return EFAIL;
        }
    }
    if (option.ShouldGenMacroConfig()) {
        if (!MacroGen(parser.GetAst()).Output()) {
            return EFAIL;
        }
    }

    if (option.ShouldGenStartConfig()) {
        if (!StartupCfgGen(parser.GetAst()).Output()) {
            return EFAIL;
        }
    }

    return 0;
}
