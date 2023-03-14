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

#include "ipc/installd_proxy.h"

#include "ipc_types.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
constexpr int32_t WAIT_TIME = 3000;
}

InstalldProxy::InstalldProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IInstalld>(object)
{
    APP_LOGI("installd proxy instance is created");
}

InstalldProxy::~InstalldProxy()
{
    APP_LOGI("installd proxy instance is destroyed");
}

ErrCode InstalldProxy::CreateBundleDir(const std::string &bundleDir)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(bundleDir));

    MessageParcel reply;
    MessageOption option;
    return TransactInstalldCmd(IInstalld::Message::CREATE_BUNDLE_DIR, data, reply, option);
}

ErrCode InstalldProxy::ExtractModuleFiles(const std::string &srcModulePath, const std::string &targetPath,
    const std::string &targetSoPath, const std::string &cpuAbi)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(srcModulePath));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(targetPath));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(targetSoPath));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(cpuAbi));

    MessageParcel reply;
    MessageOption option;
    return TransactInstalldCmd(IInstalld::Message::EXTRACT_MODULE_FILES, data, reply, option);
}

ErrCode InstalldProxy::RenameModuleDir(const std::string &oldPath, const std::string &newPath)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(oldPath));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(newPath));

    MessageParcel reply;
    MessageOption option;
    return TransactInstalldCmd(IInstalld::Message::RENAME_MODULE_DIR, data, reply, option);
}

ErrCode InstalldProxy::CreateBundleDataDir(const std::string &bundleName,
    const int userid, const int uid, const int gid, const std::string &apl)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    INSTALLD_PARCEL_WRITE(data, Int32, userid);
    INSTALLD_PARCEL_WRITE(data, Int32, uid);
    INSTALLD_PARCEL_WRITE(data, Int32, gid);
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(apl));

    MessageParcel reply;
    MessageOption option;
    return TransactInstalldCmd(IInstalld::Message::CREATE_BUNDLE_DATA_DIR, data, reply, option);
}

ErrCode InstalldProxy::RemoveBundleDataDir(const std::string &bundleName, const int userid)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    INSTALLD_PARCEL_WRITE(data, Int32, userid);

    MessageParcel reply;
    MessageOption option;
    return TransactInstalldCmd(IInstalld::Message::REMOVE_BUNDLE_DATA_DIR, data, reply, option);
}

ErrCode InstalldProxy::RemoveModuleDataDir(const std::string &ModuleName, const int userid)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(ModuleName));
    INSTALLD_PARCEL_WRITE(data, Int32, userid);

    MessageParcel reply;
    MessageOption option;
    return TransactInstalldCmd(IInstalld::Message::REMOVE_MODULE_DATA_DIR, data, reply, option);
}

ErrCode InstalldProxy::RemoveDir(const std::string &dir)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(dir));

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    return TransactInstalldCmd(IInstalld::Message::REMOVE_DIR, data, reply, option);
}

ErrCode InstalldProxy::CleanBundleDataDir(const std::string &bundleDir)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(bundleDir));

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC, WAIT_TIME);
    return TransactInstalldCmd(IInstalld::Message::CLEAN_BUNDLE_DATA_DIR, data, reply, option);
}

ErrCode InstalldProxy::GetBundleStats(
    const std::string &bundleName, const int32_t userId, std::vector<int64_t> &bundleStats)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    INSTALLD_PARCEL_WRITE(data, Int32, userId);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    auto ret = TransactInstalldCmd(IInstalld::Message::GET_BUNDLE_STATS, data, reply, option);
    if (ret == ERR_OK) {
        if (reply.ReadInt64Vector(&bundleStats)) {
            return ERR_OK;
        } else {
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ret;
}

ErrCode InstalldProxy::SetDirApl(const std::string &dir, const std::string &bundleName, const std::string &apl)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(dir));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(bundleName));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(apl));

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    return TransactInstalldCmd(IInstalld::Message::SET_DIR_APL, data, reply, option);
}

ErrCode InstalldProxy::GetBundleCachePath(const std::string &dir, std::vector<std::string> &cachePath)
{
    MessageParcel data;
    INSTALLD_PARCEL_WRITE_INTERFACE_TOKEN(data, (GetDescriptor()));
    INSTALLD_PARCEL_WRITE(data, String16, Str8ToStr16(dir));
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    auto ret = TransactInstalldCmd(IInstalld::Message::GET_BUNDLE_CACHE_PATH, data, reply, option);
    if (ret == ERR_OK) {
        if (reply.ReadStringVector(&cachePath)) {
            return ERR_OK;
        } else {
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ret;
}

ErrCode InstalldProxy::TransactInstalldCmd(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        APP_LOGE("fail to send %{public}u cmd to service due to remote object is null", code);
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }

    if (remote->SendRequest(code, data, reply, option) != OHOS::NO_ERROR) {
        APP_LOGE("fail to send %{public}u request to service due to transact error", code);
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }
    return reply.ReadInt32();
}
}  // namespace AppExecFwk
}  // namespace OHOS