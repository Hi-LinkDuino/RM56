/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_AST_MODULE_H
#define OHOS_HDI_AST_MODULE_H

#include <map>

#include "ast/ast.h"

namespace OHOS {
namespace HDI {
class ASTModule : public LightRefCountBase {
public:
    using ASTMap = std::unordered_map<String, AutoPtr<AST>, StringHashFunc, StringEqualFunc>;

    void AddAST(const String &astName, const AutoPtr<AST> ast);

    AutoPtr<AST> GetAst(const String &astName);

    inline const ASTMap &GetAllAsts() const
    {
        return asts_;
    }

private:
    // all ast data
    ASTMap asts_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_AST_MODULE_H