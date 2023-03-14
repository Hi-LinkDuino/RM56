/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/c_client_proxy_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
bool CClientProxyCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFilePath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CClientProxyCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CClientProxyCodeEmitter::EmitCode()
{
    EmitProxySourceFile();
}

void CClientProxyCodeEmitter::EmitProxySourceFile()
{
    String filePath = String::Format("%s/%s.c", directory_.string(), FileName(proxyName_).string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitProxyInclusions(sb);
    sb.Append("\n");
    EmitProxyDefinition(sb);
    sb.Append("\n");
    if (!isKernelCode_) {
        EmitProxyCallMethodImpl(sb);
    } else {
        EmitProxyKernelCallMethodImpl(sb);
    }

    sb.Append("\n");
    EmitProxyMethodImpls(sb);
    sb.Append("\n");
    EmitProxyConstruction(sb);
    sb.Append("\n");
    EmitProxyExternalMethodImpl(sb);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CClientProxyCodeEmitter::EmitProxyInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_MODULE_HEADER_FILE, EmitVersionHeaderName(interfaceName_)));
    GetHeaderOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CClientProxyCodeEmitter::GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_sbuf"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "osal_mem"));

    if (isKernelCode_) {
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_io_service_if"));
    } else {
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "servmgr_hdi"));
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_dlist"));
    }

    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_STRING || type->GetTypeKind() == TypeKind::TYPE_UNION) {
            headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec"));
            break;
        }
    }
}

void CClientProxyCodeEmitter::EmitProxyDefinition(StringBuilder &sb)
{
    sb.AppendFormat("struct %sProxy {\n", baseName_.string());
    sb.Append(TAB).AppendFormat("struct %s impl;\n", interfaceName_.string());
    if (isKernelCode_) {
        sb.Append(TAB).Append("struct HdfIoService *serv;\n");
    } else {
        sb.Append(TAB).Append("struct HdfRemoteService *remote;\n");
    }

    sb.Append("};\n");
}

