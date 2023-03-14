/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#define private public
#include "bundle_mgr_service_event_handler.h"
#undef private

#include <gtest/gtest.h>

#include <climits>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"
#include "bundle_scanner.h"
#include "system_bundle_installer.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using OHOS::DelayedSingleton;

namespace {

const std::string TEST_DIR = "/data/test/resource";
const std::string BUNDLE_FILENAME_1 = "app1.hap";
const std::string BUNDLE_FILENAME_2 = "app2.hap";
const std::string BUNDLE_FILENAME_3 = "app3.hap";
const std::string BUNDLE_FILENAME_4 = "app1.ha";
const std::string BUNDLE_FILENAME_5 = "app2..ap";
const std::string BUNDLE_FILENAME_6 = "app3";
const std::string SYSTEM_APP_SCAN_PATH = "/system/app/com.ohos.photos/";
const std::string THIRD_SYSTEM_APP_SCAN_PATH = "/system/vendor";
const std::string TEST_ERROR_DIR = "/data/error";
const std::string PHOTOS_HAP = "Photos.hap";
const std::string PHOTOS_HAP_NAME = "com.ohos.photos";

}  // namespace

class BmsServiceBundleScanTest : public testing::Test {
public:
    BmsServiceBundleScanTest();
    ~BmsServiceBundleScanTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    void CreateDir(const std::string &path) const;
    void DeleteDir(const std::string &path) const;
    void CreateFile(const std::string &path) const;
    void DeleteFile(const std::string &path) const;

    // return scan result list number
    long TriggerScan();
    bool IsScanResultContain(const std::string name) const;

private:
    std::list<std::string> bundleList_{};
};

BmsServiceBundleScanTest::BmsServiceBundleScanTest()
{}

BmsServiceBundleScanTest::~BmsServiceBundleScanTest()
{}

void BmsServiceBundleScanTest::SetUpTestCase()
{}

void BmsServiceBundleScanTest::TearDownTestCase()
{}

void BmsServiceBundleScanTest::SetUp()
{
    CreateDir(TEST_DIR);
}

void BmsServiceBundleScanTest::TearDown()
{
    bundleList_.clear();
    DeleteDir(TEST_DIR);
}

void BmsServiceBundleScanTest::CreateDir(const std::string &path) const
{
    if (access(path.c_str(), F_OK) != 0) {
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            APP_LOGE("CreateDir:%{private}s error", path.c_str());
        }
    }
}

void BmsServiceBundleScanTest::DeleteDir(const std::string &path) const
{
    if (access(path.c_str(), F_OK) == 0) {
        if (rmdir(path.c_str()) != 0) {
            APP_LOGE("DeleteDir:%{private}s error", path.c_str());
        }
    }
}

void BmsServiceBundleScanTest::CreateFile(const std::string &path) const
{
    if (path.size() > PATH_MAX) {
        APP_LOGE("path too long");
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
    }
    if (access(path.c_str(), F_OK) != 0) {
        APP_LOGE("CreateFile-checkFile:%{private}s not exist", path.c_str());
    }
}

void BmsServiceBundleScanTest::DeleteFile(const std::string &path) const
{
    if (remove(path.c_str()) != 0) {
        APP_LOGE("DeleteFile:%{private}s fail", path.c_str());
    }
}

long BmsServiceBundleScanTest::TriggerScan()
{
    auto scanner = std::make_unique<BundleScanner>();
    bundleList_ = scanner->Scan(TEST_DIR);
    return bundleList_.size();
}

bool BmsServiceBundleScanTest::IsScanResultContain(const std::string name) const
{
    auto it = std::find(bundleList_.begin(), bundleList_.end(), name);
    if (it == bundleList_.end()) {
        GTEST_LOG_(ERROR) << "not find file in list";
        APP_LOGE("file:%{public}s not find in list", name.c_str());
        return false;
    }
    return true;
}
/**
 * @tc.number: BundleScan_0100
 * @tc.name:  Scan
 * @tc.desc: 1. scan dir not exist
 *           2. verify scan result file number is 0
 */
