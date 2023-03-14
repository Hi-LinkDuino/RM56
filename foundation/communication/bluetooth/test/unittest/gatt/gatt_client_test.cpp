/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bluetooth_gatt_client.h"
#include "bluetooth_gatt_descriptor.h"
#include "bluetooth_host.h"
#include "uuid.h"
#include "vector"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Bluetooth {
class GattClientCallbackTest : public GattClientCallback {
public:
    GattClientCallbackTest()
    {}
    ~GattClientCallbackTest()
    {}

    void OnConnectionStateChanged(int connectionState, int ret)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnConnectionStateChanged called";
    }

    void OnCharacteristicChanged(const GattCharacteristic &characteristic)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnCharacteristicChanged called";
    }

    void OnCharacteristicReadResult(const GattCharacteristic &characteristic, int ret)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnCharacteristicReadResult called";
    }

    void OnCharacteristicWriteResult(const GattCharacteristic &characteristic, int ret)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnCharacteristicWriteResult called";
    }

    void OnDescriptorReadResult(const GattDescriptor &descriptor, int ret)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnDescriptorReadResult called";
    }

    void OnDescriptorWriteResult(const GattDescriptor &descriptor, int ret)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnDescriptorWriteResult called";
    }

    void OnMtuUpdate(int mtu, int ret)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnMtuUpdate called";
    }

    void OnServicesDiscovered(int status)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnServicesDiscovered called";
    }

    void OnConnectionParameterChanged(int interval, int latency, int timeout, int status)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnConnectionParameterChanged called";
    }

    void OnSetNotifyCharacteristic(int status)
    {
        GTEST_LOG_(INFO) << "GattClientCallbackTest::OnSetNotifyCharacteristic called";
    }
};

class GattClientTest : public testing::Test {
public:
    GattClientTest()
    {}
    ~GattClientTest()
    {}

    int tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void GattClientTest::SetUpTestCase(void)
{}
void GattClientTest::TearDownTestCase(void)
{}
void GattClientTest::SetUp()
{
    tempData_ = 0;
}

void GattClientTest::TearDown()
{}

/*
 * @tc.number: GattClient001
 * @tc.name: GattClient
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_GattClient, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_GattClient start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_GattClient end";
}

/*
 * @tc.number: GattClient002
 * @tc.name: Connect
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_Connect start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    int result = client.Connect(callback_, isAutoConnect, transport);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_Connect end";
}

/*
 * @tc.number: GattClient003
 * @tc.name: Disconnect
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_Disconnect start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    int result = client.Disconnect();
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_Disconnect end";
}

/*
 * @tc.number: GattClient004
 * @tc.name: DiscoverServices
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_DiscoverServices, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_DiscoverServices start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    int result = client.DiscoverServices();
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_DiscoverServices end";
}

/*
 * @tc.number: GattClient005
 * @tc.name: GetService_1
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_GetService_1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_GetService_1 start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    bool ret = false;
    client.Connect(callback_, isAutoConnect, transport);
    UUID id = UUID::RandomUUID();
    client.GetService(id);
    if (client.GetService(id) != std::nullopt) {
        ret = true;
    }
    EXPECT_EQ(ret, false);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_GetService_1 end";
}

/*
 * @tc.number: GattClient006
 * @tc.name: GetService_2
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_GetService_2, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_GetService_2 start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    EXPECT_EQ((int)client.GetService().size(), 0);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_GetService_2 end";
}

/*
 * @tc.number: GattClient007
 * @tc.name: ReadCharacteristic
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_ReadCharacteristic, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_ReadCharacteristic start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    UUID uuid_ = UUID::RandomUUID();
    int permissions = 17;
    int properties = 37;
    GattCharacteristic characteristic = GattCharacteristic(uuid_, permissions, properties);
    int result = client.ReadCharacteristic(characteristic);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_ReadCharacteristic end";
}

/*
 * @tc.number: GattClient008
 * @tc.name: ReadDescriptor
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_ReadDescriptor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_ReadDescriptor start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    UUID uuid_ = UUID::RandomUUID();
    int permissions = 17;
    GattDescriptor descriptor = GattDescriptor(uuid_, permissions);
    int result = client.ReadDescriptor(descriptor);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_ReadDescriptor end";
}

/*
 * @tc.number: GattClient009
 * @tc.name: RequestBleMtuSize
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_RequestBleMtuSize, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_RequestBleMtuSize start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    int result = client.Connect(callback_, isAutoConnect, transport);
    int mtu = 17;
    result = client.RequestBleMtuSize(mtu);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_RequestBleMtuSize end";
}

/*
 * @tc.number: GattClient010
 * @tc.name: SetNotifyCharacteristic
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_SetNotifyCharacteristic, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_SetNotifyCharacteristic start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    UUID uuid_ = UUID::RandomUUID();
    int permissions = 17;
    int properties = 37;
    GattCharacteristic characteristic = GattCharacteristic(uuid_, permissions, properties);
    bool enable = true;
    int result = client.SetNotifyCharacteristic(characteristic, enable);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_SetNotifyCharacteristic end";
}

/*
 * @tc.number: GattClient011
 * @tc.name: WriteCharacteristic
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_WriteCharacteristic, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_WriteCharacteristic start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    UUID uuid_ = UUID::RandomUUID();
    int permissions = 17;
    int properties = 37;
    GattCharacteristic characteristic = GattCharacteristic(uuid_, permissions, properties);
    int result = client.WriteCharacteristic(characteristic);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_WriteCharacteristic end";
}

/*
 * @tc.number: GattClient012
 * @tc.name: WriteDescriptor
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_WriteDescriptor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_WriteDescriptor start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    UUID uuid_ = UUID::RandomUUID();
    int permissions = 17;
    GattDescriptor descriptor = GattDescriptor(uuid_, permissions);
    int result = client.WriteDescriptor(descriptor);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_WriteDescriptor end";
}

/*
 * @tc.number: GattClient013
 * @tc.name: RequestConnectionPriority
*/
HWTEST_F(GattClientTest, GattClient_UnitTest_RequestConnectionPriority, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattClient_UnitTest_RequestConnectionPriority start";
    BluetoothRemoteDevice device;
    GattClient client(device);
    GattClientCallbackTest callback_;
    bool isAutoConnect = true;
    int transport = 1;
    client.Connect(callback_, isAutoConnect, transport);
    int connPriority = 2;
    int result = client.RequestConnectionPriority(connPriority);
    EXPECT_EQ(result, -18);
    GTEST_LOG_(INFO) << "GattClient_UnitTest_RequestConnectionPriority end";
}

}  // namespace Bluetooth
}  // namespace OHOS
