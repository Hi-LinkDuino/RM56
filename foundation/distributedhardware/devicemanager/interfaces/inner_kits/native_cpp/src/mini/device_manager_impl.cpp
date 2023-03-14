/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "command_dispatch.h"
#include "dm_log.h"
#include "device_manager_notify.h"
#include "dm_constants.h"
#include "ipc_def.h"
#include "ipc_get_trustdevice_req.h"
#include "ipc_get_trustdevice_rsp.h"
#include "ipc_start_discovery_req.h"
#include "ipc_stop_discovery_req.h"
#include "ipc_set_useroperation_req.h"
#include "ipc_authenticate_device_req.h"
#include "ipc_verify_authenticate_req.h"
#include "ipc_get_local_device_info_rsp.h"
#include "ipc_get_info_by_network_rsp.h"
#include "ipc_get_info_by_network_req.h"
#include "ipc_unauthenticate_device_req.h"
#include "ipc_get_dmfaparam_rsp.h"
#include "device_manager_impl.h"

namespace OHOS {
namespace DistributedHardware {
DeviceManagerImpl &DeviceManagerImpl::GetInstance()
{
    static DeviceManagerImpl instance;
    return instance;
}

int32_t DeviceManagerImpl::InitDeviceManager(const std::string &pkgName, std::shared_ptr<DmInitCallback> dmInitCallback)
{
    LOGI("DeviceManager::InitDeviceManager start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty() || dmInitCallback == nullptr) {
        LOGE("InitDeviceManager error: Invalid parameter");
        return DM_INVALID_VALUE;
    }

    CommandDispatch::GetInstance().AddPkgName(pkgName);
    DeviceManagerNotify::GetInstance().RegisterDeathRecipientCallback(pkgName, dmInitCallback);
    LOGI("InitDeviceManager success");
    return DM_OK;
}

int32_t DeviceManagerImpl::UnInitDeviceManager(const std::string &pkgName)
{
    LOGI("DeviceManager::UnInitDeviceManager start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("InitDeviceManager error: Invalid parameter");
        return DM_INVALID_VALUE;
    }

    CommandDispatch::GetInstance().DeletePkgName(pkgName);
    DeviceManagerNotify::GetInstance().UnRegisterPackageCallback(pkgName);
    LOGI("UnInitDeviceManager success");
    return DM_OK;
}

int32_t DeviceManagerImpl::GetTrustedDeviceList(const std::string &pkgName, const std::string &extra,
                                                std::vector<DmDeviceInfo> &deviceList)
{
    LOGI("DeviceManager::GetTrustedDeviceList start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("GetTrustedDeviceList error: Invalid para");
        return DM_INVALID_VALUE;
    }

    std::shared_ptr<IpcGetTrustDeviceReq> req = std::make_shared<IpcGetTrustDeviceReq>();
    std::shared_ptr<IpcGetTrustDeviceRsp> rsp = std::make_shared<IpcGetTrustDeviceRsp>();
    req->SetPkgName(pkgName);
    req->SetExtra(extra);

    if (CommandDispatch::GetInstance().MessageSendCmd(GET_TRUST_DEVICE_LIST,  req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("GetTrustedDeviceList error: failed ret: %d", ret);
        return ret;
    }

    deviceList = rsp->GetDeviceVec();
    LOGI("GetTrustedDeviceList completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::GetLocalDeviceInfo(const std::string &pkgName, DmDeviceInfo &info)
{
    LOGI("DeviceManager::GetLocalDeviceInfo start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("GetLocalDeviceInfo error: Invalid para");
        return DM_INVALID_VALUE;
    }

    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcGetLocalDeviceInfoRsp> rsp = std::make_shared<IpcGetLocalDeviceInfoRsp>();
    req->SetPkgName(pkgName);

    if (CommandDispatch::GetInstance().MessageSendCmd(GET_LOCAL_DEVICE_INFO, req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    if (rsp->GetErrCode() != DM_OK) {
        LOGE("GetLocalDeviceInfo error: failed ret: %d", rsp->GetErrCode());
        return DM_IPC_RESPOND_ERROR;
    }

    info = rsp->GetLocalDeviceInfo();
    LOGI("GetLocalDeviceInfo completed,pkgname%s", req->GetPkgName().c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::RegisterDevStateCallback(const std::string &pkgName, const std::string &extra,
    std::shared_ptr<DeviceStateCallback> callback)
{
    LOGI("DeviceManager::RegisterDevStateCallback start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty() || callback == nullptr) {
        LOGE("RegisterDevStateCallback error: Invalid para");
        return DM_INVALID_VALUE;
    }

    DeviceManagerNotify::GetInstance().RegisterDeviceStateCallback(pkgName, callback);
    LOGI("RegisterDevStateCallback completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::UnRegisterDevStateCallback(const std::string &pkgName)
{
    LOGI("DeviceManager::UnRegisterDevStateCallback start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("UnRegisterDevStateCallback error: Invalid para");
        return DM_INVALID_VALUE;
    }

    DeviceManagerNotify::GetInstance().UnRegisterDeviceStateCallback(pkgName);
    LOGI("UnRegisterDevStateCallback completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}
int32_t DeviceManagerImpl::StartDeviceDiscovery(const std::string &pkgName, const DmSubscribeInfo &subscribeInfo,
                                                const std::string &extra, std::shared_ptr<DiscoveryCallback> callback)
{
    LOGI("DeviceManager::StartDeviceDiscovery start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty() || callback == nullptr) {
        LOGE("StartDeviceDiscovery error: Invalid para");
        return DM_INVALID_VALUE;
    }

    LOGI("DeviceManager StartDeviceDiscovery in, pkgName %s", pkgName.c_str());
    DeviceManagerNotify::GetInstance().RegisterDiscoveryCallback(pkgName, subscribeInfo.subscribeId, callback);

    std::shared_ptr<IpcStartDiscoveryReq> req = std::make_shared<IpcStartDiscoveryReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    req->SetExtra(extra);
    req->SetSubscribeInfo(subscribeInfo);

    if (CommandDispatch::GetInstance().MessageSendCmd(START_DEVICE_DISCOVER,  req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }
    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("StartDeviceDiscovery error: Failed with ret %d", ret);
        return ret;
    }
    return DM_OK;
}

int32_t DeviceManagerImpl::StopDeviceDiscovery(const std::string &pkgName, uint16_t subscribeId)
{
    LOGI("DeviceManager::StopDeviceDiscovery start , pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("StopDeviceDiscovery error: Invalid para");
        return DM_INVALID_VALUE;
    }

    LOGI("StopDeviceDiscovery in, pkgName %s", pkgName.c_str());
    std::shared_ptr<IpcStopDiscoveryReq> req = std::make_shared<IpcStopDiscoveryReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    req->SetSubscribeId(subscribeId);

    if (CommandDispatch::GetInstance().MessageSendCmd(STOP_DEVICE_DISCOVER,  req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }
    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("StopDeviceDiscovery error: Failed with ret %d", ret);
        return ret;
    }
        
    DeviceManagerNotify::GetInstance().UnRegisterDiscoveryCallback(pkgName, subscribeId);
    LOGI("StopDeviceDiscovery completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::AuthenticateDevice(const std::string &pkgName, int32_t authType,
                                              const DmDeviceInfo &deviceInfo, const std::string &extra,
                                              std::shared_ptr<AuthenticateCallback> callback)
{
    LOGI("DeviceManager::AuthenticateDevice start , pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("AuthenticateDevice error: Invalid para");
        return DM_INVALID_VALUE;
    }

    std::string strDeviceId = deviceInfo.deviceId;
    DeviceManagerNotify::GetInstance().RegisterAuthenticateCallback(pkgName, strDeviceId, callback);
    std::shared_ptr<IpcAuthenticateDeviceReq> req = std::make_shared<IpcAuthenticateDeviceReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    req->SetExtra(extra);
    req->SetAuthType(authType);
    req->SetDeviceInfo(deviceInfo);

    if (CommandDispatch::GetInstance().MessageSendCmd(AUTHENTICATE_DEVICE,  req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("AuthenticateDevice error: Failed with ret %d", ret);
        return ret;
    }

    LOGI("DeviceManager::AuthenticateDevice completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::UnAuthenticateDevice(const std::string &pkgName, const std::string &deviceId)
{
    LOGI("DeviceManager::UnAuthenticateDevice start , pkgName: %s, deviceId: %s", pkgName.c_str(), deviceId.c_str());
    if (deviceId.empty()) {
        LOGE("UnAuthenticateDevice error: Invalid para");
        return DM_INVALID_VALUE;
    }

    DmDeviceInfo deviceInfo;
    strcpy_s(deviceInfo.deviceId, DM_MAX_DEVICE_ID_LEN, deviceId.c_str());
    std::shared_ptr<IpcUnAuthenticateDeviceReq> req = std::make_shared<IpcUnAuthenticateDeviceReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    req->SetDeviceInfo(deviceInfo);

    if (CommandDispatch::GetInstance().MessageSendCmd(UNAUTHENTICATE_DEVICE,  req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("UnAuthenticateDevice error: Failed with ret %d", ret);
        return ret;
    }

    LOGI("UnAuthenticateDevice completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::RegisterDeviceManagerFaCallback(const std::string &packageName,
    std::shared_ptr<DeviceManagerFaCallback> callback)
{
    LOGI("DeviceManager::RegisterDeviceManagerFaCallback start, pkgName: %s", packageName.c_str());
    if (packageName.empty() || callback == nullptr) {
        LOGE("RegisterDeviceManagerFaCallback error: Invalid para");
        return DM_INVALID_VALUE;
    }

    DeviceManagerNotify::GetInstance().RegisterDeviceManagerFaCallback(packageName, callback);
    LOGI("DeviceManager::RegisterDevStateCallback completed, pkgName: %s", packageName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::UnRegisterDeviceManagerFaCallback(const std::string &pkgName)
{
    LOGI("DeviceManager::UnRegisterDeviceManagerFaCallback start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("UnRegisterDeviceManagerFaCallback error: Invalid para");
        return DM_INVALID_VALUE;
    }

    DeviceManagerNotify::GetInstance().UnRegisterDeviceManagerFaCallback(pkgName);
    LOGI("DeviceManager::UnRegisterDevStateCallback completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}
int32_t DeviceManagerImpl::VerifyAuthentication(const std::string &pkgName, const std::string &authPara,
                                                std::shared_ptr<VerifyAuthCallback> callback)
{
    LOGI("DeviceManager::VerifyAuthentication start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("VerifyAuthentication error: Invalid para");
        return DM_INVALID_VALUE;
    }

    DeviceManagerNotify::GetInstance().RegisterVerifyAuthenticationCallback(pkgName, authPara, callback);

    std::shared_ptr<IpcVerifyAuthenticateReq> req = std::make_shared<IpcVerifyAuthenticateReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    req->SetAuthPara(authPara);

    if (CommandDispatch::GetInstance().MessageSendCmd(VERIFY_AUTHENTICATION,  req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("VerifyAuthentication error: Failed with ret %d", ret);
        return ret;
    }

    LOGI("VerifyAuthentication completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::GetFaParam(const std::string &pkgName, DmAuthParam &dmFaParam)
{
    LOGI("DeviceManager::GetFaParam start, pkgName: %s", pkgName.c_str());
    if (pkgName.empty()) {
        LOGE("GetFaParam failed, pkgName is empty");
        return DM_INVALID_VALUE;
    }

    std::shared_ptr<IpcReq> req = std::make_shared<IpcReq>();
    std::shared_ptr<IpcGetDmFaParamRsp> rsp = std::make_shared<IpcGetDmFaParamRsp>();
    req->SetPkgName(pkgName);

    if (CommandDispatch::GetInstance().MessageSendCmd(SERVER_GET_DMFA_INFO, req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    dmFaParam = rsp->GetDmAuthParam();
    LOGI("GetFaParam completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::SetUserOperation(const std::string &pkgName, int32_t action)
{
    LOGI("DeviceManager::SetUserOperation start, pkgName: %s", pkgName.c_str());

    if (pkgName.empty()) {
        LOGE("VerifyAuthentication failed, pkgName is empty");
        return DM_INVALID_VALUE;
    }

    std::shared_ptr<IpcGetOperationReq> req = std::make_shared<IpcGetOperationReq>();
    std::shared_ptr<IpcRsp> rsp = std::make_shared<IpcRsp>();
    req->SetPkgName(pkgName);
    req->SetOperation(action);

    if (CommandDispatch::GetInstance().MessageSendCmd(SERVER_USER_AUTH_OPERATION,  req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }
    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("SetUserOperation Failed with ret %d", ret);
        return ret;
    }

    LOGI("SetUserOperation completed, pkgName: %s", pkgName.c_str());
    return DM_OK;
}

int32_t DeviceManagerImpl::GetUdidByNetworkId(const std::string &pkgName, const std::string &netWorkId,
                                              std::string &udid)
{
    if (pkgName.empty()) {
        LOGE("GetUdidByNetworkId failed, pkgName is empty");
        return DM_INVALID_VALUE;
    }

    std::shared_ptr<IpcGetInfoByNetWorkReq> req = std::make_shared<IpcGetInfoByNetWorkReq>();
    std::shared_ptr<IpcGetInfoByNetWorkRsp> rsp = std::make_shared<IpcGetInfoByNetWorkRsp>();
    req->SetPkgName(pkgName);
    req->SetNetWorkId(netWorkId);

    if (CommandDispatch::GetInstance().MessageSendCmd(GET_UDID_BY_NETWORK, req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("DeviceManager::GetUdidByNetworkId Failed with ret %d", ret);
        return ret;
    }

    udid = rsp->GetUdid();
    return DM_OK;
}

int32_t DeviceManagerImpl::GetUuidByNetworkId(const std::string &pkgName, const std::string &netWorkId,
                                              std::string &uuid)
{
    if (pkgName.empty()) {
        LOGE("GetUuidByNetworkId failed, pkgName is empty");
        return DM_INVALID_VALUE;
    }

    std::shared_ptr<IpcGetInfoByNetWorkReq> req = std::make_shared<IpcGetInfoByNetWorkReq>();
    std::shared_ptr<IpcGetInfoByNetWorkRsp> rsp = std::make_shared<IpcGetInfoByNetWorkRsp>();
    req->SetPkgName(pkgName);
    req->SetNetWorkId(netWorkId);

    if (CommandDispatch::GetInstance().MessageSendCmd(GET_UUID_BY_NETWORK, req, rsp) != DM_OK) {
        return DEVICEMANAGER_MESSAGE_FAILED;
    }

    int32_t ret = rsp->GetErrCode();
    if (ret != DM_OK) {
        LOGE("GetUuidByNetworkId Failed with ret %d", ret);
        return ret;
    }
    uuid = rsp->GetUuid();
    return DM_OK;
}

int32_t DeviceManagerImpl::RegisterDevStateCallback(const std::string &pkgName, const std::string &extra)
{
}

int32_t DeviceManagerImpl::UnRegisterDevStateCallback(const std::string &pkgName, const std::string &extra)
{
}
} // namespace DistributedHardware
} // namespace OHOS
