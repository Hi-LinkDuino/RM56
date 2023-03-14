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

#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_extractor.h"
#include "bundle_mgr_service.h"
#include "bundle_parser.h"
#include "bundle_scanner.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using OHOS::DelayedSingleton;
namespace {
const std::string TEST_DIR = "bundle_scan_module_test";
const std::string BUNDLE_FILENAME_NORMAL = "t11.hap";
const std::string BUNDLE_FILENAME_ABNORMAL = "t11.rpk";
const string RESOURCE_ROOT_PATH = "/hos/test/bms_bundle/";
const int CYCLE_NUMBER = 10;
}  // namespace

class BmsServiceStartModuleTest : public testing::Test {
public:
    BmsServiceStartModuleTest();
    ~BmsServiceStartModuleTest();

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    void CreateDir(const std::string &path) const;
    void DeleteDir(const std::string &path) const;
    void CreateFile(const std::string &path) const;
    void DeleteFile(const std::string &path) const;
    long TriggerScan();
    bool IsScanResultContain(const std::string &name) const;

    std::shared_ptr<BundleMgrService> GetServiceInstance()
    {
        return bms_;
    }

protected:
    std::ostringstream pathStream_;

private:
    std::shared_ptr<BundleMgrService> bms_ = DelayedSingleton<BundleMgrService>::GetInstance();
    std::list<std::string> bundleList_{};
};

BmsServiceStartModuleTest::BmsServiceStartModuleTest()
{}

BmsServiceStartModuleTest::~BmsServiceStartModuleTest()
{
    bms_.reset();
}

void BmsServiceStartModuleTest::SetUpTestCase()
{}

void BmsServiceStartModuleTest::TearDownTestCase()
{}

void BmsServiceStartModuleTest::SetUp()
{
    CreateDir(TEST_DIR);
}

void BmsServiceStartModuleTest::TearDown()
{
    bundleList_.clear();
    DeleteDir(TEST_DIR);
}

void BmsServiceStartModuleTest::CreateDir(const std::string &path) const
{
    if (access(path.c_str(), F_OK) != 0) {
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            APP_LOGE("CreateDir:%{private}s error", path.c_str());
        }
    }
}

void BmsServiceStartModuleTest::DeleteDir(const std::string &path) const
{
    if (access(path.c_str(), F_OK) == 0) {
        if (rmdir(path.c_str()) != 0) {
            APP_LOGE("DeleteDir:%{private}s error", path.c_str());
        }
    }
}

void BmsServiceStartModuleTest::CreateFile(const std::string &path) const
{
    if (path.size() > PATH_MAX) {
        APP_LOGE("CreateFile the length of path is too long");
        return;
    }
    mode_t mode = 0666;
    int fd = open(path.c_str(), O_RDWR | O_CREAT, mode);
    if (fd == -1) {
        APP_LOGE("CreateFile-open:%{private}s error", path.c_str());
        return;
    }
    if (close(fd) != 0) {
        APP_LOGW("CreateFile-close:%{private}s error", path.c_str());
        return;
    }

    if (access(path.c_str(), F_OK) != 0) {
        APP_LOGE("CreateFile-checkFile:%{private}s not exist", path.c_str());
    }
}

void BmsServiceStartModuleTest::DeleteFile(const std::string &path) const
{
    if (remove(path.c_str()) != 0) {
        APP_LOGE("DeleteFile:%{private}s fail", path.c_str());
    }
}

long BmsServiceStartModuleTest::TriggerScan()
{
    auto scanner = std::make_unique<BundleScanner>();
    if (scanner == nullptr) {
        return -1;
    }

    bundleList_ = scanner->Scan(TEST_DIR);
    return bundleList_.size();
}

bool BmsServiceStartModuleTest::IsScanResultContain(const std::string &name) const
{
    auto it = std::find(bundleList_.begin(), bundleList_.end(), name);
    if (it == bundleList_.end()) {
        return false;
    }
    return true;
}

/**
 * @tc.number: BmsStartup_0100
 * @tc.name: Bundle manager service startup
 * @tc.desc: Bundle manager service startup 1000 times,only the first time is successful
 */
