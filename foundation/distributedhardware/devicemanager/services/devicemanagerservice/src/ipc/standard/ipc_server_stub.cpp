/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ipc_server_stub.h"

#include <algorithm>
#include <thread>

#include "device_manager_impl.h"
#include "device_manager_service.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "if_system_ability_manager.h"
#include "ipc_cmd_register.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(IpcServerStub);

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&IpcServerStub::GetInstance());

IpcServerStub::IpcServerStub() : SystemAbility(DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID, true)
{
    registerToService_ = false;
    state_ = ServiceRunningState::STATE_NOT_START;
}

void IpcServerStub::OnStart()
{
    LOGI("IpcServerStub::OnStart start");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        LOGI("IpcServerStub has already started.");
        return;
    }
    if (!Init()) {
        LOGE("failed to init IpcServerStub");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
}

bool IpcServerStub::Init()
{
    LOGI("IpcServerStub::Init ready to init.");
    if (!registerToService_) {
        bool ret = Publish(this);
        if (!ret) {
            LOGE("IpcServerStub::Init Publish failed!");
            return false;
        }
        registerToService_ = true;
    }
    DeviceManagerService::GetInstance().Init();
    return true;
}

void IpcServerStub::OnStop()
{
    LOGI("IpcServerStub::OnStop ready to stop service.");
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
}

int32_t IpcServerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    LOGI("code = %u, flags= %d.", code, option.GetFlags());
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        LOGI("ReadInterfaceToken fail!");
        return ERR_INVALID_STATE;
    }

    int32_t ret = IpcCmdRegister::GetInstance().OnIpcCmd((int32_t)code, data, reply);
    if (ret == DM_IPC_NOT_REGISTER_FUNC) {
        LOGW("unsupported code: %d", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ret;
}

int32_t IpcServerStub::SendCmd(int32_t cmdCode, std::shared_ptr<IpcReq> req, std::shared_ptr<IpcRsp> rsp)
{
    LOGI("SendCmd cmdCode: %d", cmdCode);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (IpcCmdRegister::GetInstance().SetRequest(cmdCode, req, data) != DM_OK) {
        LOGE("set request cmd failed");
        return DM_IPC_FAILED;
    }
    
    int32_t ret = IpcCmdRegister::GetInstance().OnIpcCmd(cmdCode, data, reply);
    if (ret == DM_IPC_NOT_REGISTER_FUNC) {
        LOGW("unsupported code: %d", cmdCode);
        return IPCObjectStub::OnRemoteRequest(cmdCode, data, reply, option);
    }
    return IpcCmdRegister::GetInstance().ReadResponse(cmdCode, reply, rsp);
}

ServiceRunningState IpcServerStub::QueryServiceState() const
{
    return state_;
}

int32_t IpcServerStub::RegisterDeviceManagerListener(std::string &pkgName, sptr<IRemoteObject> listener)
{
    if (pkgName.empty() || listener == nullptr) {
        LOGE("RegisterDeviceManagerListener error: input parameter invalid.");
        return DM_POINT_NULL;
    }

    if (!DeviceManagerService::GetInstance().IsServiceInitialized()) {
        LOGE("Device manager service has not been initialized or initialized failed.");
        return DM_NOT_INIT;
    }

    LOGI("Register device manager listener for pakage name: %s", pkgName.c_str());
    std::lock_guard<std::mutex> autoLock(listenerLock_);
    auto iter = dmListener_.find(pkgName);
    if (iter != dmListener_.end()) {
        LOGI("RegisterDeviceManagerListener: listener already exists");
        return DM_OK;
    }
    try {
        sptr<AppDeathRecipient> appRecipient = sptr<AppDeathRecipient>(new AppDeathRecipient());
        if (!listener->AddDeathRecipient(appRecipient)) {
            LOGE("RegisterDeviceManagerListener: AddDeathRecipient Failed");
        }
        dmListener_[pkgName] = listener;
        appRecipient_[pkgName] = appRecipient;
    } catch (const std::bad_alloc &e) {
        LOGE("new AppDeathRecipient failed");
        return DM_MALLOC_ERROR;
    }
    return DM_OK;
}

int32_t IpcServerStub::UnRegisterDeviceManagerListener(std::string &pkgName)
{
    if (pkgName.empty()) {
        LOGE("Error: parameter invalid");
        return DM_POINT_NULL;
    }
    LOGI("In, pkgName: %s", pkgName.c_str());
    std::lock_guard<std::mutex> autoLock(listenerLock_);
    auto listenerIter = dmListener_.find(pkgName);
    if (listenerIter == dmListener_.end()) {
        LOGI("UnRegisterDeviceManagerListener: listener not exists");
        return DM_OK;
    }
    auto recipientIter = appRecipient_.find(pkgName);
    if (recipientIter == appRecipient_.end()) {
        LOGI("UnRegisterDeviceManagerListener: appRecipient not exists");
        dmListener_.erase(pkgName);
        return DM_OK;
    }
    auto listener = listenerIter->second;
    auto appRecipient = recipientIter->second;
    listener->RemoveDeathRecipient(appRecipient);
    appRecipient_.erase(pkgName);
    dmListener_.erase(pkgName);
    return DM_OK;
}

const std::map<std::string, sptr<IRemoteObject>> &IpcServerStub::GetDmListener()
{
    return dmListener_;
}

const sptr<IpcRemoteBroker> IpcServerStub::GetDmListener(std::string pkgName) const
{
    auto iter = dmListener_.find(pkgName);
    if (iter == dmListener_.end()) {
        return nullptr;
    }
    auto remote = iter->second;
    sptr<IpcRemoteBroker> dmListener = iface_cast<IpcRemoteBroker>(remote);
    return dmListener;
}

void AppDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    LOGW("AppDeathRecipient: OnRemoteDied");
    std::map<std::string, sptr<IRemoteObject>> listeners = IpcServerStub::GetInstance().GetDmListener();
    std::string pkgName;
    for (auto iter : listeners) {
        if (iter.second == remote.promote()) {
            pkgName = iter.first;
            break;
        }
    }
    if (pkgName.empty()) {
        LOGE("AppDeathRecipient: OnRemoteDied, no pkgName matched");
        return;
    }
    LOGI("AppDeathRecipient: OnRemoteDied for %s", pkgName.c_str());
    IpcServerStub::GetInstance().UnRegisterDeviceManagerListener(pkgName);
}
} // namespace DistributedHardware
} // namespace OHOS
