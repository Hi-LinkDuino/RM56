/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_ASTENUMTYPE_H
#define OHOS_HDI_ASTENUMTYPE_H

#include "ast/ast_type.h"
#include "util/autoptr.h"
#include "util/string.h"

#include <vector>

namespace OHOS {
namespace HDI {
class ASTEnumValue : public LightRefCountBase {
public:
    explicit ASTEnumValue(const String &name) : mName_(name), isDefault_(true), mValue_(0) {}

    inline virtual ~ASTEnumValue() {}

    inline String GetName()
    {
        return mName_;
    }

    inline void SetType(const AutoPtr<ASTType> &type)
    {
        mType_ = type;
    }

    inline AutoPtr<ASTType> GetType()
    {
        return mType_;
    }

    inline void SetValue(unsigned long value)
    {
        isDefault_ = false;
        mValue_ = value;
    }

    inline unsigned long GetValue()
    {
        return mValue_;
    }

    inline bool isDefaultValue()
    {
        return isDefault_;
    }

private:
    String mName_;
    AutoPtr<ASTType> mType_;
    bool isDefault_;
    unsigned long mValue_;
};

class ASTEnumType : public ASTType {
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

    inline void SetDisplayBase(bool display)
    {
        isDisplayBase_ = display;
    }

    inline bool IsDisplayBase()
    {
        return isDisplayBase_;
    }

    void SetBaseType(const AutoPtr<ASTType> &baseType);

    inline AutoPtr<ASTType> GetBaseType()
    {
        return baseType_;
    }

    void AddMember(const AutoPtr<ASTEnumValue> &member);

    inline size_t GetMemberNumber()
    {
        return members_.size();
    }

    inline AutoPtr<ASTEnumValue> GetMember(size_t index)
    {
        if (index >= members_.size()) {
            return nullptr;
        }
        return members_[index];
    }

    bool IsEnumType() override;

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

private:
    bool isFull_ = false;
    bool isLite_ = false;
    bool isDisplayBase_ = false;
    AutoPtr<ASTType> baseType_ = nullptr;

    std::vector<AutoPtr<ASTEnumValue>> members_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_ASTENUMTYPE_H