/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/c_custom_types_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/options.h"

namespace OHOS {
namespace HDI {
bool CCustomTypesCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() != ASTFileType::AST_TYPES) {
        return false;
    }

    directory_ = GetFilePath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("CCustomTypesCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CCustomTypesCodeEmitter::EmitCode()
{
    EmitCustomTypesHeaderFile();
    EmitCustomTypesSourceFile();
}

void CCustomTypesCodeEmitter::EmitCustomTypesHeaderFile()
{
    String filePath = String::Format("%s/%s.h", directory_.string(), FileName(baseName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, baseName_);
    sb.Append("\n");
    EmitHeaderInclusions(sb);
    sb.Append("\n");
    EmitHeadExternC(sb);
    sb.Append("\n");
    EmitCustomTypeDecls(sb);
    sb.Append("\n");
    EmitCustomTypeFuncDecl(sb);
    sb.Append("\n");
    EmitTailExternC(sb);
    sb.Append("\n");
    EmitTailMacro(sb, baseName_);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CCustomTypesCodeEmitter::EmitHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    GetHeaderOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CCustomTypesCodeEmitter::GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_sbuf"));
            break;
        }
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeDecls(StringBuilder &sb)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        EmitCustomTypeDecl(sb, type);
        if (i + 1 < ast_->GetTypeDefinitionNumber()) {
            sb.Append("\n");
        }
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeDecl(StringBuilder &sb, const AutoPtr<ASTType> &type)
{
    switch (type->GetTypeKind()) {
        case TypeKind::TYPE_ENUM: {
            AutoPtr<ASTEnumType> enumType = dynamic_cast<ASTEnumType *>(type.Get());
            sb.Append(enumType->EmitCTypeDecl()).Append("\n");
            break;
        }
        case TypeKind::TYPE_STRUCT: {
            AutoPtr<ASTStructType> structType = dynamic_cast<ASTStructType *>(type.Get());
            sb.Append(structType->EmitCTypeDecl()).Append("\n");
            break;
        }
        case TypeKind::TYPE_UNION: {
            AutoPtr<ASTUnionType> unionType = dynamic_cast<ASTUnionType *>(type.Get());
            sb.Append(unionType->EmitCTypeDecl()).Append("\n");
            break;
        }
        default:
            break;
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeFuncDecl(StringBuilder &sb)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            AutoPtr<ASTStructType> structType = dynamic_cast<ASTStructType *>(type.Get());
            EmitCustomTypeMarshallingDecl(sb, structType);
            sb.Append("\n");
            EmitCustomTypeUnmarshallingDecl(sb, structType);
            sb.Append("\n");
            EmitCustomTypeFreeDecl(sb, structType);
            if (i + 1 < ast_->GetTypeDefinitionNumber()) {
                sb.Append("\n");
            }
        }
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeMarshallingDecl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    sb.AppendFormat("bool %sBlockMarshalling(struct HdfSBuf *data, const %s *%s);\n", type->GetName().string(),
        type->EmitCType().string(), objName.string());
}

void CCustomTypesCodeEmitter::EmitCustomTypeUnmarshallingDecl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    sb.AppendFormat("bool %sBlockUnmarshalling(struct HdfSBuf *data, %s *%s);\n", type->GetName().string(),
        type->EmitCType().string(), objName.string());
}

void CCustomTypesCodeEmitter::EmitCustomTypeFreeDecl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    sb.AppendFormat("void %sFree(%s *%s, bool freeSelf);\n", type->GetName().string(), type->EmitCType().string(),
        objName.string());
}

void CCustomTypesCodeEmitter::EmitCustomTypesSourceFile()
{
    String filePath = String::Format("%s/%s.c", directory_.string(), FileName(baseName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitSoucreInclusions(sb);
    sb.Append("\n");
    EmitCustomTypeDataProcess(sb);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CCustomTypesCodeEmitter::EmitSoucreInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(baseName_)));
    GetSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CCustomTypesCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "osal_mem"));

    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT || type->GetTypeKind() == TypeKind::TYPE_UNION) {
            headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
            break;
        }
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeDataProcess(StringBuilder &sb)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            AutoPtr<ASTStructType> structType = dynamic_cast<ASTStructType *>(type.Get());
            EmitCustomTypeMarshallingImpl(sb, structType);
            sb.Append("\n");
            EmitCustomTypeUnmarshallingImpl(sb, structType);
            sb.Append("\n");
            EmitCustomTypeFreeImpl(sb, structType);
            if (i + 1 < ast_->GetTypeDefinitionNumber()) {
                sb.Append("\n");
            }
        }
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeMarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    sb.AppendFormat("bool %sBlockMarshalling(struct HdfSBuf *data, const %s *%s)\n", type->GetName().string(),
        type->EmitCType().string(), objName.string());
    sb.Append("{\n");

    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        if (isKernelCode_) {
            if (memberType->GetTypeKind() == TypeKind::TYPE_ARRAY || memberType->GetTypeKind() == TypeKind::TYPE_LIST) {
                sb.Append(TAB).Append("uint32_t i = 0;\n");
                break;
            }
        }
    }

    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        String memberName = type->GetMemberName(i);
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        String name = String::Format("%s->%s", objName.string(), memberName.string());
        memberType->EmitCMarshalling(name, sb, TAB);
        sb.Append("\n");
    }

    sb.Append(TAB).Append("return true;\n");
    sb.Append("}\n");
}

