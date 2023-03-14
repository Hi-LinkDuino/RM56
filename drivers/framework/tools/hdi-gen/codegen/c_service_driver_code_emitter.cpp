/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/c_service_driver_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/options.h"

namespace OHOS {
namespace HDI {
bool CServiceDriverCodeEmitter::ResolveDirectory(const String &targetDirectory)
{
    if (ast_->GetASTFileType() != ASTFileType::AST_IFACE) {
        return false;
    }

    directory_ = GetFilePath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("CServiceDriverCodeEmitter", "Create '%s' failed!", directory_.string());
        return false;
    }

    return true;
}

void CServiceDriverCodeEmitter::EmitCode()
{
    // the callback interface or interface as method parameter have no driver file.
    if (!interface_->IsSerializable()) {
        EmitDriverSourceFile();
    }
}

void CServiceDriverCodeEmitter::EmitDriverSourceFile()
{
    String filePath = String::Format("%s/%s.c", directory_.string(), FileName(baseName_ + "Driver").string());
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitDriverInclusions(sb);
    sb.Append("\n");
    EmitDriverServiceDecl(sb);
    sb.Append("\n");
    EmitDriverDispatch(sb);
    sb.Append("\n");
    EmitDriverInit(sb);
    sb.Append("\n");
    EmitDriverBind(sb);
    sb.Append("\n");
    EmitDriverRelease(sb);
    sb.Append("\n");
    EmitDriverEntryDefinition(sb);

    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CServiceDriverCodeEmitter::EmitDriverInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFile(HeaderFileType::OWN_MODULE_HEADER_FILE, EmitVersionHeaderName(implName_)));
    GetDriverSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().string());
    }
}

void CServiceDriverCodeEmitter::GetDriverSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "osal_mem"));
    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_device_desc"));
    if (!isKernelCode_) {
        headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_device_object"));
    }
}

void CServiceDriverCodeEmitter::EmitDriverServiceDecl(StringBuilder &sb)
{
    sb.AppendFormat("struct Hdf%sHost {\n", baseName_.string());
    sb.Append(TAB).AppendFormat("struct IDeviceIoService ioservice;\n");
    sb.Append(TAB).AppendFormat("struct %s *service;\n", implName_.string());
    sb.Append("};\n");
}

void CServiceDriverCodeEmitter::EmitDriverDispatch(StringBuilder &sb)
{
    String hostName = baseName_.ToLowerCase() + "Host";
    sb.AppendFormat(
        "static int32_t %sDriverDispatch(struct HdfDeviceIoClient *client, int cmdId,\n", baseName_.string());
    sb.Append(TAB).Append("struct HdfSBuf *data, struct HdfSBuf *reply)\n");
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("struct Hdf%sHost *%s = CONTAINER_OF(\n", baseName_.string(), hostName.string());
    sb.Append(TAB).Append(TAB).AppendFormat(
        "client->device->service, struct Hdf%sHost, ioservice);\n", baseName_.string());

    sb.Append(TAB).AppendFormat("if (%s->service == NULL || %s->service->stub.OnRemoteRequest == NULL) {\n",
        hostName.string(), hostName.string());
    sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: invalid service obj\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return HDF_ERR_INVALID_OBJECT;\n");
    sb.Append(TAB).Append("}\n\n");

    if (!isKernelCode_) {
        sb.Append(TAB).AppendFormat("if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {\n");
        sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: check interface desc failed!\", __func__);\n");
        sb.Append(TAB).Append(TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
        sb.Append(TAB).Append("}\n\n");
    }

    sb.Append(TAB).AppendFormat("return %s->service->stub.OnRemoteRequest(", hostName.string());
    sb.AppendFormat("&%s->service->stub.interface, cmdId, data, reply);\n", hostName.string());
    sb.Append("}\n");
}

