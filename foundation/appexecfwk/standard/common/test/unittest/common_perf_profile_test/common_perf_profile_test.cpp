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

#include "perf_profile.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace {
const int64_t INVALID_TIME = -1;

const int64_t BMS_LOAD_INIT_START_TIME = 0;
const int64_t BMS_LOAD_SET_START_TIME = 10;
const int64_t BMS_LOAD_INIT_END_TIME = 0;
const int64_t BMS_LOAD_SET_END_TIME = 120;

const int64_t BUNDLE_SCAN_INIT_START_TIME = 0;
const int64_t BUNDLE_SCAN_SET_START_TIME = 100;
const int64_t BUNDLE_SCAN_INIT_END_TIME = 0;
const int64_t BUNDLE_SCAN_SET_END_TIME = 150;

const int64_t BUNDLE_DOWNLOAD_INIT_START_TIME = 0;
const int64_t BUNDLE_DOWNLOAD_SET_START_TIME = 50;
const int64_t BUNDLE_DOWNLOAD_INIT_END_TIME = 0;
const int64_t BUNDLE_DOWNLOAD_SET_END_TIME = 500;

const int64_t BUNDLE_INSTALL_INIT_START_TIME = 0;
const int64_t BUNDLE_INSTALL_SET_START_TIME = 50;
const int64_t BUNDLE_INSTALL_SET_START_TIME_SECOND = 400;
const int64_t BUNDLE_INSTALL_INIT_END_TIME = 0;
const int64_t BUNDLE_INSTALL_SET_END_TIME = 300;
const int64_t BUNDLE_INSTALL_SET_END_TIME_SECOND = 500;
const int64_t BUNDLE_INSTALL_INIT_TOTAL_TIME = 0;

const int64_t BUNDLE_UNINSTALL_INIT_START_TIME = 0;
const int64_t BUNDLE_UNINSTALL_SET_START_TIME = 30;
const int64_t BUNDLE_UNINSTALL_INIT_END_TIME = 0;
const int64_t BUNDLE_UNINSTALL_SET_END_TIME = 130;

const int64_t BUNDLE_PARSE_INIT_START_TIME = 0;
const int64_t BUNDLE_PARSE_SET_START_TIME = 50;
const int64_t BUNDLE_PARSE_INIT_END_TIME = 0;
const int64_t BUNDLE_PARSE_SET_END_TIME = 250;

const int64_t AMS_LOAD_INIT_START_TIME = 0;
const int64_t AMS_LOAD_SET_START_TIME = 10;
const int64_t AMS_LOAD_INIT_END_TIME = 0;
const int64_t AMS_LOAD_SET_END_TIME = 90;

const int64_t ABILITY_LOAD_INIT_START_TIME = 0;
const int64_t ABILITY_LOAD_SET_START_TIME = 50;
const int64_t ABILITY_LOAD_INIT_END_TIME = 0;
const int64_t ABILITY_LOAD_SET_END_TIME = 1000;

const int64_t APP_FORK_INIT_START_TIME = 0;
const int64_t APP_FORK_SET_START_TIME = 50;
const int64_t APP_FORK_INIT_END_TIME = 0;
const int64_t APP_FORK_SET_END_TIME = 100;
}  // namespace

class CommonPerfProfileTest : public testing::Test {
public:
    CommonPerfProfileTest();
    ~CommonPerfProfileTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

CommonPerfProfileTest::CommonPerfProfileTest()
{}

CommonPerfProfileTest::~CommonPerfProfileTest()
{}

void CommonPerfProfileTest::SetUpTestCase()
{}

void CommonPerfProfileTest::TearDownTestCase()
{}

void CommonPerfProfileTest::SetUp()
{
    PerfProfile::GetInstance().Reset();
}

void CommonPerfProfileTest::TearDown()
{}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBmsLoadTime
 * SubFunction: NA
 * FunctionPoints: SetBmsLoadTime
 * EnvConditions: NA
 * CaseDescription: verify bms load time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBmsLoadTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetBmsLoadStartTime(BMS_LOAD_SET_START_TIME);
    int64_t bmsLoadStartTime = PerfProfile::GetInstance().GetBmsLoadStartTime();
    EXPECT_EQ(bmsLoadStartTime, BMS_LOAD_SET_START_TIME) << "bms load start time " << bmsLoadStartTime;

