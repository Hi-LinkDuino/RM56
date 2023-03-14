/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/c_service_stub_code_emitter.h"

#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
bool CServiceStubCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFilePath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CServiceStubCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CServiceStubCodeEmitter::EmitCode()
{
    EmitServiceStubHeaderFile();
    EmitServiceStubSourceFile();
}

void CServiceStubCodeEmitter::EmitServiceStubHeaderFile()
{
    String filePath = String::Format("%s/%s.h", directory_.string(), FileName(stubName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, stubFullName_);
    sb.Append("\n");
    EmitStubHeaderInclusions(sb);
    sb.Append("\n");
    EmitHeadExternC(sb);
    sb.Append("\n");
    EmitCbServiceStubDef(sb);
    sb.Append("\n");
    EmitCbServiceStubMethodsDcl(sb);
    sb.Append("\n");
    EmitTailExternC(sb);
    sb.Append("\n");
    EmitTailMacro(sb, stubFullName_);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CServiceStubCodeEmitter::EmitStubHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_MODULE_HEADER_FILE, EmitVersionHeaderName(interfaceName_)));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_sbuf"));

    if (interface_->IsSerializable()) {
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_remote_service"));
    }

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CServiceStubCodeEmitter::EmitCbServiceStubDef(StringBuilder &sb)
{
    sb.AppendFormat("struct %sStub\n", baseName_.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("struct %s interface;\n", interfaceName_.string());

    if (interface_->IsSerializable()) {
        sb.Append(TAB).Append("struct HdfRemoteService *remote;\n");
        sb.Append(TAB).Append("struct HdfRemoteDispatcher dispatcher;\n");
    } else {
        sb.Append(TAB).AppendFormat("int32_t (*OnRemoteRequest)(struct %s *serviceImpl, ", interfaceName_.string());
        sb.Append("int code, struct HdfSBuf *data, struct HdfSBuf *reply);\n");
    }

    sb.Append("};\n");
}

void CServiceStubCodeEmitter::EmitCbServiceStubMethodsDcl(StringBuilder &sb)
{
    sb.AppendFormat("bool %sStubConstruct(struct %sStub *stub);\n", baseName_.string(), baseName_.string());

    if (interface_->IsSerializable()) {
        sb.AppendFormat("void %sStubRelease(struct %sStub *stub);\n", baseName_.string(), baseName_.string());
    }
}

void CServiceStubCodeEmitter::EmitServiceStubSourceFile()
{
    String filePath = String::Format("%s/%s.c", directory_.string(), FileName(stubName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitStubSourceInclusions(sb);
    sb.Append("\n");
    EmitServiceStubMethodImpls(sb, "");
    sb.Append("\n");
    EmitStubOnRequestMethodImpl(sb, "");
    sb.Append("\n");
    EmitStubGetMethodImpl(sb);

    if (!isKernelCode_ && interface_->IsSerializable()) {
        sb.Append("\n");
        EmitStubReleaseImpl(sb);
    }

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CServiceStubCodeEmitter::EmitStubSourceInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(stubName_)));
    GetSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CServiceStubCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    if (!isKernelCode_) {
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
    } else {
        const AST::TypeStringMap &types = ast_->GetTypes();
        for (const auto &pair : types) {
            AutoPtr<ASTType> type = pair.second;
            if (type->GetTypeKind() == TypeKind::TYPE_STRING || type->GetTypeKind() == TypeKind::TYPE_UNION) {
                headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
                break;
            }
        }
    }

    if (interface_->IsSerializable()) {
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_dlist"));
    }

    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "osal_mem"));
}

void CServiceStubCodeEmitter::EmitServiceStubMethodImpls(StringBuilder &sb, const String &prefix)
{
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitServiceStubMethodImpl(method, sb, prefix);
        sb.Append("\n");
    }

    EmitStubGetVerMethodImpl(interface_->GetVersionMethod(), sb, prefix);
    if (!isKernelCode_) {
        sb.Append("\n");
        EmitStubAsObjectMethodImpl(sb, prefix);
    }
}