void CCustomTypesCodeEmitter::EmitCustomTypeUnmarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    freeObjStatements_.clear();

    sb.AppendFormat("bool %sBlockUnmarshalling(struct HdfSBuf *data, %s *%s)\n", type->GetName().string(),
        type->EmitCType().string(), objName.string());
    sb.Append("{\n");

    if (isKernelCode_) {
        for (size_t i = 0; i < type->GetMemberNumber(); i++) {
            AutoPtr<ASTType> memberType = type->GetMemberType(i);
            if (memberType->GetTypeKind() == TypeKind::TYPE_ARRAY || memberType->GetTypeKind() == TypeKind::TYPE_LIST) {
                sb.Append(TAB).Append("uint32_t i = 0;\n");
                break;
            }
        }
    }

    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.string());
    sb.Append(TAB).Append(TAB).Append("return false;\n");
    sb.Append(TAB).Append("}\n");

    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        EmitMemberUnmarshalling(memberType, objName, type->GetMemberName(i), sb, TAB);
    }

    sb.Append(TAB).AppendFormat("return true;\n");
    sb.AppendFormat("%s:\n", errorsLabelName_);
    EmitCustomTypeMemoryRecycle(type, objName, sb, TAB);

    sb.Append(TAB).Append("return false;\n");
    sb.Append("}\n");
}

void CCustomTypesCodeEmitter::EmitMemberUnmarshalling(
    const AutoPtr<ASTType> &type, const String &name, const String &memberName, StringBuilder &sb, const String &prefix)
{
    String varName = String::Format("%s->%s", name.string(), memberName.string());
    switch (type->GetTypeKind()) {
        case TypeKind::TYPE_STRING: {
            String tmpName = String::Format("%sCp", memberName.string());
            type->EmitCUnMarshalling(tmpName, errorsLabelName_, sb, TAB, freeObjStatements_);

            if (Options::GetInstance().DoGenerateKernelCode()) {
                sb.Append(TAB).AppendFormat(
                    "%s = (char*)OsalMemCalloc(strlen(%s) + 1);\n", varName.string(), tmpName.string());
                sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", varName.string());
                sb.Append(TAB).Append(TAB).AppendFormat("goto %s;\n", errorsLabelName_);
                sb.Append(TAB).Append("}\n");

                sb.Append(TAB).AppendFormat("if (strcpy_s(%s, (strlen(%s) + 1), %s) != HDF_SUCCESS) {\n",
                    varName.string(), tmpName.string(), tmpName.string());
                sb.Append(TAB).Append(TAB).AppendFormat("goto %s;\n", errorsLabelName_);
                sb.Append(TAB).Append("}\n");
            } else {
                sb.Append(TAB).AppendFormat("%s = strdup(%s);\n", varName.string(), tmpName.string());
            }

            sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", varName.string());
            sb.Append(TAB).Append(TAB).AppendFormat("goto %s;\n", errorsLabelName_);
            sb.Append(TAB).Append("}\n");
            sb.Append("\n");
            break;
        }
        case TypeKind::TYPE_STRUCT: {
            String paramName = String::Format("&%s", varName.string());
            type->EmitCUnMarshalling(paramName, errorsLabelName_, sb, TAB, freeObjStatements_);
            sb.Append("\n");
            break;
        }
        case TypeKind::TYPE_UNION: {
            String tmpName = String::Format("%sCp", memberName.string());
            type->EmitCUnMarshalling(tmpName, errorsLabelName_, sb, TAB, freeObjStatements_);
            sb.Append(TAB).AppendFormat("(void)memcpy_s(&%s, sizeof(%s), %s, sizeof(%s));\n", varName.string(),
                type->EmitCType().string(), tmpName.string(), type->EmitCType().string());
            sb.Append("\n");
            break;
        }
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST: {
            EmitArrayMemberUnmarshalling(type, memberName, varName, sb, TAB);
            sb.Append("\n");
            break;
        }
        default: {
            type->EmitCUnMarshalling(varName, errorsLabelName_, sb, TAB, freeObjStatements_);
            sb.Append("\n");
        }
    }
}

