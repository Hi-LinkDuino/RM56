/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_sequenceable_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
void ASTSequenceableType::SetNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    ASTType::SetNamespace(nspace);
    if (namespace_ != nullptr) {
        namespace_->AddSequenceable(this);
    }
}

bool ASTSequenceableType::IsSequenceableType()
{
    return true;
}

String ASTSequenceableType::ToString()
{
    return name_;
}

TypeKind ASTSequenceableType::GetTypeKind()
{
    return TypeKind::TYPE_SEQUENCEABLE;
}

String ASTSequenceableType::Dump(const String &prefix)
{
    StringBuilder sb;

    sb.Append(prefix).Append("sequenceable ");
    if (namespace_ != nullptr) {
        sb.Append(namespace_->ToString());
    }
    sb.Append(name_);
    sb.Append(";\n");

    return sb.ToString();
}

String ASTSequenceableType::GetFullName() const
{
    return namespace_->ToString() + name_;
}

String ASTSequenceableType::EmitCType(TypeMode mode) const
{
    // c language has no Sequenceable type
    return "/";
}

String ASTSequenceableType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("sptr<%s>", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("const sptr<%s>&", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("sptr<%s>&", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("sptr<%s>", name_.string());
        default:
            return "unknow type";
    }
}

String ASTSequenceableType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return name_;
}

void ASTSequenceableType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteStrongParcelable(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTSequenceableType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("sptr<%s> %s = %s.ReadStrongParcelable<%s>();\n", name_.string(), name.string(),
            parcelName.string(), name_.string());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = %s.ReadStrongParcelable<%s>();\n", name.string(), parcelName.string(), name_.string());
    }
}

void ASTSequenceableType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteStrongParcelable(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTSequenceableType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = %s.ReadStrongParcelable<%s>();\n", EmitCppType().string(),
            name.string(), parcelName.string(), name_.string());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = %s.ReadStrongParcelable<%s>();\n", name.string(), parcelName.string(), name_.string());
    }
}

void ASTSequenceableType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    if (EmitJavaType(TypeMode::NO_MODE).Equals("IRemoteObject")) {
        sb.Append(prefix).AppendFormat("%s.writeRemoteObject(%s);\n", parcelName.string(), name.string());
        return;
    }
    sb.Append(prefix).AppendFormat("%s.writeSequenceable(%s);\n", parcelName.string(), name.string());
}

void ASTSequenceableType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    if (EmitJavaType(TypeMode::NO_MODE).Equals("IRemoteObject")) {
        sb.Append(prefix).AppendFormat("%s = %s.readRemoteObject();\n", name.string(), parcelName.string());
        return;
    }
    sb.Append(prefix).AppendFormat("%s.readSequenceable(%s);\n", parcelName.string(), name.string());
}

void ASTSequenceableType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    if (!isInner && EmitJavaType(TypeMode::NO_MODE).Equals("IRemoteObject")) {
        sb.Append(prefix).AppendFormat(
            "IRemoteObject %s = %s.readRemoteObject();\n", name.string(), parcelName.string());
        return;
    }
    if (!isInner) {
        sb.Append(prefix).AppendFormat("%s %s = new %s();\n", EmitJavaType(TypeMode::NO_MODE).string(), name.string(),
            EmitJavaType(TypeMode::NO_MODE).string());
    }
    sb.Append(prefix).AppendFormat("%s.readSequenceable(%s);\n", parcelName.string(), name.string());
}
} // namespace HDI
} // namespace OHOS