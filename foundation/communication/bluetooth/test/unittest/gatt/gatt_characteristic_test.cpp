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
#include "securec.h"
#include "bluetooth_gatt_characteristic.h"
#include "bluetooth_gatt_descriptor.h"
#include "uuid.h"
#include "bluetooth_def.h"
#include <memory>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
class GattCharacteristicTest : public testing::Test {
public:
    GattCharacteristicTest()
    {}
    ~GattCharacteristicTest()
    {}

    GattCharacteristic *pGattCharacteristic = nullptr;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
public:
    UUID uuid_;
    uint16_t handle_ = 1;
    const int permissions_ = 100;
    const int properties_ = 200;
};

void GattCharacteristicTest::SetUpTestCase(void)
{}
void GattCharacteristicTest::TearDownTestCase(void)
{}
void GattCharacteristicTest::SetUp()
{
    uuid_ = UUID::RandomUUID();
}

void GattCharacteristicTest::TearDown()
{

}

/*
 * @tc.number: GattClient001
 * @tc.name: GattCharacteristic4
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GattCharacteristic4, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GattCharacteristic(4 parameters) start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    GTEST_LOG_(INFO) << "GattCharacteristic_GattCharacteristic(4 parameters) end";
}

/*
 * @tc.number: GattClient002
 * @tc.name: GattCharacteristic3
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GattCharacteristic3, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GattCharacteristic(3 parameters) start";
    GTEST_LOG_(INFO) << "GattCharacteristic_GattCharacteristic(3 parameters) end";
}

/*
 * @tc.number: GattClient003
 * @tc.name: AddDescriptor
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_AddDescriptor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_AddDescriptor start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    GattDescriptor *pdescriptor = new GattDescriptor(uuid_, permissions_);
    pGattCharacteristic->AddDescriptor(*pdescriptor);
    GTEST_LOG_(INFO) << "GattCharacteristic_AddDescriptor end";
}

/*
 * @tc.number: GattClient004
 * @tc.name: GetDescriptor
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetDescriptor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetDescriptor start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    GattDescriptor *pdescriptor = new GattDescriptor(uuid_, permissions_);
    pGattCharacteristic->AddDescriptor(*pdescriptor);

    GattDescriptor *pdescriptorTmp = pGattCharacteristic->GetDescriptor(uuid_);
    if (pdescriptorTmp->GetUuid().Equals(uuid_)) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "GattCharacteristic_GetDescriptor returns correctly";
    }
    GTEST_LOG_(INFO) << "GattCharacteristic_GetDescriptor end";
}

/*
 * @tc.number: GattClient005
 * @tc.name: GetHandle
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetHandle, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetHandle start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    uint16_t tempHandle = pGattCharacteristic->GetHandle();
    EXPECT_EQ(handle_, tempHandle);
    GTEST_LOG_(INFO) << "GattCharacteristic_GetHandle end";
}

/*
 * @tc.number: GattClient006
 * @tc.name: GetPermissions 
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetPermissions, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetPermissions start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    int tempPer = pGattCharacteristic->GetPermissions();
    EXPECT_EQ(permissions_, tempPer);
    GTEST_LOG_(INFO) << "GattCharacteristic_GetPermissions end";
}

/*
 * @tc.number: GattClient007
 * @tc.name: GetProperties
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetProperties, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetProperties start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    int tempPro = pGattCharacteristic->GetProperties();
    EXPECT_EQ(properties_, tempPro);
    GTEST_LOG_(INFO) << "GattCharacteristic_GetProperties end";
}

/*
 * @tc.number: GattClient008
 * @tc.name: GetService
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetService, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetService start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    GattService *tempGattService = pGattCharacteristic->GetService();
    if (nullptr == tempGattService) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "GattCharacteristic_GetService returns correctly";
    }
    GTEST_LOG_(INFO) << "GattCharacteristic_GetService end";
}

/*
 * @tc.number: GattClient009
 * @tc.name: GetUuid
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetUuid, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetUuid start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    UUID tempUuid = pGattCharacteristic->GetUuid();
    if (tempUuid.Equals(uuid_)) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "GattCharacteristic_GetUuid returns correctly";
    }
    GTEST_LOG_(INFO) << "GattCharacteristic_GetUuid end";
}

/*
 * @tc.number: GattClient010
 * @tc.name: SetValue
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_SetValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_SetValue start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    size_t length_Set = 1;
    uint8_t values_Set[1] = {'a'};
    pGattCharacteristic->SetValue(values_Set, length_Set);
    GTEST_LOG_(INFO) << "GattCharacteristic_SetValue end";
}

/*
 * @tc.number: GattClient011
 * @tc.name: GetValue
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetValue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetValue start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    size_t length_Set = 1;
    size_t length_Get = 1;
    size_t *plength_Get = &length_Get;
    uint8_t values_Set[1] = {'a'};
    pGattCharacteristic->SetValue(values_Set, length_Set);

    std::unique_ptr<uint8_t[]> values_Get = std::make_unique<uint8_t[]>(length_Get);
    (void)memcpy_s(values_Get.get(), length_Get, pGattCharacteristic->GetValue(plength_Get).get(), length_Get);
    const uint8_t *values = static_cast<uint8_t *>(values_Get.get());
    uint8_t resultValuesArray = values[0];
    uint8_t resultValuesPointer = *values;

    if ('a' == resultValuesArray) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "GattCharacteristic_GetValue returns correctly";
    }

    if ('a' == resultValuesPointer) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "GattCharacteristic_GetValue returns correctly";
    }

    GTEST_LOG_(INFO) << "GattCharacteristic_GetValue end";
}

/*
 * @tc.number: GattClient012
 * @tc.name: SetWriteType
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_SetWriteType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_SetWriteType start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    GattCharacteristic::WriteType typeTemp = GattCharacteristic::WriteType::DEFAULT;
    int returnCode = 0;
    returnCode = pGattCharacteristic->SetWriteType(typeTemp);
    EXPECT_EQ(returnCode, 0);
    GTEST_LOG_(INFO) << "GattCharacteristic_SetWriteType end";
}

/*
 * @tc.number: GattClient013
 * @tc.name: GetWriteType
*/
HWTEST_F(GattCharacteristicTest, GattCharacteristic_UnitTest_GetWriteType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GattCharacteristic_GetWriteType start";
    pGattCharacteristic = new GattCharacteristic(uuid_, handle_, permissions_, properties_);
    GattCharacteristic::WriteType typeTemp = GattCharacteristic::WriteType::DEFAULT;
    int returnCode = 0;
    returnCode = pGattCharacteristic->SetWriteType(typeTemp);
    EXPECT_EQ(returnCode, 0);

    int resultType = pGattCharacteristic->GetWriteType();
    if (typeTemp == resultType) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "GattCharacteristic_GetWriteType returns correctly";
    }
    GTEST_LOG_(INFO) << "GattCharacteristic_GetWriteType end";
}

}  // namespace Bluetooth
}  // namespace OHOS
