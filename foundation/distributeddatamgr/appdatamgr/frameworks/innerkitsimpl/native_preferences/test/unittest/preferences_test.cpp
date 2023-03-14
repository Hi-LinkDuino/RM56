/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "preferences.h"

#include <gtest/gtest.h>

#include <cctype>
#include <iostream>
#include <string>

#include "logger.h"
#include "preferences_errno.h"
#include "preferences_helper.h"
#include "preferences_observer.h"

using namespace testing::ext;
using namespace OHOS::NativePreferences;

class PreferencesTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::shared_ptr<Preferences> pref;

    static const std::string LONG_KEY;

    static const std::string KEY_TEST_INT_ELEMENT;
    static const std::string KEY_TEST_LONG_ELEMENT;
    static const std::string KEY_TEST_FLOAT_ELEMENT;
    static const std::string KEY_TEST_DOUBLE_ELEMENT;
    static const std::string KEY_TEST_BOOL_ELEMENT;
    static const std::string KEY_TEST_STRING_ELEMENT;
};

std::shared_ptr<Preferences> PreferencesTest::pref = nullptr;

const std::string PreferencesTest::LONG_KEY = std::string(Preferences::MAX_KEY_LENGTH, std::toupper('a'));
;

const std::string PreferencesTest::KEY_TEST_INT_ELEMENT = "key_test_int";
const std::string PreferencesTest::KEY_TEST_LONG_ELEMENT = "key_test_long";
const std::string PreferencesTest::KEY_TEST_FLOAT_ELEMENT = "key_test_float";
const std::string PreferencesTest::KEY_TEST_DOUBLE_ELEMENT = "key_test_double";
const std::string PreferencesTest::KEY_TEST_BOOL_ELEMENT = "key_test_bool";
const std::string PreferencesTest::KEY_TEST_STRING_ELEMENT = "key_test_string";

void PreferencesTest::SetUpTestCase(void)
{
    int errCode = E_OK;
    pref = PreferencesHelper::GetPreferences("/data/test/test", errCode);
    EXPECT_EQ(errCode, E_OK);
}

void PreferencesTest::TearDownTestCase(void)
{
    pref = nullptr;
    int ret = PreferencesHelper::DeletePreferences("/data/test/test");
    EXPECT_EQ(ret, E_OK);
}

void PreferencesTest::SetUp(void)
{
}

void PreferencesTest::TearDown(void)
{
    /* clear all data after every case */
    if (pref) {
        pref->Clear();
        pref->Flush();
    }
}

class PreferencesObserverCounter : public PreferencesObserver {
public:
    virtual ~PreferencesObserverCounter();
    void OnChange(Preferences &preferences, const std::string &key) override;

    std::atomic_int notifyTimes;
    static const std::vector<std::string> NOTIFY_KEYS_VECTOR;
};

PreferencesObserverCounter::~PreferencesObserverCounter()
{
}

void PreferencesObserverCounter::OnChange(Preferences &preferences, const std::string &key)
{
    for (auto it = NOTIFY_KEYS_VECTOR.cbegin(); it != NOTIFY_KEYS_VECTOR.cend(); it++) {
        if (key.compare(*it)) {
            notifyTimes++;
            break;
        }
    }
}

const std::vector<std::string> PreferencesObserverCounter::NOTIFY_KEYS_VECTOR = { PreferencesTest::KEY_TEST_INT_ELEMENT,
    PreferencesTest::KEY_TEST_LONG_ELEMENT, PreferencesTest::KEY_TEST_FLOAT_ELEMENT,
    PreferencesTest::KEY_TEST_BOOL_ELEMENT, PreferencesTest::KEY_TEST_STRING_ELEMENT };

