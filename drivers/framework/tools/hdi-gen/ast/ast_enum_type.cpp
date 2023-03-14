/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_enum_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
void ASTEnumType::SetBaseType(const AutoPtr<ASTType> &baseType)
{
    if (baseType == nullptr) {
        return;
    }
    baseType_ = baseType;
}

void ASTEnumType::AddMember(const AutoPtr<ASTEnumValue> &member)
{
    members_.push_back(member);
}

bool ASTEnumType::IsEnumType()
{
    return true;
}

String ASTEnumType::ToString()
{
    return "enum " + name_;
}

String ASTEnumType::Dump(const String &prefix)
{
    StringBuilder sb;

    std::vector<String> attributes;
    if (isFull_) {
        attributes.push_back("full");
    }
    if (isLite_) {
        attributes.push_back("lite");
    }
    if (attributes.size() > 0) {
        sb.Append("[");
        for (size_t i = 0; i < attributes.size(); i++) {
            sb.Append(attributes[i]);
            if (i < attributes.size() - 1) {
                sb.Append(',');
            }
        }
        sb.Append("] ");
    }

    if (isDisplayBase_ && baseType_ != nullptr) {
        sb.AppendFormat("enum %s : %s {\n", name_.string(), baseType_->ToString().string());
    } else {
        sb.AppendFormat("enum %s {\n", name_.string());
    }

    if (members_.size() > 0) {
        for (auto it : members_) {
            if (it->isDefaultValue()) {
                sb.Append("  ").AppendFormat("%s,\n", it->GetName().string());
            } else {
                sb.Append("  ").AppendFormat("%s = %lu,\n", it->GetName().string(), it->GetValue());
            }
        }
    }

    sb.Append(prefix).Append("};\n");

    return sb.ToString();
}

TypeKind ASTEnumType::GetTypeKind()
{
    return TypeKind::TYPE_ENUM;
}

String ASTEnumType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("enum %s", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("enum %s", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("enum %s*", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("enum %s", name_.string());
        default:
            return "unknow type";
    }
}

String ASTEnumType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("%s", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("%s", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("%s&", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("%s", name_.string());
        default:
            return "unknow type";
    }
}

String ASTEnumType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // currently, Java does not support the enum type.
    return "/";
}

String ASTEnumType::EmitCTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("enum %s {\n", name_.string());

    for (auto it : members_) {
        if (it->isDefaultValue()) {
            sb.Append(TAB).AppendFormat("%s,\n", it->GetName().string());
        } else {
            sb.Append(TAB).AppendFormat("%s = %lu,\n", it->GetName().string(), it->GetValue());
        }
    }

    sb.Append("};");
    return sb.ToString();
}

String ASTEnumType::EmitCppTypeDecl() const
{
    StringBuilder sb;
    if (isDisplayBase_ && baseType_ != nullptr) {
        sb.AppendFormat("enum %s : %s {\n", name_.string(), baseType_->EmitCppType().string());
    } else {
        sb.AppendFormat("enum %s {\n", name_.string());
    }

    for (auto it : members_) {
        if (it->isDefaultValue()) {
            sb.Append(TAB).AppendFormat("%s,\n", it->GetName().string());
        } else {
            sb.Append(TAB).AppendFormat("%s = %lu,\n", it->GetName().string(), it->GetValue());
        }
    }

    sb.Append("};");
    return sb.ToString();
}

String ASTEnumType::EmitJavaTypeDecl() const
{
    StringBuilder sb;

    return sb.ToString();
}

void ASTEnumType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!HdfSbufWriteUint32(%s, (uint32_t)%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTEnumType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!HdfSbufReadUint32(%s, (uint32_t*)%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTEnumType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!HdfSbufReadUint32(%s, (uint32_t*)%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTEnumType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32((uint32_t)%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTEnumType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("%s %s = (%s)%s.ReadUint32();\n", EmitCppType().string(), name.string(),
            EmitCppType().string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = (%s)%s.ReadUint32();\n", name.string(), EmitCppType().string(), parcelName.string());
    }
}

void ASTEnumType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteInt32(data, (int32_t)%s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTEnumType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadInt32(data, (int32_t*)&%s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTEnumType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32((uint32_t)%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTEnumType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = (%s)%s.ReadUint32();\n", EmitCppType().string(), name.string(),
            EmitCppType().string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = (%s)%s.ReadUint32();\n", name.string(), EmitCppType().string(), parcelName.string());
    }
}
} // namespace HDI
} // namespace OHOS