void CServiceStubCodeEmitter::EmitServiceStubMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat(
        "static int32_t SerStub%s(struct %s *serviceImpl, struct HdfSBuf *%s, struct HdfSBuf *%s)\n",
        method->GetName().string(), interfaceName_.string(), dataParcelName_.string(), replyParcelName_.string());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("int32_t %s = HDF_FAILURE;\n", errorCodeName_.string());
    bool readFlag = NeedFlag(method);
    if (readFlag) {
        sb.Append(prefix + TAB).AppendFormat("bool %s = false;\n", flagOfSetMemName_.string());
    }

    // Local variable definitions must precede all execution statements.
    EmitInitLoopVar(method, sb, prefix + TAB);

    if (method->GetParameterNumber() > 0) {
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitStubLocalVariable(param, sb, prefix + TAB);
        }

        sb.Append("\n");
        EmitReadFlagVariable(readFlag, sb, prefix + TAB);
        for (int i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() == ParamAttr::PARAM_IN) {
                EmitReadStubMethodParameter(param, dataParcelName_, finishedLabelName_, sb, prefix + TAB);
                sb.Append("\n");
            } else {
                EmitOutVarMemInitialize(param, dataParcelName_, finishedLabelName_, sb, prefix + TAB);
            }
        }
    }

    EmitStubCallMethod(method, finishedLabelName_, sb, prefix + TAB);
    sb.Append("\n");

    for (int i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() == ParamAttr::PARAM_OUT) {
            param->EmitCWriteVar(replyParcelName_, errorCodeName_, finishedLabelName_, sb, prefix + TAB);
            sb.Append("\n");
        }
    }

    EmitErrorHandle(method, finishedLabelName_, false, sb, prefix);
    sb.Append(prefix + TAB).AppendFormat("return %s;\n", errorCodeName_.string());
    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitReadFlagVariable(bool readFlag, StringBuilder &sb, const String &prefix)
{
    if (!readFlag) {
        return;
    }

    sb.Append(prefix).AppendFormat(
        "if (!HdfSbufReadUint8(%s, (uint8_t *)&%s)) {\n", dataParcelName_.string(), flagOfSetMemName_.string());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: read flag of memory setting failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", finishedLabelName_);
    sb.Append(prefix).Append("}\n\n");
}

void CServiceStubCodeEmitter::EmitStubLocalVariable(
    const AutoPtr<ASTParameter> &param, StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTType> type = param->GetType();
    sb.Append(prefix).Append(param->EmitCLocalVar()).Append("\n");
    if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST ||
        (type->GetTypeKind() == TypeKind::TYPE_STRING && param->GetAttribute() == ParamAttr::PARAM_OUT)) {
        sb.Append(prefix).AppendFormat("uint32_t %sLen = 0;\n", param->GetName().string());
    }
}