void CClientProxyCodeEmitter::EmitProxyCallMethodImpl(StringBuilder &sb)
{
    sb.AppendFormat("static int32_t %sProxyCall(struct %s *self, int32_t id, struct HdfSBuf *data,\n",
        baseName_.string(), interfaceName_.string());
    sb.Append(TAB).Append("struct HdfSBuf *reply, bool isOneWay)\n");
    sb.Append("{\n");

    String remoteName = "remote";
    sb.Append(TAB).AppendFormat("struct HdfRemoteService *%s = self->AsObject(self);\n", remoteName.string());
    sb.Append(TAB).AppendFormat("if (%s == NULL\n", remoteName.string());
    sb.Append(TAB).Append(TAB).AppendFormat("|| %s->dispatcher == NULL\n", remoteName.string());
    sb.Append(TAB).Append(TAB).AppendFormat("|| %s->dispatcher->Dispatch == NULL\n", remoteName.string());
    sb.Append(TAB).Append(TAB).AppendFormat("|| %s->dispatcher->DispatchAsync == NULL) {\n", remoteName.string());
    sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: Invalid HdfRemoteService obj\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return HDF_ERR_INVALID_OBJECT;\n");
    sb.Append(TAB).Append("}\n");

    sb.Append(TAB).AppendFormat("if (isOneWay) {\n");
    sb.Append(TAB).Append(TAB).AppendFormat(
        "return %s->dispatcher->DispatchAsync(%s, id, data, reply);\n", remoteName.string(), remoteName.string());
    sb.Append(TAB).AppendFormat("} else {\n");
    sb.Append(TAB).Append(TAB).AppendFormat(
        "return %s->dispatcher->Dispatch(%s, id, data, reply);\n", remoteName.string(), remoteName.string());
    sb.Append(TAB).AppendFormat("}\n");
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyKernelCallMethodImpl(StringBuilder &sb)
{
    sb.AppendFormat("static int32_t %sProxyCall(struct %s *self, int32_t id, struct HdfSBuf *data,\n",
        baseName_.string(), interfaceName_.string());
    sb.Append(TAB).Append("struct HdfSBuf *reply)\n");
    sb.Append("{\n");

    String remoteName = "serv";
    sb.Append(TAB).AppendFormat("struct %sProxy *proxy = CONTAINER_OF(self, struct %sProxy, impl);\n",
        baseName_.string(), baseName_.string(), remoteName.string());
    sb.Append(TAB).AppendFormat("struct HdfIoService *%s = proxy->%s;\n", remoteName.string(), remoteName.string());

    sb.Append(TAB).AppendFormat("if (%s == NULL\n", remoteName.string());
    sb.Append(TAB).Append(TAB).AppendFormat("|| %s->dispatcher == NULL\n", remoteName.string());
    sb.Append(TAB).Append(TAB).AppendFormat("|| %s->dispatcher->Dispatch == NULL) {\n", remoteName.string());
    sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: Invalid HdfRemoteService obj\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return HDF_ERR_INVALID_OBJECT;\n");
    sb.Append(TAB).Append("}\n\n");

    sb.Append(TAB).AppendFormat("return %s->dispatcher->Dispatch(", remoteName.string());
    sb.AppendFormat("(struct HdfObject *)&(%s->object), id, data, reply);\n", remoteName.string());
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyMethodImpls(StringBuilder &sb)
{
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitProxyMethodImpl(method, sb);
        sb.Append("\n");
    }

    EmitProxyMethodImpl(interface_->GetVersionMethod(), sb);

    if (!isKernelCode_) {
        sb.Append("\n");
        EmitProxyAsObjectMethodImpl(sb);
    }
}

void CClientProxyCodeEmitter::EmitProxyMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb)
{
    if (method->GetParameterNumber() == 0) {
        sb.AppendFormat("static int32_t %sProxy%s(struct %s *self)\n", baseName_.string(), method->GetName().string(),
            interfaceName_.string());
    } else {
        StringBuilder paramStr;
        paramStr.AppendFormat("static int32_t %sProxy%s(", baseName_.string(), method->GetName().string());
        paramStr.AppendFormat("struct %s *self, ", interfaceName_.string());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitInterfaceMethodParameter(param, paramStr, "");
            if (i + 1 < method->GetParameterNumber()) {
                paramStr.Append(", ");
            }
        }

        paramStr.Append(")");
        sb.Append(SpecificationParam(paramStr, TAB));
        sb.Append("\n");
    }
    EmitProxyMethodBody(method, sb, "");
}

void CClientProxyCodeEmitter::EmitProxyMethodBody(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("int32_t %s = HDF_FAILURE;\n", errorCodeName_.string());

    // Local variable definitions must precede all execution statements.
    EmitInitLoopVar(method, sb, prefix + TAB);

    sb.Append("\n");
    EmitCreateBuf(dataParcelName_, replyParcelName_, sb, prefix + TAB);

    if (!isKernelCode_) {
        sb.Append("\n");
        EmitWriteInterfaceToken(dataParcelName_, sb, prefix + TAB);
    }

    sb.Append("\n");
    EmitWriteFlagOfNeedSetMem(method, dataParcelName_, sb, prefix + TAB);
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() == ParamAttr::PARAM_IN) {
            param->EmitCWriteVar(dataParcelName_, errorCodeName_, finishedLabelName_, sb, prefix + TAB);
            sb.Append("\n");
        } else if (param->EmitCProxyWriteOutVar(
            dataParcelName_, errorCodeName_, finishedLabelName_, sb, prefix + TAB)) {
            sb.Append("\n");
        }
    }

    EmitStubCallMethod(method, sb, prefix + TAB);
    sb.Append("\n");

    if (!method->IsOneWay()) {
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() == ParamAttr::PARAM_OUT) {
                EmitReadProxyMethodParameter(param, replyParcelName_, finishedLabelName_, sb, prefix + TAB);
                sb.Append("\n");
            }
        }
    }

    sb.Append(prefix).AppendFormat("%s:\n", finishedLabelName_);
    EmitReleaseBuf(dataParcelName_, replyParcelName_, sb, prefix + TAB);

    sb.Append(prefix + TAB).AppendFormat("return %s;\n", errorCodeName_.string());
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitCreateBuf(
    const String &dataBufName, const String &replyBufName, StringBuilder &sb, const String &prefix)
{
    if (isKernelCode_) {
        sb.Append(prefix).AppendFormat("struct HdfSBuf *%s = HdfSbufObtainDefaultSize();\n", dataBufName.string());
        sb.Append(prefix).AppendFormat("struct HdfSBuf *%s = HdfSbufObtainDefaultSize();\n", replyBufName.string());
    } else {
        sb.Append(prefix).AppendFormat("struct HdfSBuf *%s = HdfSbufTypedObtain(SBUF_IPC);\n", dataBufName.string());
        sb.Append(prefix).AppendFormat("struct HdfSBuf *%s = HdfSbufTypedObtain(SBUF_IPC);\n", replyBufName.string());
    }

    sb.Append("\n");
    sb.Append(prefix).AppendFormat("if (%s == NULL || %s == NULL) {\n", dataBufName.string(), replyBufName.string());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: HdfSubf malloc failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", finishedLabelName_);
    sb.Append(prefix).Append("}\n");
}

