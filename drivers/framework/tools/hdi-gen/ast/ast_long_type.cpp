/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_long_type.h"

namespace OHOS {
namespace HDI {
bool ASTLongType::IsLongType()
{
    return true;
}

String ASTLongType::ToString()
{
    return "long";
}

TypeKind ASTLongType::GetTypeKind()
{
    return TypeKind::TYPE_LONG;
}

String ASTLongType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int64_t";
        case TypeMode::PARAM_IN:
            return "int64_t";
        case TypeMode::PARAM_OUT:
            return "int64_t*";
        case TypeMode::LOCAL_VAR:
            return "int64_t";
        default:
            return "unknow type";
    }
}

String ASTLongType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int64_t";
        case TypeMode::PARAM_IN:
            return "int64_t";
        case TypeMode::PARAM_OUT:
            return "int64_t&";
        case TypeMode::LOCAL_VAR:
            return "int64_t";
        default:
            return "unknow type";
    }
}

String ASTLongType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return isInnerType ? "Long" : "long";
}

void ASTLongType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteInt64(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTLongType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadInt64(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTLongType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadInt64(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTLongType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteInt64(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTLongType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadInt64();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadInt64();\n", name.string(), parcelName.string());
    }
}

void ASTLongType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteInt64(data, %s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTLongType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadInt64(data, &%s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    EmitFreeStatements(freeObjStatements, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTLongType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteInt64(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTLongType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadInt64();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadInt64();\n", name.string(), parcelName.string());
    }
}

void ASTLongType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeLong(%s);\n", parcelName.string(), name.string());
}

void ASTLongType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = %s.readLong();\n", name.string(), parcelName.string());
}

void ASTLongType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "%s %s = %s.readLong();\n", EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
}
} // namespace HDI
} // namespace OHOS