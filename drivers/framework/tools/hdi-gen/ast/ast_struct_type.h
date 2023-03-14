/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_ASTSTRUCTTYPE_H
#define OHOS_HDI_ASTSTRUCTTYPE_H

#include <tuple>
#include <vector>

#include "ast/ast_type.h"
#include "util/autoptr.h"
#include "util/string.h"

namespace OHOS {
namespace HDI {
class ASTStructType : public ASTType {
public:
    inline void SetName(const String &name)
    {
        name_ = name;
    }

    inline String GetName()
    {
        return name_;
    }

    inline void SetFull(bool full)
    {
        isFull_ = full;
    }

    inline bool IsFull()
    {
        return isFull_;
    }

    inline void SetLite(bool lite)
    {
        isLite_ = lite;
    }

    inline bool IsLite()
    {
        return isLite_;
    }

    void AddMember(const AutoPtr<ASTType> &typeName, String name);

    inline size_t GetMemberNumber()
    {
        return members_.size();
    }

    inline String GetMemberName(size_t index)
    {
        if (index >= members_.size()) {
            return String("");
        }
        return std::get<0>(members_[index]);
    }

    inline AutoPtr<ASTType> GetMemberType(size_t index)
    {
        if (index >= members_.size()) {
            return nullptr;
        }
        return std::get<1>(members_[index]);
    }

    bool IsStructType() override;

    String ToString() override;

    String Dump(const String &prefix) override;

    TypeKind GetTypeKind() override;

    String EmitCType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitCppType(TypeMode mode = TypeMode::NO_MODE) const override;

    String EmitJavaType(TypeMode mode, bool isInnerType = false) const override;

    String EmitCTypeDecl() const;

    String EmitCppTypeDecl() const;

    String EmitJavaTypeDecl() const;

    void EmitCWriteVar(const String &parcelName, const String &name, const String &ecName, const String &gotoLabel,
        StringBuilder &sb, const String &prefix) const override;

    void EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const override;

    void EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName, const String &gotoLabel,
        StringBuilder &sb, const String &prefix) const override;

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

private:
    bool isFull_ = false;
    bool isLite_ = false;
    std::vector<std::tuple<String, AutoPtr<ASTType>>> members_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_ASTSTRUCTTYPE_H