void CClientProxyCodeEmitter::EmitWriteInterfaceToken(
    const String &dataBufName, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat(
        "if (!HdfRemoteServiceWriteInterfaceToken(self->AsObject(self), %s)) {\n", dataBufName.string());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: write interface token failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", finishedLabelName_);
    sb.Append(prefix).Append("}\n");
}

void CClientProxyCodeEmitter::EmitWriteFlagOfNeedSetMem(
    const AutoPtr<ASTMethod> &method, const String &dataBufName, StringBuilder &sb, const String &prefix)
{
    if (NeedFlag(method)) {
        sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint8(%s, 1)) {\n", dataBufName.string());
        sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: write flag of memory setting failed!\", __func__);\n");
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", finishedLabelName_);
        sb.Append(prefix).Append("}\n\n");
    }
}

void CClientProxyCodeEmitter::EmitReleaseBuf(
    const String &dataBufName, const String &replyBufName, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", dataBufName.string());
    sb.Append(prefix + TAB).AppendFormat("HdfSbufRecycle(%s);\n", dataBufName.string());
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", replyBufName.string());
    sb.Append(prefix + TAB).AppendFormat("HdfSbufRecycle(%s);\n", replyBufName.string());
    sb.Append(prefix).Append("}\n");
}

void CClientProxyCodeEmitter::EmitReadProxyMethodParameter(const AutoPtr<ASTParameter> &param, const String &parcelName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTType> type = param->GetType();
    if (type->GetTypeKind() == TypeKind::TYPE_STRING) {
        String cloneName = String::Format("%sCopy", param->GetName().string());
        type->EmitCProxyReadVar(parcelName, cloneName, false, errorCodeName_, gotoLabel, sb, prefix);
        sb.Append(prefix).AppendFormat("if (strcpy_s(%s, %sLen, %s) != EOK) {\n", param->GetName().string(),
            param->GetName().string(), cloneName.string());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", param->GetName().string());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.string());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
        sb.Append(prefix).Append("}\n");
    } else if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        type->EmitCProxyReadVar(parcelName, param->GetName().string(), false, errorCodeName_, gotoLabel, sb, prefix);
    } else if (type->GetTypeKind() == TypeKind::TYPE_UNION) {
        String cpName = String::Format("%sCp", param->GetName().string());
        type->EmitCProxyReadVar(parcelName, cpName, false, errorCodeName_, gotoLabel, sb, prefix);
        sb.Append(prefix).AppendFormat("(void)memcpy_s(%s, sizeof(%s), %s, sizeof(%s));\n", param->GetName().string(),
            type->EmitCType().string(), cpName.string(), type->EmitCType().string());
    } else {
        type->EmitCProxyReadVar(parcelName, param->GetName(), false, errorCodeName_, gotoLabel, sb, prefix);
    }
}

