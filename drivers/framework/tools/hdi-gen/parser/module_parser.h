/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_MODULE_PARSER_H
#define OHOS_HDI_MODULE_PARSER_H

#include <memory>
#include <set>

#include "ast/ast_module.h"
#include "parser/file_detail.h"
#include "util/options.h"

namespace OHOS {
namespace HDI {
class ModuleParser {
public:
    using FileDetailMap = std::unordered_map<String, std::shared_ptr<FileDetail>, StringHashFunc, StringEqualFunc>;

    explicit ModuleParser(const Options &option)
        : option_(option),
        sourceFiles_(),
        searchedFile_(),
        traceFilePtr_(nullptr),
        allCirclesFile_(),
        compileFiles_(),
        module_(new ASTModule())
    {
    }

    ~ModuleParser() = default;

    AutoPtr<ASTModule> Parse();

private:
    // parser file and circular reference
    bool ParserDependencies();

    bool CompileFiles();

    // parse all idl file involved in compilation.
    bool ParserAllImports(const String &rootFilePath);

    bool ParserAllImportsRecursion(const std::shared_ptr<FileDetail> &fileInfo);

    bool ParserAllidlFile(const std::vector<String> &idlSourceFile);

    // check circular reference and reverse topology sorting of all idl file
    bool CheckCircularReference();

    static const char *TAG;

    const Options &option_;
    FileDetailMap sourceFiles_;

    std::set<String> searchedFile_;
    std::shared_ptr<std::vector<String>> traceFilePtr_;
    std::vector<std::vector<String>> allCirclesFile_;

    std::vector<String> compileFiles_;

    AutoPtr<ASTModule> module_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_MODULE_PARSER_H