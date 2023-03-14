/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_fd_type.h"

namespace OHOS {
namespace HDI {
bool ASTFdType::IsFdType()
{
    return true;
}

String ASTFdType::ToString()
{
    return "FileDescriptor";
}

TypeKind ASTFdType::GetTypeKind()
{
    return TypeKind::TYPE_FILEDESCRIPTOR;
}

String ASTFdType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int";
        case TypeMode::PARAM_IN:
            return "int";
        case TypeMode::PARAM_OUT:
            return "int*";
        case TypeMode::LOCAL_VAR:
            return "int";
        default:
            return "unknow type";
    }
}

String ASTFdType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int";
        case TypeMode::PARAM_IN:
            return "int";
        case TypeMode::PARAM_OUT:
            return "int&";
        case TypeMode::LOCAL_VAR:
            return "int";
        default:
            return "unknow type";
    }
}

String ASTFdType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return isInnerType ? "Integer" : "int";
}

void ASTFdType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteFileDescriptor(%s, %s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTFdType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    if (isInnerType) {
        sb.Append(prefix).AppendFormat("%s = HdfSbufReadFileDescriptor(%s);\n", name.string(), parcelName.string());
        sb.Append(prefix).AppendFormat("if (%s < 0) {\n", name.string());
    } else {
        sb.Append(prefix).AppendFormat("*%s = HdfSbufReadFileDescriptor(%s);\n", name.string(), parcelName.string());
        sb.Append(prefix).AppendFormat("if (*%s < 0) {\n", name.string());
    }
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTFdType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = HdfSbufReadFileDescriptor(%s);\n", name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("if (%s < 0) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTFdType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteFileDescriptor(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTFdType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadFileDescriptor();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadFileDescriptor();\n", name.string(), parcelName.string());
    }
}

void ASTFdType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteFileDescriptor(data, %s)) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTFdType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb, const String &prefix,
    std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("%s = HdfSbufReadFileDescriptor(data);\n", name.string());
    sb.Append(prefix).AppendFormat("if (%s < 0) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTFdType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteFileDescriptor(%s)) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTFdType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadFileDescriptor();\n", EmitCppType().string(), name.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadFileDescriptor();\n", name.string(), parcelName.string());
    }
}

void ASTFdType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s);\n", parcelName.string(), name.string());
}

void ASTFdType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = %s.readInt();\n", name.string(), parcelName.string());
}

void ASTFdType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "%s %s = %s.readInt();\n", EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
}
} // namespace HDI
} // namespace OHOS