void CServiceStubCodeEmitter::EmitReadStubMethodParameter(const AutoPtr<ASTParameter> &param, const String &parcelName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTType> type = param->GetType();

    if (type->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitReadCStringStubMethodParameter(param, parcelName, gotoLabel, sb, prefix, type);
    } else if (type->GetTypeKind() == TypeKind::TYPE_INTERFACE) {
        type->EmitCStubReadVar(parcelName, param->GetName(), errorCodeName_, gotoLabel, sb, prefix);
    } else if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemAlloc(sizeof(%s));\n", param->GetName().string(),
            type->EmitCType(TypeMode::NO_MODE).string(), type->EmitCType(TypeMode::NO_MODE).string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", finishedLabelName_);
        sb.Append(prefix).Append("}\n");
        type->EmitCStubReadVar(parcelName, param->GetName(), errorCodeName_, gotoLabel, sb, prefix);
    } else if (type->GetTypeKind() == TypeKind::TYPE_UNION) {
        String cpName = String::Format("%sCp", param->GetName().string());
        type->EmitCStubReadVar(parcelName, cpName, errorCodeName_, gotoLabel, sb, prefix);
        sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemAlloc(sizeof(%s));\n", param->GetName().string(),
            type->EmitCType(TypeMode::NO_MODE).string(), type->EmitCType(TypeMode::NO_MODE).string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", finishedLabelName_);
        sb.Append(prefix).Append("}\n");
        sb.Append(prefix).AppendFormat("(void)memcpy_s(%s, sizeof(%s), %s, sizeof(%s));\n", param->GetName().string(),
            type->EmitCType(TypeMode::NO_MODE).string(), cpName.string(), type->EmitCType(TypeMode::NO_MODE).string());
    } else if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST) {
        type->EmitCStubReadVar(parcelName, param->GetName(), errorCodeName_, gotoLabel, sb, prefix);
    } else if (type->GetTypeKind() == TypeKind::TYPE_FILEDESCRIPTOR) {
        type->EmitCStubReadVar(parcelName, param->GetName(), errorCodeName_, gotoLabel, sb, prefix);
    } else {
        String name = String::Format("&%s", param->GetName().string());
        type->EmitCStubReadVar(parcelName, name, errorCodeName_, gotoLabel, sb, prefix);
    }
}

void CServiceStubCodeEmitter::EmitReadCStringStubMethodParameter(const AutoPtr<ASTParameter> &param,
    const String &parcelName, const String &gotoLabel, StringBuilder &sb, const String &prefix, AutoPtr<ASTType> &type)
{
    String cloneName = String::Format("%sCp", param->GetName().string());
    type->EmitCStubReadVar(parcelName, cloneName, errorCodeName_, gotoLabel, sb, prefix);
    if (isKernelCode_) {
        sb.Append("\n");
        sb.Append(prefix).AppendFormat(
            "%s = (char*)OsalMemCalloc(strlen(%s) + 1);\n", param->GetName().string(), cloneName.string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (strcpy_s(%s, (strlen(%s) + 1), %s) != HDF_SUCCESS) {\n",
            param->GetName().string(), cloneName.string(), cloneName.string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", param->GetName().string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix).Append("}\n");
    } else {
        sb.Append(prefix).AppendFormat("%s = strdup(%s);\n", param->GetName().string(), cloneName.string());
    }
}

void CServiceStubCodeEmitter::EmitOutVarMemInitialize(const AutoPtr<ASTParameter> &param, const String &parcelName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTType> type = param->GetType();
    if (type->IsStructType() || type->IsUnionType()) {
        sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s));\n", param->GetName().string(),
            type->EmitCType(TypeMode::NO_MODE).string(), type->EmitCType(TypeMode::NO_MODE).string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix).Append("}\n\n");
    } else if (type->IsStringType() || type->IsArrayType() || type->IsListType()) {
        param->EmitCStubReadOutVar(
            bufferSizeMacroName_, flagOfSetMemName_, parcelName, errorCodeName_, gotoLabel, sb, prefix);
        sb.Append("\n");
    }
}

