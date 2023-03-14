/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_array_type.h"
#include "util/options.h"

namespace OHOS {
namespace HDI {
bool ASTArrayType::IsArrayType()
{
    return true;
}

String ASTArrayType::ToString()
{
    return String::Format("%s[]", elementType_->ToString().string());
}

TypeKind ASTArrayType::GetTypeKind()
{
    return TypeKind::TYPE_ARRAY;
}

String ASTArrayType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("%s*", elementType_->EmitCType(TypeMode::NO_MODE).string());
        case TypeMode::PARAM_IN: {
            if (elementType_->GetTypeKind() == TypeKind::TYPE_STRING) {
                return String::Format("%s*", elementType_->EmitCType(TypeMode::NO_MODE).string());
            }
            return String::Format("const %s*", elementType_->EmitCType(TypeMode::NO_MODE).string());
        }
        case TypeMode::PARAM_OUT:
            return String::Format("%s*", elementType_->EmitCType(TypeMode::NO_MODE).string());
        case TypeMode::LOCAL_VAR:
            return String::Format("%s*", elementType_->EmitCType(TypeMode::NO_MODE).string());
        default:
            return "unknow type";
    }
}

String ASTArrayType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("std::vector<%s>", elementType_->EmitCppType(TypeMode::NO_MODE).string());
        case TypeMode::PARAM_IN:
            return String::Format("const std::vector<%s>&", elementType_->EmitCppType(TypeMode::NO_MODE).string());
        case TypeMode::PARAM_OUT:
            return String::Format("std::vector<%s>&", elementType_->EmitCppType(TypeMode::NO_MODE).string());
        case TypeMode::LOCAL_VAR:
            return String::Format("std::vector<%s>", elementType_->EmitCppType(TypeMode::NO_MODE).string());
        default:
            return "unknow type";
    }
}

String ASTArrayType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return String::Format("%s[]", elementType_->EmitJavaType(TypeMode::NO_MODE, false).string());
}

