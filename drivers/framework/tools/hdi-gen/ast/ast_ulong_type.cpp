/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_ulong_type.h"

namespace OHOS {
namespace HDI {
bool ASTUlongType::IsUlongType()
{
    return true;
}

String ASTUlongType::ToString()
{
    return "unsigned long";
}

TypeKind ASTUlongType::GetTypeKind()
{
    return TypeKind::TYPE_ULONG;
}

String ASTUlongType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "uint64_t";
        case TypeMode::PARAM_IN:
            return "uint64_t";
        case TypeMode::PARAM_OUT:
            return "uint64_t*";
        case TypeMode::LOCAL_VAR:
            return "uint64_t";
        default:
            return "unknow type";
    }
}

String ASTUlongType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "uint64_t";
        case TypeMode::PARAM_IN:
            return "uint64_t";
        case TypeMode::PARAM_OUT:
            return "uint64_t&";
        case TypeMode::LOCAL_VAR:
            return "uint64_t";
        default:
            return "unknow type";
    }
}

String ASTUlongType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // unsupported type
    return "/";
}

void ASTUlongType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint64(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUlongType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint64(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUlongType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint64(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUlongType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint64(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUlongType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadUint64();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadUint64();\n", name.string(), parcelName.string());
    }
}

void ASTUlongType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint64(data, %s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUlongType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint64(data, &%s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    EmitFreeStatements(freeObjStatements, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUlongType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint64(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUlongType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadUint64();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadUint64();\n", name.string(), parcelName.string());
    }
}
} // namespace HDI
} // namespace OHOS