void CServiceStubCodeEmitter::EmitStubCallMethod(
    const AutoPtr<ASTMethod> &method, const String &gotoLabel, StringBuilder &sb, const String &prefix)
{
    if (method->GetParameterNumber() == 0) {
        sb.Append(prefix).AppendFormat(
            "%s = serviceImpl->%s(serviceImpl);\n", errorCodeName_.string(), method->GetName().string());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = serviceImpl->%s(serviceImpl, ", errorCodeName_.string(), method->GetName().string());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitCallParameter(sb, param->GetType(), param->GetAttribute(), param->GetName());
            if (i + 1 < method->GetParameterNumber()) {
                sb.Append(", ");
            }
        }
        sb.AppendFormat(");\n", method->GetName().string());
    }

    sb.Append(prefix).AppendFormat("if (%s != HDF_SUCCESS) {\n", errorCodeName_.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: call %s function failed!\", __func__);\n", method->GetName().string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitCallParameter(
    StringBuilder &sb, const AutoPtr<ASTType> &type, ParamAttr attribute, const String &name)
{
    if (attribute == ParamAttr::PARAM_OUT) {
        if (type->GetTypeKind() == TypeKind::TYPE_STRING || type->GetTypeKind() == TypeKind::TYPE_ARRAY ||
            type->GetTypeKind() == TypeKind::TYPE_LIST || type->GetTypeKind() == TypeKind::TYPE_STRUCT ||
            type->GetTypeKind() == TypeKind::TYPE_UNION) {
            sb.AppendFormat("%s", name.string());
        } else {
            sb.AppendFormat("&%s", name.string());
        }

        if (type->GetTypeKind() == TypeKind::TYPE_STRING) {
            sb.AppendFormat(", %sLen", name.string());
        } else if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST) {
            sb.AppendFormat(", &%sLen", name.string());
        }
    } else {
        sb.AppendFormat("%s", name.string());
        if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST) {
            sb.AppendFormat(", %sLen", name.string());
        }
    }
}

