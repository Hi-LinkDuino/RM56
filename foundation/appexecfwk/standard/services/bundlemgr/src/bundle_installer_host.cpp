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

#include "bundle_installer_host.h"

#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_permission_mgr.h"
#include "bundle_sandbox_installer.h"
#include "bundle_util.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string INSTALL_THREAD = "install_thread";
const std::string GET_MANAGER_FAIL = "fail to get bundle installer manager";
int32_t INVALID_APP_INDEX = 0;
int32_t LOWER_DLP_TYPE_BOUND = 0;
int32_t UPPER_DLP_TYPE_BOUND = 3;
}  // namespace

BundleInstallerHost::BundleInstallerHost()
{
    APP_LOGI("create bundle installer host instance");
}

BundleInstallerHost::~BundleInstallerHost()
{
    APP_LOGI("destroy bundle installer host instance");
}

bool BundleInstallerHost::Init()
{
    APP_LOGD("begin to init");
    auto installRunner = EventRunner::Create(INSTALL_THREAD);
    if (!installRunner) {
        APP_LOGE("create install runner fail");
        return false;
    }
    manager_ = std::make_shared<BundleInstallerManager>(installRunner);
    APP_LOGD("init successfully");
    return true;
}

int BundleInstallerHost::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGD("bundle installer host onReceived message, the message code is %{public}u", code);
    std::u16string descripter = GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("fail to write reply message in bundle mgr host due to the reply is nullptr");
        return OBJECT_NULL;
    }

    switch (code) {
        case IBundleInstaller::Message::INSTALL:
            HandleInstallMessage(data);
            break;
        case IBundleInstaller::Message::INSTALL_MULTIPLE_HAPS:
            HandleInstallMultipleHapsMessage(data);
            break;
        case IBundleInstaller::Message::UNINSTALL:
            HandleUninstallMessage(data);
            break;
        case IBundleInstaller::Message::UNINSTALL_MODULE:
            HandleUninstallModuleMessage(data);
            break;
        case IBundleInstaller::Message::RECOVER:
            HandleRecoverMessage(data);
            break;
        case IBundleInstaller::Message::INSTALL_SANDBOX_APP:
            HandleInstallSandboxApp(data, reply);
            break;
        case IBundleInstaller::Message::UNINSTALL_SANDBOX_APP:
            HandleUninstallSandboxApp(data, reply);
            break;
        case IBundleInstaller::Message::CREATE_STREAM_INSTALLER:
            HandleCreateStreamInstaller(data, reply);
            break;
        case IBundleInstaller::Message::DESTORY_STREAM_INSTALLER:
            HandleDestoryBundleStreamInstaller(data, reply);
            break;
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}

void BundleInstallerHost::HandleInstallMessage(Parcel &data)
{
    APP_LOGD("handle install message");
    std::string bundlePath = Str16ToStr8(data.ReadString16());
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return;
    }
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Install(bundlePath, *installParam, statusReceiver);
    APP_LOGD("handle install message finished");
}

void BundleInstallerHost::HandleRecoverMessage(Parcel &data)
{
    APP_LOGD("handle install message by bundleName");
    std::string bundleName = Str16ToStr8(data.ReadString16());
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return;
    }
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Recover(bundleName, *installParam, statusReceiver);
    APP_LOGD("handle install message by bundleName finished");
}

void BundleInstallerHost::HandleInstallMultipleHapsMessage(Parcel &data)
{
    APP_LOGD("handle install multiple haps message");
    int32_t size = data.ReadInt32();
    std::vector<std::string> pathVec;
    for (int i = 0; i < size; ++i) {
        pathVec.emplace_back(Str16ToStr8(data.ReadString16()));
    }
    if (size == 0 || pathVec.empty()) {
        APP_LOGE("inputted bundlepath vector is empty");
        return;
    }
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return;
    }
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Install(pathVec, *installParam, statusReceiver);
    APP_LOGD("handle install multiple haps finished");
}

void BundleInstallerHost::HandleUninstallMessage(Parcel &data)
{
    APP_LOGD("handle uninstall message");
    std::string bundleName = Str16ToStr8(data.ReadString16());
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return;
    }
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Uninstall(bundleName, *installParam, statusReceiver);
    APP_LOGD("handle uninstall message finished");
}

