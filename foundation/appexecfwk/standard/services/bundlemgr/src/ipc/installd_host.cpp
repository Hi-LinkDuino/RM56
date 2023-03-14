/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ipc/installd_host.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
InstalldHost::InstalldHost()
{
    init();
    APP_LOGI("installd host instance is created");
}

InstalldHost::~InstalldHost()
{
    APP_LOGI("installd host instance is destroyed");
}

void InstalldHost::init()
{
    funcMap_.emplace(IInstalld::Message::CREATE_BUNDLE_DIR, &InstalldHost::HandleCreateBundleDir);
    funcMap_.emplace(IInstalld::Message::EXTRACT_MODULE_FILES, &InstalldHost::HandleExtractModuleFiles);
    funcMap_.emplace(IInstalld::Message::RENAME_MODULE_DIR, &InstalldHost::HandleRenameModuleDir);
    funcMap_.emplace(IInstalld::Message::CREATE_BUNDLE_DATA_DIR, &InstalldHost::HandleCreateBundleDataDir);
    funcMap_.emplace(IInstalld::Message::REMOVE_BUNDLE_DATA_DIR, &InstalldHost::HandleRemoveBundleDataDir);
    funcMap_.emplace(IInstalld::Message::REMOVE_MODULE_DATA_DIR, &InstalldHost::HandleRemoveModuleDataDir);
    funcMap_.emplace(IInstalld::Message::CLEAN_BUNDLE_DATA_DIR, &InstalldHost::HandleCleanBundleDataDir);
    funcMap_.emplace(IInstalld::Message::SET_DIR_APL, &InstalldHost::HandleSetDirApl);
    funcMap_.emplace(IInstalld::Message::REMOVE_DIR, &InstalldHost::HandleRemoveDir);
    funcMap_.emplace(IInstalld::Message::GET_BUNDLE_STATS, &InstalldHost::HandleGetBundleStats);
    funcMap_.emplace(IInstalld::Message::GET_BUNDLE_CACHE_PATH, &InstalldHost::HandleGetBundleCachePath);
}

int InstalldHost::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGD(
        "installd host receives message from client, code = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descripter = InstalldHost::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("installd host fail to write reply message due to the reply is nullptr");
        return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
    }
    bool result = true;
    APP_LOGD("funcMap_ size is %{public}d", static_cast<int32_t>(funcMap_.size()));
    if (funcMap_.find(code) != funcMap_.end() && funcMap_[code] != nullptr) {
        result = (this->*funcMap_[code])(data, reply);
    } else {
        APP_LOGW("installd host receives unknown code, code = %{public}u", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    APP_LOGD("installd host finish to process message from client");
    return result ? NO_ERROR : OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
}

bool InstalldHost::HandleCreateBundleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = Str16ToStr8(data.ReadString16());
    APP_LOGI("bundleName %{public}s", bundleDir.c_str());
    ErrCode result = CreateBundleDir(bundleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleExtractModuleFiles(MessageParcel &data, MessageParcel &reply)
{
    std::string srcModulePath = Str16ToStr8(data.ReadString16());
    std::string targetPath = Str16ToStr8(data.ReadString16());
    std::string targetSoPath = Str16ToStr8(data.ReadString16());
    std::string cpuAbi = Str16ToStr8(data.ReadString16());
    APP_LOGI("extract module %{private}s", targetPath.c_str());
    ErrCode result = ExtractModuleFiles(srcModulePath, targetPath, targetSoPath, cpuAbi);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRenameModuleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string oldPath = Str16ToStr8(data.ReadString16());
    std::string newPath = Str16ToStr8(data.ReadString16());
    APP_LOGI("rename moduleDir %{private}s", oldPath.c_str());
    ErrCode result = RenameModuleDir(oldPath, newPath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCreateBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int userid = data.ReadInt32();
    int uid = data.ReadInt32();
    int gid = data.ReadInt32();
    std::string apl = Str16ToStr8(data.ReadString16());
    ErrCode result = CreateBundleDataDir(bundleName, userid, uid, gid, apl);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int userid = data.ReadInt32();
    ErrCode result = RemoveBundleDataDir(bundleName, userid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveModuleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string moduleNmae = Str16ToStr8(data.ReadString16());
    int userid = data.ReadInt32();
    ErrCode result = RemoveModuleDataDir(moduleNmae, userid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveDir(MessageParcel &data, MessageParcel &reply)
{
    std::string removedDir = Str16ToStr8(data.ReadString16());
    ErrCode result = RemoveDir(removedDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCleanBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = Str16ToStr8(data.ReadString16());
    ErrCode result = CleanBundleDataDir(bundleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleGetBundleStats(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int32_t userId = data.ReadInt32();
    std::vector<int64_t> bundleStats;
    ErrCode result = GetBundleStats(bundleName, userId, bundleStats);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteInt64Vector(bundleStats)) {
        APP_LOGE("HandleGetBundleStats write failed");
        return false;
    }
    return true;
}

bool InstalldHost::HandleSetDirApl(MessageParcel &data, MessageParcel &reply)
{
    std::string dataDir = Str16ToStr8(data.ReadString16());
    std::string bundleName = Str16ToStr8(data.ReadString16());
    std::string apl = Str16ToStr8(data.ReadString16());
    ErrCode result = SetDirApl(dataDir, bundleName, apl);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleGetBundleCachePath(MessageParcel &data, MessageParcel &reply)
{
    std::string dir = Str16ToStr8(data.ReadString16());
    std::vector<std::string> cachePath;
    ErrCode result = GetBundleCachePath(dir, cachePath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteStringVector(cachePath)) {
        APP_LOGE("fail to GetBundleCachePath from reply");
        return false;
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