/**
 * @tc.name: NativePreferencesTest_001
 * @tc.desc: normal testcase of FlushSync
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_001, TestSize.Level1)
{
    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 2);
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    pref->FlushSync();

    int ret1 = pref->GetInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 6);
    EXPECT_EQ(ret1, 2);
    std::string ret2 = pref->GetString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test1");
    EXPECT_EQ(ret2, "test");
}

/**
 * @tc.name: NativePreferencesTest_002
 * @tc.desc: normal testcase of HasKey
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_002, TestSize.Level1)
{
    bool ret = pref->HasKey(LONG_KEY + 'x');
    EXPECT_EQ(ret, false);

    ret = pref->HasKey(std::string());
    EXPECT_EQ(ret, false);

    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    ret = pref->HasKey(KEY_TEST_STRING_ELEMENT);
    EXPECT_EQ(ret, true);

    pref->Flush();
    ret = pref->HasKey(KEY_TEST_STRING_ELEMENT);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: NativePreferencesTest_003
 * @tc.desc: normal testcase of pref
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_003, TestSize.Level1)
{
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test1");

    std::string ret = pref->GetString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "defaultvalue");
    EXPECT_EQ(ret, "test1");

    pref->Flush();
    ret = pref->GetString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "defaultvalue");
    EXPECT_EQ(ret, "test1");
}

/**
 * @tc.name: NativePreferencesTest_004
 * @tc.desc: normal testcase of GetBool
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_004, TestSize.Level1)
{
    bool ret = pref->GetBool(PreferencesTest::LONG_KEY + 'x', true);
    EXPECT_EQ(ret, true);

    ret = pref->GetBool("", true);
    EXPECT_EQ(ret, true);

    pref->PutBool(PreferencesTest::KEY_TEST_BOOL_ELEMENT, false);
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "false");
    pref->Flush();

    ret = pref->GetBool(PreferencesTest::KEY_TEST_BOOL_ELEMENT, true);
    EXPECT_EQ(ret, false);

    ret = pref->GetBool(PreferencesTest::KEY_TEST_STRING_ELEMENT, true);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: NativePreferencesTest_005
 * @tc.desc: normal testcase of GetFloat
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_005, TestSize.Level1)
{
    float ret = pref->GetFloat(PreferencesTest::LONG_KEY + 'x', 3.0f);
    EXPECT_EQ(ret, 3.0f);

    ret = pref->GetFloat("", 3.0f);
    EXPECT_EQ(ret, 3.0f);

    pref->PutFloat(PreferencesTest::KEY_TEST_FLOAT_ELEMENT, 5.0f);
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "5.0");
    pref->Flush();

    ret = pref->GetFloat(PreferencesTest::KEY_TEST_FLOAT_ELEMENT, true);
    EXPECT_EQ(ret, 5.0f);

    ret = pref->GetFloat(PreferencesTest::KEY_TEST_STRING_ELEMENT, 3.0f);
    EXPECT_EQ(ret, 3.0f);
}

/**
 * @tc.name: NativePreferencesTest_006
 * @tc.desc: normal testcase of GetInt
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_006, TestSize.Level1)
{
    int ret = pref->GetInt(PreferencesTest::LONG_KEY + 'x', 35);
    EXPECT_EQ(ret, 35);

    ret = pref->GetInt("", 35);
    EXPECT_EQ(ret, 35);

    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 5);
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "5");
    pref->Flush();

    ret = pref->GetInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 3);
    EXPECT_EQ(ret, 5);

    ret = pref->GetInt(PreferencesTest::KEY_TEST_STRING_ELEMENT, 3);
    EXPECT_EQ(ret, 3);
}

/**
 * @tc.name: NativePreferencesTest_007
 * @tc.desc: normal testcase of GetLong
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_007, TestSize.Level1)
{
    int64_t ret = pref->GetLong(PreferencesTest::LONG_KEY + 'x', 35L);
    EXPECT_EQ(ret, 35L);

    ret = pref->GetLong("", 35L);
    EXPECT_EQ(ret, 35L);

    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 5);
    pref->PutLong(PreferencesTest::KEY_TEST_LONG_ELEMENT, 5L);
    pref->Flush();

    ret = pref->GetLong(PreferencesTest::KEY_TEST_INT_ELEMENT, 3L);
    EXPECT_EQ(ret, 3L);

    ret = pref->GetLong(PreferencesTest::KEY_TEST_LONG_ELEMENT, 3L);
    EXPECT_EQ(ret, 5L);
}

/**
 * @tc.name: NativePreferencesTest_008
 * @tc.desc: normal testcase of GetString
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_008, TestSize.Level1)
{
    std::string ret = pref->GetString(PreferencesTest::LONG_KEY + 'x', "test");
    EXPECT_EQ(ret, "test");

    ret = pref->GetString("", "test");
    EXPECT_EQ(ret, "test");

    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 5);
    pref->PutString(PreferencesTest::KEY_TEST_LONG_ELEMENT, "test");
    pref->Flush();

    ret = pref->GetString(PreferencesTest::KEY_TEST_INT_ELEMENT, "defaultvalue");
    EXPECT_EQ(ret, "defaultvalue");

    ret = pref->GetString(PreferencesTest::KEY_TEST_LONG_ELEMENT, "defaultvalue");
    EXPECT_EQ(ret, "test");
}

/**
 * @tc.name: NativePreferencesTest_009
 * @tc.desc: normal testcase of GetDefValue
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_009, TestSize.Level1)
{
    int ret0 = pref->GetInt(PreferencesTest::KEY_TEST_INT_ELEMENT, -1);
    EXPECT_EQ(ret0, -1);

    float ret1 = pref->GetFloat(PreferencesTest::KEY_TEST_FLOAT_ELEMENT, 1.0f);
    EXPECT_EQ(ret1, 1.0f);

    int64_t ret2 = pref->GetLong(PreferencesTest::KEY_TEST_LONG_ELEMENT, 10000L);
    EXPECT_EQ(ret2, 10000L);

    bool ret3 = pref->GetBool(PreferencesTest::KEY_TEST_BOOL_ELEMENT, true);
    EXPECT_EQ(ret3, true);

    std::string ret4 = pref->GetString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    EXPECT_EQ(ret4, "test");
}

/**
 * @tc.name: NativePreferencesTest_010
 * @tc.desc: normal testcase of PutBool
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_010, TestSize.Level1)
{
    pref->PutBool(PreferencesTest::LONG_KEY + 'x', true);
    pref->PutBool("", true);
    pref->Flush();

    bool ret = pref->GetBool(PreferencesTest::LONG_KEY + 'x', false);
    EXPECT_EQ(ret, false);
    ret = pref->GetBool("", false);
    EXPECT_EQ(ret, false);

    pref->PutBool(PreferencesTest::LONG_KEY, true);
    pref->PutBool(PreferencesTest::KEY_TEST_BOOL_ELEMENT, true);
    pref->Flush();

    ret = pref->GetBool(PreferencesTest::LONG_KEY, false);
    EXPECT_EQ(ret, true);
    ret = pref->GetBool(PreferencesTest::KEY_TEST_BOOL_ELEMENT, false);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: NativePreferencesTest_011
 * @tc.desc: normal testcase of PutFloat
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_011, TestSize.Level1)
{
    pref->PutFloat(PreferencesTest::LONG_KEY + 'x', 3.0f);
    pref->PutFloat("", 3.0f);
    pref->Flush();

    float ret = pref->GetFloat(PreferencesTest::LONG_KEY + 'x', 1.0f);
    EXPECT_EQ(ret, 1.0f);
    ret = pref->GetFloat("", 1.0f);
    EXPECT_EQ(ret, 1.0f);

    pref->PutFloat(PreferencesTest::LONG_KEY, 3.0f);
    pref->PutFloat(PreferencesTest::KEY_TEST_FLOAT_ELEMENT, 3.0f);
    pref->Flush();

    ret = pref->GetFloat(PreferencesTest::LONG_KEY, 1.0f);
    EXPECT_EQ(ret, 3.0f);
    ret = pref->GetFloat(PreferencesTest::KEY_TEST_FLOAT_ELEMENT, 1.0f);
    EXPECT_EQ(ret, 3.0f);
}

/**
 * @tc.name: NativePreferencesTest_012
 * @tc.desc: normal testcase of PutInt
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_012, TestSize.Level1)
{
    pref->PutInt(PreferencesTest::LONG_KEY + 'x', 3);
    pref->PutInt("", 3);
    pref->Flush();

    int ret = pref->GetInt(PreferencesTest::LONG_KEY + 'x', 1);
    EXPECT_EQ(ret, 1);
    ret = pref->GetInt("", 1);
    EXPECT_EQ(ret, 1);

    pref->PutInt(PreferencesTest::LONG_KEY, 3);
    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 3);
    pref->Flush();

    ret = pref->GetInt(PreferencesTest::LONG_KEY, 1);
    EXPECT_EQ(ret, 3);
    ret = pref->GetInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 1);
    EXPECT_EQ(ret, 3);
}

/**
 * @tc.name: NativePreferencesTest_013
 * @tc.desc: normal testcase of PutLong
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_013, TestSize.Level1)
{
    pref->PutLong(PreferencesTest::LONG_KEY + 'x', 3L);
    pref->PutLong("", 3L);
    pref->Flush();

    int64_t ret = pref->GetLong(PreferencesTest::LONG_KEY + 'x', 1L);
    EXPECT_EQ(ret, 1L);
    ret = pref->GetLong("", 1L);
    EXPECT_EQ(ret, 1L);

    pref->PutLong(PreferencesTest::LONG_KEY, 3L);
    pref->PutLong(PreferencesTest::KEY_TEST_LONG_ELEMENT, 3L);
    pref->Flush();

    ret = pref->GetLong(PreferencesTest::LONG_KEY, 1L);
    EXPECT_EQ(ret, 3L);
    ret = pref->GetLong(PreferencesTest::KEY_TEST_LONG_ELEMENT, 1L);
    EXPECT_EQ(ret, 3L);
}

/**
 * @tc.name: NativePreferencesTest_014
 * @tc.desc: normal testcase of PutString
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_014, TestSize.Level1)
{
    pref->PutString(PreferencesTest::LONG_KEY + 'x', "test");
    pref->PutString("", "test");
    pref->Flush();

    std::string ret = pref->GetString(PreferencesTest::LONG_KEY + 'x', "defaultValue");
    EXPECT_EQ(ret, "defaultValue");
    ret = pref->GetString("", "defaultValue");
    EXPECT_EQ(ret, "defaultValue");

    pref->PutString(PreferencesTest::LONG_KEY, "test");
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    pref->Flush();

    ret = pref->GetString(PreferencesTest::LONG_KEY, "defaultValue");
    EXPECT_EQ(ret, "test");
    ret = pref->GetString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "defaultValue");
    EXPECT_EQ(ret, "test");
}

/**
 * @tc.name: NativePreferencesTest_015
 * @tc.desc: normal testcase of Delete
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_015, TestSize.Level1)
{
    pref->PutString("test", "remove");
    pref->Flush();

    std::string ret = pref->GetString("test", "defaultValue");
    EXPECT_EQ(ret, "remove");

    pref->Delete("test");
    pref->Flush();
    ret = pref->GetString("test", "defaultValue");
    EXPECT_EQ(ret, "defaultValue");
}

/**
 * @tc.name: NativePreferencesTest_016
 * @tc.desc: normal testcase of RegisterPreferencesObserver
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_016, TestSize.Level1)
{
    std::shared_ptr<PreferencesObserver> counter =
        std::make_shared<PreferencesObserverCounter>();
    pref->RegisterObserver(counter);

    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    pref->Flush();
    EXPECT_EQ(static_cast<PreferencesObserverCounter *>(counter.get())->notifyTimes, 1);

    /* same value */
    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 2);
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    pref->Flush();
    EXPECT_EQ(static_cast<PreferencesObserverCounter *>(counter.get())->notifyTimes, 2);

    pref->UnRegisterObserver(counter);
}