void BundleInstallerHost::HandleUninstallModuleMessage(Parcel &data)
{
    APP_LOGD("handle uninstall module message");
    std::string bundleName = Str16ToStr8(data.ReadString16());
    std::string modulePackage = Str16ToStr8(data.ReadString16());
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return;
    }
    sptr<IStatusReceiver> statusReceiver = iface_cast<IStatusReceiver>(object);

    Uninstall(bundleName, modulePackage, *installParam, statusReceiver);
    APP_LOGD("handle uninstall message finished");
}

void BundleInstallerHost::HandleInstallSandboxApp(Parcel &data, Parcel &reply)
{
    APP_LOGD("handle install sandbox app message");
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int32_t dplType = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    int32_t appIndex = Constants::INITIAL_APP_INDEX;
    auto ret = InstallSandboxApp(bundleName, dplType, userId, appIndex);
    if (!reply.WriteInt32(ret)) {
        APP_LOGE("write failed");
    }
    if (ret == ERR_OK && !reply.WriteInt32(appIndex)) {
        APP_LOGE("write failed");
    }
    APP_LOGD("handle install sandbox app message finished");
}

void BundleInstallerHost::HandleUninstallSandboxApp(Parcel &data, Parcel &reply)
{
    APP_LOGD("handle install sandbox app message");
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int32_t appIndex = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    auto ret = UninstallSandboxApp(bundleName, appIndex, userId);
    if (!reply.WriteInt32(ret)) {
        APP_LOGE("write failed");
    }
    APP_LOGD("handle install sandbox app message finished");
}

void BundleInstallerHost::HandleCreateStreamInstaller(Parcel &data, Parcel &reply)
{
    APP_LOGD("handle create stream installer message begin");
    std::unique_ptr<InstallParam> installParam(data.ReadParcelable<InstallParam>());
    if (!installParam) {
        APP_LOGE("ReadParcelable<InstallParam> failed");
        return;
    }

    sptr<IBundleStreamInstaller> streamInstaller = CreateStreamInstaller(*installParam);
    if (!streamInstaller) {
        if (!reply.WriteBool(false)) {
            APP_LOGE("write result failed");
            return;
        }
    }
    if (!reply.WriteBool(true)) {
        APP_LOGE("write result failed");
        return;
    }
    if (!reply.WriteUint32(streamInstaller->GetInstallerId())) {
        APP_LOGE("write stream installe id failed");
        return;
    }
    if (!reply.WriteRemoteObject(streamInstaller->AsObject())) {
        APP_LOGE("write stream installer remote object failed");
        return;
    }

    std::lock_guard<std::mutex> lock(streamInstallMutex_);
    streamInstallers_.emplace_back(streamInstaller);
    APP_LOGD("handle create stream installer message finish");
}

void BundleInstallerHost::HandleDestoryBundleStreamInstaller(Parcel &data, Parcel &reply)
{
    APP_LOGD("handle destory stream installer message begin");
    uint32_t installeId = data.ReadUint32();
    DestoryBundleStreamInstaller(installeId);
    APP_LOGD("handle destoy stream installer message finish");
}


bool BundleInstallerHost::Install(
    const std::string &bundleFilePath, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("install permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED, "");
        return false;
    }

    manager_->CreateInstallTask(bundleFilePath, CheckInstallParam(installParam), statusReceiver);
    return true;
}

bool BundleInstallerHost::Install(const std::vector<std::string> &bundleFilePaths, const InstallParam &installParam,
    const sptr<IStatusReceiver> &statusReceiver)
{
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("install permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED, "");
        return false;
    }

    manager_->CreateInstallTask(bundleFilePaths, CheckInstallParam(installParam), statusReceiver);
    return true;
}

bool BundleInstallerHost::Recover(
    const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("install permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED, "");
        return false;
    }
    manager_->CreateRecoverTask(bundleName, CheckInstallParam(installParam), statusReceiver);
    return true;
}

bool BundleInstallerHost::Uninstall(
    const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("uninstall permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED, "");
        return false;
    }
    manager_->CreateUninstallTask(bundleName, CheckInstallParam(installParam), statusReceiver);
    return true;
}

bool BundleInstallerHost::Uninstall(const std::string &bundleName, const std::string &modulePackage,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("uninstall permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED, "");
        return false;
    }
    manager_->CreateUninstallTask(
        bundleName, modulePackage, CheckInstallParam(installParam), statusReceiver);
    return true;
}

