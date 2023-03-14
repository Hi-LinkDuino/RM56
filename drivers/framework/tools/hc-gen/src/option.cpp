/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstring>
#include <getopt.h>
#include <iomanip>
#include <iostream>

#include "file.h"
#include "logger.h"
#include "option.h"

using namespace OHOS::Hardware;
static constexpr int HCS_COMPILER_VERSION_MAJOR = 00;
static constexpr int HCS_COMPILER_VERSION_MINOR = 7;

static constexpr int ARG_COUNT_MIN = 2;

Option &Option::Instance()
{
    static Option option;
    return option;
}

static constexpr int OPTION_END = -1;
static constexpr const char *HCS_SUPPORT_ARGS = "o:ap:bditmvVhs";

Option &Option::Parse(int argc, char **argv)
{
    do {
        if (argc < ARG_COUNT_MIN) {
            showUsage_ = true;
            break;
        }

        if (!ParseOptions(argc, argv)) {
            break;
        }

        if (optind >= argc) {
            Logger().Error() << "Miss input file name";
            SetOptionError();
            break;
        }
        SetSourceOption(argv[optind]);
        return *this;
    } while (false);

    return *this;
}

bool Option::ParseOptions(int argc, char **argv)
{
    int32_t op = 0;
    while (op != OPTION_END) {
        op = getopt(argc, argv, HCS_SUPPORT_ARGS);
        SetOptionData(op);
        if (ShouldShowUsage() || ShouldShowVersion()) {
            return false;
        }
    }

    return true;
}

void Option::SetOptionData(char op)
{
    switch (op) {
        case 'o':
            outputName_ = optarg;
            break;
        case 'a':
            shouldAlign_ = true;
            break;
        case 'b':
            shouldGenByteCodeConfig_ = true;
            break;
        case 't':
            shouldGenTextConfig_ = true;
            shouldGenByteCodeConfig_ = false;
            shouldGenMacroConfig_ = false;
            break;
        case 'm':
            shouldGenTextConfig_ = false;
            shouldGenByteCodeConfig_ = false;
            shouldGenMacroConfig_ = true;
            break;
        case 'p':
            symbolNamePrefix_ = optarg;
            break;
        case 'i':
            showGenHexDump_ = true;
            break;
        case 'V':
            verboseLog_ = true;
            break;
        case 'd':
            shouldDecompile_ = true;
            break;
        case 'v':
            showVersion_ = true;
            break;
        case 'h': /* fall-through */
            showUsage_ = true;
            break;
        case 's':
            genStartCfg_ = true;
            shouldGenByteCodeConfig_ = false;
            break;
        case '?':
            SetOptionError();
            break;
        default:
            break;
    }
}

void Option::ShowUsage()
{
    Logger() << "Usage: hc-gen [Options] [File]\n"
             << "options:";
    ShowOption("-o <file>", "output file name, default same as input");
    ShowOption("-a", "hcb align with four bytes");
    ShowOption("-b", "output binary output, default enable");
    ShowOption("-t", "output config in C language source file style");
    ShowOption("-m", "output config in macro file style");
    ShowOption("-s", "output start config of host");
    ShowOption("-i", "output binary hex dump in C language source file style");
    ShowOption("-p <prefix>", "prefix of generated symbol name");
    ShowOption("-d", "decompile hcb to hcs");
    ShowOption("-V", "show verbose info");
    ShowOption("-v", "show version");
    ShowOption("-h", "show this help message");
}

void Option::ShowOption(const ::std::string &option, const ::std::string &helpInfo)
{
    Logger() << "  " << ::std::setw(12) << ::std::left << option << "  " << helpInfo;
}

bool Option::ShouldShowUsage() const
{
    return showUsage_;
}

void Option::ShowVersion()
{
    Logger() << "Hcs compiler " << HCS_COMPILER_VERSION_MAJOR << "." << HCS_COMPILER_VERSION_MINOR;
    Logger() << "Copyright (c) 2020-2021 Huawei Device Co., Ltd.";
}

bool Option::OptionError() const
{
    return optionError_;
}

bool Option::ShouldShowVersion() const
{
    return showVersion_;
}

bool Option::ShouldAlign() const
{
    return shouldAlign_;
}

bool Option::ShouldGenTextConfig() const
{
    return shouldGenTextConfig_;
}

bool Option::ShouldGenMacroConfig() const
{
    return shouldGenMacroConfig_;
}

bool Option::ShouldGenBinaryConfig() const
{
    return shouldGenByteCodeConfig_;
}

bool Option::ShouldGenHexDump() const
{
    return showGenHexDump_;
}

bool Option::ShouldDecompile() const
{
    return shouldDecompile_;
}

bool Option::ShouldGenStartConfig() const
{
    return genStartCfg_;
}

std::string Option::GetSymbolPrefix()
{
    return symbolNamePrefix_;
}

bool Option::VerboseLog() const
{
    return verboseLog_;
}

void Option::SetOptionError(bool shouldShowUsage)
{
    showUsage_ = shouldShowUsage;
    optionError_ = true;
}

std::string Option::GetSourceName()
{
    return sourceName_;
}

std::string Option::GetSourceNameBase()
{
    return sourceNameBase_;
}

std::string Option::GetOutputName()
{
    return outputName_;
}

std::string Option::GetSourceDir()
{
    return sourceDir_;
}

bool Option::SetSourceOption(const char *srcName)
{
    std::string srcAbsPath(Util::File::AbsPath(srcName));
    if (srcAbsPath.empty()) {
        Logger().Error() << "invalid source file: " << srcName << ", " << strerror(errno);
        SetOptionError(false);
        return false;
    }

    sourceName_ = srcAbsPath;
    sourceNameBase_ = Util::File::FileNameBase(srcAbsPath);
    return true;
}

void Option::GetVersion(uint32_t &minor, uint32_t &major)
{
    minor = HCS_COMPILER_VERSION_MINOR;
    major = HCS_COMPILER_VERSION_MAJOR;
}
