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

#ifndef OHOS_DEVICE_MANAGER_IMPL_H
#define OHOS_DEVICE_MANAGER_IMPL_H

#include "device_manager.h"
#if !defined(__LITEOS_M__)
#include "ipc_client_manager.h"
#include "ipc_client_proxy.h"
#endif

namespace OHOS {
namespace DistributedHardware {
class DeviceManagerImpl : public DeviceManager {
public:
    static DeviceManagerImpl &GetInstance();

public:
    virtual int32_t InitDeviceManager(const std::string &pkgName,
                                      std::shared_ptr<DmInitCallback> dmInitCallback) override;
    virtual int32_t UnInitDeviceManager(const std::string &pkgName) override;
    virtual int32_t GetTrustedDeviceList(const std::string &pkgName, const std::string &extra,
                                         std::vector<DmDeviceInfo> &deviceList) override;
    virtual int32_t GetLocalDeviceInfo(const std::string &pkgName, DmDeviceInfo &deviceInfo) override;
    virtual int32_t RegisterDevStateCallback(const std::string &pkgName, const std::string &extra,
                                             std::shared_ptr<DeviceStateCallback> callback) override;
    virtual int32_t UnRegisterDevStateCallback(const std::string &pkgName) override;
    virtual int32_t StartDeviceDiscovery(const std::string &pkgName, const DmSubscribeInfo &subscribeInfo,
                                         const std::string &extra,
                                         std::shared_ptr<DiscoveryCallback> callback) override;
    virtual int32_t StopDeviceDiscovery(const std::string &pkgName, uint16_t subscribeId) override;
    virtual int32_t AuthenticateDevice(const std::string &pkgName, int32_t authType, const DmDeviceInfo &deviceInfo,
                                       const std::string &extra,
                                       std::shared_ptr<AuthenticateCallback> callback) override;
    virtual int32_t UnAuthenticateDevice(const std::string &pkgName, const std::string &deviceId) override;
    virtual int32_t VerifyAuthentication(const std::string &pkgName, const std::string &authPara,
                                         std::shared_ptr<VerifyAuthCallback> callback) override;
    virtual int32_t RegisterDeviceManagerFaCallback(const std::string &packageName,
                                                    std::shared_ptr<DeviceManagerFaCallback> callback) override;
    virtual int32_t UnRegisterDeviceManagerFaCallback(const std::string &pkgName) override;
    virtual int32_t GetFaParam(const std::string &pkgName, DmAuthParam &faParam) override;
    virtual int32_t SetUserOperation(const std::string &pkgName, int32_t action) override;
    virtual int32_t GetUdidByNetworkId(const std::string &pkgName, const std::string &netWorkId,
                                       std::string &udid) override;
    virtual int32_t GetUuidByNetworkId(const std::string &pkgName, const std::string &netWorkId,
                                       std::string &uuid) override;
    virtual int32_t RegisterDevStateCallback(const std::string &pkgName, const std::string &extra) override;
    virtual int32_t UnRegisterDevStateCallback(const std::string &pkgName, const std::string &extra) override;

private:
    DeviceManagerImpl() = default;
    ~DeviceManagerImpl() = default;
    DeviceManagerImpl(const DeviceManagerImpl &) = delete;
    DeviceManagerImpl &operator=(const DeviceManagerImpl &) = delete;
    DeviceManagerImpl(DeviceManagerImpl &&) = delete;
    DeviceManagerImpl &operator=(DeviceManagerImpl &&) = delete;

private:
#if !defined(__LITEOS_M__)
    std::shared_ptr<IpcClientProxy> ipcClientProxy_ =
        std::make_shared<IpcClientProxy>(std::make_shared<IpcClientManager>());
#endif
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DEVICE_MANAGER_IMPL_H
