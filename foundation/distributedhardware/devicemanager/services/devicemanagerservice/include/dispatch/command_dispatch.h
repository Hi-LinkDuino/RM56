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

#ifndef OHOS_DEVICE_MANAGER_COMMAND_DISPATCH_H
#define OHOS_DEVICE_MANAGER_COMMAND_DISPATCH_H

#include <cstdint>
#include <string>
#include <memory>
#include <list>
#include "single_instance.h"
#include "ipc_req.h"
#include "ipc_rsp.h"

namespace OHOS {
namespace DistributedHardware {
#define DEVICEMANAGER_MESSAGE_FAILED (-1)
static int32_t GetTrustedDeviceList(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t GetLocalDeviceInfo(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t GetUdidByNetworkId(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t GetUuidByNetworkId(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t StartDeviceDiscovery(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t StopDeviceDiscovery(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t SetUserOperation(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t GetFaParam(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t AuthenticateDevice(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t UnAuthenticateDevice(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
static int32_t VerifyAuthentication(const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);

typedef struct {
    int32_t cmdCode;
    int32_t (*MsgProcess) (const std::shared_ptr<IpcReq> &req, const std::shared_ptr<IpcRsp> &rsp);
} CmdMap;

static const CmdMap g_cmdMap[] = {
    {GET_TRUST_DEVICE_LIST, GetTrustedDeviceList},
    {GET_LOCAL_DEVICE_INFO, GetLocalDeviceInfo},
    {GET_UDID_BY_NETWORK, GetUdidByNetworkId},
    {GET_UUID_BY_NETWORK, GetUuidByNetworkId},
    {START_DEVICE_DISCOVER, StartDeviceDiscovery},
    {STOP_DEVICE_DISCOVER, StopDeviceDiscovery},
    {SERVER_USER_AUTH_OPERATION, SetUserOperation},
    {SERVER_GET_DMFA_INFO, GetFaParam},
    {AUTHENTICATE_DEVICE, AuthenticateDevice},
    {UNAUTHENTICATE_DEVICE, UnAuthenticateDevice},
    {VERIFY_AUTHENTICATION, VerifyAuthentication},
};

class CommandDispatch {
DECLARE_SINGLE_INSTANCE(CommandDispatch);
public:
    int32_t MessageSendCmd(int32_t cmdCode, const std::shared_ptr<IpcReq> &req,
                           const std::shared_ptr<IpcRsp> &rsp);
    void AddPkgName(const std::string &pkgName);
    void DeletePkgName(const std::string &pkgName);
    const std::list<std::string>& GetPkgNameList() const;
private:
    std::list<std::string> dmPkgName_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DEVICE_MANAGER_COMMAND_DISPATCH_H
