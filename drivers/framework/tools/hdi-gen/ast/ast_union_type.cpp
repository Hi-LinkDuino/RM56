/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_union_type.h"
#include "util/options.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
void ASTUnionType::AddMember(const AutoPtr<ASTType> &typeName, String name)
{
    for (auto it : members_) {
        if (std::get<0>(it).Equals(name)) {
            return;
        }
    }
    members_.push_back(std::make_tuple(name, typeName));
}

bool ASTUnionType::IsUnionType()
{
    return true;
}

String ASTUnionType::ToString()
{
    return "union " + name_;
}

String ASTUnionType::Dump(const String &prefix)
{
    StringBuilder sb;
    sb.Append(prefix);

    std::vector<String> attributes;
    if (isFull_)
        attributes.push_back("full");
    if (isLite_)
        attributes.push_back("lite");
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

    sb.AppendFormat("union %s {\n", name_.string());

    if (members_.size() > 0) {
        for (auto it : members_) {
            sb.Append(prefix + "  ");
            sb.AppendFormat("%s %s;\n", std::get<1>(it)->ToString().string(), std::get<0>(it).string());
        }
    }

    sb.Append(prefix).Append("};\n");

    return sb.ToString();
}

TypeKind ASTUnionType::GetTypeKind()
{
    return TypeKind::TYPE_UNION;
}

String ASTUnionType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("union %s", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("const union %s*", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("union %s*", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("union %s*", name_.string());
        default:
            return "unknow type";
    }
}

String ASTUnionType::EmitCppType(TypeMode mode) const
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

String ASTUnionType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // unsupported type
    return "/";
}

String ASTUnionType::EmitCTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("union %s {\n", name_.string());

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

String ASTUnionType::EmitCppTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("union %s {\n", name_.string());

    for (auto it : members_) {
        AutoPtr<ASTType> member = std::get<1>(it);
        String memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCppType().string(), memberName.string());
    }

    sb.Append("};");
    return sb.ToString();
}

String ASTUnionType::EmitJavaTypeDecl() const
{
    StringBuilder sb;

    return sb.ToString();
}

void ASTUnionType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("if (!HdfSbufWriteBuffer(%s, (const void *)%s, sizeof(%s))) {\n",
            parcelName.string(), name.string(), EmitCType().string());
    } else {
        sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUnpadBuffer(%s, (const uint8_t *)%s, sizeof(%s))) {\n",
            parcelName.string(), name.string(), EmitCType().string());
    }
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("%s *%s = NULL;\n", EmitCType().string(), name.string());
        sb.Append(prefix).Append("uint32_t len = 0;\n");
        sb.Append(prefix).AppendFormat(
            "if (!HdfSbufReadBuffer(%s, (const void **)&%s, &len)) {\n", parcelName.string(), name.string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (%s == NULL || sizeof(%s) != len) {\n", name.string(), EmitCType().string());
    } else {
        sb.Append(prefix).AppendFormat("const %s *%s = (%s *)HdfSbufReadUnpadBuffer(%s, sizeof(%s));\n",
            EmitCType().string(), name.string(), EmitCType().string(), parcelName.string(), EmitCType().string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.string());
    }

    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("%s *%s = NULL;\n", EmitCType().string(), name.string());
        sb.Append(prefix).Append("uint32_t len = 0;\n");
        sb.Append(prefix).AppendFormat(
            "if (!HdfSbufReadBuffer(%s, (const void **)&%s, &len)) {\n", parcelName.string(), name.string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (%s == NULL || sizeof(%s) != len) {\n", name.string(), EmitCType().string());
    } else {
        sb.Append(prefix).AppendFormat("const %s *%s = (%s *)HdfSbufReadUnpadBuffer(%s, sizeof(%s));\n",
            EmitCType().string(), name.string(), EmitCType().string(), parcelName.string(), EmitCType().string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.string());
    }

    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUnpadBuffer((const uint8_t *)&%s, sizeof(%s))) {\n",
        parcelName.string(), name.string(), EmitCppType().string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), name.string());
    }
    sb.Append(prefix).AppendFormat("const %s *%sCp = reinterpret_cast<const %s *>(%s.ReadUnpadBuffer(sizeof(%s)));\n",
        EmitCppType().string(), name.string(), EmitCppType().string(), parcelName.string(), EmitCppType().string());
    sb.Append(prefix).AppendFormat("if (%sCp == nullptr) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).AppendFormat("(void)memcpy_s(&%s, sizeof(%s), %sCp, sizeof(%s));\n", name.string(),
        EmitCppType().string(), name.string(), EmitCppType().string());
}

void ASTUnionType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat(
            "if (!HdfSbufWriteBuffer(data, (const void *)&%s, sizeof(%s))) {\n", name.string(), EmitCType().string());
    } else {
        sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUnpadBuffer(data, (const uint8_t *)&%s, sizeof(%s))) {\n",
            name.string(), EmitCType().string());
    }
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("%s *%s = NULL;\n", EmitCType().string(), name.string());
        sb.Append(prefix).Append("uint32_t len = 0;\n");
        sb.Append(prefix).AppendFormat("if (!HdfSbufReadBuffer(data, (const void **)&%s, &len)) {\n", name.string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (%s == NULL || sizeof(%s) != len) {\n", name.string(), EmitCType().string());
    } else {
        sb.Append(prefix).AppendFormat("const %s *%s = (const %s *)HdfSbufReadUnpadBuffer(data, sizeof(%s));\n",
            EmitCType().string(), name.string(), EmitCType().string(), EmitCType().string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.string());
    }

    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUnpadBuffer((const void*)&%s, sizeof(%s))) {\n", parcelName.string(),
        name.string(), EmitCppType().string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("const %s *%s = reinterpret_cast<const %s*>(%s.ReadUnpadBuffer(sizeof(%s)));\n",
        EmitCppType().string(), name.string(), EmitCppType().string(), parcelName.string(), EmitCppType().string());
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUnionType::EmitMemoryRecycle(
    const String &name, bool isClient, bool ownership, StringBuilder &sb, const String &prefix) const
{
    if (ownership) {
        String varName = isClient ? String::Format("*%s", name.string()) : name;
        sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", varName.string());
        sb.Append(prefix + TAB).AppendFormat("OsalMemFree(%s);\n", varName.string());
        sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", varName.string());
        sb.Append(prefix).Append("}\n");
    }
}
} // namespace HDI
} // namespace OHOS