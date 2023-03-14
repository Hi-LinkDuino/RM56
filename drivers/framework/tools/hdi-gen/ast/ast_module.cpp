/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_module.h"

namespace OHOS {
namespace HDI {
void ASTModule::AddAST(const String &astName, const AutoPtr<AST> ast)
{
    asts_[astName] = ast;
}

AutoPtr<AST> ASTModule::GetAst(const String &astName)
{
    auto astIter = asts_.find(astName);
    if (astIter == asts_.end()) {
        return nullptr;
    }
    return astIter->second;
}
} // namespace HDI
} // namespace OHOS