/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_void_type.h"

namespace OHOS {
namespace HDI {
bool ASTVoidType::IsVoidType()
{
    return true;
}

String ASTVoidType::ToString()
{
    return "void";
}

TypeKind ASTVoidType::GetTypeKind()
{
    return TypeKind::TYPE_VOID;
}

String ASTVoidType::EmitCType(TypeMode mode) const
{
    return "void";
}

String ASTVoidType::EmitCppType(TypeMode mode) const
{
    return "void";
}

String ASTVoidType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return "void";
}
} // namespace HDI
} // namespace OHOS