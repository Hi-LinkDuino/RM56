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
#include <thread>

#include "logger.h"
#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_helper.h"
#include "preferences_xml_utils.h"

using namespace testing::ext;
using namespace OHOS::NativePreferences;

class PreferencesFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PreferencesFileTest::SetUpTestCase(void)
{
}

void PreferencesFileTest::TearDownTestCase(void)
{
}

void PreferencesFileTest::SetUp(void)
{
}

void PreferencesFileTest::TearDown(void)
{
}

/**
 * @tc.name: NativePreferencesFileTest_001
 * @tc.desc: normal testcase of backup file
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: liulinna
 */
HWTEST_F(PreferencesFileTest, NativePreferencesFileTest_001, TestSize.Level1)
{
    std::string file = "/data/test/test";
    std::string backupFile = "/data/test/test.bak";

    std::remove(file.c_str());
    std::remove(backupFile.c_str());

    std::vector<Element> settings;
    Element elem;
    elem.key_ = "intKey";
    elem.tag_ = std::string("int");
    elem.value_ = std::to_string(10);
    settings.push_back(elem);
    PreferencesXmlUtils::WriteSettingXml(backupFile, settings);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);
    int ret = pref->GetInt("intKey", 0);
    EXPECT_EQ(ret, 10);

    struct stat st = { 0 };
    ret = stat(file.c_str(), &st);
    EXPECT_EQ(ret, 0);
    ret = (st.st_mode & (S_IXUSR | S_IXGRP | S_IRWXO));
    EXPECT_EQ(ret, 0);

    pref = nullptr;
    ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: NativePreferencesFileTest_002
 * @tc.desc: normal testcase of file permission
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: liulinna
 */
HWTEST_F(PreferencesFileTest, NativePreferencesFileTest_002, TestSize.Level1)
{
    std::string file = "/data/test/test";
    std::remove(file.c_str());

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    pref->PutInt("key1", 2);
    pref->PutString("key2", "test");
    int ret = pref->FlushSync();
    EXPECT_EQ(ret, E_OK);

    struct stat st = { 0 };
    int result = stat(file.c_str(), &st);
    EXPECT_EQ(result, 0);
    result = st.st_mode & (S_IXUSR | S_IXGRP | S_IRWXO);
    EXPECT_EQ(result, 0);

    pref = nullptr;
    result = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(result, E_OK);
}

/**
 * @tc.name: NativePreferencesFileTest_003
 * @tc.desc: test two same file path
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: liulinna
 */
HWTEST_F(PreferencesFileTest, NativePreferencesFileTest_003, TestSize.Level1)
{
    std::string absolutePath = "/data/test1";
    std::string relativePath = "/data/test/../test1";
    std::remove(absolutePath.c_str());

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref1 = PreferencesHelper::GetPreferences(absolutePath, errCode);
    EXPECT_EQ(errCode, E_OK);

    errCode = E_OK;
    std::shared_ptr<Preferences> pref2 = PreferencesHelper::GetPreferences(relativePath, errCode);
    EXPECT_EQ(errCode, E_OK);

    EXPECT_EQ(pref1, pref2);

    pref1 = nullptr;
    pref2 = nullptr;
    int result = PreferencesHelper::DeletePreferences(absolutePath);
    EXPECT_EQ(result, E_OK);
}

/**
 * @tc.name: NativePreferencesFileTest_004
 * @tc.desc: test FlushSync one times and five times
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: liulinna
 */
HWTEST_F(PreferencesFileTest, NativePreferencesFileTest_004, TestSize.Level1)
{
    std::string file = "/data/test/test";
    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    pref->PutInt("intKey", 1);
    pref->PutString("stringKey", "string1");
    ret = pref->FlushSync();
    EXPECT_EQ(ret, E_OK);

    EXPECT_EQ(1, pref->GetInt("intKey", 0));
    EXPECT_EQ("string1", pref->GetString("stringKey", ""));

    pref = nullptr;
    ret = PreferencesHelper::RemovePreferencesFromCache("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);

    errCode = E_OK;
    pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    EXPECT_EQ(1, pref->GetInt("intKey", 0));
    EXPECT_EQ("string1", pref->GetString("stringKey", ""));

    pref->PutInt("intKey", 2);
    pref->PutString("stringKey", "string2");
    ret = pref->FlushSync();
    EXPECT_EQ(ret, E_OK);
    pref->PutInt("intKey", 3);
    pref->PutString("stringKey", "string3");
    ret = pref->FlushSync();
    EXPECT_EQ(ret, E_OK);

    pref->PutInt("intKey", 4);
    pref->PutString("stringKey", "string4");
    ret = pref->FlushSync();
    EXPECT_EQ(ret, E_OK);

    pref = PreferencesHelper::GetPreferences(file, errCode);
    pref->PutInt("intKey", 5);
    pref->PutString("stringKey", "string5");
    ret = pref->FlushSync();
    EXPECT_EQ(ret, E_OK);

    EXPECT_EQ(5, pref->GetInt("intKey", 0));
    EXPECT_EQ("string5", pref->GetString("stringKey", ""));

    pref = nullptr;
    ret = PreferencesHelper::RemovePreferencesFromCache("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);

    errCode = E_OK;
    pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    EXPECT_EQ(5, pref->GetInt("intKey", 0));
    EXPECT_EQ("string5", pref->GetString("stringKey", ""));

    pref = nullptr;
    ret = PreferencesHelper::DeletePreferences("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);
}

/**
 * @tc.name: NativePreferencesFileTest_005
 * @tc.desc: test Flush one times and five times
 * @tc.type: FUNC
 * @tc.require: AR000CU2BN
 * @tc.author: liulinna
 */
HWTEST_F(PreferencesFileTest, NativePreferencesFileTest_005, TestSize.Level3)
{
    std::string file = "/data/test/test";
    int ret = PreferencesHelper::DeletePreferences(file);
    EXPECT_EQ(ret, E_OK);

    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    pref->PutInt("intKey", 1);
    pref->PutString("stringKey", "string1");
    pref->Flush();

    EXPECT_EQ(1, pref->GetInt("intKey", 0));
    EXPECT_EQ("string1", pref->GetString("stringKey", ""));

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    pref = nullptr;
    ret = PreferencesHelper::RemovePreferencesFromCache("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);

    errCode = E_OK;
    pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    EXPECT_EQ(1, pref->GetInt("intKey", 0));
    EXPECT_EQ("string1", pref->GetString("stringKey", ""));

    pref->PutInt("intKey", 2);
    pref->PutString("stringKey", "string2");
    pref->Flush();
    pref->PutInt("intKey", 3);
    pref->PutString("stringKey", "string3");
    pref->Flush();

    pref->PutInt("intKey", 4);
    pref->PutString("stringKey", "string4");
    pref->Flush();

    pref->PutInt("intKey", 5);
    pref->PutString("stringKey", "string5");
    pref->Flush();

    EXPECT_EQ(5, pref->GetInt("intKey", 0));
    EXPECT_EQ("string5", pref->GetString("stringKey", ""));

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    pref = nullptr;
    ret = PreferencesHelper::RemovePreferencesFromCache("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);

    errCode = E_OK;
    pref = PreferencesHelper::GetPreferences(file, errCode);
    EXPECT_EQ(errCode, E_OK);

    EXPECT_EQ(5, pref->GetInt("intKey", 0));
    EXPECT_EQ("string5", pref->GetString("stringKey", ""));

    pref = nullptr;
    ret = PreferencesHelper::DeletePreferences("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);
}