void CServiceStubCodeEmitter::EmitStubGetVerMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat(
        "static int32_t SerStub%s(struct %s *serviceImpl, struct HdfSBuf *%s, struct HdfSBuf *%s)\n",
        method->GetName().string(), interfaceName_.string(), dataParcelName_.string(), replyParcelName_.string());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("int32_t %s = HDF_SUCCESS;\n", errorCodeName_.string());

    AutoPtr<ASTType> type = new ASTUintType();
    type->EmitCWriteVar(replyParcelName_, majorVerName_, errorCodeName_, finishedLabelName_, sb, prefix + TAB);
    sb.Append("\n");
    type->EmitCWriteVar(replyParcelName_, minorVerName_, errorCodeName_, finishedLabelName_, sb, prefix + TAB);
    sb.Append("\n");

    sb.Append(finishedLabelName_).Append(":\n");
    sb.Append(prefix + TAB).AppendFormat("return %s;\n", errorCodeName_.string());
    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubAsObjectMethodImpl(StringBuilder &sb, const String &prefix)
{
    String objName = "self";
    sb.Append(prefix).AppendFormat("static struct HdfRemoteService *%sStubAsObject(struct %s *%s)\n",
        baseName_.string(), interfaceName_.string(), objName.string());
    sb.Append(prefix).Append("{\n");

    if (interface_->IsSerializable()) {
        sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", objName.string());
        sb.Append(prefix + TAB + TAB).Append("return NULL;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).AppendFormat("struct %sStub *stub = CONTAINER_OF(%s, struct %sStub, interface);\n",
            baseName_.string(), objName.string(), baseName_.string());
        sb.Append(prefix + TAB).Append("return stub->remote;\n");
    } else {
        sb.Append(prefix + TAB).Append("return NULL;\n");
    }

    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubOnRequestMethodImpl(StringBuilder &sb, const String &prefix)
{
    String remoteName = "remote";
    String implName = "serviceImpl";
    String codeName = "code";
    String funcName = String::Format("%sOnRemoteRquest", baseName_.string());
    if (interface_->IsSerializable()) {
        sb.Append(prefix).AppendFormat(
            "static int32_t %s(struct HdfRemoteService *%s, ", funcName.string(), remoteName.string());
    } else {
        sb.Append(prefix).AppendFormat(
            "static int32_t %s(struct %s *%s, ", funcName.string(), interfaceName_.string(), implName.string());
    }
    sb.AppendFormat("int %s, struct HdfSBuf *data, struct HdfSBuf *reply)\n", codeName.string());
    sb.Append(prefix).Append("{\n");

    if (interface_->IsSerializable()) {
        sb.Append(prefix + TAB).AppendFormat("struct %s *%s = (struct %s*)%s;\n", interfaceName_.string(),
            implName.string(), interfaceName_.string(), remoteName.string());

        sb.Append(prefix + TAB).AppendFormat("if (!HdfRemoteServiceCheckInterfaceToken(%s->AsObject(%s), data)) {\n",
            implName.string(), implName.string());
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: interface token check failed\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
    }

    sb.Append(prefix + TAB).AppendFormat("switch (%s) {\n", codeName.string());
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(method).string());
        sb.Append(prefix + TAB + TAB + TAB)
            .AppendFormat("return SerStub%s(%s, data, reply);\n", method->GetName().string(), implName.string());
    }

    AutoPtr<ASTMethod> getVerMethod = interface_->GetVersionMethod();
    sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(getVerMethod).string());
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("return SerStub%s(%s, data, reply);\n", getVerMethod->GetName().string(), implName.string());

    sb.Append(prefix + TAB + TAB).Append("default: {\n");
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: not support cmd %%{public}d\", __func__, %s);\n", codeName.string());
    sb.Append(prefix + TAB + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubGetMethodImpl(StringBuilder &sb)
{
    String stubTypeName = String::Format("%sStub", baseName_.string());
    String objName = "stub";
    String funcName = String::Format("%sOnRemoteRquest", baseName_.string());

    sb.AppendFormat(
        "bool %sConstruct(struct %s *%s)\n", stubTypeName.string(), stubTypeName.string(), objName.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.string());
    sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: stub is null!\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return false;\n");
    sb.Append(TAB).Append("}\n\n");

    if (interface_->IsSerializable()) {
        sb.Append(TAB).AppendFormat("%s->dispatcher.Dispatch = %s;\n", objName.string(), funcName.string());
        sb.Append(TAB).AppendFormat(
            "%s->remote = HdfRemoteServiceObtain((struct HdfObject*)%s, &(%s->dispatcher));\n", objName.string(),
            objName.string(), objName.string());
        sb.Append(TAB).AppendFormat("if (%s->remote == NULL) {\n", objName.string());
        sb.Append(TAB).Append(TAB).AppendFormat(
            "HDF_LOGE(\"%%{public}s: %s->remote is null\", __func__);\n", objName.string());
        sb.Append(TAB).Append(TAB).Append("return false;\n");
        sb.Append(TAB).Append("}\n\n");

        sb.Append(TAB).AppendFormat("if (!HdfRemoteServiceSetInterfaceDesc(%s->remote, %s)) {\n", objName.string(),
            EmitDescMacroName().string());
        sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: ");
        sb.Append("failed to set remote service interface descriptor\", __func__);\n");
        sb.Append(TAB).Append(TAB).AppendFormat("%sRelease(%s);\n", stubTypeName.string(), objName.string());
        sb.Append(TAB).Append(TAB).Append("return false;\n");
        sb.Append(TAB).Append("}\n\n");
    } else {
        sb.Append(TAB).AppendFormat("%s->OnRemoteRequest = %s;\n", objName.string(), funcName.string());
    }

    if (!isKernelCode_) {
        sb.Append(TAB).AppendFormat(
            "%s->interface.AsObject = %sAsObject;\n", objName.string(), stubTypeName.string());
    }
    sb.Append(TAB).Append("return true;\n");
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubReleaseImpl(StringBuilder &sb)
{
    String objName = "stub";
    String stubTypeName = String::Format("%sStub", baseName_.string());
    sb.AppendFormat("void %sRelease(struct %s *%s)\n", stubTypeName.string(), stubTypeName.string(), objName.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.string());
    sb.Append(TAB).Append(TAB).Append("return;\n");
    sb.Append(TAB).Append("}\n\n");

    sb.Append(TAB).AppendFormat("HdfRemoteServiceRecycle(%s->remote);\n", objName.string());
    sb.Append(TAB).AppendFormat("%s->remote = NULL;\n", objName.string());
    sb.Append("}");
}
} // namespace HDI
} // namespace OHOS