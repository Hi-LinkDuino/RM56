/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_OPTION_H
#define HC_GEN_OPTION_H

#include <memory>
#include <string>

namespace OHOS {
namespace Hardware {

class Option {
public:
    Option() = default;

    ~Option() = default;

    static Option &Instance();

    Option &Parse(int argc, char *argv[]);

    void ShowUsage();

    void ShowVersion();

    bool ShouldShowUsage() const;

    bool OptionError() const;

    bool ShouldShowVersion() const;

    bool ShouldAlign() const;

    bool ShouldGenTextConfig() const;

    bool ShouldGenMacroConfig() const;

    bool ShouldGenBinaryConfig() const;

    bool ShouldGenStartConfig() const;

    bool ShouldGenHexDump() const;

    bool ShouldDecompile() const;

    std::string GetSymbolPrefix();

    std::string GetSourceName();

    std::string GetSourceNameBase();

    std::string GetOutputName();

    void GetVersion(uint32_t &minor, uint32_t &major);

    bool VerboseLog() const;

    std::string GetSourceDir();

    static std::string RealPathSourcePath(const char *path);

private:
    static void ShowOption(const std::string &option, const std::string &helpInfo);
    bool ParseOptions(int argc, char *argv[]);
    void SetOptionError(bool shouldShowUsage = true);
    bool SetSourceOption(const char *srcName);

    bool showUsage_ = false;
    bool showVersion_ = false;
    bool shouldAlign_ = false;
    bool shouldGenTextConfig_ = false;
    bool shouldGenMacroConfig_ = false;
    bool shouldGenByteCodeConfig_ = true;
    bool genStartCfg_ = false;
    bool showGenHexDump_ = false;
    bool shouldDecompile_ = false;
    bool verboseLog_ = false;
    bool optionError_ = false;
    std::string symbolNamePrefix_;
    std::string sourceName_;
    std::string sourceNameBase_;
    std::string outputName_;
    std::string sourceDir_;
    void SetOptionData(char op);
};
} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_OPTION_H
