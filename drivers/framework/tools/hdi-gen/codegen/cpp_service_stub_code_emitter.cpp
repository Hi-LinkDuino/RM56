/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/cpp_service_stub_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
bool CppServiceStubCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFilePath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CppServiceStubCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CppServiceStubCodeEmitter::EmitCode()
{
    EmitStubHeaderFile();
    EmitStubSourceFile();
}

void CppServiceStubCodeEmitter::EmitStubHeaderFile()
{
    String filePath = String::Format("%s/%s.h", directory_.string(), FileName(stubName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, stubFullName_);
    sb.Append("\n");
    EmitStubHeaderInclusions(sb);
    sb.Append("\n");
    EmitStubDecl(sb);
    sb.Append("\n");
    EmitTailMacro(sb, stubFullName_);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CppServiceStubCodeEmitter::EmitStubHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_MODULE_HEADER_FILE, EmitVersionHeaderName(interfaceName_)));
    GetHeaderOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CppServiceStubCodeEmitter::GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "message_parcel"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "message_option"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "iremote_stub"));
}

void CppServiceStubCodeEmitter::EmitStubDecl(StringBuilder &sb)
{
    EmitBeginNamespace(sb);
    sb.Append("\n");
    EmitStubUsingNamespace(sb);
    sb.Append("\n");
    sb.AppendFormat("class %s : public IRemoteStub<%s> {\n", stubName_.string(), interfaceName_.string());
    EmitStubBody(sb, TAB);
    sb.Append("};\n");
    sb.Append("\n");
    EmitEndNamespace(sb);
}

void CppServiceStubCodeEmitter::EmitStubUsingNamespace(StringBuilder &sb)
{
    sb.Append("using namespace OHOS;\n");
}

void CppServiceStubCodeEmitter::EmitStubBody(StringBuilder &sb, const String &prefix)
{
    sb.Append("public:\n");
    EmitStubDestruction(sb, prefix);
    sb.Append("\n");
    EmitStubOnRequestDecl(sb, prefix);
    sb.Append("\n");
    EmitGetVersionDecl(sb, prefix);
    sb.Append("\n");
    EmitStubMethodDecls(sb, prefix);
}

void CppServiceStubCodeEmitter::EmitStubDestruction(StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("virtual ~%s() {}\n", stubName_.string());
}

void CppServiceStubCodeEmitter::EmitStubOnRequestDecl(StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).Append("int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, ");
    sb.Append("MessageOption &option) override;\n");
}

void CppServiceStubCodeEmitter::EmitGetVersionDecl(StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTMethod> method = interface_->GetVersionMethod();

    StringBuilder paramStr;
    sb.Append(prefix).AppendFormat("int32_t %s(", method->GetName().string());
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        EmitInterfaceMethodParameter(param, paramStr, "");
        if (i + 1 < method->GetParameterNumber()) {
            paramStr.Append(", ");
        }
    }

    paramStr.Append(") override;");

    sb.Append(SpecificationParam(paramStr, prefix + TAB));
    sb.Append("\n");
}

void CppServiceStubCodeEmitter::EmitStubMethodDecls(StringBuilder &sb, const String &prefix)
{
    sb.Append("private:\n");
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitStubMethodDecl(method, sb, prefix);
        sb.Append("\n");
    }
    EmitStubMethodDecl(interface_->GetVersionMethod(), sb, prefix);
}

void CppServiceStubCodeEmitter::EmitStubMethodDecl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("int32_t %s%s(MessageParcel& %s, MessageParcel& %s, MessageOption& %s);\n",
        stubName_.string(), method->GetName().string(), dataParcelName_.string(), replyParcelName_.string(),
        optionName_.string());
}

void CppServiceStubCodeEmitter::EmitStubSourceFile()
{
    String filePath = String::Format("%s/%s.cpp", directory_.string(), FileName(stubName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitStubSourceInclusions(sb);
    sb.Append("\n");
    EmitBeginNamespace(sb);
    sb.Append("\n");
    EmitStubOnRequestMethodImpl(sb, "");
    sb.Append("\n");
    EmitGetVersionMethodImpl(sb, "");
    sb.Append("\n");
    EmitStubMethodImpls(sb, "");
    sb.Append("\n");
    EmitEndNamespace(sb);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CppServiceStubCodeEmitter::EmitStubSourceInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(stubName_)));
    GetSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CppServiceStubCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    if (!interface_->IsSerializable()) {
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
    } else {
        const AST::TypeStringMap &types = ast_->GetTypes();
        for (const auto &pair : types) {
            AutoPtr<ASTType> type = pair.second;
            if (type->GetTypeKind() == TypeKind::TYPE_UNION) {
                headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
                break;
            }
        }
    }

    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log"));
}