HWTEST_F(BmsServiceBundleScanTest, BundleScan_0100, Function | SmallTest | Level0)
{
    DeleteDir(TEST_DIR);
    int number = static_cast<int>(TriggerScan());
    EXPECT_EQ(0, number);
}
/**
 * @tc.number: BundleScan_0200
 * @tc.name:  Scan
 * @tc.desc: 1. scan dir exist, no bundle file
 *           2. verify scan result file number is 0
 */
HWTEST_F(BmsServiceBundleScanTest, BundleScan_0200, Function | SmallTest | Level0)
{
    int number = static_cast<int>(TriggerScan());
    EXPECT_EQ(0, number);
}
/**
 * @tc.number: BundleScan_0300
 * @tc.name:  Scan
 * @tc.desc: 1. scan dir exist, 3 legal file exist
 *           2. verify scan result file number is 3 and file name correct
 */
HWTEST_F(BmsServiceBundleScanTest, BundleScan_0300, Function | SmallTest | Level0)
{
    CreateDir(TEST_DIR);
    const std::string TEST_FILE_NAME_1 = TEST_DIR + "/" + BUNDLE_FILENAME_1;
    const std::string TEST_FILE_NAME_2 = TEST_DIR + "/" + BUNDLE_FILENAME_2;
    const std::string TEST_FILE_NAME_3 = TEST_DIR + "/" + BUNDLE_FILENAME_3;
    CreateFile(TEST_FILE_NAME_1);
    CreateFile(TEST_FILE_NAME_2);
    CreateFile(TEST_FILE_NAME_3);

    int number = static_cast<int>(TriggerScan());
    EXPECT_EQ(3, number);

    EXPECT_TRUE(IsScanResultContain(TEST_FILE_NAME_1));
    EXPECT_TRUE(IsScanResultContain(TEST_FILE_NAME_2));
    EXPECT_TRUE(IsScanResultContain(TEST_FILE_NAME_3));

    DeleteFile(TEST_FILE_NAME_1);
    DeleteFile(TEST_FILE_NAME_2);
    DeleteFile(TEST_FILE_NAME_3);
}
/**
 * @tc.number: BundleScan_0400
 * @tc.name:  Scan
 * @tc.desc: 1. scan dir exist, 3 illegal file exist
 *           2. verify scan result file number is 3
 */
HWTEST_F(BmsServiceBundleScanTest, BundleScan_0400, Function | SmallTest | Level0)
{
    const std::string TEST_FILE_NAME_4 = TEST_DIR + "/" + BUNDLE_FILENAME_4;
    const std::string TEST_FILE_NAME_5 = TEST_DIR + "/" + BUNDLE_FILENAME_5;
    const std::string TEST_FILE_NAME_6 = TEST_DIR + "/" + BUNDLE_FILENAME_6;

    CreateFile(TEST_FILE_NAME_4);
    CreateFile(TEST_FILE_NAME_5);
    CreateFile(TEST_FILE_NAME_6);

    int number = static_cast<int>(TriggerScan());
    EXPECT_EQ(3, number);

    DeleteFile(TEST_FILE_NAME_4);
    DeleteFile(TEST_FILE_NAME_5);
    DeleteFile(TEST_FILE_NAME_6);
}
/**
 * @tc.number: BundleScan_0500
 * @tc.name:  Scan
 * @tc.desc: 1. scan dir exist, 3 illegal and 3 legal file exist
 *           2. verify scan result file number is 6 and file name correct
 */
