/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_OPTION_H
#define OHOS_HDI_OPTION_H

#include <unordered_map>
#include <vector>

#include "util/string.h"

namespace OHOS {
namespace HDI {
class Options {
public:
    using PkgPathMap = std::unordered_map<String, String, StringHashFunc, StringEqualFunc>;

    static Options &GetInstance();

    Options(const Options &other) = delete;
    Options operator=(const Options &other) = delete;

    Options &Parse(int argc, char *argv[]);

    ~Options() = default;

    inline bool DoShowUsage() const
    {
        return doShowUsage_;
    }

    inline bool DoShowVersion() const
    {
        return doShowVersion_;
    }

    inline bool DoCompile() const
    {
        return doCompile_;
    }

    inline bool DoDumpAST() const
    {
        return doDumpAST_;
    }

    inline bool DoGetHashKey() const
    {
        return doGetHashKey_;
    }

    inline bool DoGenerateCode() const
    {
        return doGenerateCode_;
    }

    inline bool DoGenerateKernelCode() const
    {
        return doModeKernel_;
    }

    inline bool HasErrors() const
    {
        return !errors_.empty();
    }

    inline std::vector<String> GetSourceFiles() const
    {
        return sourceFiles_;
    }

    inline PkgPathMap GetPackagePath() const
    {
        return packagePath_;
    }

    inline String GetTargetLanguage() const
    {
        return targetLanguage_;
    }

    inline String GetCodePart() const
    {
        return codePart_;
    }

    inline String GetModuleName() const
    {
        return doSetModuleName_ ? moduleName_ : "sample";
    }

    inline String GetGenerationDirectory() const
    {
        return generationDirectory_;
    }

    void ShowErrors() const;

    void ShowVersion() const;

    void ShowUsage() const;

    String GetRootPackage(const String &package);

    String GetSubPackage(const String &package);

    String GetPackagePath(const String &package);

private:
    Options()
        : program_(),
        sourceFiles_(0),
        targetLanguage_(),
        codePart_("all"),
        generationDirectory_(),
        illegalOptions_(),
        errors_(),
        doShowUsage_(false),
        doShowVersion_(false),
        doCompile_(false),
        doDumpAST_(false),
        doGetHashKey_(false),
        doGenerateCode_(false),
        doModeKernel_(false),
        doGeneratePart_(false),
        doSetModuleName_(false),
        doOutDir_(false)
    {
    }

    void SetOptionData(char op);

    void AddPackagePath(const String &packagePath);

    void SetLanguage(const String &language);

    void SetCodePart(const String &part);

    void CheckOptions();

    static const char *optSupportArgs;
    static constexpr int OPT_END = -1;

    static constexpr int VERSION_MAJOR = 0;
    static constexpr int VERSION_MINOR = 1;

    String program_;
    std::vector<String> sourceFiles_;
    String targetLanguage_;
    String codePart_;
    String moduleName_;
    String generationDirectory_;
    String illegalOptions_;
    std::vector<String> errors_;
    PkgPathMap packagePath_;

    bool doShowUsage_;
    bool doShowVersion_;
    bool doCompile_;
    bool doDumpAST_;
    bool doGetHashKey_;
    bool doGenerateCode_;
    bool doModeKernel_;
    bool doGeneratePart_;
    bool doSetModuleName_;
    bool doOutDir_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDIL_OPTION_H