bool BundleInstallerHost::InstallByBundleName(const std::string &bundleName,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    if (!CheckBundleInstallerManager(statusReceiver)) {
        APP_LOGE("statusReceiver invalid");
        return false;
    }

    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("install permission denied");
        statusReceiver->OnFinished(ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED, "");
        return false;
    }

    manager_->CreateInstallByBundleNameTask(bundleName, CheckInstallParam(installParam), statusReceiver);
    return true;
}

ErrCode BundleInstallerHost::InstallSandboxApp(const std::string &bundleName, int32_t dplType, int32_t userId,
    int32_t &appIndex)
{
    if (bundleName.empty() || dplType <= LOWER_DLP_TYPE_BOUND || dplType >= UPPER_DLP_TYPE_BOUND) {
        APP_LOGE("install sandbox failed due to error parameters");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    std::shared_ptr<BundleSandboxInstaller> installer = std::make_shared<BundleSandboxInstaller>();
    if (installer == nullptr) {
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }
    auto res = installer->InstallSandboxApp(bundleName, dplType, userId, appIndex);
    if (res != ERR_OK) {
        APP_LOGE("install sandbox failed due to error code : %{public}d", res);
    }
    return res;
}

ErrCode BundleInstallerHost::UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId)
{
    // check bundle name
    if (bundleName.empty()) {
        APP_LOGE("install sandbox failed due to empty bundleName");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    // check appIndex
    if (appIndex <= INVALID_APP_INDEX || appIndex > Constants::MAX_APP_INDEX) {
        APP_LOGE("the appIndex %{public}d is invalid", appIndex);
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    std::shared_ptr<BundleSandboxInstaller> installer = std::make_shared<BundleSandboxInstaller>();
    if (installer == nullptr) {
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }
    auto res = installer->UninstallSandboxApp(bundleName, appIndex, userId);
    if (res != ERR_OK) {
        APP_LOGE("uninstall sandbox failed due to error code : %{public}d", res);
    }
    return res;
}

ErrCode BundleInstallerHost::StreamInstall(const std::vector<std::string> &bundleFilePaths,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    return ERR_OK;
}

sptr<IBundleStreamInstaller> BundleInstallerHost::CreateStreamInstaller(const InstallParam &installParam)
{
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("install permission denied");
        return nullptr;
    }
    auto uid = IPCSkeleton::GetCallingUid();
    sptr<BundleStreamInstallerHostImpl> streamInstaller(new (std::nothrow) BundleStreamInstallerHostImpl(
        ++streamInstallerIds_, uid));
    if (streamInstaller == nullptr) {
        APP_LOGE("streamInstaller is nullptr");
        return nullptr;
    }
    bool res = streamInstaller->Init(installParam);
    if (!res) {
        APP_LOGE("stream installer init failed");
        return nullptr;
    }
    return streamInstaller;
}

bool BundleInstallerHost::DestoryBundleStreamInstaller(uint32_t streamInstallerId)
{
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("install permission denied");
        return false;
    }
    std::lock_guard<std::mutex> lock(streamInstallMutex_);
    for (auto it = streamInstallers_.begin(); it != streamInstallers_.end();) {
        if ((*it)->GetInstallerId() == streamInstallerId) {
            (*it)->UnInit();
            it = streamInstallers_.erase(it);
        } else {
            it++;
        }
    }
    return true;
}

bool BundleInstallerHost::CheckBundleInstallerManager(const sptr<IStatusReceiver> &statusReceiver) const
{
    if (!statusReceiver) {
        APP_LOGE("the receiver is nullptr");
        return false;
    }
    if (!manager_) {
        APP_LOGE("the bundle installer manager is nullptr");
        statusReceiver->OnFinished(ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR, GET_MANAGER_FAIL);
        return false;
    }
    return true;
}

InstallParam BundleInstallerHost::CheckInstallParam(const InstallParam &installParam)
{
    if (installParam.userId == Constants::UNSPECIFIED_USERID) {
        APP_LOGI("installParam userId is unspecified and get calling userId by callingUid");
        InstallParam callInstallParam = installParam;
        callInstallParam.userId = BundleUtil::GetUserIdByCallingUid();
        return callInstallParam;
    }

    return installParam;
}

}  // namespace AppExecFwk
}  // namespace OHOS