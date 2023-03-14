/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_ASTVOIDTYPE_H
#define OHOS_HDI_ASTVOIDTYPE_H

#include "ast/ast_type.h"

namespace OHOS {
namespace HDI {
class ASTVoidType : public ASTType {
public:
    bool IsVoidType() override;

    String ToString() override;

    TypeKind GetTypeKind() override;

    String EmitCType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitCppType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitJavaType(TypeMode mode, bool isInnerType = false) const override;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_ASTBOOLEANTYPE_H