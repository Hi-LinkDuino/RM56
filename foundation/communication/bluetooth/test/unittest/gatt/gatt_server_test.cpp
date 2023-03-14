/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "bluetooth_gatt_server.h"
#include "bluetooth_host.h"
#include "bluetooth_remote_device.h"

using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
const int TIME = 2;
class BluetoothGattServerCallbackCommon : public GattServerCallback {
public:
    BluetoothGattServerCallbackCommon() = default;
    virtual ~BluetoothGattServerCallbackCommon() = default;
    void OnConnectionStateUpdate(const BluetoothRemoteDevice &device, int state) override
    {}
    void OnServiceAdded(GattService *Service, int ret) override
    {}
    void OnCharacteristicReadRequest(
        const BluetoothRemoteDevice &device, GattCharacteristic &characteristic, int requestId) override
    {}
    void OnCharacteristicWriteRequest(
        const BluetoothRemoteDevice &device, GattCharacteristic &characteristic, int requestId) override
    {}
    void OnDescriptorReadRequest(
        const BluetoothRemoteDevice &device, GattDescriptor &descriptor, int requestId) override
    {}
    void OnDescriptorWriteRequest(
        const BluetoothRemoteDevice &device, GattDescriptor &descriptor, int requestId) override
    {}
    void OnMtuUpdate(const BluetoothRemoteDevice &device, int mtu) override
    {}
    void OnNotificationCharacteristicChanged(const BluetoothRemoteDevice &device, int result) override
    {}
    void OnConnectionParameterChanged(
        const BluetoothRemoteDevice &device, int interval, int latency, int timeout, int status) override
    {}

private:
};
static BluetoothGattServerCallbackCommon callback_;

class GattServerTest : public testing::Test {
public:
    GattServerTest()
    {}
    ~GattServerTest()
    {}

    int tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    BluetoothHost *host_;
};

void GattServerTest::SetUpTestCase(void)
{}
void GattServerTest::TearDownTestCase(void)
{}
void GattServerTest::SetUp()
{
    tempData_ = 0;
    host_ = &BluetoothHost::GetDefaultHost();
    host_->EnableBt();
    host_->EnableBle();
    sleep(TIME);
}

void GattServerTest::TearDown()
{
    host_->DisableBt();
    host_->DisableBle();
    host_ = nullptr;
    sleep(TIME);
}

/*
 * @tc.number: GattServer001
 * @tc.name: AddService
 * @tc.desc: Check the addservice interface.
*/
HWTEST_F(GattServerTest, GattServer_ModuleTest_AddService, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_AddService start";
    GattServer server(callback_);
    UUID uuidSerPer;
    uuidSerPer = UUID::FromString("00001810-0000-1000-8000-00805F9B34F7");
    GattService serviceOne(uuidSerPer, GattServiceType::PRIMARY);
    int ret = server.AddService(serviceOne);

    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_AddService end";
}

/*
 * @tc.number: GattServer002
 * @tc.name: CancelConnection
 * @tc.desc: Check the CancelConnection interface.
*/
HWTEST_F(GattServerTest, GattServer_ModuleTest_CancelConnection, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_CancelConnection start";
    GattServer server(callback_);

    BluetoothRemoteDevice deviceBle_;

    server.CancelConnection(deviceBle_);
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_CancelConnection end";
}

/*
 * @tc.number: GattServer003
 * @tc.name: RemoveService
 * @tc.desc: Check the RemoveService interface.
*/
HWTEST_F(GattServerTest, GattServer_ModuleTest_RemoveService, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_RemoveService start";
    GattServer server(callback_);

    UUID uuidSerPer;
    uuidSerPer = UUID::FromString("00001810-0000-1000-8000-00805F9B34FB");
    GattService serviceOne(uuidSerPer, 1, 1, GattServiceType::PRIMARY);
    int result = server.AddService(serviceOne);
    EXPECT_EQ(result, 0);
    sleep(5);
    std::list<GattService> &list = server.GetServices();
    GTEST_LOG_(INFO) << list.size();
    int ret = server.RemoveGattService(list.back());
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_RemoveService end";
}