void ASTArrayType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String lenName = String::Format("%sLen", name.string());
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(%s, %s)) {\n", parcelName.string(), lenName.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.string());
    } else {
        sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.string());
    }

    String elementName = "";
    if (elementType_->GetTypeKind() == TypeKind::TYPE_STRUCT || elementType_->GetTypeKind() == TypeKind::TYPE_UNION) {
        elementName = String::Format("&%s[i]", name.string());
    } else {
        elementName = String::Format("%s[i]", name.string());
    }

    elementType_->EmitCWriteVar(parcelName, elementName, ecName, gotoLabel, sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCProxyWriteOutVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String lenName = String::Format("*%sLen", name.string());
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(%s, %s)) {\n", parcelName.string(), lenName.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String lenName = String::Format("%sLen", name.string());
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(%s, %s)) {\n", parcelName.string(), lenName.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("for (i = 0; i < *%s; i++) {\n", lenName.string());
    } else {
        sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < *%s; i++) {\n", lenName.string());
    }

    if (elementType_->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitCProxyReadStrElement(parcelName, name, ecName, gotoLabel, sb, prefix);
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        String element = String::Format("&%s[i]", name.string());
        elementType_->EmitCProxyReadVar(parcelName, element, true, ecName, gotoLabel, sb, prefix + TAB);
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_UNION) {
        String element = String::Format("&%s[i]", name.string());
        String elementCp = String::Format("%sElementCp", name.string());
        elementType_->EmitCProxyReadVar(parcelName, elementCp, true, ecName, gotoLabel, sb, prefix + TAB);
        sb.Append(prefix + TAB).AppendFormat("(void)memcpy_s(%s, sizeof(%s), %s, sizeof(%s));\n", element.string(),
            elementType_->EmitCType().string(), elementCp.string(), elementType_->EmitCType().string());
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_FILEDESCRIPTOR) {
        String element = String::Format("%s[i]", name.string());
        elementType_->EmitCProxyReadVar(parcelName, element, true, ecName, gotoLabel, sb, prefix + TAB);
    } else {
        String element = String::Format("&%s[i]", name.string());
        elementType_->EmitCProxyReadVar(parcelName, element, true, ecName, gotoLabel, sb, prefix + TAB);
    }
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String lenName = String::Format("%sLen", name.string());
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(%s, &%s)) {\n", parcelName.string(), lenName.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("if (%s > 0) {\n", lenName.string());
    EmitCMallocVar(name, lenName, false, ecName, gotoLabel, sb, prefix + TAB);
    sb.Append("\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix + TAB).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.string());
    } else {
        sb.Append(prefix + TAB).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.string());
    }
    if (elementType_->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitCStubReadStrElement(parcelName, name, ecName, gotoLabel, sb, prefix);
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        String element = String::Format("&%s[i]", name.string());
        elementType_->EmitCStubReadVar(parcelName, element, ecName, gotoLabel, sb, prefix + TAB + TAB);
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_UNION) {
        String element = String::Format("%s[i]", name.string());
        String elementCp = String::Format("%sElementCp", name.string());
        elementType_->EmitCStubReadVar(parcelName, elementCp, ecName, gotoLabel, sb, prefix + TAB + TAB);
        sb.Append(prefix + TAB + TAB).AppendFormat("(void)memcpy_s(&%s, sizeof(%s), %s, sizeof(%s));\n",
            element.string(), elementType_->EmitCType().string(), elementCp.string(),
            elementType_->EmitCType().string());
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_FILEDESCRIPTOR) {
        String element = String::Format("%s[i]", name.string());
        elementType_->EmitCStubReadVar(parcelName, element, ecName, gotoLabel, sb, prefix + TAB + TAB);
    } else {
        String element = String::Format("&%s[i]", name.string());
        elementType_->EmitCStubReadVar(parcelName, element, ecName, gotoLabel, sb, prefix + TAB + TAB);
    }
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCStubReadOutVar(const String &buffSizeName, const String &memFlagName, const String &parcelName,
    const String &name, const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String lenName = String::Format("%sLen", name.string());

    sb.Append(prefix).AppendFormat("if (%s) {\n", memFlagName.string());
    sb.Append(prefix + TAB)
        .AppendFormat("if (!HdfSbufReadUint32(%s, &%s)) {\n", parcelName.string(), lenName.string());
    sb.Append(prefix + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).AppendFormat("if (%s > 0) {\n", lenName.string());
    EmitCMallocVar(name, lenName, false, ecName, gotoLabel, sb, prefix + TAB + TAB);
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("} else {\n");
    sb.Append(prefix + TAB).AppendFormat("%s = (%s*)OsalMemCalloc(%s);\n", name.string(),
        elementType_->EmitCType(TypeMode::NO_MODE).string(), buffSizeName.string());
    sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", name.string());
    sb.Append(prefix + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", name.string());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", ecName.string());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix + TAB).Append("}\n");

    sb.Append(prefix + TAB).AppendFormat("%sLen = (%s / sizeof(%s));\n", name.string(), buffSizeName.string(),
        elementType_->EmitCType(TypeMode::NO_MODE).string());
    sb.Append(prefix).Append("}\n\n");
}

