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
#include "bluetooth_hf_call.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Bluetooth {
class HandsFreeUnitCallTest : public testing::Test {
public:
    HandsFreeUnitCallTest()
    {}
    ~HandsFreeUnitCallTest()
    {}

    int tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void HandsFreeUnitCallTest::SetUpTestCase(void)
{}
void HandsFreeUnitCallTest::TearDownTestCase(void)
{}
void HandsFreeUnitCallTest::SetUp()
{
}

void HandsFreeUnitCallTest::TearDown()
{
}
/*
 * @tc.number: HandsFreeUnitCall001
 * @tc.name: HandsFreeUnitCall_1
 * @tc.desc: Construct a new HandsFreeUnitCall object.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_HandsFreeUnitCall_1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_HandsFreeUnitCall_1 start";
 
    HandsFreeUnitCall call("000", 0, 0, "000", false, false, false);
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_HandsFreeUnitCall_1 end";
}

/*
 * @tc.number: HandsFreeUnitCall002
 * @tc.name: HandsFreeUnitCall_2
 * @tc.desc: Construct a new HandsFreeUnitCall object.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_HandsFreeUnitCall_2, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_HandsFreeUnitCall_2 start";
 
    HandsFreeUnitCall call("000", 0, 0, "000", UUID::FromString("000"), false, false, false, 0);
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_HandsFreeUnitCall_2 end";
}

/*
 * @tc.number: HandsFreeUnitCall003
 * @tc.name: GetRemoteDevice
 * @tc.desc: Get the remote device address.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_GetRemoteDevice, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetRemoteDevice start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ("", call.GetRemoteDevice());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetRemoteDevice end";
}

/*
 * @tc.number: HandsFreeUnitCall004
 * @tc.name: GetId
 * @tc.desc: Get the call index.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_GetId, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetId start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ(0, call.GetId());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetId end";
}


/*
 * @tc.number: HandsFreeUnitCall005
 * @tc.name: GetUuid
 * @tc.desc: Get the uuid object.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_GetUuid, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetUuid start";
 
    HandsFreeUnitCall call;
    UUID uuid = call.GetUuid();
    EXPECT_EQ("", uuid.ToString());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetUuid end";
}


/*
 * @tc.number: HandsFreeUnitCall006
 * @tc.name: GetState
 * @tc.desc: Get the call state.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_GetState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetState start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ(-1, call.GetState());

    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetState end";
}


/*
 * @tc.number: HandsFreeUnitCall007
 * @tc.name: GetNumber
 * @tc.desc: Get the call number.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_GetNumber, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetNumber start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ("", call.GetNumber());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetNumber end";
}


/*
 * @tc.number: HandsFreeUnitCall008
 * @tc.name: GetCreationTime
 * @tc.desc: Get the call's creation time.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_GetCreationTime, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetCreationTime start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ(0, call.GetCreationTime());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_GetCreationTime end";
}


/*
 * @tc.number: HandsFreeUnitCall009
 * @tc.name: IsMultiParty
 * @tc.desc: Get the IsMultiParty flag.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_IsMultiParty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_IsMultiParty start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ(false, call.IsMultiParty());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_IsMultiParty end";
}


/*
 * @tc.number: HandsFreeUnitCall010
 * @tc.name: IsOutgoing
 * @tc.desc: Get the IsOutgoing flag.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_IsOutgoing, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_IsOutgoing start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ(false, call.IsOutgoing());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_IsOutgoing end";
}


/*
 * @tc.number: HandsFreeUnitCall011
 * @tc.name: IsInBandRing
 * @tc.desc: Get the IsInBandRing flag.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_IsInBandRing, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_IsInBandRing start";
 
    HandsFreeUnitCall call;
    EXPECT_EQ(false, call.IsInBandRing());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_IsInBandRing end";
}

/*
 * @tc.number: HandsFreeUnitCall012
 * @tc.name: SetState
 * @tc.desc: Set the call state.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_SetState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_SetState start";
 
    HandsFreeUnitCall call;
    call.SetState(1);
    EXPECT_EQ(1, call.GetState());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_SetState end";
}


/*
 * @tc.number: HandsFreeUnitCall013
 * @tc.name: SetNumber
 * @tc.desc: Set the call number.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_SetNumber, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_SetNumber start";
 
    HandsFreeUnitCall call;
    call.SetNumber("1");
    EXPECT_EQ("1", call.GetNumber());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_SetNumber end";
}

/*
 * @tc.number: HandsFreeUnitCall014
 * @tc.name: SetMultiParty
 * @tc.desc: Set the multiParty flag.
*/
HWTEST_F(HandsFreeUnitCallTest, HandsFreeUnitCall_UnitTest_SetMultiParty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_SetMultiParty start";
 
    HandsFreeUnitCall call;
    call.SetMultiParty(true);
    EXPECT_EQ(true, call.IsMultiParty());
    
    GTEST_LOG_(INFO) << "HandsFreeUnitCall_UnitTest_SetMultiParty end";
}



}  // namespace Bluetooth
}  // namespace OHOS