void CServiceDriverCodeEmitter::EmitDriverInit(StringBuilder &sb)
{
    sb.AppendFormat("int Hdf%sDriverInit(struct HdfDeviceObject *deviceObject)\n", baseName_.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("HDF_LOGI(\"Hdf%sDriverInit enter.\");\n", baseName_.string());
    sb.Append(TAB).Append("return HDF_SUCCESS;\n");
    sb.Append("}\n");
}

void CServiceDriverCodeEmitter::EmitDriverBind(StringBuilder &sb)
{
    String hostName = baseName_.ToLowerCase() + "Host";
    sb.AppendFormat("int Hdf%sDriverBind(struct HdfDeviceObject *deviceObject)\n", baseName_.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("HDF_LOGI(\"Hdf%sDriverBind enter.\");\n", baseName_.string());
    sb.Append("\n");

    if (!isKernelCode_) {
        sb.Append(TAB).AppendFormat(
            "int32_t ret = HdfDeviceObjectSetInterfaceDesc(deviceObject, %s);\n", EmitDescMacroName().string());
        sb.Append(TAB).Append("if (ret != HDF_SUCCESS) {\n");
        sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"failed to set interface descriptor of device object\");\n");
        sb.Append(TAB).Append(TAB).Append("return ret;\n");
        sb.Append(TAB).Append("}\n\n");
    }

    sb.Append(TAB).AppendFormat("struct Hdf%sHost *%s = (struct Hdf%sHost *)OsalMemAlloc(\n", baseName_.string(),
        hostName.string(), baseName_.string());
    sb.Append(TAB).Append(TAB).AppendFormat("sizeof(struct Hdf%sHost));\n", baseName_.string());
    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", hostName.string());
    sb.Append(TAB).Append(TAB).AppendFormat(
        "HDF_LOGE(\"Hdf%sDriverBind OsalMemAlloc Hdf%sHost failed!\");\n", baseName_.string(), baseName_.string());
    sb.Append(TAB).Append(TAB).Append("return HDF_FAILURE;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append("\n");
    sb.Append(TAB).AppendFormat(
        "%s->ioservice.Dispatch = %sDriverDispatch;\n", hostName.string(), baseName_.string());
    sb.Append(TAB).AppendFormat("%s->ioservice.Open = NULL;\n", hostName.string());
    sb.Append(TAB).AppendFormat("%s->ioservice.Release = NULL;\n", hostName.string());
    sb.Append(TAB).AppendFormat("%s->service = %sServiceGet();\n", hostName.string(), baseName_.string());
    sb.Append(TAB).AppendFormat("if (%s->service == NULL) {\n", hostName.string());
    sb.Append(TAB).Append(TAB).AppendFormat("OsalMemFree(%s);\n", hostName.string());
    sb.Append(TAB).Append(TAB).Append("return HDF_FAILURE;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append("\n");
    sb.Append(TAB).AppendFormat("deviceObject->service = &%s->ioservice;\n", hostName.string());
    sb.Append(TAB).Append("return HDF_SUCCESS;\n");
    sb.Append("}\n");
}

void CServiceDriverCodeEmitter::EmitDriverRelease(StringBuilder &sb)
{
    String hostName = baseName_.ToLowerCase() + "Host";
    sb.AppendFormat("void Hdf%sDriverRelease(struct HdfDeviceObject *deviceObject)\n", baseName_.string());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("HDF_LOGI(\"Hdf%sDriverRelease enter.\");\n", baseName_.string());
    sb.Append(TAB).AppendFormat("struct Hdf%sHost *%s = CONTAINER_OF(", baseName_.string(), hostName.string());
    sb.AppendFormat("deviceObject->service, struct Hdf%sHost, ioservice);\n", baseName_.string());
    sb.Append(TAB).AppendFormat("%sServiceRelease(%s->service);\n", baseName_.string(), hostName.string());
    sb.Append(TAB).AppendFormat("OsalMemFree(%s);\n", hostName.string());
    sb.Append("}\n");
}

void CServiceDriverCodeEmitter::EmitDriverEntryDefinition(StringBuilder &sb)
{
    sb.AppendFormat("struct HdfDriverEntry g_%sDriverEntry = {\n", baseName_.ToLowerCase().string());
    sb.Append(TAB).Append(".moduleVersion = 1,\n");
    sb.Append(TAB).AppendFormat(".moduleName = \"%s\",\n", Options::GetInstance().GetModuleName().string());
    sb.Append(TAB).AppendFormat(".Bind = Hdf%sDriverBind,\n", baseName_.string());
    sb.Append(TAB).AppendFormat(".Init = Hdf%sDriverInit,\n", baseName_.string());
    sb.Append(TAB).AppendFormat(".Release = Hdf%sDriverRelease,\n", baseName_.string());
    sb.Append("};\n\n");
    sb.AppendFormat("HDF_INIT(g_%sDriverEntry);", baseName_.ToLowerCase().string());
}
} // namespace HDI
} // namespace OHOS