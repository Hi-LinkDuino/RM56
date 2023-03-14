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

#include <string>

#include "logger.h"
#include "parcel.h"
#include "value_object.h"
#include "values_bucket.h"
#include "message_parcel.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NativeRdb;

class ValuesBucketTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ValuesBucketTest::SetUpTestCase(void)
{
}

void ValuesBucketTest::TearDownTestCase(void)
{
}

void ValuesBucketTest::SetUp(void)
{
}

void ValuesBucketTest::TearDown(void)
{
}

/**
 * @tc.name: Values_Bucket_001
 * @tc.desc: test Values Bucket parcel
 * @tc.type: FUNC
 * @tc.require: AR000CU2BO
 * @tc.author: zengmin
 */
HWTEST_F(ValuesBucketTest, Values_Bucket_001, TestSize.Level1)
{
    ValuesBucket values;
    values.PutInt("id", 1);
    values.PutString("name", std::string("zhangsan"));
    values.PutLong("No.", 9223372036854775807L);
    values.PutDouble("salary", 100.5);
    values.PutBool("graduated", true);
    values.PutBlob("codes", std::vector<uint8_t>{ 1, 2, 3 });
    values.PutNull("mark");

    OHOS::MessageParcel data;
    data.WriteParcelable(&values);

    ValuesBucket *valuesBucket = data.ReadParcelable<ValuesBucket>();
    ValueObject valueObject;

    valuesBucket->GetObject("id", valueObject);
    EXPECT_EQ(ValueObjectType::TYPE_INT, valueObject.GetType());
    int intVal;
    valueObject.GetInt(intVal);
    EXPECT_EQ(1, intVal);

    valuesBucket->GetObject("name", valueObject);
    EXPECT_EQ(ValueObjectType::TYPE_STRING, valueObject.GetType());
    std::string strVal;
    valueObject.GetString(strVal);
    EXPECT_EQ("zhangsan", strVal);

    valuesBucket->GetObject("No.", valueObject);
    EXPECT_EQ(ValueObjectType::TYPE_INT, valueObject.GetType());
    int64_t int64Val;
    valueObject.GetLong(int64Val);
    EXPECT_EQ(9223372036854775807L, int64Val);

    valuesBucket->GetObject("salary", valueObject);
    EXPECT_EQ(ValueObjectType::TYPE_DOUBLE, valueObject.GetType());
    double doubleVal;
    valueObject.GetDouble(doubleVal);
    EXPECT_EQ(100.5, doubleVal);

    valuesBucket->GetObject("graduated", valueObject);
    EXPECT_EQ(ValueObjectType::TYPE_BOOL, valueObject.GetType());
    bool boolVal = false;
    valueObject.GetBool(boolVal);
    EXPECT_EQ(true, boolVal);

    valuesBucket->GetObject("codes", valueObject);
    EXPECT_EQ(ValueObjectType::TYPE_BLOB, valueObject.GetType());
    std::vector<uint8_t> blobVal;
    valueObject.GetBlob(blobVal);
    EXPECT_EQ((uint32_t)3, blobVal.size());
    EXPECT_EQ(1, blobVal.at(0));
    EXPECT_EQ(2, blobVal.at(1));
    EXPECT_EQ(3, blobVal.at(2));

    valuesBucket->GetObject("mark", valueObject);
    EXPECT_EQ(ValueObjectType::TYPE_NULL, valueObject.GetType());
}