HWTEST_F(BmsServiceBundleScanTest, BundleScan_0500, Function | SmallTest | Level0)
{
    const std::string TEST_FILE_NAME_1 = TEST_DIR + "/" + BUNDLE_FILENAME_1;
    const std::string TEST_FILE_NAME_2 = TEST_DIR + "/" + BUNDLE_FILENAME_2;
    const std::string TEST_FILE_NAME_3 = TEST_DIR + "/" + BUNDLE_FILENAME_3;
    const std::string TEST_FILE_NAME_4 = TEST_DIR + "/" + BUNDLE_FILENAME_4;
    const std::string TEST_FILE_NAME_5 = TEST_DIR + "/" + BUNDLE_FILENAME_5;
    const std::string TEST_FILE_NAME_6 = TEST_DIR + "/" + BUNDLE_FILENAME_6;

    CreateFile(TEST_FILE_NAME_1);
    CreateFile(TEST_FILE_NAME_2);
    CreateFile(TEST_FILE_NAME_3);
    CreateFile(TEST_FILE_NAME_4);
    CreateFile(TEST_FILE_NAME_5);
    CreateFile(TEST_FILE_NAME_6);

    int number = static_cast<int>(TriggerScan());
    EXPECT_EQ(6, number);

    EXPECT_TRUE(IsScanResultContain(TEST_FILE_NAME_1));
    EXPECT_TRUE(IsScanResultContain(TEST_FILE_NAME_2));
    EXPECT_TRUE(IsScanResultContain(TEST_FILE_NAME_3));

    DeleteFile(TEST_FILE_NAME_1);
    DeleteFile(TEST_FILE_NAME_2);
    DeleteFile(TEST_FILE_NAME_3);
    DeleteFile(TEST_FILE_NAME_4);
    DeleteFile(TEST_FILE_NAME_5);
    DeleteFile(TEST_FILE_NAME_6);
}
/**
 * @tc.number: RebootBundleScan_0100
 * @tc.name: test reboot scan bundle path
 * @tc.desc: 1. reboot scan dir exist
 *           2. Bundle information can be scanned
 */
HWTEST_F(BmsServiceBundleScanTest, RebootBundleScan_0100, Function | SmallTest | Level0)
{
    auto scanner = std::make_unique<BundleScanner>();
    if (!scanner) {
        APP_LOGE("make scanner failed");
        return;
    }
    std::string scanDir = Constants::SYSTEM_APP_SCAN_PATH;
    std::list<std::string> bundleList = scanner->Scan(scanDir);
    auto result = false;
    if (bundleList.size() > 0) {
        result = true;
    }
    EXPECT_TRUE(result);
}

/**
 * @tc.number: RebootBundleScan_0200
 * @tc.name: test reboot scan bundle path
 * @tc.desc: 1. reboot scan dir not exist
 *           2. Bundle information can't be scanned
 */
HWTEST_F(BmsServiceBundleScanTest, RebootBundleScan_0200, Function | SmallTest | Level1)
{
    auto scanner = std::make_unique<BundleScanner>();
    if (!scanner) {
        APP_LOGE("make scanner failed");
        return;
    }
    std::string scanDir = TEST_ERROR_DIR;
    std::list<std::string> bundleList = scanner->Scan(scanDir);
    auto result = false;
    if (bundleList.size() > 0) {
        result = true;
    }
    EXPECT_FALSE(result);
}

/**
 * @tc.number: RebootBundleScan_0300
 * @tc.name: Get bundleinfo of HAP by path
 * @tc.desc: Pass a HAP of SYSTEM_APP path. Can find information
 */
HWTEST_F(BmsServiceBundleScanTest, RebootBundleScan_0300, Function | SmallTest | Level0)
{
    std::shared_ptr<BMSEventHandler> handler_;
    std::unordered_map<std::string, InnerBundleInfo> infos;
    const std::string PATH = SYSTEM_APP_SCAN_PATH + "/" + PHOTOS_HAP;
    auto result = handler_->ParseHapFiles(PATH, infos);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: RebootBundleScan_0400
 * @tc.name: Get bundleinfo of HAP by path
 * @tc.desc: Pass a HAP of SYSTEM_APP path. Can't find information
 */
HWTEST_F(BmsServiceBundleScanTest, RebootBundleScan_0400, Function | SmallTest | Level1)
{
    std::shared_ptr<EventRunner> runner_;
    runner_ = EventRunner::Create(Constants::BMS_SERVICE_NAME);
    std::shared_ptr<BMSEventHandler> handler_;
    handler_ = std::make_shared<BMSEventHandler>(runner_);
    std::unordered_map<std::string, InnerBundleInfo> infos;
    const std::string PATH = TEST_ERROR_DIR + "/" + PHOTOS_HAP;
    auto result = handler_->ParseHapFiles(PATH, infos);
    EXPECT_FALSE(result);
}
