/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/c_service_impl_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
bool CServiceImplCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFilePath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CServiceImplCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CServiceImplCodeEmitter::EmitCode()
{
    EmitServiceImplHeaderFile();
    EmitServiceImplSourceFile();
}

void CServiceImplCodeEmitter::EmitServiceImplHeaderFile()
{
    String filePath = String::Format("%s/%s.h", directory_.string(), FileName(baseName_ + "Service").string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, implFullName_);
    sb.Append("\n");
    EmitServiceImplHeaderInclusions(sb);
    sb.Append("\n");
    EmitHeadExternC(sb);
    if (!interface_->IsSerializable()) {
        sb.Append("\n");
        EmitServiceImplDef(sb);
    }
    sb.Append("\n");
    EmitServiceImplConstructDecl(sb);
    sb.Append("\n");
    EmitTailExternC(sb);
    sb.Append("\n");
    EmitTailMacro(sb, implFullName_);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CServiceImplCodeEmitter::EmitServiceImplHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_MODULE_HEADER_FILE, EmitVersionHeaderName(stubName_)));

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CServiceImplCodeEmitter::EmitServiceImplConstructDecl(StringBuilder &sb)
{
    String instTypeName = interface_->IsSerializable() ? interfaceName_ : implName_;
    sb.AppendFormat("struct %s *%sServiceGet(void);\n\n", instTypeName.string(), baseName_.string());
    sb.AppendFormat("void %sServiceRelease(struct %s *instance);\n", baseName_.string(), instTypeName.string());
}

void CServiceImplCodeEmitter::EmitServiceImplSourceFile()
{
    String filePath = String::Format("%s/%s.c", directory_.string(), FileName(baseName_ + "Service").string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitServiceImplSourceInclusions(sb);
    if (interface_->IsSerializable()) {
        sb.Append("\n");
        EmitServiceImplDef(sb);
    }

    sb.Append("\n");
    EmitServiceImplMethodImpls(sb, "");
    sb.Append("\n");
    EmitServiceImplGetMethod(sb);
    sb.Append("\n");
    EmitServiceImplReleaseMethod(sb);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CServiceImplCodeEmitter::EmitServiceImplSourceInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(implName_)));
    GetSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CServiceImplCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "osal_mem"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
}

void CServiceImplCodeEmitter::EmitServiceImplDef(StringBuilder &sb)
{
    sb.AppendFormat("struct %sService\n", baseName_.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("struct %sStub stub;\n\n", baseName_.string());
    sb.Append(TAB).Append("// please add private data here\n");
    sb.Append("};\n");
}

void CServiceImplCodeEmitter::EmitServiceImplMethodImpls(StringBuilder &sb, const String &prefix)
{
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitServiceImplMethodImpl(method, sb, prefix);
        if (i + 1 < interface_->GetMethodNumber()) {
            sb.Append("\n");
        }
    }
}

void CServiceImplCodeEmitter::EmitServiceImplMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    if (method->GetParameterNumber() == 0) {
        sb.Append(prefix).AppendFormat(
            "int32_t %s%s(struct %s *self)\n", baseName_.string(), method->GetName().string(), interfaceName_.string());
    } else {
        StringBuilder paramStr;
        paramStr.Append(prefix).AppendFormat(
            "int32_t %s%s(struct %s *self, ", baseName_.string(), method->GetName().string(), interfaceName_.string());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitInterfaceMethodParameter(param, paramStr, "");
            if (i + 1 < method->GetParameterNumber()) {
                paramStr.Append(", ");
            }
        }

        paramStr.Append(")");
        sb.Append(SpecificationParam(paramStr, prefix + TAB));
        sb.Append("\n");
    }

    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("return HDF_SUCCESS;\n");
    sb.Append(prefix).Append("}\n");
}

void CServiceImplCodeEmitter::EmitServiceImplGetMethod(StringBuilder &sb)
{
    String implTypeName = String::Format("%sService", baseName_.string());
    String objName = "service";

    if (!interface_->IsSerializable()) {
        sb.AppendFormat("struct %s *%sGet(void)\n", implName_.string(), implTypeName.string());
    } else {
        sb.AppendFormat("struct %s *%sGet(void)\n", interfaceName_.string(), implTypeName.string());
    }
    sb.Append("{\n");

    sb.Append(TAB).AppendFormat("struct %s *%s = (struct %s *)OsalMemCalloc(sizeof(struct %s));\n",
        implTypeName.string(), objName.string(), implTypeName.string(), implTypeName.string());
    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.string());
    sb.Append(TAB).Append(TAB).AppendFormat(
        "HDF_LOGE(\"%%{public}s: malloc %s obj failed!\", __func__);\n", implTypeName.string());
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n\n");

    sb.Append(TAB).AppendFormat("if (!%sStubConstruct(&%s->stub)) {\n", baseName_.string(), objName.string());
    sb.Append(TAB).Append(TAB).AppendFormat(
        "HDF_LOGE(\"%%{public}s: construct %sStub obj failed!\", __func__);\n", baseName_.string());
    sb.Append(TAB).Append(TAB).AppendFormat("OsalMemFree(%s);\n", objName.string());
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n\n");

    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.Append(TAB).AppendFormat("%s->stub.interface.%s = %s%s;\n", objName.string(), method->GetName().string(),
            baseName_.string(), method->GetName().string());
    }

    if (interface_->IsSerializable()) {
        sb.Append(TAB).AppendFormat("return &%s->stub.interface;\n", objName.string());
    } else {
        sb.Append(TAB).AppendFormat("return service;\n", objName.string());
    }
    sb.Append("}\n");
}

void CServiceImplCodeEmitter::EmitServiceImplReleaseMethod(StringBuilder &sb)
{
    String implTypeName = String::Format("%sService", baseName_.string());
    String instName = "instance";
    String objName = "service";

    if (interface_->IsSerializable()) {
        sb.AppendFormat(
            "void %sRelease(struct %s *%s)\n", implTypeName.string(), interfaceName_.string(), instName.string());
        sb.Append("{\n");
        sb.Append(TAB).AppendFormat("struct %s *%s = (struct %s *)%s;\n", implTypeName.string(), objName.string(),
            implTypeName.string(), instName.string());
        sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.string());
        sb.Append(TAB).Append(TAB).Append("return;\n");
        sb.Append(TAB).Append("}\n\n");
        sb.Append(TAB).AppendFormat("%sStubRelease(&%s->stub);\n", baseName_.string(), objName.string());
        sb.Append(TAB).AppendFormat("OsalMemFree(%s);\n", objName.string());
    } else {
        sb.AppendFormat(
            "void %sRelease(struct %s *%s)\n", implTypeName.string(), implTypeName.string(), instName.string());
        sb.Append("{\n");
        sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", instName.string());
        sb.Append(TAB).Append(TAB).Append("return;\n");
        sb.Append(TAB).Append("}\n\n");
        sb.Append(TAB).AppendFormat("OsalMemFree(%s);\n", instName.string());
    }
    sb.Append("}\n");
}
} // namespace HDI
} // namespace OHOS