/*
 * @tc.number: GattServer005
 * @tc.name: ClearServices
 * @tc.desc: Check the ClearServices interface.
*/
HWTEST_F(GattServerTest, GattServer_ModuleTest_ClearServices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_ClearServices start";
    GattServer server(callback_);

    std::list<GattService>& list = server.GetServices();
    GTEST_LOG_(INFO) << (int)list.size();
    server.ClearServices();
    list = server.GetServices();
    EXPECT_EQ((int)list.size(), 0);
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_ClearServices end";
}

/*
 * @tc.number: GattServer006
 * @tc.name: NotifyCharacteristicChanged
 * @tc.desc: Check the NotifyCharacteristicChanged interface.
*/
HWTEST_F(GattServerTest, GattServer_ModuleTest_NotifyCharacteristicChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_Notify start";
    GattServer server(callback_);

    BluetoothRemoteDevice deviceBle_;
    UUID uuidSerPer;
    uuidSerPer = UUID::FromString("00001810-0000-1000-8000-00805F9B34FB");
    GattCharacteristic* aa = new GattCharacteristic(uuidSerPer, 1, 1);
    int res = server.NotifyCharacteristicChanged(deviceBle_, *aa, false);
    EXPECT_EQ(res, -18);

    GTEST_LOG_(INFO) << "GattServer_ModuleTest_Notify end";
}

/*
 * @tc.number: GattServer007
 * @tc.name: SendResponse
 * @tc.desc: Check the SendResponse interface.
*/
HWTEST_F(GattServerTest, GattServer_ModuleTest_SendResponse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_SendResponse start";
    GattServer server(callback_);

    BluetoothRemoteDevice deviceBle_;
    string valueChrTwo = "2";
    int ret = server.SendResponse(deviceBle_,
        0,
        (int)GattStatus::GATT_SUCCESS,
        1,
        (uint8_t *)valueChrTwo.c_str(),
        valueChrTwo.size());
    EXPECT_EQ(ret, -18);
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_SendResponse end";
}

/*
 * @tc.number: GattServer008
 * @tc.name: GetServices
 * @tc.desc: Check the GetServices interface.
*/
HWTEST_F(GattServerTest, GattServer_ModuleTest_GetServices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_GetServices start";
    GattServer server(callback_);

    UUID uuidSerPer;
    uuidSerPer = UUID::FromString("00001810-0000-1000-8000-00805F9B34F5");

    GattService serviceOne(uuidSerPer, GattServiceType::PRIMARY);
    int ret = server.AddService(serviceOne);

    EXPECT_EQ(ret, 0);
    sleep(TIME);
    std::list<GattService> list = server.GetServices();
    EXPECT_EQ((int)list.size(), 1);

    GTEST_LOG_(INFO) << "GattServer_ModuleTest_GetServices end";
}

/*
 * @tc.number: GattServer009
 * @tc.name: GetService
 * @tc.desc: Check the GetService interface.
*/

HWTEST_F(GattServerTest, GattServer_ModuleTest_GetService, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_GetService start";
    GattServer server(callback_);

    UUID uuidSerPer;
    uuidSerPer = UUID::FromString("00001810-0000-1000-8000-00805F9B34F9");

    GattService serviceOne(uuidSerPer, GattServiceType::SECONDARY);
    int ret = server.AddService(serviceOne);

    EXPECT_EQ(ret, 0);
    sleep(5);

    std::optional<GattService> listSecondary = server.GetService(uuidSerPer, false);
    EXPECT_FALSE(listSecondary->IsPrimary());
    GTEST_LOG_(INFO) << "GattServer_ModuleTest_GetService end";
}
}  // namespace Bluetooth
}  // namespace OHOS