void CClientProxyCodeEmitter::EmitStubCallMethod(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    if (!isKernelCode_) {
        sb.Append(prefix).AppendFormat("%s = %sCall(self, %s, %s, %s, %s);\n", errorCodeName_.string(),
            proxyName_.string(), EmitMethodCmdID(method).string(), dataParcelName_.string(), replyParcelName_.string(),
            method->IsOneWay() ? "true" : "false");
    } else {
        sb.Append(prefix).AppendFormat("%s = %sCall(self, %s, %s, %s);\n", errorCodeName_.string(), proxyName_.string(),
            EmitMethodCmdID(method).string(), dataParcelName_.string(), replyParcelName_.string());
    }
    sb.Append(prefix).AppendFormat("if (%s != HDF_SUCCESS) {\n", errorCodeName_.string());
    sb.Append(prefix + TAB).AppendFormat(
        "HDF_LOGE(\"%%{public}s: call failed! error code is %%{public}d\", __func__, %s);\n", errorCodeName_.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", finishedLabelName_);
    sb.Append(prefix).Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyAsObjectMethodImpl(StringBuilder &sb)
{
    sb.AppendFormat("static struct HdfRemoteService *%sProxyAsObject(struct %s *self)\n", baseName_.string(),
        interfaceName_.string());
    sb.Append("{\n");
    sb.Append(TAB).Append("if (self == NULL) {\n");
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append(TAB).AppendFormat(
        "struct %sProxy *proxy = CONTAINER_OF(self, struct %sProxy, impl);\n", baseName_.string(), baseName_.string());
    sb.Append(TAB).Append("return proxy->remote;\n");
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyConstruction(StringBuilder &sb)
{
    String objName = "impl";
    sb.AppendFormat(
        "static void %sProxyConstruct(struct %s *%s)\n", baseName_.string(), interfaceName_.string(), objName.string());
    sb.Append("{\n");

    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.Append(TAB).AppendFormat("%s->%s = %sProxy%s;\n", objName.string(), method->GetName().string(),
            baseName_.string(), method->GetName().string());
    }

    AutoPtr<ASTMethod> getVerMethod = interface_->GetVersionMethod();
    sb.Append(TAB).AppendFormat("%s->%s = %sProxy%s;\n", objName.string(), getVerMethod->GetName().string(),
        baseName_.string(), getVerMethod->GetName().string());

    if (!isKernelCode_) {
        sb.Append(TAB).AppendFormat("%s->AsObject = %sProxyAsObject;\n", objName.string(), baseName_.string());
    }

    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyExternalMethodImpl(StringBuilder &sb)
{
    String objName = "client";
    String serMajorName = "serMajorVer";
    String serMinorName = "serMinorVer";
    String remoteName = "remote";
    String serviceName = "serviceName";

    if (isKernelCode_) {
        remoteName = "serv";
        EmitProxyGetMethodImpl(sb);
        sb.Append("\n");
        EmitKernelProxyGetInstanceMethodImpl(objName, serMajorName, serMinorName, remoteName, serviceName, sb);
        sb.Append("\n");
        EmitProxyReleaseMethodImpl(remoteName, "HdfIoServiceRecycle", sb);
    } else if (interface_->IsSerializable()) {
        EmitCbProxyGetMethodImpl(objName, serMajorName, serMinorName, remoteName, sb);
        sb.Append("\n");
        EmitProxyReleaseMethodImpl(remoteName, "HdfRemoteServiceRecycle", sb);
    } else {
        EmitProxyGetMethodImpl(sb);
        sb.Append("\n");
        EmitProxyGetInstanceMethodImpl(objName, serMajorName, serMinorName, remoteName, serviceName, sb);
        sb.Append("\n");
        EmitProxyReleaseMethodImpl(remoteName, "HdfRemoteServiceRecycle", sb);
    }
}

void CClientProxyCodeEmitter::EmitProxyGetMethodImpl(StringBuilder &sb)
{
    sb.AppendFormat("struct %s *%sGet()\n", interfaceName_.string(), baseName_.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("return %sGetInstance(\"%s\");\n", baseName_.string(), FileName(implName_).string());
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyGetInstanceMethodImpl(const String &objName, const String &serMajorName,
    const String &serMinorName, const String &remoteName, const String &serviceName, StringBuilder &sb)
{
    sb.AppendFormat("struct %s *%sGetInstance(const char *%s)\n", interfaceName_.string(), baseName_.string(),
        serviceName.string());
    sb.Append("{\n");
    EmitProxyGetRemoteService(remoteName, serviceName, sb, TAB);
    sb.Append("\n");
    EmitProxySetInterfaceDesc(remoteName, "HdfRemoteServiceRecycle", sb, TAB);
    sb.Append("\n");
    EmitProxyCreateProxyObject(objName, remoteName, "HdfRemoteServiceRecycle", sb, TAB);
    sb.Append("\n");
    EmitProxyCheckVersion(objName, serMajorName, serMinorName, sb, TAB);
    sb.Append("\n");
    sb.Append(TAB).AppendFormat("return %s;\n", objName.string());
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitKernelProxyGetInstanceMethodImpl(const String &objName, const String &serMajorName,
    const String &serMinorName, const String &remoteName, const String &serviceName, StringBuilder &sb)
{
    sb.AppendFormat("struct %s *%sGetInstance(const char* %s)\n", interfaceName_.string(), baseName_.string(),
        serviceName.string());
    sb.Append("{\n");
    EmitProxyGetRemoteService(remoteName, serviceName, sb, TAB);
    sb.Append("\n");
    EmitProxyCreateProxyObject(objName, remoteName, "HdfIoServiceRecycle", sb, TAB);
    sb.Append("\n");
    EmitProxyCheckVersion(objName, serMajorName, serMinorName, sb, TAB);
    sb.Append("\n");
    sb.Append(TAB).AppendFormat("return %s;\n", objName.string());
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitCbProxyGetMethodImpl(const String &objName, const String &serMajorName,
    const String &serMinorName, const String &remoteName, StringBuilder &sb)
{
    sb.AppendFormat("struct %s *%sGet(struct HdfRemoteService *%s)\n", interfaceName_.string(), baseName_.string(),
        remoteName.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", remoteName.string());
    sb.Append(TAB).Append(TAB).AppendFormat("HDF_LOGE(\"%%{public}s: remote is null\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n\n");
    EmitProxySetInterfaceDesc(remoteName, "HdfRemoteServiceRecycle", sb, TAB);
    sb.Append("\n");
    EmitProxyCreateProxyObject(objName, remoteName, "HdfRemoteServiceRecycle", sb, TAB);
    sb.Append("\n");
    EmitProxyCheckVersion(objName, serMajorName, serMinorName, sb, TAB);
    sb.Append("\n");
    sb.Append(TAB).AppendFormat("return %s;\n", objName.string());
    sb.Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyGetRemoteService(
    const String &remoteName, const String &serviceName, StringBuilder &sb, const String &prefix)
{
    if (!isKernelCode_) {
        sb.Append(prefix).Append("struct HDIServiceManager *serviceMgr = HDIServiceManagerGet();\n");
        sb.Append(prefix).Append("if (serviceMgr == NULL) {\n");
        sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: HDIServiceManager not found!\", __func__);\n");
        sb.Append(prefix + TAB).Append("return NULL;\n");
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("struct HdfRemoteService *%s = ", remoteName.string());
        sb.AppendFormat("serviceMgr->GetService(serviceMgr, %s);\n", serviceName.string());
        sb.Append(prefix).Append("HDIServiceManagerRelease(serviceMgr);\n");
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", remoteName.string());
        sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to get remote!\", __func__);\n");
        sb.Append(prefix + TAB).Append("return NULL;\n");
        sb.Append(prefix).Append("}\n");
    } else {
        sb.Append(prefix).AppendFormat("struct HdfIoService *%s = ", remoteName.string());
        sb.AppendFormat("HdfIoServiceBind(%s);\n", serviceName.string());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", remoteName.string());
        sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to get io service!\", __func__);\n");
        sb.Append(prefix + TAB).Append("return NULL;\n");
        sb.Append(prefix).Append("}\n");
    }
}

void CClientProxyCodeEmitter::EmitProxySetInterfaceDesc(
    const String &remoteName, const String &recycleFuncName, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat(
        "if (!HdfRemoteServiceSetInterfaceDesc(%s, %s)) {\n", remoteName.string(), EmitDescMacroName().string());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: set interface token failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("%s(%s);\n", recycleFuncName.string(), remoteName.string());
    sb.Append(prefix + TAB).Append("return NULL;\n");
    sb.Append(prefix).Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyCreateProxyObject(const String &clientObjName, const String &remoteName,
    const String &recycleFuncName, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("struct %sProxy *proxy = (struct %sProxy *)OsalMemAlloc(sizeof(struct %sProxy));\n",
        baseName_.string(), baseName_.string(), baseName_.string());
    sb.Append(prefix).Append("if (proxy == NULL) {\n");
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s proxy failed!\", __func__);\n", interfaceName_.string());
    if (!interface_->IsSerializable()) {
        sb.Append(prefix + TAB).AppendFormat("%s(%s);\n", recycleFuncName.string(), remoteName.string());
    }
    sb.Append(prefix + TAB).Append("return NULL;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append("\n");
    sb.Append(prefix).AppendFormat("proxy->%s = %s;\n", remoteName.string(), remoteName.string());
    sb.Append(prefix).AppendFormat("%sProxyConstruct(&proxy->impl);\n", baseName_.string());
    sb.Append(prefix).AppendFormat("struct %s *%s = &proxy->impl;\n", interfaceName_.string(), clientObjName.string());
}

void CClientProxyCodeEmitter::EmitProxyCheckVersion(const String &clientObjName, const String &serMajorName,
    const String &serMinorName, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("uint32_t %s = 0;\n", serMajorName.string());
    sb.Append(prefix).AppendFormat("uint32_t %s = 0;\n", serMinorName.string());
    sb.Append(prefix).AppendFormat("int32_t %s = %s->GetVersion(%s, &%s, &%s);\n", errorCodeName_.string(),
        clientObjName.string(), clientObjName.string(), serMajorName.string(), serMinorName.string());
    sb.Append(prefix).AppendFormat("if (%s != HDF_SUCCESS) {\n", errorCodeName_.string());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: get version failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("%sRelease(%s);\n", baseName_.string(), clientObjName.string());
    sb.Append(prefix + TAB).Append("return NULL;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (%s != %s) {\n", serMajorName.string(), majorVerName_.string());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s:check version failed! ");
    sb.Append("version of service:%u.%u, version of client:%u.%u\", __func__,\n");
    sb.Append(prefix + TAB + TAB).AppendFormat("%s, %s, %s, %s);\n", serMajorName.string(), serMinorName.string(),
        majorVerName_.string(), minorVerName_.string());
    sb.Append(prefix + TAB).AppendFormat("%sRelease(%s);\n", baseName_.string(), clientObjName.string());
    sb.Append(prefix + TAB).Append("return NULL;\n");
    sb.Append(prefix).Append("}\n");
}

void CClientProxyCodeEmitter::EmitProxyReleaseMethodImpl(
    const String &remoteName, const String &recycleFuncName, StringBuilder &sb)
{
    sb.AppendFormat("void %sRelease(struct %s *instance)\n", baseName_.string(), interfaceName_.string());
    sb.Append("{\n");
    sb.Append(TAB).Append("if (instance == NULL) {\n");
    sb.Append(TAB).Append(TAB).Append("return;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append(TAB).AppendFormat("struct %sProxy *proxy = CONTAINER_OF(instance, struct %sProxy, impl);\n",
        baseName_.string(), baseName_.string());
    sb.Append(TAB).AppendFormat("%s(proxy->%s);\n", recycleFuncName.string(), remoteName.string());
    sb.Append(TAB).Append("OsalMemFree(proxy);\n");
    sb.Append("}\n");
}
} // namespace HDI
} // namespace OHOS