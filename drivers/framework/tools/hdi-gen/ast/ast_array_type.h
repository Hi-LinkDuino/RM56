/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_ASTARRAYTYPE_H
#define OHOS_HDI_ASTARRAYTYPE_H

#include "ast/ast_type.h"

namespace OHOS {
namespace HDI {
class ASTArrayType : public ASTType {
public:
    inline void SetElementType(const AutoPtr<ASTType> &elementType)
    {
        elementType_ = elementType;
    }

    inline AutoPtr<ASTType> GetElementType()
    {
        return elementType_;
    }

    bool IsArrayType() override;

    String ToString() override;

    TypeKind GetTypeKind() override;

    String EmitCType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitCppType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitJavaType(TypeMode mode, bool isInnerType = false) const override;

    void EmitCWriteVar(const String &parcelName, const String &name, const String &ecName, const String &gotoLabel,
        StringBuilder &sb, const String &prefix) const override;

    void EmitCProxyWriteOutVar(const String &parcelName, const String &name, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const override;

    void EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const override;

    void EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName, const String &gotoLabel,
        StringBuilder &sb, const String &prefix) const override;

    void EmitCStubReadOutVar(const String &buffSizeName, const String &memFlagName, const String &parcelName,
        const String &name, const String &ecName, const String &gotoLabel, StringBuilder &sb,
        const String &prefix) const override;

    void EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        unsigned int innerLevel = 0) const override;

    void EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        bool initVariable, unsigned int innerLevel = 0) const override;

    void EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const override;

    void EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb, const String &prefix,
        std::vector<String> &freeObjStatements) const override;

    void EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        unsigned int innerLevel = 0) const override;

    void EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        bool emitType, unsigned int innerLevel = 0) const override;

    void EmitMemoryRecycle(
        const String &name, bool isClient, bool ownership, StringBuilder &sb, const String &prefix) const override;

    void EmitJavaWriteVar(
        const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const override;

    void EmitJavaReadVar(
        const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const override;

    void EmitJavaReadInnerVar(const String &parcelName, const String &name, bool isInner, StringBuilder &sb,
        const String &prefix) const override;

private:
    void EmitJavaWriteArrayVar(
        const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const;

    void EmitCMallocVar(const String &name, const String &lenName, bool isClient, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const;

    void EmitCProxyReadStrElement(const String &parcelName, const String &name, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const;

    void EmitCStubReadStrElement(const String &parcelName, const String &name, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const;

    void EmitCStringElementUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
        const String &newPrefix, std::vector<String> &freeObjStatements) const;

    AutoPtr<ASTType> elementType_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_ASTARRAYTYPE_H