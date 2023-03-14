/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_struct_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
void ASTStructType::AddMember(const AutoPtr<ASTType> &typeName, String name)
{
    for (auto it : members_) {
        if (std::get<0>(it).Equals(name)) {
            return;
        }
    }
    members_.push_back(std::make_tuple(name, typeName));
}

bool ASTStructType::IsStructType()
{
    return true;
}

String ASTStructType::ToString()
{
    return "struct " + name_;
}

String ASTStructType::Dump(const String &prefix)
{
    StringBuilder sb;
    sb.Append(prefix);

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

    sb.AppendFormat("struct %s {\n", name_.string());

    if (members_.size() > 0) {
        for (auto it : members_) {
            sb.Append(prefix + "  ");
            sb.AppendFormat("%s %s;\n", std::get<1>(it)->ToString().string(), std::get<0>(it).string());
        }
    }

    sb.Append(prefix).Append("};\n");

    return sb.ToString();
}

TypeKind ASTStructType::GetTypeKind()
{
    return TypeKind::TYPE_STRUCT;
}

String ASTStructType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("struct %s", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("const struct %s*", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("struct %s*", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("struct %s*", name_.string());
        default:
            return "unknow type";
    }
}

String ASTStructType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("%s", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("const %s&", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("%s&", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("%s", name_.string());
        default:
            return "unknow type";
    }
}

String ASTStructType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // currently, Java does not support the struct type.
    return "/";
}

String ASTStructType::EmitCTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("struct %s {\n", name_.string());

    for (auto it : members_) {
        AutoPtr<ASTType> member = std::get<1>(it);
        String memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCType().string(), memberName.string());
        if (member->GetTypeKind() == TypeKind::TYPE_ARRAY || member->GetTypeKind() == TypeKind::TYPE_LIST) {
            sb.Append(TAB).AppendFormat("uint32_t %sLen;\n", memberName.string());
        }
    }

    sb.Append("};");
    return sb.ToString();
}

String ASTStructType::EmitCppTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("struct %s {\n", name_.string());

    for (auto it : members_) {
        AutoPtr<ASTType> member = std::get<1>(it);
        String memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCppType().string(), memberName.string());
    }

    sb.Append("};");
    return sb.ToString();
}

String ASTStructType::EmitJavaTypeDecl() const
{
    StringBuilder sb;

    return sb.ToString();
}

void ASTStructType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockMarshalling(%s, %s)) {\n", name_.string(), parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockUnmarshalling(%s, %s)) {\n", name_.string(), parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockUnmarshalling(%s, %s)) {\n", name_.string(), parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).AppendFormat("}\n");
}

void ASTStructType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockMarshalling(%s, %s)) {\n", EmitCppType().string(), parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), name.string());
    }
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockUnmarshalling(%s, %s)) {\n", name_.string(), parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sBlockMarshalling(data, &%s)) {\n", name_.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!%sBlockUnmarshalling(data, %s)) {\n", name_.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockMarshalling(%s, %s)) {\n", name_.string(), parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%sBlockUnmarshalling(data, %s)) {\n", EmitCppType().string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTStructType::EmitMemoryRecycle(
    const String &name, bool isClient, bool ownership, StringBuilder &sb, const String &prefix) const
{
    String varName = name;
    if (ownership) {
        sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", varName.string());
        sb.Append(prefix + TAB).AppendFormat("%sFree(%s, true);\n", name_.string(), varName.string());
        sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", varName.string());
        sb.Append(prefix).Append("}\n");
    } else {
        sb.Append(prefix).AppendFormat("%sFree(&%s, false);\n", name_.string(), name.string());
    }
}
} // namespace HDI
} // namespace OHOS