void ASTArrayType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: write %s.size() failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
    String elementName = String::Format("it%d", innerLevel++);
    sb.Append(prefix).AppendFormat("for (const auto& %s : %s) {\n", elementName.string(), name.string());

    elementType_->EmitCppWriteVar(parcelName, elementName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), name.string());
    }
    sb.Append(prefix).AppendFormat("uint32_t %sSize = %s.ReadUint32();\n", name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("%s.reserve(%sSize);\n", name.string(), name.string());
    sb.Append(prefix).AppendFormat(
        "for (uint32_t i%d = 0; i%d < %sSize; ++i%d) {\n", innerLevel, innerLevel, name.string(), innerLevel);

    String valueName = String::Format("value%d", innerLevel++);
    elementType_->EmitCppReadVar(parcelName, valueName, sb, prefix + TAB, true, innerLevel);
    sb.Append(prefix + TAB).AppendFormat("%s.push_back(%s);\n", name.string(), valueName.string());
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(data, %sLen)) {\n", name.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: write %sLen failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("for (i = 0; i < %sLen; i++) {\n", name.string());
    } else {
        sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %sLen; i++) {\n", name.string());
    }

    String elementName = String::Format("(%s)[i]", name.string());
    elementType_->EmitCMarshalling(elementName, sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &prefix, std::vector<String> &freeObjStatements) const
{
    String lenName = String::Format("%sLen", name.string());
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(data, &%s)) {\n", lenName.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", lenName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).AppendFormat("if (%s > 0) {\n", lenName.string());
    String newPrefix = prefix + TAB;

    sb.Append(newPrefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s) * %s);\n", name.string(),
        elementType_->EmitCType().string(), elementType_->EmitCType().string(), lenName.string());
    sb.Append(newPrefix).AppendFormat("if (%s == NULL) {\n", name.string());
    sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(newPrefix).Append("}\n");
    freeObjStatements.push_back(String::Format("OsalMemFree(%s);\n", name.string()));

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(newPrefix).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.string());
    } else {
        sb.Append(newPrefix).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.string());
    }

    if (elementType_->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitCStringElementUnMarshalling(name, gotoLabel, sb, newPrefix, freeObjStatements);
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        String element = String::Format("&%s[i]", name.string());
        elementType_->EmitCUnMarshalling(element, gotoLabel, sb, newPrefix + TAB, freeObjStatements);
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_UNION) {
        String element = String::Format("%s[i]", name.string());
        String elementCp = String::Format("%sElementCp", name.string());
        elementType_->EmitCUnMarshalling(elementCp, gotoLabel, sb, newPrefix + TAB, freeObjStatements);
        sb.Append(newPrefix + TAB).AppendFormat("(void)memcpy_s(&%s, sizeof(%s), %s, sizeof(%s));\n", element.string(),
            elementType_->EmitCType().string(), elementCp.string(), elementType_->EmitCType().string());
    } else {
        String element = String::Format("%s[i]", name.string());
        elementType_->EmitCUnMarshalling(element, gotoLabel, sb, newPrefix + TAB, freeObjStatements);
    }
    sb.Append(newPrefix).Append("}\n");
    sb.Append(prefix).Append("}\n");
    freeObjStatements.pop_back();
}

void ASTArrayType::EmitCStringElementUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
    const String &newPrefix, std::vector<String> &freeObjStatements) const
{
    String element = String::Format("%sElement", name.string());
    elementType_->EmitCUnMarshalling(element, gotoLabel, sb, newPrefix + TAB, freeObjStatements);
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(newPrefix).AppendFormat(
            "%s[i] = (char*)OsalMemCalloc(strlen(%s) + 1);\n", name.string(), element.string());
        sb.Append(newPrefix).AppendFormat("if (%s[i] == NULL) {\n", name.string());
        sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(newPrefix).Append("}\n\n");

        sb.Append(newPrefix).AppendFormat("if (strcpy_s((%s)[i], (strlen(%s) + 1), %s) != HDF_SUCCESS) {\n",
            name.string(), element.string(), element.string());
        sb.Append(newPrefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", element.string());
        sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(newPrefix).Append("}\n");
    } else {
        sb.Append(newPrefix).Append(TAB).AppendFormat("%s[i] = strdup(%s);\n", name.string(), element.string());
    }
}