/**
 * @tc.name: NativePreferencesTest_017
 * @tc.desc: normal testcase of UnRegisterPreferencesObserver
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_017, TestSize.Level1)
{
    std::shared_ptr<PreferencesObserver> counter =
        std::make_shared<PreferencesObserverCounter>();
    pref->RegisterObserver(counter);

    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 2);
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    pref->Flush();
    EXPECT_EQ(static_cast<PreferencesObserverCounter *>(counter.get())->notifyTimes, 2);

    pref->UnRegisterObserver(counter);

    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 6);
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test1");
    pref->Flush();
    EXPECT_EQ(static_cast<PreferencesObserverCounter *>(counter.get())->notifyTimes, 2);
}

/**
 * @tc.name: NativePreferencesTest_018
 * @tc.desc: normal testcase of Clear
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_018, TestSize.Level1)
{
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "test");
    pref->PutInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 3);
    pref->Flush();

    pref->Clear();
    std::string ret = pref->GetString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "defaultvalue");
    EXPECT_EQ(ret, "defaultvalue");
    int ret1 = pref->GetInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 0);
    EXPECT_EQ(ret1, 0);

    pref->Flush();
    ret = pref->GetString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "defaultvalue");
    EXPECT_EQ(ret, "defaultvalue");
    ret1 = pref->GetInt(PreferencesTest::KEY_TEST_INT_ELEMENT, 0);
    EXPECT_EQ(ret1, 0);
}

/**
 * @tc.name: NativePreferencesTest_019
 * @tc.desc: normal testcase of GetDouble
 * @tc.type: FUNC
 * @tc.require: Na
 * @tc.author: mang tsang
 */
HWTEST_F(PreferencesTest, NativePreferencesTest_019, TestSize.Level1)
{
    double ret = pref->GetDouble(PreferencesTest::LONG_KEY + 'x', 35.99999);
    EXPECT_EQ(ret, 35.99999);

    ret = pref->GetDouble("", 35.99999);
    EXPECT_EQ(ret, 35.99999);

    pref->PutDouble(PreferencesTest::KEY_TEST_DOUBLE_ELEMENT, (std::numeric_limits<double>::max)());
    pref->PutString(PreferencesTest::KEY_TEST_STRING_ELEMENT, "5.99");
    pref->Flush();

    ret = pref->GetDouble(PreferencesTest::KEY_TEST_DOUBLE_ELEMENT, 3.99);
    EXPECT_EQ(ret, (std::numeric_limits<double>::max)());

    ret = pref->GetDouble(PreferencesTest::KEY_TEST_STRING_ELEMENT, 3.99);
    EXPECT_EQ(ret, 3.99);
}
