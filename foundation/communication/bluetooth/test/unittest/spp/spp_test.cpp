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

#include <gtest/gtest.h>
#include "bluetooth_socket.h"
#include "bluetooth_gatt_characteristic.h"
#include "uuid.h"
#include "bluetooth_remote_device.h"

using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
class SocketTest : public testing::Test {
public:
    SocketTest()
    {}
    ~SocketTest()
    {}

    SppClientSocket *sppClientSocket_ = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    BluetoothRemoteDevice *pbluetoothRomote_ = nullptr;
    UUID randomUuid_;
    UUID insecureUuid_;

    SppServerSocket *server_;
    UUID uuid_;
};

void SocketTest::SetUpTestCase(void)
{}
void SocketTest::TearDownTestCase(void)
{}
void SocketTest::SetUp()
{
    pbluetoothRomote_ = new BluetoothRemoteDevice();
    randomUuid_ = UUID::RandomUUID();
    sppClientSocket_ = new SppClientSocket(*pbluetoothRomote_, randomUuid_, TYPE_RFCOMM, false);
    insecureUuid_ = UUID::FromString("00001101-0000-1000-8000-00805F9B34FB");

    server_ = nullptr;
    uuid_ = UUID::FromString("11111111-0000-1000-8000-00805F9B34FB");
    if (!server_)
        GTEST_LOG_(INFO) << "SocketFactory::DataListenRfcommByServiceRecord starts";
    server_ = SocketFactory::DataListenRfcommByServiceRecord("server", uuid_);
}

void SocketTest::TearDown()
{
    delete pbluetoothRomote_;
    pbluetoothRomote_ = nullptr;
    delete sppClientSocket_;
    sppClientSocket_ = nullptr;
}

/**
 * @tc.number:Spp_UnitTest001
 * @tc.name: Connect
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_Connect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SppClientSocket::Connect starts";
    sppClientSocket_->Connect();
    GTEST_LOG_(INFO) << "SppClientSocket::Connect ends";
}

/**
 * @tc.number:Spp_UnitTest002
 * @tc.name: Close
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_Close, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SppClientSocket::Close starts";
    sppClientSocket_->Close();
    GTEST_LOG_(INFO) << "SppClientSocket::Close ends";
}

/**
 * @tc.number:Spp_UnitTest003
 * @tc.name: GetInputStream
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_GetInputStream, TestSize.Level1)
{
    int fd = 37;
    BluetoothRemoteDevice device_;
    SppClientSocket *pfd_SppClientSocket = new SppClientSocket(fd, device_.GetDeviceAddr());

    char receive[512];
    int DATASIZE = 1024;
    ssize_t returnInput = 0;
    GTEST_LOG_(INFO) << "SppClientSocket::GetInputStream starts";
    InputStream input = pfd_SppClientSocket->GetInputStream();
    returnInput = input.Read(receive, DATASIZE);
    GTEST_LOG_(INFO) << "SppClientSocket::GetInputStream ends";
}

/**
 * @tc.number:Spp_UnitTest004
 * @tc.name: GetOutputStream
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_GetOutputStream, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SppClientSocket::GetOutputStream starts";
    char multiChar[10] = {'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n'};
    int fd = 37;
    BluetoothRemoteDevice device_;
    SppClientSocket *pfd_SppClientSocket = new SppClientSocket(fd, device_.GetDeviceAddr());
    OutputStream output = pfd_SppClientSocket->GetOutputStream();
    size_t returnOutput = 0;
    returnOutput = output.Write(multiChar, 10);
    GTEST_LOG_(INFO) << "SppClientSocket::GetOutputStream ends";
}

/**
 * @tc.number:Spp_UnitTest005
 * @tc.name: GetRemoteDevice
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_GetRemoteDevice, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SppClientSocket::GetRemoteDevice starts";
    int fd = 37;
    BluetoothRemoteDevice device_;
    SppClientSocket *pfd_SppClientSocket = new SppClientSocket(fd, device_.GetDeviceAddr());
    BluetoothRemoteDevice tempRemoteDevice = pfd_SppClientSocket->GetRemoteDevice();
    GTEST_LOG_(INFO) << "SppClientSocket::GetRemoteDevice ends";
}
    
/**
 * @tc.number:Spp_UnitTest006
 * @tc.name: IsConnected
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_IsConnected, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SppClientSocket::IsConnected starts";
    int fd = 37;
    BluetoothRemoteDevice device_;
    SppClientSocket *pfd_SppClientSocket = new SppClientSocket(fd, device_.GetDeviceAddr());
    bool IsConnected = false;
    IsConnected = pfd_SppClientSocket->IsConnected();
    EXPECT_EQ(IsConnected, true);
    GTEST_LOG_(INFO) << "SppClientSocket::IsConnected ends";
}

/**
 * @tc.number:Spp_UnitTest007
 * @tc.name: GetStringTag
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_GetStringTag, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SppServerSocket::GetStringTag starts";
    std::string returnStr{""};
    returnStr = server_->GetStringTag();
    GTEST_LOG_(INFO) << "SppServerSocket::GetStringTag ends";
}

/**
 * @tc.number:Spp_UnitTest008
 * @tc.name: Accept
 * @tc.desc: 
 */
HWTEST_F(SocketTest, Spp_UnitTest_Accept, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SppServerSocket::Accept starts";
    
    int SERTIMEOUT = 10;
    std::unique_ptr<SppClientSocket> preturn_SppClientSocket = server_->Accept(SERTIMEOUT);
    GTEST_LOG_(INFO) << "SppServerSocket::Accept ends";
}



}  // namespace Bluetooth
}  // namespace OHOS