void ASTArrayType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: write %s.size failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
    String elementName = String::Format("it%d", innerLevel++);
    sb.Append(prefix).AppendFormat("for (const auto& %s : %s) {\n", elementName.string(), name.string());

    elementType_->EmitCppMarshalling(parcelName, elementName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    int index = name.IndexOf('.', 0);
    String memberName = name.Substring(index + 1);
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), memberName.string());
    }
    sb.Append(prefix).AppendFormat("uint32_t %sSize = %s.ReadUint32();\n", memberName.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("%s.reserve(%sSize);\n", name.string(), memberName.string());
    sb.Append(prefix).AppendFormat(
        "for (uint32_t i%d = 0; i%d < %sSize; ++i%d) {\n", innerLevel, innerLevel, memberName.string(), innerLevel);

    String valueName = String::Format("value%d", innerLevel++);
    if (elementType_->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        sb.Append(prefix + TAB).AppendFormat("%s %s;\n", elementType_->EmitCppType().string(), valueName.string());
        elementType_->EmitCppUnMarshalling(parcelName, valueName, sb, prefix + TAB, true, innerLevel);
        sb.Append(prefix + TAB).AppendFormat("%s.push_back(%s);\n", name.string(), valueName.string());
    } else if (elementType_->GetTypeKind() == TypeKind::TYPE_UNION) {
        sb.Append(prefix + TAB).AppendFormat("%s %s;\n", elementType_->EmitCppType().string(), valueName.string());
        String cpName = String::Format("%sCp", valueName.string());
        elementType_->EmitCppUnMarshalling(parcelName, cpName, sb, prefix + TAB, true, innerLevel);
        sb.Append(prefix + TAB).AppendFormat("(void)memcpy_s(&%s, sizeof(%s), %s, sizeof(%s));\n", valueName.string(),
            elementType_->EmitCppType().string(), cpName.string(), elementType_->EmitCppType().string());
        sb.Append(prefix + TAB).AppendFormat("%s.push_back(%s);\n", name.string(), valueName.string());
    } else {
        elementType_->EmitCppUnMarshalling(parcelName, valueName, sb, prefix + TAB, true, innerLevel);
        sb.Append(prefix + TAB).AppendFormat("%s.push_back(%s);\n", name.string(), valueName.string());
    }
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitMemoryRecycle(
    const String &name, bool isClient, bool ownership, StringBuilder &sb, const String &prefix) const
{
    String varName = name;
    String lenName = isClient ? String::Format("*%sLen", name.string()) : String::Format("%sLen", name.string());

    sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", varName.string());
    if (elementType_->GetTypeKind() == TypeKind::TYPE_STRING || elementType_->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        if (Options::GetInstance().DoGenerateKernelCode()) {
            sb.Append(prefix + TAB).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.string());
        } else {
            sb.Append(prefix + TAB).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.string());
        }

        String elementName = String::Format("%s[i]", varName.string());
        elementType_->EmitMemoryRecycle(elementName, false, false, sb, prefix + TAB + TAB);
        sb.Append(prefix + TAB).Append("}\n");
    }

    sb.Append(prefix + TAB).AppendFormat("OsalMemFree(%s);\n", varName.string());
    if (isClient) {
        sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", varName.string());
    }

    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s == null) {\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s.writeInt(-1);\n", parcelName.string());
    sb.Append(prefix).Append("} else { \n");
    EmitJavaWriteArrayVar(parcelName, name, sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    switch (elementType_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
            sb.Append(prefix).AppendFormat("%s.readBooleanArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_BYTE:
            sb.Append(prefix).AppendFormat("%s.readByteArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_SHORT:
            sb.Append(prefix).AppendFormat("%s.readShortArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(prefix).AppendFormat("%s.readIntArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_LONG:
            sb.Append(prefix).AppendFormat("%s.readLongArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_FLOAT:
            sb.Append(prefix).AppendFormat("%s.readFloatArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_DOUBLE:
            sb.Append(prefix).AppendFormat("%s.readDoubleArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("%s.readStringArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(prefix).AppendFormat("%s.readSequenceableArray(%s);\n", parcelName.string(), name.string());
            break;
        default:
            break;
    }
}

void ASTArrayType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    switch (elementType_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readBooleanArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_BYTE:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readByteArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_SHORT:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readShortArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readIntArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_LONG:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readLongArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_FLOAT:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readFloatArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_DOUBLE:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readDoubleArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readStringArray();\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(), parcelName.string());
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(prefix).AppendFormat("int size = %s.readInt();\n", parcelName.string());
            sb.Append(prefix).AppendFormat("%s %s = new %s[size];\n",
                elementType_->EmitJavaType(TypeMode::NO_MODE).string(), name.string(),
                elementType_->EmitJavaType(TypeMode::NO_MODE).string());
            sb.Append(prefix).AppendFormat("for (int i = 0; i < size; ++i) {\n");
            elementType_->EmitJavaReadInnerVar(parcelName, "value", true, sb, prefix + TAB);
            sb.Append(prefix + TAB).AppendFormat("%s[i] = value;\n", name.string());
            sb.Append(prefix).Append("}\n");
            break;
        default:
            break;
    }
}

void ASTArrayType::EmitJavaWriteArrayVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    switch (elementType_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
            sb.Append(prefix).AppendFormat("%s.writeBooleanArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_BYTE:
            sb.Append(prefix).AppendFormat("%s.writeByteArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_SHORT:
            sb.Append(prefix).AppendFormat("%s.writeShortArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(prefix).AppendFormat("%s.writeIntArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_LONG:
            sb.Append(prefix).AppendFormat("%s.writeLongArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_FLOAT:
            sb.Append(prefix).AppendFormat("%s.writeFloatArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_DOUBLE:
            sb.Append(prefix).AppendFormat("%s.writeDoubleArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("%s.writeStringArray(%s);\n", parcelName.string(), name.string());
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(prefix).AppendFormat("%s.writeSequenceableArray(%s);\n", parcelName.string(), name.string());
            break;
        default:
            break;
    }
}

void ASTArrayType::EmitCMallocVar(const String &name, const String &lenName, bool isClient, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String varName = isClient ? String::Format("*%s", name.string()) : name;
    String lenVarName = isClient ? String::Format("*%s", lenName.string()) : lenName;

    sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s) * (%s));\n", varName.string(),
        elementType_->EmitCType().string(), elementType_->EmitCType().string(), lenVarName.string());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", varName.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", varName.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTArrayType::EmitCProxyReadStrElement(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String cpName = String::Format("%sCp", name.string());
    elementType_->EmitCProxyReadVar(parcelName, cpName, true, ecName, gotoLabel, sb, prefix + TAB);
    sb.Append("\n");
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix + TAB)
            .AppendFormat("%s[i] = (char*)OsalMemCalloc(strlen(%s) + 1);\n", name.string(), cpName.string());
        sb.Append(prefix + TAB).AppendFormat("if (%s[i] == NULL) {\n", name.string());
        sb.Append(prefix + TAB + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s[i] failed\", __func__);\n", name.string());
        sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", ecName.string());
        sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).AppendFormat("if (strcpy_s(%s[i], (strlen(%s) + 1), %s) != HDF_SUCCESS) {\n",
            name.string(), cpName.string(), cpName.string());
        sb.Append(prefix + TAB + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", cpName.string());
        sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
        sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix + TAB).Append("}\n");
    } else {
        sb.Append(prefix + TAB).AppendFormat("%s[i] = strdup(%sCp);\n", name.string(), name.string());
    }
}

void ASTArrayType::EmitCStubReadStrElement(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String element = String::Format("%sCp", name.string());
    String newPrefix = prefix + TAB + TAB;
    elementType_->EmitCStubReadVar(parcelName, element, ecName, gotoLabel, sb, newPrefix);
    sb.Append("\n");
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(newPrefix).AppendFormat(
            "%s[i] = (char*)OsalMemCalloc(strlen(%s) + 1);\n", name.string(), element.string());
        sb.Append(newPrefix).AppendFormat("if (%s[i] == NULL) {\n", name.string());
        sb.Append(newPrefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", ecName.string());
        sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(newPrefix).Append("}\n\n");

        sb.Append(newPrefix).AppendFormat("if (strcpy_s(%s[i], (strlen(%s) + 1), %s) != HDF_SUCCESS) {\n",
            name.string(), element.string(), element.string());
        sb.Append(newPrefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", element.string());
        sb.Append(newPrefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
        sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(newPrefix).Append("}\n");
    } else {
        sb.Append(newPrefix).AppendFormat("%s[i] = strdup(%sCp);\n", name.string(), name.string());
    }
}
} // namespace HDI
} // namespace OHOS