void CppServiceStubCodeEmitter::EmitStubOnRequestMethodImpl(StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("int32_t %s::OnRemoteRequest(uint32_t code, ", stubName_.string());
    sb.Append("MessageParcel& data, MessageParcel& reply, MessageOption& option)\n");
    sb.Append(prefix).Append("{\n");

    sb.Append(prefix + TAB).Append("switch (code) {\n");

    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(method).string());
        sb.Append(prefix + TAB + TAB + TAB)
            .AppendFormat("return %sStub%s(data, reply, option);\n", baseName_.string(), method->GetName().string());
    }

    AutoPtr<ASTMethod> getVerMethod = interface_->GetVersionMethod();
    sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(getVerMethod).string());
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("return %sStub%s(data, reply, option);\n", baseName_.string(), getVerMethod->GetName().string());

    sb.Append(prefix + TAB + TAB).Append("default: {\n");
    sb.Append(prefix + TAB + TAB + TAB)
        .Append("HDF_LOGE(\"%{public}s: not support cmd %{public}d\", __func__, code);\n");
    sb.Append(prefix + TAB + TAB + TAB)
        .Append("return IPCObjectStub::OnRemoteRequest(code, data, reply, option);\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append("}\n");
}

void CppServiceStubCodeEmitter::EmitGetVersionMethodImpl(StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTMethod> method = interface_->GetVersionMethod();
    sb.Append(prefix).AppendFormat("int32_t %sStub::%s(", baseName_.string(), method->GetName().string());
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        EmitInterfaceMethodParameter(param, sb, "");
        if (i + 1 < method->GetParameterNumber()) {
            sb.Append(", ");
        }
    }

    sb.AppendFormat(")\n");
    sb.Append(prefix).Append("{\n");

    AutoPtr<ASTParameter> majorParam = method->GetParameter(0);
    sb.Append(prefix + TAB).AppendFormat("%s = %s;\n", majorParam->GetName().string(), majorVerName_.string());
    AutoPtr<ASTParameter> minorParam = method->GetParameter(1);
    sb.Append(prefix + TAB).AppendFormat("%s = %s;\n", minorParam->GetName().string(), minorVerName_.string());

    sb.Append(prefix + TAB).Append("return HDF_SUCCESS;\n");
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubMethodImpls(StringBuilder &sb, const String &prefix)
{
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitStubMethodImpl(method, sb, prefix);
        sb.Append("\n");
    }

    EmitStubMethodImpl(interface_->GetVersionMethod(), sb, prefix);
}

void CppServiceStubCodeEmitter::EmitStubMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("int32_t %s::%s%s(MessageParcel& %s, MessageParcel& %s, MessageOption& %s)\n",
        stubName_.string(), stubName_.string(), method->GetName().string(), dataParcelName_.string(),
        replyParcelName_.string(), optionName_.string());
    sb.Append(prefix).Append("{\n");

    // read interface token and check it
    EmitStubReadInterfaceToken(dataParcelName_, sb, prefix + TAB);
    sb.Append("\n");

    EmitStubReadMemFlag(method, dataParcelName_, sb, prefix + TAB);

    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() == ParamAttr::PARAM_IN) {
            EmitReadMethodParameter(param, dataParcelName_, true, sb, prefix + TAB);
            sb.Append("\n");
        } else {
            EmitLocalVariable(param, dataParcelName_, sb, prefix + TAB);
            sb.Append("\n");
        }
    }

    EmitStubCallMethod(method, sb, prefix + TAB);
    sb.Append("\n");

    if (!method->IsOneWay()) {
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() == ParamAttr::PARAM_OUT) {
                EmitWriteMethodParameter(param, replyParcelName_, sb, prefix + TAB);
                sb.Append("\n");
            }
        }
    }

    sb.Append(prefix + TAB).Append("return HDF_SUCCESS;\n");
    sb.Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubCallMethod(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("int32_t %s = %s(", errorCodeName_.string(), method->GetName().string());
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        sb.Append(param->GetName());
        if (i + 1 < method->GetParameterNumber()) {
            sb.Append(", ");
        }
    }
    sb.Append(");\n");

    sb.Append(prefix).AppendFormat("if (%s != HDF_SUCCESS) {\n", errorCodeName_.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s failed, error code is %%d\", __func__, %s);\n", errorCodeName_.string());
    sb.Append(prefix + TAB).AppendFormat("return %s;\n", errorCodeName_.string());
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubReadInterfaceToken(
    const String &parcelName, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("if (%s.ReadInterfaceToken() != GetDescriptor()) {\n", parcelName.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: interface token check failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubReadMemFlag(
    const AutoPtr<ASTMethod> &method, const String &parcelName, StringBuilder &sb, const String &prefix)
{
    if (NeedFlag(method)) {
        sb.Append(prefix).AppendFormat("bool %s = %s.ReadBool();\n\n", flagOfSetMemName_.string(), parcelName.string());
    }
}

void CppServiceStubCodeEmitter::EmitLocalVariable(
    const AutoPtr<ASTParameter> &param, const String &parcelName, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).Append(param->EmitCppLocalVar()).Append("\n");
    AutoPtr<ASTType> type = param->GetType();
    if (type->IsStringType() || type->IsArrayType() || type->IsListType()) {
        sb.Append(prefix).AppendFormat("if (%s) {\n", flagOfSetMemName_.string());
        sb.Append(prefix + TAB)
            .AppendFormat("%s.reserve(%s.ReadUint32());\n", param->GetName().string(), parcelName.string());
        sb.Append(prefix).Append("}\n");
    }
}
} // namespace HDI
} // namespace OHOS