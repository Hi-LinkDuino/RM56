/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/cpp_custom_types_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
bool CppCustomTypesCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() != ASTFileType::AST_TYPES) {
        return false;
    }

    directory_ = GetFilePath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("CppCustomTypesCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CppCustomTypesCodeEmitter::EmitCode()
{
    EmitCustomTypesHeaderFile();
    EmitCustomTypesSourceFile();
}

void CppCustomTypesCodeEmitter::EmitCustomTypesHeaderFile()
{
    String filePath = String::Format("%s/%s.h", directory_.string(), FileName(baseName_).string());
    File file(filePath, File::WRITE);
    String marcoName = String::Format("%s.%s", ast_->GetPackageName().string(), baseName_.string());
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, marcoName);
    sb.Append("\n");
    EmitHeaderFileInclusions(sb);
    sb.Append("\n");
    EmitBeginNamespace(sb);
    sb.Append("\n");
    EmitUsingNamespace(sb);
    sb.Append("\n");
    EmitCustomTypeDecls(sb);
    sb.Append("\n");
    EmitCustomTypeFuncDecl(sb);
    sb.Append("\n");
    EmitEndNamespace(sb);
    sb.Append("\n");
    EmitTailMacro(sb, marcoName);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CppCustomTypesCodeEmitter::EmitHeaderFileInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    GetStdlibInclusions(headerFiles);
    GetImportInclusions(headerFiles);
    GetHeaderOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CppCustomTypesCodeEmitter::GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "message_parcel"));
}

void CppCustomTypesCodeEmitter::EmitCustomTypeDecls(StringBuilder &sb)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        EmitCustomTypeDecl(sb, type);
        if (i + 1 < ast_->GetTypeDefinitionNumber()) {
            sb.Append("\n");
        }
    }
}

void CppCustomTypesCodeEmitter::EmitCustomTypeDecl(StringBuilder &sb, const AutoPtr<ASTType> &type)
{
    switch (type->GetTypeKind()) {
        case TypeKind::TYPE_ENUM: {
            AutoPtr<ASTEnumType> enumType = dynamic_cast<ASTEnumType *>(type.Get());
            sb.Append(enumType->EmitCppTypeDecl()).Append("\n");
            break;
        }
        case TypeKind::TYPE_STRUCT: {
            AutoPtr<ASTStructType> structType = dynamic_cast<ASTStructType *>(type.Get());
            sb.Append(structType->EmitCppTypeDecl()).Append("\n");
            break;
        }
        case TypeKind::TYPE_UNION: {
            AutoPtr<ASTUnionType> unionType = dynamic_cast<ASTUnionType *>(type.Get());
            sb.Append(unionType->EmitCppTypeDecl()).Append("\n");
            break;
        }
        default:
            break;
    }
}

void CppCustomTypesCodeEmitter::EmitCustomTypeFuncDecl(StringBuilder &sb)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            AutoPtr<ASTStructType> structType = dynamic_cast<ASTStructType *>(type.Get());
            EmitCustomTypeMarshallingDecl(sb, structType);
            sb.Append("\n");
            EmitCustomTypeUnmarshallingDecl(sb, structType);
            if (i + 1 < ast_->GetTypeDefinitionNumber()) {
                sb.Append("\n");
            }
        }
    }
}

void CppCustomTypesCodeEmitter::EmitCustomTypeMarshallingDecl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    sb.AppendFormat("bool %sBlockMarshalling(OHOS::MessageParcel &data, const %s& %s);\n", type->GetName().string(),
        type->EmitCppType().string(), objName.string());
}

void CppCustomTypesCodeEmitter::EmitCustomTypeUnmarshallingDecl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");
    sb.AppendFormat("bool %sBlockUnmarshalling(OHOS::MessageParcel &data, %s& %s);\n", type->GetName().string(),
        type->EmitCppType().string(), objName.string());
}

void CppCustomTypesCodeEmitter::EmitCustomTypesSourceFile()
{
    String filePath = String::Format("%s/%s.cpp", directory_.string(), FileName(baseName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitSourceFileInclusions(sb);
    sb.Append("\n");
    EmitBeginNamespace(sb);
    sb.Append("\n");
    EmitCustomTypeDataProcess(sb);
    sb.Append("\n");
    EmitEndNamespace(sb);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CppCustomTypesCodeEmitter::EmitSourceFileInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(baseName_)));
    GetSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CppCustomTypesCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log"));
    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT || type->GetTypeKind() == TypeKind::TYPE_UNION) {
            headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
            break;
        }
    }
}

