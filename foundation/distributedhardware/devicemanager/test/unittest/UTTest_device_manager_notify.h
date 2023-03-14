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

#ifndef OHOS_DEVICE_MANAGER_NOTIFY_TEST_H
#define OHOS_DEVICE_MANAGER_NOTIFY_TEST_H

#include <gtest/gtest.h>
#include <mutex>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <list>

#include "dm_device_info.h"
#include "dm_subscribe_info.h"
#include "device_manager_callback.h"
#include "single_instance.h"
namespace OHOS {
namespace DistributedHardware {
class DeviceManagerNotifyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

class DmInitCallbackTest : public DmInitCallback {
public:
    explicit DmInitCallbackTest(int &count);
    virtual ~DmInitCallbackTest() override {}
    virtual void OnRemoteDied() override;
private:
    int *count_ = nullptr;
};

class DeviceStateCallbackTest : public DeviceStateCallback {
public:
    explicit DeviceStateCallbackTest(int &count);
    virtual ~DeviceStateCallbackTest() override {}
    virtual void OnDeviceOnline(const DmDeviceInfo &deviceInfo) override;
    virtual void OnDeviceReady(const DmDeviceInfo &deviceInfo) override;
    virtual void OnDeviceOffline(const DmDeviceInfo &deviceInfo) override;
    virtual void OnDeviceChanged(const DmDeviceInfo &deviceInfo) override;
private:
    int *count_ = nullptr;
};

class DiscoveryCallbackTest : public DiscoveryCallback {
public:
    explicit DiscoveryCallbackTest(int &count);
    virtual ~DiscoveryCallbackTest() override {}
    virtual void OnDiscoverySuccess(uint16_t subscribeId) override;
    virtual void OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason) override;
    virtual void OnDeviceFound(uint16_t subscribeId, const DmDeviceInfo &deviceInfo) override;
private:
    int *count_ = nullptr;
};

class AuthenticateCallbackTest : public AuthenticateCallback {
public:
    explicit AuthenticateCallbackTest(int &count);
    virtual ~AuthenticateCallbackTest() override {}
    virtual void OnAuthResult(const std::string &deviceId, const std::string &token, int32_t status,
                              int32_t reason);
private:
    int *count_ = nullptr;
};

class DeviceManagerFaCallbackTest : public DeviceManagerFaCallback {
public:
    explicit DeviceManagerFaCallbackTest(int &count);
    virtual ~DeviceManagerFaCallbackTest() override {}
    virtual void OnCall(const std::string &paramJson) override;
private:
    int *count_ = nullptr;
};
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DEVICE_MANAGER_NOTIFY_TEST_H
