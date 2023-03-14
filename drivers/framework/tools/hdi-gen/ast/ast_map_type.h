/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_ASTMAPTYPE_H
#define OHOS_HDI_ASTMAPTYPE_H

#include "ast/ast_type.h"
#include "util/autoptr.h"

namespace OHOS {
namespace HDI {
class ASTMapType : public ASTType {
public:
    inline void SetKeyType(const AutoPtr<ASTType> &keyType)
    {
        keyType_ = keyType;
    }

    inline AutoPtr<ASTType> GetKeyType()
    {
        return keyType_;
    }

    inline void SetValueType(const AutoPtr<ASTType> &valueType)
    {
        valueType_ = valueType;
    }

    inline AutoPtr<ASTType> GetValueType()
    {
        return valueType_;
    }

    bool IsMapType() override;

    String ToString() override;

    TypeKind GetTypeKind() override;

    String EmitCType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitCppType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitJavaType(TypeMode mode, bool isInnerType = false) const override;

    void EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        unsigned int innerLevel = 0) const override;

    void EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        bool initVariable, unsigned int innerLevel = 0) const override;

    void EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        unsigned int innerLevel = 0) const override;

    void EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        bool emitType, unsigned int innerLevel = 0) const override;

    void EmitJavaWriteVar(
        const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const override;

    void EmitJavaReadVar(
        const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const override;

    void EmitJavaReadInnerVar(const String &parcelName, const String &name, bool isInner, StringBuilder &sb,
        const String &prefix) const override;

private:
    AutoPtr<ASTType> keyType_;
    AutoPtr<ASTType> valueType_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_ASTMAPTYPE_H