/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_short_type.h"

namespace OHOS {
namespace HDI {
bool ASTShortType::IsShortType()
{
    return true;
}

String ASTShortType::ToString()
{
    return "short";
}

TypeKind ASTShortType::GetTypeKind()
{
    return TypeKind::TYPE_SHORT;
}

String ASTShortType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int16_t";
        case TypeMode::PARAM_IN:
            return "int16_t";
        case TypeMode::PARAM_OUT:
            return "int16_t*";
        case TypeMode::LOCAL_VAR:
            return "int16_t";
        default:
            return "unknow type";
    }
}

String ASTShortType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int16_t";
        case TypeMode::PARAM_IN:
            return "int16_t";
        case TypeMode::PARAM_OUT:
            return "int16_t&";
        case TypeMode::LOCAL_VAR:
            return "int16_t";
        default:
            return "unknow type";
    }
}

String ASTShortType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return isInnerType ? "Short" : "short";
}

void ASTShortType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteInt16(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTShortType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadInt16(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTShortType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadInt16(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTShortType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteInt16(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTShortType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("%s %s = (%s)%s.ReadInt16();\n", EmitCppType().string(), name.string(),
            EmitCppType().string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = (%s)%s.ReadInt16();\n", name.string(), EmitCppType().string(), parcelName.string());
    }
}

void ASTShortType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteInt16(data, %s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTShortType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadInt16(data, &%s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    EmitFreeStatements(freeObjStatements, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTShortType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteInt16(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTShortType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = (%s)%s.ReadInt16();\n", EmitCppType().string(), name.string(),
            EmitCppType().string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadInt16();\n", name.string(), parcelName.string());
    }
}

void ASTShortType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeShort(%s);\n", parcelName.string(), name.string());
}

void ASTShortType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = %s.readShort();\n", name.string(), parcelName.string());
}

void ASTShortType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "%s %s = %s.readShort();\n", EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
}
} // namespace HDI
} // namespace OHOS