void CCustomTypesCodeEmitter::EmitArrayMemberUnmarshalling(const AutoPtr<ASTType> &type, const String &memberName,
    const String &varName, StringBuilder &sb, const String &prefix)
{
    String tmpName = String::Format("%sCp", memberName.string());
    AutoPtr<ASTType> elementType = nullptr;
    if (type->GetTypeKind() == TypeKind::TYPE_ARRAY) {
        AutoPtr<ASTArrayType> arrayType = dynamic_cast<ASTArrayType *>(type.Get());
        elementType = arrayType->GetElementType();
    } else {
        AutoPtr<ASTListType> listType = dynamic_cast<ASTListType *>(type.Get());
        elementType = listType->GetElementType();
    }

    sb.Append(prefix).AppendFormat("%s* %s = NULL;\n", elementType->EmitCType().string(), tmpName.string());
    sb.Append(prefix).AppendFormat("uint32_t %sLen = 0;\n", tmpName.string());
    type->EmitCUnMarshalling(tmpName, errorsLabelName_, sb, prefix, freeObjStatements_);
    sb.Append(prefix).AppendFormat("%s = %s;\n", varName.string(), tmpName.string());
    sb.Append(prefix).AppendFormat("%sLen = %sLen;\n", varName.string(), tmpName.string());
}

void CCustomTypesCodeEmitter::EmitCustomTypeFreeImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    sb.AppendFormat(
        "void %sFree(%s *%s, bool freeSelf)\n", type->GetName().string(), type->EmitCType().string(), objName.string());
    sb.Append("{\n");

    if (isKernelCode_) {
        for (size_t i = 0; i < type->GetMemberNumber(); i++) {
            AutoPtr<ASTType> memberType = type->GetMemberType(i);
            if (NeedEmitInitVar(memberType)) {
                sb.Append(TAB).Append("uint32_t i = 0;\n");
                break;
            }
        }
    }

    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.string());
    sb.Append(TAB).Append(TAB).Append("return;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append("\n");

    EmitCustomTypeMemoryRecycle(type, objName, sb, TAB);

    sb.Append(TAB).Append("if (freeSelf) {\n");
    sb.Append(TAB).Append(TAB).Append("OsalMemFree(dataBlock);\n");
    sb.Append(TAB).Append("}\n");
    sb.Append("}\n");
}

bool CCustomTypesCodeEmitter::NeedEmitInitVar(const AutoPtr<ASTType> &type)
{
    if (type == nullptr) {
        return false;
    }

    if (type->IsArrayType()) {
        AutoPtr<ASTArrayType> ArrType = dynamic_cast<ASTArrayType *>(type.Get());
        AutoPtr<ASTType> elementType = ArrType->GetElementType();
        if (elementType->IsStringType() || elementType->IsStructType()) {
            return true;
        }
    } else if (type->IsListType()) {
        AutoPtr<ASTListType> ListType = dynamic_cast<ASTListType *>(type.Get());
        AutoPtr<ASTType> elementType = ListType->GetElementType();
        if (elementType->IsStringType() || elementType->IsStructType()) {
            return true;
        }
    }

    return false;
}

void CCustomTypesCodeEmitter::EmitCustomTypeMemoryRecycle(
    const AutoPtr<ASTStructType> &type, const String &name, StringBuilder &sb, const String &prefix)
{
    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        String memberName = type->GetMemberName(i);
        String varName = String::Format("%s->%s", name.string(), memberName.string());
        switch (memberType->GetTypeKind()) {
            case TypeKind::TYPE_STRING:
            case TypeKind::TYPE_STRUCT:
            case TypeKind::TYPE_ARRAY:
            case TypeKind::TYPE_LIST:
                memberType->EmitMemoryRecycle(varName, false, false, sb, prefix);
                sb.Append("\n");
                break;
            default:
                break;
        }
    }
}
} // namespace HDI
} // namespace OHOS