/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <vector>

#include "data_ability_predicates.h"
#include "refbase.h"

using namespace testing::ext;
using namespace OHOS::NativeRdb;
const int MAX_PARCEL_SIZE = 1000;
char g_data[MAX_PARCEL_SIZE];
int g_invalidObjectFlag = 0;
const std::string MAX = "2 147 483 647";
class DataAbilityPredicatesTest : public testing::Test {
public:
    static void TearDownTestCase(void);
};

void DataAbilityPredicatesTest::TearDownTestCase(void)
{
    for (int i = 0; i < MAX_PARCEL_SIZE; i++) {
        g_data[i] = 0;
    }
}

/* *
 * @tc.name: DataAbilityPredicates_001
 * @tc.desc: test DataAbilityPredicates()
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_001, TestSize.Level1)
{
    OHOS::sptr<DataAbilityPredicates> predicates = new DataAbilityPredicates();
    EXPECT_EQ(false, predicates->IsRawSelection());
}

/* *
 * @tc.name: DataAbilityPredicates_002
 * @tc.desc: test DataAbilityPredicates()
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_002, TestSize.Level1)
{
    DataAbilityPredicates *predicates1 = new DataAbilityPredicates();

    predicates1->EqualTo("stringValue", "ABCDEFGHIJKLMN")
            ->BeginWrap()
            ->EqualTo("integerValue", "1")
            ->Or()
            ->EqualTo("integerValue", MAX)
            ->EndWrap()->OrderByDesc("integerValue")->Limit(2);

    EXPECT_EQ(true, predicates1->GetLimit() == 2);

    predicates1->SetOrder("ohos");
    predicates1->Distinct();

    EXPECT_EQ("ohos", predicates1->GetOrder());
    EXPECT_EQ(true, predicates1->IsDistinct());

    EXPECT_EQ(false, predicates1->IsRawSelection());
    predicates1->Clear();

    EXPECT_EQ(-1, predicates1->GetLimit());
    EXPECT_EQ(true, predicates1->GetWhereClause().empty());
    EXPECT_EQ(true, predicates1->GetWhereArgs().empty());
    EXPECT_EQ(true, predicates1->GetOrder().empty());
    EXPECT_EQ(false, predicates1->IsDistinct());

    EXPECT_EQ(false, predicates1->IsRawSelection());
}
/* *
 * @tc.name: DataAbilityPredicates_003
 * @tc.desc: test DataAbilityPredicates
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_003, TestSize.Level1)
{
    DataAbilityPredicates *predicates = new DataAbilityPredicates();
    predicates->SetWhereClause("`name` = ? ");
    std::vector<std::string> whereArgsArray = {"zhangsan", "lisi"};
    predicates->SetWhereArgs(whereArgsArray);
    predicates->SetOrder("ASC");
    OHOS::Parcel outParcel(0);
    OHOS::Parcel &coutParcel = outParcel;
    bool isMarshalling = predicates->Marshalling(coutParcel);
    EXPECT_EQ(true, isMarshalling);

    EXPECT_EQ(false, outParcel.ReadBool());
    std::string whereClause = (outParcel.ReadInt32() != g_invalidObjectFlag) ? outParcel.ReadString() : "";
    EXPECT_EQ(whereClause, predicates->GetWhereClause());
    std::vector<std::string> whereArgs;
    if (outParcel.ReadInt32() != g_invalidObjectFlag) {
        outParcel.ReadStringVector(&whereArgs);
    }
    EXPECT_EQ(whereArgs, predicates->GetWhereArgs());
    EXPECT_EQ(false, outParcel.ReadBool());
    std::string indexedBy = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ("", indexedBy);

    outParcel.FlushBuffer();
}

/* *
 * @tc.name: DataAbilityPredicates_004
 * @tc.desc: test DataAbilityPredicates(std::string rawSelection);
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_004, TestSize.Level1)
{
    std::string rawSelection = "select * from table";
    OHOS::sptr<DataAbilityPredicates> predicates = new DataAbilityPredicates(rawSelection);
    EXPECT_EQ(true, predicates->IsRawSelection());
    EXPECT_EQ(rawSelection, predicates->GetWhereClause());
}

/* *
 * @tc.name: DataAbilityPredicates_005
 * @tc.desc: test DataAbilityPredicates(std::string rawSelection);
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_005, TestSize.Level1)
{
    std::string rawSelection = "";
    OHOS::sptr<DataAbilityPredicates> predicates = new DataAbilityPredicates(rawSelection);
    EXPECT_EQ(true, predicates->IsRawSelection());
    EXPECT_EQ(rawSelection, predicates->GetWhereClause());
}

/* *
 * @tc.name: DataAbilityPredicates_006
 * @tc.desc: test DataAbilityPredicates(std::string rawSelection);
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_006, TestSize.Level1)
{
    std::string rawSelection = "select * from user order by @(*.{}`。 age";
    OHOS::sptr<DataAbilityPredicates> predicates = new DataAbilityPredicates(rawSelection);
    EXPECT_EQ(true, predicates->IsRawSelection());
    EXPECT_EQ(rawSelection, predicates->GetWhereClause());
}

/* *
 * @tc.name: DataAbilityPredicates_007
 * @tc.desc: test DataAbilityPredicates(OHOS::Parcel *source)
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_007, TestSize.Level1)
{
    OHOS::sptr<DataAbilityPredicates> predicates = new DataAbilityPredicates(nullptr);
    EXPECT_EQ(false, predicates->GetJudgeSource());
}

/* *
 * @tc.name: DataAbilityPredicates_008
 * @tc.desc: test DataAbilityPredicates(OHOS::Parcel *source)
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_008, TestSize.Level1)
{
    DataAbilityPredicates *predicates = new DataAbilityPredicates();
    predicates->SetWhereClause("`name` = ? ");
    OHOS::Parcel outParcel(0);
    OHOS::Parcel &coutParcel = outParcel;
    bool isMarshalling = predicates->Marshalling(coutParcel);
    OHOS::sptr<DataAbilityPredicates> createdPredicates = new DataAbilityPredicates(&outParcel);
    EXPECT_EQ(true, isMarshalling);
    EXPECT_EQ("`name` = ? ", createdPredicates->GetWhereClause());

    outParcel.FlushBuffer();
}
/* *
 * @tc.name: DataAbilityPredicates_009
 * @tc.desc: test DataAbilityPredicates(OHOS::Parcel *source)
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_009, TestSize.Level1)
{
    DataAbilityPredicates *predicates = new DataAbilityPredicates();
    predicates->SetWhereClause("`name` = ? ");
    std::vector<std::string> whereArgsArray = {"zhangsan", "lisi"};
    predicates->SetWhereArgs(whereArgsArray);
    predicates->SetOrder("ASC");
    OHOS::Parcel outParcel(0);
    OHOS::Parcel &coutParcel = outParcel;
    bool isMarshalling = predicates->Marshalling(coutParcel);
    EXPECT_EQ(true, isMarshalling);

    OHOS::sptr<DataAbilityPredicates> newPredicates = new DataAbilityPredicates(&outParcel);
    EXPECT_EQ(newPredicates->GetWhereClause(), predicates->GetWhereClause());
    EXPECT_EQ(newPredicates->GetWhereArgs(), predicates->GetWhereArgs());
    EXPECT_EQ(newPredicates->GetOrder(), predicates->GetOrder());
    EXPECT_EQ(-1, newPredicates->GetLimit());

    outParcel.FlushBuffer();
}

/* *
 * @tc.name: DataAbilityPredicates_010
 * @tc.desc: test DataAbilityPredicates Marshalling
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_010, TestSize.Level1)
{
    DataAbilityPredicates *predicates = new DataAbilityPredicates();
    std::vector<std::string> group = {"salary"};
    predicates->GreaterThan("age", "20")->OrderByAsc("age")->GroupBy(group)->Limit(3)->Offset(1);

    OHOS::Parcel outParcel(0);
    OHOS::Parcel &coutParcel = outParcel;
    bool isMarshalling = predicates->Marshalling(coutParcel);
    EXPECT_EQ(true, isMarshalling);

    EXPECT_EQ(false, outParcel.ReadBool());
    std::string whereClause = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ(whereClause, predicates->GetWhereClause());
    std::vector<std::string> whereArgs;
    if (outParcel.ReadInt32() != g_invalidObjectFlag) {
        outParcel.ReadStringVector(&whereArgs);
    }
    EXPECT_EQ(whereArgs, predicates->GetWhereArgs());
    bool isDistinct = outParcel.ReadBool();
    EXPECT_EQ(false, isDistinct);
    std::string indexedBy = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ("", indexedBy);
    std::string groupBy = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ(groupBy, predicates->GetGroup());
    std::string order = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ(order, predicates->GetOrder());
    int limit = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadInt32() : -1;
    EXPECT_EQ(true, predicates->GetLimit() == limit);
    int offset = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadInt32() : -1;
    EXPECT_EQ(offset, predicates->GetOffset());

    predicates->Unmarshalling(coutParcel);
    EXPECT_EQ(true, OHOS::NativeRdb::DataAbilityPredicates::result);
    OHOS::sptr<DataAbilityPredicates> newPredicates = new DataAbilityPredicates(&outParcel);

    outParcel.FlushBuffer();
}

/* *
 * @tc.name: DataAbilityPredicates_011
 * @tc.desc: test DataAbilityPredicates Marshalling
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_011, TestSize.Level1)
{
    DataAbilityPredicates *predicates = new DataAbilityPredicates();

    OHOS::Parcel outParcel(0);
    OHOS::Parcel &coutParcel = outParcel;
    bool isMarshalling = predicates->Marshalling(coutParcel);
    EXPECT_EQ(true, isMarshalling);

    EXPECT_EQ(false, outParcel.ReadBool());
    std::string whereClause = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ(whereClause, predicates->GetWhereClause());

    std::vector<std::string> whereArgs;
    if (outParcel.ReadInt32() != g_invalidObjectFlag) {
        outParcel.ReadStringVector(&whereArgs);
    }
    EXPECT_EQ(whereArgs, predicates->GetWhereArgs());
    EXPECT_EQ(false, outParcel.ReadBool());
    std::string indexedBy = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ("", indexedBy);
    std::string groupBy = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ(groupBy, predicates->GetGroup());
    std::string order = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadString() : "";
    EXPECT_EQ(order, predicates->GetOrder());
    int limit = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadInt32() : -1;
    EXPECT_EQ(limit, predicates->GetLimit());
    int offset = outParcel.ReadInt32() != g_invalidObjectFlag ? outParcel.ReadInt32() : -1;
    EXPECT_EQ(offset, predicates->GetOffset());

    outParcel.FlushBuffer();
}

/* *
 * @tc.name: DataAbilityPredicates_012
 * @tc.desc: test DataAbilityPredicates Marshalling
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_012, TestSize.Level1)
{
    OHOS::Parcel parcel(nullptr);
    OHOS::sptr<DataAbilityPredicates> dataAbilityPredicatesWriteNull;
    bool result = parcel.WriteStrongParcelable(dataAbilityPredicatesWriteNull);
    EXPECT_EQ(true, result);

    OHOS::sptr<DataAbilityPredicates> dataAbilityPredicatesWrite = new DataAbilityPredicates();

    bool test = dataAbilityPredicatesWrite->TestBehavior(OHOS::Parcelable::BehaviorFlag::HOLD_OBJECT);
    EXPECT_EQ(false, test);
    test = dataAbilityPredicatesWrite->TestBehavior(OHOS::Parcelable::BehaviorFlag::IPC);
    EXPECT_EQ(false, test);
    test = dataAbilityPredicatesWrite->TestBehavior(OHOS::Parcelable::BehaviorFlag::RPC);
    EXPECT_EQ(false, test);

    result = parcel.WriteStrongParcelable(dataAbilityPredicatesWrite);
    EXPECT_EQ(true, result);

    OHOS::sptr<DataAbilityPredicates> parcelableReadNull = parcel.ReadParcelable<DataAbilityPredicates>();
    EXPECT_EQ(nullptr, parcelableReadNull);

    test = dataAbilityPredicatesWrite->TestBehavior(OHOS::Parcelable::BehaviorFlag::HOLD_OBJECT);
    EXPECT_EQ(true, test);
    test = dataAbilityPredicatesWrite->TestBehavior(OHOS::Parcelable::BehaviorFlag::IPC);
    EXPECT_EQ(false, test);
    test = dataAbilityPredicatesWrite->TestBehavior(OHOS::Parcelable::BehaviorFlag::RPC);
    EXPECT_EQ(false, test);

    dataAbilityPredicatesWrite->ClearBehavior(OHOS::Parcelable::BehaviorFlag::HOLD_OBJECT);
    test = dataAbilityPredicatesWrite->TestBehavior(OHOS::Parcelable::BehaviorFlag::HOLD_OBJECT);
    EXPECT_EQ(false, test);
}

/* *
 * @tc.name: DataAbilityPredicates_013
 * @tc.desc: test DataAbilityPredicates Unmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_013, TestSize.Level1)
{
    DataAbilityPredicates *predicates = new DataAbilityPredicates();
    OHOS::Parcel outParcel(0);
    OHOS::Parcel &coutParcel = outParcel;
    predicates->Unmarshalling(coutParcel);
    EXPECT_EQ(true, OHOS::NativeRdb::DataAbilityPredicates::result);

    outParcel.FlushBuffer();
}

/* *
 * @tc.name: DataAbilityPredicates_014
 * @tc.desc: test DataAbilityPredicates Unmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_014, TestSize.Level1)
{
    OHOS::Parcel parcel(nullptr);
    OHOS::sptr<DataAbilityPredicates> dataAbilityPredicatesWrite = new DataAbilityPredicates();
    bool result = false;

    result = parcel.WriteParcelable(dataAbilityPredicatesWrite);
    EXPECT_EQ(true, result);
    EXPECT_EQ(parcel.GetWritePosition(), parcel.GetDataSize());

    OHOS::sptr<DataAbilityPredicates> dataAbilityPredicatesRead = parcel.ReadParcelable<DataAbilityPredicates>();
    EXPECT_EQ(parcel.GetWritePosition(), parcel.GetDataSize());
}

/* *
 * @tc.name: DataAbilityPredicates_015
 * @tc.desc: test DataAbilityPredicates Unmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000FKD4F
 */
HWTEST_F(DataAbilityPredicatesTest, DataAbilityPredicates_015, TestSize.Level1)
{
    OHOS::Parcel parcel(nullptr);

    bool result = parcel.WriteParcelable(nullptr);
    EXPECT_EQ(true, result);

    OHOS::sptr<DataAbilityPredicates> dataAbilityPredicatesRead = parcel.ReadParcelable<DataAbilityPredicates>();
    EXPECT_EQ(nullptr, dataAbilityPredicatesRead);
}