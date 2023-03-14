/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_CODEGENERATOR_H
#define OHOS_HDI_CODEGENERATOR_H

#include "ast/ast_module.h"
#include "codegen/code_emitter.h"

namespace OHOS {
namespace HDI {
using CodeEmitMap = std::unordered_map<String, AutoPtr<CodeEmitter>, StringHashFunc, StringEqualFunc>;

class CodeGenerator : public LightRefCountBase {
public:
    explicit CodeGenerator(const AutoPtr<ASTModule> &astModule)
        : LightRefCountBase(), astModule_(astModule), targetDirectory_()
    {
    }

    bool Generate();

private:
    void GenerateCCode(const AutoPtr<AST> &ast, const String &outDir, const String &codePart, bool isKernel);
    void GenerateCppCode(const AutoPtr<AST> &ast, const String &outDir, const String &codePart);
    void GenerateJavaCode(const AutoPtr<AST> &ast, const String &outDir, const String &codePart);

    AutoPtr<ASTModule> astModule_;
    String targetDirectory_;

    static CodeEmitMap cCodeEmitters_;
    static CodeEmitMap cppCodeEmitters_;
    static CodeEmitMap javaCodeEmitters_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_CODEGENERATOR_H