/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_ushort_type.h"

namespace OHOS {
namespace HDI {
bool ASTUshortType::IsUshortType()
{
    return true;
}

String ASTUshortType::ToString()
{
    return "unsigned short";
}

TypeKind ASTUshortType::GetTypeKind()
{
    return TypeKind::TYPE_USHORT;
}

String ASTUshortType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "uint16_t";
        case TypeMode::PARAM_IN:
            return "uint16_t";
        case TypeMode::PARAM_OUT:
            return "uint16_t*";
        case TypeMode::LOCAL_VAR:
            return "uint16_t";
        default:
            return "unknow type";
    }
}

String ASTUshortType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "uint16_t";
        case TypeMode::PARAM_IN:
            return "uint16_t";
        case TypeMode::PARAM_OUT:
            return "uint16_t&";
        case TypeMode::LOCAL_VAR:
            return "uint16_t";
        default:
            return "unknow type";
    }
}

String ASTUshortType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // unsupported type
    return "/";
}

void ASTUshortType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint16(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUshortType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint16(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUshortType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint16(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUshortType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint16(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUshortType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadUint16();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadUint16();\n", name.string(), parcelName.string());
    }
}

void ASTUshortType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint16(data, %s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUshortType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint16(data, &%s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    EmitFreeStatements(freeObjStatements, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTUshortType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint16(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTUshortType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadUint16();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadUint16();\n", name.string(), parcelName.string());
    }
}
} // namespace HDI
} // namespace OHOS