    PerfProfile::GetInstance().SetBmsLoadEndTime(BMS_LOAD_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t bmsLoadEndTime = PerfProfile::GetInstance().GetBmsLoadEndTime();
    EXPECT_EQ(bmsLoadEndTime, BMS_LOAD_SET_END_TIME) << "bms load end time " << bmsLoadEndTime;

    // after reset the perf profile, the bms load start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initBmsLoadStartTime = PerfProfile::GetInstance().GetBmsLoadStartTime();
    int64_t initBmsLoadEndTime = PerfProfile::GetInstance().GetBmsLoadEndTime();
    EXPECT_EQ(initBmsLoadStartTime, BMS_LOAD_INIT_START_TIME) << "bms init load start time " << initBmsLoadStartTime;
    EXPECT_EQ(initBmsLoadEndTime, BMS_LOAD_INIT_END_TIME) << "bms init load end time " << initBmsLoadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBmsLoadTime
 * SubFunction: NA
 * FunctionPoints: SetBmsLoadTime
 * EnvConditions: NA
 * CaseDescription: verify bms load time can set correct when set invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetBmsLoadTime_002, TestSize.Level0)
{
    int64_t bmsLoadStartTime = 0;
    int64_t bmsLoadEndTime = 0;

    PerfProfile::GetInstance().SetBmsLoadStartTime(INVALID_TIME);
    bmsLoadStartTime = PerfProfile::GetInstance().GetBmsLoadStartTime();
    EXPECT_EQ(bmsLoadStartTime, BMS_LOAD_INIT_START_TIME) << "bms load start time " << bmsLoadStartTime;

    PerfProfile::GetInstance().SetBmsLoadStartTime(BMS_LOAD_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetBmsLoadEndTime(INVALID_TIME);
    bmsLoadEndTime = PerfProfile::GetInstance().GetBmsLoadEndTime();
    EXPECT_EQ(bmsLoadEndTime, BMS_LOAD_SET_START_TIME) << "bms load end time " << bmsLoadEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetBmsLoadEndTime(BMS_LOAD_SET_START_TIME - 1);
    bmsLoadEndTime = PerfProfile::GetInstance().GetBmsLoadEndTime();
    EXPECT_EQ(bmsLoadEndTime, BMS_LOAD_SET_START_TIME) << "bms load end time " << bmsLoadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleScanTime
 * SubFunction: NA
 * FunctionPoints: SetBundleScanTime
 * EnvConditions: NA
 * CaseDescription: verify bundle scan time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleScanTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetBundleScanStartTime(BUNDLE_SCAN_SET_START_TIME);
    int64_t scanStartTime = PerfProfile::GetInstance().GetBundleScanStartTime();
    EXPECT_EQ(scanStartTime, BUNDLE_SCAN_SET_START_TIME) << "bundle scan start time " << scanStartTime;

    PerfProfile::GetInstance().SetBundleScanEndTime(BUNDLE_SCAN_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t scanEndTime = PerfProfile::GetInstance().GetBundleScanEndTime();
    EXPECT_EQ(scanEndTime, BUNDLE_SCAN_SET_END_TIME) << "bundle scan end time " << scanEndTime;

    // after reset the perf profile, the scan start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initScanStartTime = PerfProfile::GetInstance().GetBundleScanStartTime();
    int64_t initScanEndTime = PerfProfile::GetInstance().GetBundleScanEndTime();
    EXPECT_EQ(initScanStartTime, BUNDLE_SCAN_INIT_START_TIME) << "bundle init scan start time " << initScanStartTime;
    EXPECT_EQ(initScanEndTime, BUNDLE_SCAN_INIT_END_TIME) << "bundle init scan end time " << initScanEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleScanTime
 * SubFunction: NA
 * FunctionPoints: SetBundleScanTime
 * EnvConditions: NA
 * CaseDescription: verify bundle scan time can set correct when set invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleScanTime_002, TestSize.Level0)
{
    int64_t scanStartTime = 0;
    int64_t scanEndTime = 0;

    PerfProfile::GetInstance().SetBundleScanStartTime(INVALID_TIME);
    scanStartTime = PerfProfile::GetInstance().GetBundleScanStartTime();
    EXPECT_EQ(scanStartTime, BUNDLE_SCAN_INIT_START_TIME) << "bundle scan start time " << scanStartTime;

    PerfProfile::GetInstance().SetBundleScanStartTime(BUNDLE_SCAN_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetBundleScanEndTime(INVALID_TIME);
    scanEndTime = PerfProfile::GetInstance().GetBundleScanEndTime();
    EXPECT_EQ(scanEndTime, BUNDLE_SCAN_SET_START_TIME) << "bundle scan end time " << scanEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetBundleScanEndTime(BUNDLE_SCAN_SET_START_TIME - 1);
    scanEndTime = PerfProfile::GetInstance().GetBundleScanEndTime();
    EXPECT_EQ(scanEndTime, BUNDLE_SCAN_SET_START_TIME) << "bundle scan end time " << scanEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleDownloadTime
 * SubFunction: NA
 * FunctionPoints: SetBundleDownloadTime
 * EnvConditions: NA
 * CaseDescription: verify bundle download time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleDownloadTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetBundleDownloadStartTime(BUNDLE_DOWNLOAD_SET_START_TIME);
    int64_t downloadStartTime = PerfProfile::GetInstance().GetBundleDownloadStartTime();
    EXPECT_EQ(downloadStartTime, BUNDLE_DOWNLOAD_SET_START_TIME) << "bundle download start time " << downloadStartTime;

    PerfProfile::GetInstance().SetBundleDownloadEndTime(BUNDLE_DOWNLOAD_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t downloadEndTime = PerfProfile::GetInstance().GetBundleDownloadEndTime();
    EXPECT_EQ(downloadEndTime, BUNDLE_DOWNLOAD_SET_END_TIME) << "bundle download end time " << downloadEndTime;

    // after reset the perf profile, the download start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initDownloadStartTime = PerfProfile::GetInstance().GetBundleDownloadStartTime();
    int64_t initDownloadEndTime = PerfProfile::GetInstance().GetBundleDownloadEndTime();
    EXPECT_EQ(initDownloadStartTime, BUNDLE_DOWNLOAD_INIT_START_TIME)
        << "bundle init download start time " << initDownloadStartTime;
    EXPECT_EQ(initDownloadEndTime, BUNDLE_DOWNLOAD_INIT_END_TIME)
        << "bundle init download end time " << initDownloadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleDownloadTime
 * SubFunction: NA
 * FunctionPoints: SetBundleDownloadTime
 * EnvConditions: NA
 * CaseDescription: verify bundle download time can set correct when set invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleDownloadTime_002, TestSize.Level0)
{
    int64_t downloadStartTime = 0;
    int64_t downloadEndTime = 0;

    PerfProfile::GetInstance().SetBundleDownloadStartTime(INVALID_TIME);
    downloadStartTime = PerfProfile::GetInstance().GetBundleDownloadStartTime();
    EXPECT_EQ(downloadStartTime, BUNDLE_DOWNLOAD_INIT_START_TIME) << "bundle download start time " << downloadStartTime;

    PerfProfile::GetInstance().SetBundleDownloadStartTime(BUNDLE_DOWNLOAD_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetBundleDownloadEndTime(INVALID_TIME);
    downloadEndTime = PerfProfile::GetInstance().GetBundleDownloadEndTime();
    EXPECT_EQ(downloadEndTime, BUNDLE_DOWNLOAD_SET_START_TIME) << "bundle download end time " << downloadEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetBundleDownloadEndTime(BUNDLE_DOWNLOAD_SET_START_TIME - 1);
    downloadEndTime = PerfProfile::GetInstance().GetBundleDownloadEndTime();
    EXPECT_EQ(downloadEndTime, BUNDLE_DOWNLOAD_SET_START_TIME) << "bundle download end time " << downloadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleInstallTime
 * SubFunction: NA
 * FunctionPoints: SetBundleInstallTime
 * EnvConditions: NA
 * CaseDescription: verify bundle install time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleInstallTime_001, TestSize.Level0)
{
    int64_t expectTotalInstallTime = 0;
    int64_t realTotalInstallTime = 0;

    PerfProfile::GetInstance().SetBundleInstallStartTime(BUNDLE_INSTALL_SET_START_TIME);
    int64_t installStartTime = PerfProfile::GetInstance().GetBundleInstallStartTime();
    EXPECT_EQ(installStartTime, BUNDLE_INSTALL_SET_START_TIME) << "bundle install start time " << installStartTime;

    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t installEndTime = PerfProfile::GetInstance().GetBundleInstallEndTime();
    EXPECT_EQ(installEndTime, BUNDLE_INSTALL_SET_END_TIME) << "bundle install end time " << installEndTime;

    expectTotalInstallTime = (BUNDLE_INSTALL_SET_END_TIME - BUNDLE_INSTALL_SET_START_TIME);
    realTotalInstallTime = PerfProfile::GetInstance().GetBundleTotalInstallTime();
    EXPECT_EQ(realTotalInstallTime, expectTotalInstallTime) << "bundle total install time " << realTotalInstallTime;

    PerfProfile::GetInstance().SetBundleInstallStartTime(BUNDLE_INSTALL_SET_START_TIME_SECOND);
    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_END_TIME_SECOND);
    expectTotalInstallTime = (BUNDLE_INSTALL_SET_END_TIME - BUNDLE_INSTALL_SET_START_TIME) +
                             (BUNDLE_INSTALL_SET_END_TIME_SECOND - BUNDLE_INSTALL_SET_START_TIME_SECOND);
    realTotalInstallTime = PerfProfile::GetInstance().GetBundleTotalInstallTime();
    EXPECT_EQ(realTotalInstallTime, expectTotalInstallTime) << "bundle total install time " << realTotalInstallTime;

    // after reset the perf profile, the install start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initInstallStartTime = PerfProfile::GetInstance().GetBundleInstallStartTime();
    int64_t initInstallEndTime = PerfProfile::GetInstance().GetBundleInstallEndTime();
    int64_t initInstallTotalTime = PerfProfile::GetInstance().GetBundleInstallEndTime();
    EXPECT_EQ(initInstallStartTime, BUNDLE_INSTALL_INIT_START_TIME)
        << "bundle init install start time " << initInstallStartTime;
    EXPECT_EQ(initInstallEndTime, BUNDLE_INSTALL_INIT_END_TIME)
        << "bundle init install end time " << initInstallEndTime;
    EXPECT_EQ(initInstallTotalTime, BUNDLE_INSTALL_INIT_TOTAL_TIME)
        << "bundle init total time " << initInstallTotalTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleInstallTime
 * SubFunction: NA
 * FunctionPoints: SetBundleInstallTime
 * EnvConditions: NA
 * CaseDescription: verify bundle install time can set correct when set first invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleInstallTime_002, TestSize.Level0)
{
    int64_t installStartTime = 0;
    int64_t installEndTime = 0;
    int64_t realTotalInstallTime = 0;

    PerfProfile::GetInstance().SetBundleInstallStartTime(INVALID_TIME);
    installStartTime = PerfProfile::GetInstance().GetBundleInstallStartTime();
    EXPECT_EQ(installStartTime, BUNDLE_INSTALL_INIT_START_TIME) << "bundle install start time " << installStartTime;

    // set first install right start time and wrong end time
    PerfProfile::GetInstance().SetBundleInstallStartTime(BUNDLE_INSTALL_SET_START_TIME);
    PerfProfile::GetInstance().SetBundleInstallEndTime(INVALID_TIME);
    installEndTime = PerfProfile::GetInstance().GetBundleInstallEndTime();
    EXPECT_EQ(installEndTime, BUNDLE_INSTALL_SET_START_TIME) << "bundle install end time " << installEndTime;
    EXPECT_EQ(realTotalInstallTime, BUNDLE_INSTALL_INIT_TOTAL_TIME)
        << "bundle total install time " << realTotalInstallTime;

    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_START_TIME - 1);
    installEndTime = PerfProfile::GetInstance().GetBundleInstallEndTime();
    realTotalInstallTime = PerfProfile::GetInstance().GetBundleTotalInstallTime();
    EXPECT_EQ(installEndTime, BUNDLE_INSTALL_SET_START_TIME) << "bundle install end time " << installEndTime;
    EXPECT_EQ(realTotalInstallTime, BUNDLE_INSTALL_INIT_TOTAL_TIME)
        << "bundle total install time " << realTotalInstallTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleInstallTime
 * SubFunction: NA
 * FunctionPoints: SetBundleInstallTime
 * EnvConditions: NA
 * CaseDescription: verify bundle install time can set correct when set second invalid end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleInstallTime_003, TestSize.Level0)
{
    // set first install right start and end time
    PerfProfile::GetInstance().SetBundleInstallStartTime(BUNDLE_INSTALL_SET_START_TIME);
    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_END_TIME);

    // set second install right start time and wrong end time
    int64_t expectTotalInstallTime = 0;
    int64_t realTotalInstallTime = 0;
    PerfProfile::GetInstance().SetBundleInstallStartTime(BUNDLE_INSTALL_SET_START_TIME_SECOND);

    PerfProfile::GetInstance().SetBundleInstallEndTime(INVALID_TIME);
    expectTotalInstallTime = BUNDLE_INSTALL_SET_END_TIME - BUNDLE_INSTALL_SET_START_TIME;
    realTotalInstallTime = PerfProfile::GetInstance().GetBundleTotalInstallTime();
    EXPECT_EQ(realTotalInstallTime, expectTotalInstallTime) << "bundle total install time " << realTotalInstallTime;

    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_START_TIME_SECOND - 1);
    expectTotalInstallTime = BUNDLE_INSTALL_SET_END_TIME - BUNDLE_INSTALL_SET_START_TIME;
    realTotalInstallTime = PerfProfile::GetInstance().GetBundleTotalInstallTime();
    EXPECT_EQ(realTotalInstallTime, expectTotalInstallTime) << "bundle total install time " << realTotalInstallTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleInstallTime
 * SubFunction: NA
 * FunctionPoints: SetBundleInstallTime
 * EnvConditions: NA
 * CaseDescription: verify bundle install time can set correct when set second invalid start time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleInstallTime_004, TestSize.Level0)
{
    // set first install right start and end time
    PerfProfile::GetInstance().SetBundleInstallStartTime(BUNDLE_INSTALL_SET_START_TIME);
    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_END_TIME);

    // set second install wrong start time and right end time
    PerfProfile::GetInstance().SetBundleInstallStartTime(INVALID_TIME);
    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_END_TIME_SECOND);
    int64_t expectTotalInstallTime =
        BUNDLE_INSTALL_SET_END_TIME - BUNDLE_INSTALL_SET_START_TIME + BUNDLE_INSTALL_SET_END_TIME_SECOND;
    int64_t realTotalInstallTime = PerfProfile::GetInstance().GetBundleTotalInstallTime();
    EXPECT_EQ(realTotalInstallTime, expectTotalInstallTime) << "bundle total install time " << realTotalInstallTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleInstallTime
 * SubFunction: NA
 * FunctionPoints: SetBundleInstallTime
 * EnvConditions: NA
 * CaseDescription: verify bundle install time can set correct when set second invalid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleInstallTime_005, TestSize.Level0)
{
    // set first install right start and end time
    PerfProfile::GetInstance().SetBundleInstallStartTime(BUNDLE_INSTALL_SET_START_TIME);
    PerfProfile::GetInstance().SetBundleInstallEndTime(BUNDLE_INSTALL_SET_END_TIME);

    // set second install wrong start time and end time
    PerfProfile::GetInstance().SetBundleInstallStartTime(INVALID_TIME);
    PerfProfile::GetInstance().SetBundleInstallEndTime(INVALID_TIME);
    int64_t expectTotalInstallTime = BUNDLE_INSTALL_SET_END_TIME - BUNDLE_INSTALL_SET_START_TIME;
    int64_t realTotalInstallTime = PerfProfile::GetInstance().GetBundleTotalInstallTime();
    EXPECT_EQ(realTotalInstallTime, expectTotalInstallTime) << "bundle total install time " << realTotalInstallTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleUninstallTime
 * SubFunction: NA
 * FunctionPoints: SetBundleUninstallTime
 * EnvConditions: NA
 * CaseDescription: verify bundle uninstall time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleUninstallTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetBundleUninstallStartTime(BUNDLE_UNINSTALL_SET_START_TIME);
    int64_t uninstallStartTime = PerfProfile::GetInstance().GetBundleUninstallStartTime();
    EXPECT_EQ(uninstallStartTime, BUNDLE_UNINSTALL_SET_START_TIME)
        << "bundle uninstall start time " << uninstallStartTime;

    PerfProfile::GetInstance().SetBundleUninstallEndTime(BUNDLE_UNINSTALL_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t uninstallEndTime = PerfProfile::GetInstance().GetBundleUninstallEndTime();
    EXPECT_EQ(uninstallEndTime, BUNDLE_UNINSTALL_SET_END_TIME) << "bundle uninstall end time " << uninstallEndTime;

    // after reset the perf profile, the parse start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initUninstallStartTime = PerfProfile::GetInstance().GetBundleUninstallStartTime();
    int64_t initUninstallEndTime = PerfProfile::GetInstance().GetBundleUninstallEndTime();
    EXPECT_EQ(initUninstallStartTime, BUNDLE_UNINSTALL_INIT_START_TIME)
        << "bundle init uninstall start time " << initUninstallStartTime;
    EXPECT_EQ(initUninstallEndTime, BUNDLE_UNINSTALL_INIT_END_TIME)
        << "bundle init uninstall end time " << initUninstallEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleUninstallTime
 * SubFunction: NA
 * FunctionPoints: SetBundleUninstallTime
 * EnvConditions: NA
 * CaseDescription: verify bundle uninstall time can set correct when set the invalid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleUninstallTime_002, TestSize.Level0)
{
    int64_t uninstallStartTime = 0;
    int64_t uninstallEndTime = 0;

    PerfProfile::GetInstance().SetBundleUninstallStartTime(INVALID_TIME);
    uninstallStartTime = PerfProfile::GetInstance().GetBundleParseStartTime();
    EXPECT_EQ(uninstallStartTime, BUNDLE_UNINSTALL_INIT_START_TIME)
        << "bundle uninstall start time " << uninstallStartTime;

    PerfProfile::GetInstance().SetBundleUninstallStartTime(BUNDLE_UNINSTALL_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetBundleUninstallEndTime(INVALID_TIME);
    uninstallEndTime = PerfProfile::GetInstance().GetBundleUninstallEndTime();
    EXPECT_EQ(uninstallEndTime, BUNDLE_UNINSTALL_SET_START_TIME) << "bundle uninstall end time " << uninstallEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetBundleParseEndTime(BUNDLE_UNINSTALL_SET_START_TIME - 1);
    uninstallEndTime = PerfProfile::GetInstance().GetBundleUninstallEndTime();
    EXPECT_EQ(uninstallEndTime, BUNDLE_UNINSTALL_SET_START_TIME) << "bundle uninstall end time " << uninstallEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleParseTime
 * SubFunction: NA
 * FunctionPoints: SetBundleParseTime
 * EnvConditions: NA
 * CaseDescription: verify bundle parse time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleParseTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetBundleParseStartTime(BUNDLE_PARSE_SET_START_TIME);
    int64_t parseStartTime = PerfProfile::GetInstance().GetBundleParseStartTime();
    EXPECT_EQ(parseStartTime, BUNDLE_PARSE_SET_START_TIME) << "bundle parse start time " << parseStartTime;

    PerfProfile::GetInstance().SetBundleParseEndTime(BUNDLE_PARSE_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t parseEndTime = PerfProfile::GetInstance().GetBundleParseEndTime();
    EXPECT_EQ(parseEndTime, BUNDLE_PARSE_SET_END_TIME) << "bundle parse end time " << parseEndTime;

    // after reset the perf profile, the parse start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initParseStartTime = PerfProfile::GetInstance().GetBundleParseStartTime();
    int64_t initParseEndTime = PerfProfile::GetInstance().GetBundleParseEndTime();
    EXPECT_EQ(initParseStartTime, BUNDLE_PARSE_INIT_START_TIME)
        << "bundle init parse start time " << initParseStartTime;
    EXPECT_EQ(initParseEndTime, BUNDLE_PARSE_INIT_END_TIME) << "bundle init parse end time " << initParseEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetBundleParseTime
 * SubFunction: NA
 * FunctionPoints: SetBundleParseTime
 * EnvConditions: NA
 * CaseDescription: verify bundle parse time can set correct when set invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetBundleParseTime_002, TestSize.Level0)
{
    int64_t parseStartTime = 0;
    int64_t parseEndTime = 0;

    PerfProfile::GetInstance().SetBundleParseStartTime(INVALID_TIME);
    parseStartTime = PerfProfile::GetInstance().GetBundleParseStartTime();
    EXPECT_EQ(parseStartTime, BUNDLE_PARSE_INIT_START_TIME) << "bundle parse start time " << parseStartTime;

    PerfProfile::GetInstance().SetBundleParseStartTime(BUNDLE_PARSE_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetBundleParseEndTime(INVALID_TIME);
    parseEndTime = PerfProfile::GetInstance().GetBundleParseEndTime();
    EXPECT_EQ(parseEndTime, BUNDLE_PARSE_SET_START_TIME) << "bundle parse end time " << parseEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetBundleParseEndTime(BUNDLE_PARSE_SET_START_TIME - 1);
    parseEndTime = PerfProfile::GetInstance().GetBundleParseEndTime();
    EXPECT_EQ(parseEndTime, BUNDLE_PARSE_SET_START_TIME) << "bundle parse end time " << parseEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetAmsLoadTime
 * SubFunction: NA
 * FunctionPoints: SetAmsLoadTime
 * EnvConditions: NA
 * CaseDescription: verify ams load time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetAmsLoadTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetAmsLoadStartTime(AMS_LOAD_SET_START_TIME);
    int64_t amsLoadStartTime = PerfProfile::GetInstance().GetAmsLoadStartTime();
    EXPECT_EQ(amsLoadStartTime, AMS_LOAD_SET_START_TIME) << "ams load start time " << amsLoadStartTime;

    PerfProfile::GetInstance().SetAmsLoadEndTime(AMS_LOAD_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t amsLoadEndTime = PerfProfile::GetInstance().GetAmsLoadEndTime();
    EXPECT_EQ(amsLoadEndTime, AMS_LOAD_SET_END_TIME) << "ams load end time " << amsLoadEndTime;

    // after reset the perf profile, the ams load start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initAmsLoadStartTime = PerfProfile::GetInstance().GetAmsLoadStartTime();
    int64_t initAmsLoadEndTime = PerfProfile::GetInstance().GetAmsLoadEndTime();
    EXPECT_EQ(initAmsLoadStartTime, AMS_LOAD_INIT_START_TIME) << "ams init load start time " << initAmsLoadStartTime;
    EXPECT_EQ(initAmsLoadEndTime, AMS_LOAD_INIT_END_TIME) << "ams init load end time " << initAmsLoadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetAmsLoadTime
 * SubFunction: NA
 * FunctionPoints: SetAmsLoadTime
 * EnvConditions: NA
 * CaseDescription: verify ams load time can set correct when set invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetAmsLoadTime_002, TestSize.Level0)
{
    int64_t amsLoadStartTime = 0;
    int64_t amsLoadEndTime = 0;

    PerfProfile::GetInstance().SetAmsLoadStartTime(INVALID_TIME);
    amsLoadStartTime = PerfProfile::GetInstance().GetAmsLoadStartTime();
    EXPECT_EQ(amsLoadStartTime, AMS_LOAD_INIT_START_TIME) << "ams load start time " << amsLoadStartTime;

    PerfProfile::GetInstance().SetAmsLoadStartTime(AMS_LOAD_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetAmsLoadEndTime(INVALID_TIME);
    amsLoadEndTime = PerfProfile::GetInstance().GetAmsLoadEndTime();
    EXPECT_EQ(amsLoadEndTime, AMS_LOAD_SET_START_TIME) << "ams load end time " << amsLoadEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetAmsLoadEndTime(AMS_LOAD_SET_START_TIME - 1);
    amsLoadEndTime = PerfProfile::GetInstance().GetAmsLoadEndTime();
    EXPECT_EQ(amsLoadEndTime, AMS_LOAD_SET_START_TIME) << "ams load end time " << amsLoadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetAbilityLoadTime
 * SubFunction: NA
 * FunctionPoints: SetAbilityLoadTime
 * EnvConditions: NA
 * CaseDescription: verify ability load time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetAbilityLoadTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetAbilityLoadStartTime(ABILITY_LOAD_SET_START_TIME);
    int64_t loadStartTime = PerfProfile::GetInstance().GetAbilityLoadStartTime();
    EXPECT_EQ(loadStartTime, ABILITY_LOAD_SET_START_TIME) << "ability load start time " << loadStartTime;

    PerfProfile::GetInstance().SetAbilityLoadEndTime(ABILITY_LOAD_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t loadEndTime = PerfProfile::GetInstance().GetAbilityLoadEndTime();
    EXPECT_EQ(loadEndTime, ABILITY_LOAD_SET_END_TIME) << "ability load end time " << loadEndTime;

    // after reset the perf profile, the ability load start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initLoadStartTime = PerfProfile::GetInstance().GetAbilityLoadStartTime();
    int64_t initLoadEndTime = PerfProfile::GetInstance().GetAbilityLoadEndTime();
    EXPECT_EQ(initLoadStartTime, ABILITY_LOAD_INIT_START_TIME) << "ability init load start time " << initLoadStartTime;
    EXPECT_EQ(initLoadEndTime, ABILITY_LOAD_INIT_END_TIME) << "ability init load end time " << initLoadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetAbilityLoadTime
 * SubFunction: NA
 * FunctionPoints: SetAbilityLoadTime
 * EnvConditions: NA
 * CaseDescription: verify ability load time can set correct when set invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetAbilityLoadTime_002, TestSize.Level0)
{
    int64_t loadStartTime = 0;
    int64_t loadEndTime = 0;

    PerfProfile::GetInstance().SetAbilityLoadStartTime(INVALID_TIME);
    loadStartTime = PerfProfile::GetInstance().GetAbilityLoadStartTime();
    EXPECT_EQ(loadStartTime, ABILITY_LOAD_INIT_START_TIME) << "ability load start time " << loadStartTime;

    PerfProfile::GetInstance().SetAbilityLoadStartTime(ABILITY_LOAD_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetAbilityLoadEndTime(INVALID_TIME);
    loadEndTime = PerfProfile::GetInstance().GetAbilityLoadEndTime();
    EXPECT_EQ(loadEndTime, ABILITY_LOAD_SET_START_TIME) << "ability load end time " << loadEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetAmsLoadEndTime(ABILITY_LOAD_SET_START_TIME - 1);
    loadEndTime = PerfProfile::GetInstance().GetAbilityLoadEndTime();
    EXPECT_EQ(loadEndTime, ABILITY_LOAD_SET_START_TIME) << "ability load end time " << loadEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetAppForkTime
 * SubFunction: NA
 * FunctionPoints: SetAppForkTime
 * EnvConditions: NA
 * CaseDescription: verify app fork time can set correct when set the valid start and end time
 */
HWTEST_F(CommonPerfProfileTest, SetAppForkTime_001, TestSize.Level0)
{
    PerfProfile::GetInstance().SetAppForkStartTime(APP_FORK_SET_START_TIME);
    int64_t forkStartTime = PerfProfile::GetInstance().GetAppForkStartTime();
    EXPECT_EQ(forkStartTime, APP_FORK_SET_START_TIME) << "app fork start time " << forkStartTime;

    PerfProfile::GetInstance().SetAppForkEndTime(APP_FORK_SET_END_TIME);
    PerfProfile::GetInstance().Dump();
    int64_t forkEndTime = PerfProfile::GetInstance().GetAppForkEndTime();
    EXPECT_EQ(forkEndTime, APP_FORK_SET_END_TIME) << "app fork end time " << forkEndTime;

    // after reset the perf profile, the ability load start and end time should be zero
    PerfProfile::GetInstance().Reset();
    int64_t initForkStartTime = PerfProfile::GetInstance().GetAppForkStartTime();
    int64_t initForkEndTime = PerfProfile::GetInstance().GetAppForkEndTime();
    EXPECT_EQ(initForkStartTime, APP_FORK_INIT_START_TIME) << "app init fork start time " << initForkStartTime;
    EXPECT_EQ(initForkEndTime, APP_FORK_INIT_END_TIME) << "app init fork end time " << initForkEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: SetAppForkTime
 * SubFunction: NA
 * FunctionPoints: SetAppForkTime
 * EnvConditions: NA
 * CaseDescription: verify app fork time can set correct when set invalid start or end time
 */
HWTEST_F(CommonPerfProfileTest, SetAppForkTime_002, TestSize.Level0)
{
    int64_t forkStartTime = 0;
    int64_t forkEndTime = 0;

    PerfProfile::GetInstance().SetAppForkStartTime(INVALID_TIME);
    forkStartTime = PerfProfile::GetInstance().GetAppForkStartTime();
    EXPECT_EQ(forkStartTime, APP_FORK_INIT_START_TIME) << "app fork start time " << forkStartTime;

    PerfProfile::GetInstance().SetAppForkStartTime(APP_FORK_SET_START_TIME);

    // set the invalid end time less than zero
    PerfProfile::GetInstance().SetAppForkEndTime(INVALID_TIME);
    forkEndTime = PerfProfile::GetInstance().GetAppForkEndTime();
    EXPECT_EQ(forkEndTime, APP_FORK_SET_START_TIME) << "app fork end time " << forkEndTime;

    // set the invalid end time more than zero and less then start time
    PerfProfile::GetInstance().SetAppForkEndTime(APP_FORK_SET_START_TIME - 1);
    forkEndTime = PerfProfile::GetInstance().GetAppForkEndTime();
    EXPECT_EQ(forkEndTime, ABILITY_LOAD_SET_START_TIME) << "app fork end time " << forkEndTime;
}

/*
 * Feature: CommonPerfProfileTest
 * Function: PerfProfileEnable
 * SubFunction: NA
 * FunctionPoints: PerfProfileEnable
 * EnvConditions: NA
 * CaseDescription: verify perf profile dump can set enable or disable
 */
HWTEST_F(CommonPerfProfileTest, PerfProfileEnable_001, TestSize.Level0)
{
    bool profileFlag = false;
    PerfProfile::GetInstance().SetPerfProfileEnabled(false);
    profileFlag = PerfProfile::GetInstance().GetPerfProfileEnabled();
    PerfProfile::GetInstance().Dump();
    ASSERT_FALSE(profileFlag);

    PerfProfile::GetInstance().SetPerfProfileEnabled(true);
    profileFlag = PerfProfile::GetInstance().GetPerfProfileEnabled();
    PerfProfile::GetInstance().Dump();
    ASSERT_TRUE(profileFlag);

    // after reset the perf profile, the perf profile flag should be true
    PerfProfile::GetInstance().SetPerfProfileEnabled(false);
    PerfProfile::GetInstance().Reset();
    profileFlag = PerfProfile::GetInstance().GetPerfProfileEnabled();
    ASSERT_TRUE(profileFlag);
}