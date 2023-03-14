/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/c_interface_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
bool CInterfaceCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFilePath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CInterfaceCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CInterfaceCodeEmitter::EmitCode()
{
    EmitInterfaceHeaderFile();
}

void CInterfaceCodeEmitter::EmitInterfaceHeaderFile()
{
    String filePath = String::Format("%s/%s.h", directory_.string(), FileName(interfaceName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, interfaceFullName_);
    sb.Append("\n");
    EmitImportInclusions(sb);
    sb.Append("\n");
    EmitHeadExternC(sb);
    sb.Append("\n");
    EmitPreDeclaration(sb);
    sb.Append("\n");
    EmitInterfaceDesc(sb);
    sb.Append("\n");
    EmitInterfaceVersionMacro(sb);
    sb.Append("\n");
    EmitInterfaceBuffSizeMacro(sb);
    sb.Append("\n");
    EmitInterfaceMethodCommands(sb, "");
    sb.Append("\n");
    EmitInterfaceDefinition(sb);
    sb.Append("\n");
    EmitInterfaceGetMethodDecl(sb);
    sb.Append("\n");
    EmitInterfaceReleaseMethodDecl(sb);
    sb.Append("\n");
    EmitTailExternC(sb);
    sb.Append("\n");
    EmitTailMacro(sb, interfaceFullName_);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CInterfaceCodeEmitter::EmitImportInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    GetImportInclusions(headerFiles);
    GetHeaderOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CInterfaceCodeEmitter::GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::C_STD_HEADER_FILE, "stdint"));
}

void CInterfaceCodeEmitter::EmitPreDeclaration(StringBuilder &sb)
{
    sb.Append("struct HdfRemoteService;\n");
}

void CInterfaceCodeEmitter::EmitInterfaceDesc(StringBuilder &sb)
{
    sb.AppendFormat("#define %s \"%s\"\n", EmitDescMacroName().string(), interfaceFullName_.string());
}

void CInterfaceCodeEmitter::EmitInterfaceVersionMacro(StringBuilder &sb)
{
    sb.AppendFormat("#define %s %u\n", majorVerName_.string(), ast_->GetMajorVer());
    sb.AppendFormat("#define %s %u\n", minorVerName_.string(), ast_->GetMinorVer());
}

void CInterfaceCodeEmitter::EmitInterfaceBuffSizeMacro(StringBuilder &sb)
{
    sb.AppendFormat("#define %s    (4 * 1024) // 4KB\n", bufferSizeMacroName_.string());
}

void CInterfaceCodeEmitter::EmitInterfaceDefinition(StringBuilder &sb)
{
    sb.AppendFormat("struct %s {\n", interfaceName_.string());
    EmitInterfaceMethods(sb, TAB);
    sb.Append("};\n");
}

void CInterfaceCodeEmitter::EmitInterfaceMethods(StringBuilder &sb, const String &prefix)
{
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceMethod(method, sb, prefix);
        sb.Append("\n");
    }

    EmitInterfaceMethod(interface_->GetVersionMethod(), sb, prefix);

    if (!isKernelCode_) {
        sb.Append("\n");
        EmitAsObjectMethod(sb, TAB);
    }
}

void CInterfaceCodeEmitter::EmitInterfaceMethod(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    if (method->GetParameterNumber() == 0) {
        sb.Append(prefix).AppendFormat(
            "int32_t (*%s)(struct %s *self);\n", method->GetName().string(), interfaceName_.string());
    } else {
        StringBuilder paramStr;
        paramStr.Append(prefix).AppendFormat(
            "int32_t (*%s)(struct %s *self, ", method->GetName().string(), interfaceName_.string());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitInterfaceMethodParameter(param, paramStr, "");
            if (i + 1 < method->GetParameterNumber()) {
                paramStr.Append(", ");
            }
        }

        paramStr.Append(");");
        sb.Append(SpecificationParam(paramStr, prefix + TAB));
        sb.Append("\n");
    }
}

void CInterfaceCodeEmitter::EmitAsObjectMethod(StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("struct HdfRemoteService* (*AsObject)(struct %s *self);\n", interfaceName_.string());
}

void CInterfaceCodeEmitter::EmitInterfaceGetMethodDecl(StringBuilder &sb)
{
    if (interface_->IsSerializable()) {
        sb.AppendFormat(
            "struct %s *%sGet(struct HdfRemoteService *remote);\n", interfaceName_.string(), baseName_.string());
    } else {
        sb.AppendFormat("struct %s *%sGet(void);\n", interfaceName_.string(), baseName_.string());
        sb.Append("\n");
        sb.AppendFormat(
            "struct %s *%sGetInstance(const char *instanceName);\n", interfaceName_.string(), baseName_.string());
    }
}

void CInterfaceCodeEmitter::EmitInterfaceReleaseMethodDecl(StringBuilder &sb)
{
    sb.AppendFormat("void %sRelease(struct %s *instance);\n", baseName_.string(), interfaceName_.string());
}
} // namespace HDI
} // namespace OHOS