void CppCustomTypesCodeEmitter::EmitCustomTypeDataProcess(StringBuilder &sb)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            AutoPtr<ASTStructType> structType = dynamic_cast<ASTStructType *>(type.Get());
            EmitCustomTypeMarshallingImpl(sb, structType);
            sb.Append("\n");
            EmitCustomTypeUnmarshallingImpl(sb, structType);
            if (i + 1 < ast_->GetTypeDefinitionNumber()) {
                sb.Append("\n");
            }
        }
    }
}

void CppCustomTypesCodeEmitter::EmitCustomTypeMarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");

    sb.AppendFormat("bool %sBlockMarshalling(OHOS::MessageParcel& data, const %s& %s)\n", type->EmitCppType().string(),
        type->EmitCppType().string(), objName.string());
    sb.Append("{\n");
    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        String memberName = type->GetMemberName(i);

        String name = String::Format("%s.%s", objName.string(), memberName.string());
        memberType->EmitCppMarshalling("data", name, sb, TAB);
        if (i + 1 < type->GetMemberNumber()) {
            sb.Append("\n");
        }
    }

    sb.Append(TAB).Append("return true;\n");
    sb.Append("}\n");
}

void CppCustomTypesCodeEmitter::EmitCustomTypeUnmarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    String objName("dataBlock");

    sb.AppendFormat("bool %sBlockUnmarshalling(OHOS::MessageParcel& data, %s& %s)\n", type->GetName().string(),
        type->EmitCppType().string(), objName.string());
    sb.Append("{\n");

    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        String memberName = type->GetMemberName(i);
        String name = String::Format("%s.%s", objName.string(), memberName.string());

        if (i > 0 &&
            (memberType->GetTypeKind() == TypeKind::TYPE_STRUCT || memberType->GetTypeKind() == TypeKind::TYPE_UNION ||
                memberType->GetTypeKind() == TypeKind::TYPE_ARRAY ||
                memberType->GetTypeKind() == TypeKind::TYPE_LIST)) {
            sb.Append("\n");
        }

        if (memberType->GetTypeKind() == TypeKind::TYPE_UNION) {
            String cpName = String::Format("%sCp", memberName.string());
            memberType->EmitCppUnMarshalling("data", cpName, sb, TAB, false);
            sb.Append(TAB).AppendFormat("(void)memcpy_s(&%s, sizeof(%s), %s, sizeof(%s));\n", name.string(),
                memberType->EmitCppType().string(), cpName.string(), memberType->EmitCppType().string());
        } else if (memberType->GetTypeKind() == TypeKind::TYPE_STRING) {
            String cpName = String::Format("%sCp", memberName.string());
            memberType->EmitCppUnMarshalling("data", cpName, sb, TAB, false);
            sb.Append(TAB).AppendFormat("%s = %s;\n", name.string(), cpName.string());
        } else {
            memberType->EmitCppUnMarshalling("data", name, sb, TAB, false);
        }
    }

    sb.Append(TAB).AppendFormat("return true;\n", objName.string());
    sb.Append("}\n");
}

void CppCustomTypesCodeEmitter::EmitBeginNamespace(StringBuilder &sb)
{
    std::vector<String> cppNamespaceVec = EmitCppNameSpaceVec(ast_->GetPackageName());
    for (const auto &nspace : cppNamespaceVec) {
        sb.AppendFormat("namespace %s {\n", nspace.string());
    }
}

void CppCustomTypesCodeEmitter::EmitEndNamespace(StringBuilder &sb)
{
    std::vector<String> cppNamespaceVec = EmitCppNameSpaceVec(ast_->GetPackageName());
    for (auto nspaceIter = cppNamespaceVec.rbegin(); nspaceIter != cppNamespaceVec.rend(); nspaceIter++) {
        sb.AppendFormat("} // %s\n", nspaceIter->string());
    }
}
} // namespace HDI
} // namespace OHOS