HWTEST_F(BmsServiceStartModuleTest, BmsStartup_0100, Function | MediumTest | Level1)
{
    APP_LOGI("BmsStartup_0100 start");

    EXPECT_EQ(false, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStart();
    EXPECT_EQ(true, GetServiceInstance()->IsServiceReady());

    for (int i = 0; i < CYCLE_NUMBER; i++) {
        GetServiceInstance()->OnStart();
    }
    EXPECT_EQ(true, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStop();
    EXPECT_EQ(false, GetServiceInstance()->IsServiceReady());
    APP_LOGI("BmsStartup_0100 end");
}

/**
 * @tc.number: BmsStartup_0200
 * @tc.name: Bundle manager service startup and stop
 * @tc.desc: Bms service start and stop 1000 times alternately,Only the last start is successful
 */
HWTEST_F(BmsServiceStartModuleTest, BmsStartup_0200, Function | MediumTest | Level1)
{
    APP_LOGI("BmsStartup_0200 start");
    EXPECT_EQ(false, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStart();
    EXPECT_EQ(true, GetServiceInstance()->IsServiceReady());

    for (int i = 0; i < CYCLE_NUMBER; i++) {
        GetServiceInstance()->OnStart();
        GetServiceInstance()->OnStop();
    }
    EXPECT_EQ(false, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStart();
    EXPECT_EQ(true, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStop();
    APP_LOGI("BmsStartup_0200 end");
}

/**
 * @tc.number: BmsStartup_0300
 * @tc.name: Bundle manager service start and stop alternately
 * @tc.desc: Bundle manager service stop 1000 times alternately,the last time is successful
 */
HWTEST_F(BmsServiceStartModuleTest, BmsStartup_0300, Function | MediumTest | Level1)
{
    APP_LOGI("BmsStartup_0300 start");
    EXPECT_EQ(false, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStart();
    EXPECT_EQ(true, GetServiceInstance()->IsServiceReady());

    for (int i = 0; i < CYCLE_NUMBER; i++) {
        GetServiceInstance()->OnStop();
    }
    EXPECT_EQ(false, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStart();
    EXPECT_EQ(true, GetServiceInstance()->IsServiceReady());
    GetServiceInstance()->OnStop();
    EXPECT_EQ(false, GetServiceInstance()->IsServiceReady());
    APP_LOGI("BmsStartup_0300 end");
}

/**
 * @tc.number: TestParse_0100
 * @tc.name: Parse bundle package by config.json
 * @tc.desc: Test parse bundle with an incorrect path
 */
HWTEST_F(BmsServiceStartModuleTest, TestParse_0100, Function | MediumTest | Level2)
{
    APP_LOGI("TestParse_0100 start");
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;

    pathStream_ << RESOURCE_ROOT_PATH << "incorrect_path/demo" << Constants::INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
    APP_LOGI("TestParse_0100 end");
}

/**
 * @tc.number: TestParse_0200
 * @tc.name: Parse bundle package by config.json
 * @tc.desc: Test the path of parse bundle with unusual character
 */
HWTEST_F(BmsServiceStartModuleTest, TestParse_0200, Function | MediumTest | Level2)
{
    APP_LOGI("TestParse_0200 start");
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;

    pathStream_ << RESOURCE_ROOT_PATH << " demo" << Constants::INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);

    BundleInfo bundleInfo = innerBundleInfo.GetBaseBundleInfo();
    EXPECT_NE(bundleInfo.name, "com.third.example.helloworld");
    EXPECT_NE(bundleInfo.applicationInfo.name, "com.third.example.helloworld");
    APP_LOGI("TestParse_0200 end");
}

/**
 * @tc.number: TestParse_0300
 * @tc.name: Parse bundle package by config.json
 * @tc.desc: Test the path of parse bundle with long string
 */
HWTEST_F(BmsServiceStartModuleTest, TestParse_0300, Function | MediumTest | Level2)
{
    APP_LOGI("TestParse_0300 start");
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;

    pathStream_ << RESOURCE_ROOT_PATH
                << "deepdir_1/deepdir_2/deepdir_3/deepdir_4/"
                   "deepdir_5/deepdir_6/deepdir_7/deepdir_8/deepdir_9/demo"
                << Constants::INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
    APP_LOGI("TestParse_0300 end");
}

/**
 * @tc.number: TestBundleScan_0100
 * @tc.name: Parse bundle package by bundle.json
 * @tc.desc: Scan a normal bundle and an abnormal bundle
 */
HWTEST_F(BmsServiceStartModuleTest, TestBundleScan_0100, Function | MediumTest | Level2)
{
    APP_LOGI("TestBundleScan_0100 start");
    const std::string testFileNameNormal = TEST_DIR + "/" + BUNDLE_FILENAME_NORMAL;
    const std::string testFileNameAbnormal = TEST_DIR + "/" + BUNDLE_FILENAME_ABNORMAL;

    CreateFile(testFileNameNormal);
    CreateFile(testFileNameAbnormal);

    int number = static_cast<int>(TriggerScan());
    EXPECT_EQ(2, number);

    EXPECT_TRUE(IsScanResultContain(testFileNameNormal));
    EXPECT_TRUE(IsScanResultContain(testFileNameAbnormal));

    DeleteFile(testFileNameNormal);
    DeleteFile(testFileNameAbnormal);
    APP_LOGI("TestBundleScan_0100 end");
}

/**
 * @tc.number: TestBundleScan_0200
 * @tc.name: Parse bundle package by config.json
 * @tc.desc: scan 10 normal bundles and 10 abnormal bundles
 */
HWTEST_F(BmsServiceStartModuleTest, TestBundleScan_0200, Function | MediumTest | Level2)
{
    APP_LOGI("TestBundleScan_0200 start");
    unsigned long int i;

    for (i = 0; i < CYCLE_NUMBER; i++) {
        const std::string testFileNameNormal = TEST_DIR + "/" + std::to_string(i) + BUNDLE_FILENAME_NORMAL;
        const std::string testFileNameAbnormal = TEST_DIR + "/" + std::to_string(i) + BUNDLE_FILENAME_ABNORMAL;
        CreateFile(testFileNameNormal);
        CreateFile(testFileNameAbnormal);
    }

    int number = static_cast<int>(TriggerScan());
    EXPECT_EQ(CYCLE_NUMBER * 2, number);

    for (i = 0; i < CYCLE_NUMBER; i++) {
        const std::string testFileNameNormal = TEST_DIR + "/" + std::to_string(i) + BUNDLE_FILENAME_NORMAL;
        const std::string testFileNameAbnormal = TEST_DIR + "/" + std::to_string(i) + BUNDLE_FILENAME_ABNORMAL;
        EXPECT_TRUE(IsScanResultContain(testFileNameNormal));
        EXPECT_TRUE(IsScanResultContain(testFileNameAbnormal));
        DeleteFile(testFileNameNormal);
        DeleteFile(testFileNameAbnormal);
    }
    APP_LOGI("TestBundleScan_0200 end");
}
