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
#include "mock_wifi_manager.h"
#include "mock_wifi_settings.h"
#include "mock_wifi_sta_hal_interface.h"
#include "mock_scan_service.h"
#include "scan_state_machine.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::StrEq;
using ::testing::TypedEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
constexpr int FREQ_2_DOT_4_GHZ_VALUE = 2410;
constexpr int FREQ_5_GHZ_VALUE = 5010;

class ScanStateMachineTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetSupportHwPnoFlag()).Times(AtLeast(0));
        pScanStateMachine = std::make_unique<ScanStateMachine>();
        pScanStateMachine->InitScanStateMachine();
        pScanService = std::make_unique<MockScanService>();
        pScanStateMachine->EnrollScanStatusListener(
            std::bind(&MockScanService::HandleScanStatusReport, pScanService.get(), std::placeholders::_1));
    }
    void TearDown() override
    {
        pScanStateMachine.reset();
        pScanService.reset();
    }

    std::unique_ptr<MockScanService> pScanService;
    std::unique_ptr<ScanStateMachine> pScanStateMachine;

public:
    void InitGoInStateTest()
    {
        pScanStateMachine->initState->GoInState();
    }

    void InitGoOutStateTest()
    {
        pScanStateMachine->initState->GoOutState();
    }

    void InitExeMsgSuccess1()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_PREPARE);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess2()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_FINISH);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess3()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_COMMON_SCAN);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess4()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess5()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_STOP_PNO_SCAN);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess6()
    {
        InternalMessage msg;
        msg.SetMessageName(HARDWARE_LOAD_EVENT);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess7()
    {
        InternalMessage msg;
        msg.SetMessageName(HARDWARE_UNLOAD_EVENT);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess8()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_STOP_COMMON_SCAN);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess9()
    {
        InternalMessage msg;
        msg.SetMessageName(SYSTEM_SCAN_TIMER);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgSuccess10()
    {
        InternalMessage msg;
        msg.SetMessageName(SCAN_INNER_EVENT_INVALID);
        pScanStateMachine->initState->ExecuteStateMsg(&msg);
    }

    void InitExeMsgFail()
    {
        pScanStateMachine->initState->ExecuteStateMsg(nullptr);
    }

    void LoadDriverTest()
    {
        pScanStateMachine->initState->LoadDriver();
    }

    void UnLoadDriverTest()
    {
        pScanStateMachine->initState->UnLoadDriver();
    }

    void HardwareReadyGoInStateTest()
    {
        pScanStateMachine->hardwareReadyState->GoInState();
    }

    void HardwareReadyGoOutStateMsgTest()
    {
        pScanStateMachine->hardwareReadyState->GoOutState();
    }

    void HardwareReadyExeMsgSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(CMD_START_COMMON_SCAN);
        pScanStateMachine->hardwareReadyState->ExecuteStateMsg(&msg);
    }

    void HardwareReadyExeMsgSuccess2()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->hardwareReadyState->ExecuteStateMsg(&msg);
    }

    void HardwareReadyExeMsgFail()
    {
        pScanStateMachine->hardwareReadyState->ExecuteStateMsg(nullptr);
    }

    void CommonScanGoInStateTest()
    {
        pScanStateMachine->commonScanState->GoInState();
    }

    void CommonScanGoOutStateTest()
    {
        pScanStateMachine->commonScanState->GoOutState();
    }

    void CommonScanExeMsgSuccess()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_STOP_COMMON_SCAN);
        pScanStateMachine->commonScanState->ExecuteStateMsg(&msg);
    }

    void CommonScanExeMsgFail()
    {
        pScanStateMachine->commonScanState->ExecuteStateMsg(nullptr);
    }

    void CommonScanUnworkedGoInStateTest()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->commonScanUnworkedState->GoInState();
    }

    void CommonScanUnworkedGoOutStateTest()
    {
        pScanStateMachine->commonScanUnworkedState->GoOutState();
    }

    void CommonScanUnworkedExeMsgSuccess1()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_COMMON_SCAN);
        pScanStateMachine->commonScanUnworkedState->ExecuteStateMsg(&msg);
    }

    void CommonScanUnworkedExeMsgSuccess2()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->commonScanUnworkedState->ExecuteStateMsg(&msg);
    }

    void CommonScanUnworkedExeMsgSuccess3()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_PREPARE);
        pScanStateMachine->commonScanUnworkedState->ExecuteStateMsg(&msg);
    }

    void CommonScanUnworkedExeMsgFail()
    {
        pScanStateMachine->commonScanUnworkedState->ExecuteStateMsg(nullptr);
    }

    void CommonScanningGoInStateTest()
    {
        pScanStateMachine->commonScanningState->GoInState();
    }

    void CommonScanningGoOutStateTest()
    {
        pScanStateMachine->commonScanningState->GoOutState();
    }

    void CommonScanningExeMsgSuccess1()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_COMMON_SCAN);
        pScanStateMachine->commonScanningState->ExecuteStateMsg(&msg);
    }

    void CommonScanningExeMsgSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(SCAN_RESULT_EVENT);
        pScanStateMachine->commonScanningState->ExecuteStateMsg(&msg);
    }

    void CommonScanningExeMsgSuccess3()
    {
        InternalMessage msg;
        msg.SetMessageName(SCAN_FAILED_EVENT);
        pScanStateMachine->commonScanningState->ExecuteStateMsg(&msg);
    }

    void CommonScanningExeMsgSuccess4()
    {
        InternalMessage msg;
        msg.SetMessageName(WAIT_SCAN_RESULT_TIMER);
        pScanStateMachine->commonScanningState->ExecuteStateMsg(&msg);
    }

    void CommonScanningExeMsgSuccess5()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->commonScanningState->ExecuteStateMsg(&msg);
    }

    void CommonScanningExeMsgSuccess6()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_FINISH);
        pScanStateMachine->commonScanningState->ExecuteStateMsg(&msg);
    }

    void CommonScanningExeMsgFail()
    {
        pScanStateMachine->commonScanningState->ExecuteStateMsg(nullptr);
    }

    void PnoScanGoInStateTest()
    {
        pScanStateMachine->pnoScanState->GoInState();
    }

    void PnoScanGoOutStateTest()
    {
        pScanStateMachine->pnoScanState->GoOutState();
    }

    void PnoScanExeMsgSuccess()
    {
        InternalMessage msg;
        pScanStateMachine->pnoScanState->ExecuteStateMsg(&msg);
    }

    void PnoScanExeMsgFail()
    {
        pScanStateMachine->pnoScanState->ExecuteStateMsg(nullptr);
    }

    void PnoScanHardwareGoInStateTest()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartPnoScan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->pnoScanHardwareState->GoInState();
    }

    void PnoScanHardwareGoOutStateTest()
    {
        pScanStateMachine->pnoScanHardwareState->GoOutState();
    }

    void PnoScanHardwareExeMsgSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartPnoScan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->pnoScanHardwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanHardwareExeMsgSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(CMD_STOP_PNO_SCAN);
        pScanStateMachine->pnoScanHardwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanHardwareExeMsgSuccess3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartPnoScan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(CMD_RESTART_PNO_SCAN);
        pScanStateMachine->pnoScanHardwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanHardwareExeMsgSuccess4()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(PNO_SCAN_RESULT_EVENT);
        pScanStateMachine->pnoScanHardwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanHardwareExeMsgSuccess5()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_COMMON_SCAN);
        pScanStateMachine->pnoScanHardwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanHardwareExeMsgSuccess6()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_FINISH);
        pScanStateMachine->pnoScanHardwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanHardwareExeMsgFail()
    {
        pScanStateMachine->pnoScanHardwareState->ExecuteStateMsg(nullptr);
    }

    void CommonScanAfterPnoGoInStateTest()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->commonScanAfterPnoState->GoInState();
    }

    void CommonScanAfterPnoGoOutStateTest()
    {
        pScanStateMachine->commonScanAfterPnoState->GoOutState();
    }

    void CommonScanAfterPnoExeMsgSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(SCAN_RESULT_EVENT);
        pScanStateMachine->commonScanAfterPnoState->ExecuteStateMsg(&msg);
    }

    void CommonScanAfterPnoExeMsgSuccess2()
    {
        InternalMessage msg;
        msg.SetMessageName(SCAN_FAILED_EVENT);
        pScanStateMachine->commonScanAfterPnoState->ExecuteStateMsg(&msg);
    }

    void CommonScanAfterPnoExeMsgSuccess3()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->commonScanAfterPnoState->ExecuteStateMsg(&msg);
    }

    void CommonScanAfterPnoExeMsgSuccess4()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_COMMON_SCAN);
        pScanStateMachine->commonScanAfterPnoState->ExecuteStateMsg(&msg);
    }

    void CommonScanAfterPnoExeMsgSuccess5()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_RESTART_PNO_SCAN);
        pScanStateMachine->commonScanAfterPnoState->ExecuteStateMsg(&msg);
    }

    void CommonScanAfterPnoExeMsgSuccess6()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_FINISH);
        pScanStateMachine->commonScanAfterPnoState->ExecuteStateMsg(&msg);
    }

    void CommonScanAfterPnoExeMsgFail()
    {
        pScanStateMachine->commonScanAfterPnoState->ExecuteStateMsg(nullptr);
    }

    void PnoScanSoftwareGoInStateTest()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->pnoScanSoftwareState->GoInState();
    }

    void PnoScanSoftwareGoOutStateTest()
    {
        pScanStateMachine->pnoScanSoftwareState->GoOutState();
    }

    void PnoScanSoftwareExeMsgSuccess1()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_STOP_PNO_SCAN);
        pScanStateMachine->pnoScanSoftwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanSoftwareExeMsgSuccess2()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_FINISH);
        pScanStateMachine->pnoScanSoftwareState->ExecuteStateMsg(&msg);
    }

    void PnoScanSoftwareExeMsgFail()
    {
        pScanStateMachine->pnoScanSoftwareState->ExecuteStateMsg(nullptr);
    }

    void PnoSwScanFreeGoInStateTest()
    {
        pScanStateMachine->pnoSwScanFreeState->GoInState();
    }

    void PnoSwScanFreeGoOutStateTest()
    {
        pScanStateMachine->pnoSwScanFreeState->GoOutState();
    }

    void PnoSwScanFreeExeMsgSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->pnoSwScanFreeState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanFreeExeMsgSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(CMD_RESTART_PNO_SCAN);
        pScanStateMachine->pnoSwScanFreeState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanFreeExeMsgSuccess3()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_COMMON_SCAN);
        pScanStateMachine->pnoSwScanFreeState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanFreeExeMsgSuccess4()
    {
        InternalMessage msg;
        msg.SetMessageName(SOFTWARE_PNO_SCAN_TIMER);
        pScanStateMachine->pnoSwScanFreeState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanFreeExeMsgSuccess5()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_SCAN_FINISH);
        pScanStateMachine->pnoSwScanFreeState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanFreeExeMsgFail()
    {
        pScanStateMachine->pnoSwScanFreeState->ExecuteStateMsg(nullptr);
    }

    void PnoSwScanningGoInStateTest()
    {
        pScanStateMachine->pnoSwScanningState->GoInState();
    }

    void PnoSwScanningGoOutStateTest()
    {
        pScanStateMachine->pnoSwScanningState->GoOutState();
    }

    void PnoSwScanningExeMsgSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        msg.SetMessageName(SCAN_RESULT_EVENT);
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanningExeMsgSuccess2()
    {
        InternalMessage msg;
        msg.SetMessageName(SCAN_FAILED_EVENT);
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanningExeMsgSuccess3()
    {
        InternalMessage msg;
        msg.SetMessageName(WAIT_SCAN_RESULT_TIMER);
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanningExeMsgSuccess4()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_START_PNO_SCAN);
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanningExeMsgSuccess5()
    {
        InternalMessage msg;
        msg.SetMessageName(CMD_RESTART_PNO_SCAN);
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanningExeMsgSuccess6()
    {
        InternalMessage msg;
        msg.SetMessageName(SOFTWARE_PNO_SCAN_TIMER);
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanningExeMsgSuccess7()
    {
        InternalMessage msg;
        msg.SetMessageName(SCAN_INNER_EVENT_INVALID);
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(&msg);
    }

    void PnoSwScanningExeMsgFail()
    {
        pScanStateMachine->pnoSwScanningState->ExecuteStateMsg(nullptr);
    }

    void CommonScanRequestProcessTest()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        pScanStateMachine->CommonScanRequestProcess(&msg);
    }

    void GetCommonScanRequestInfoTest1()
    {
        InternalMessage interMessage;
        int requestIndex = 0;
        InterScanConfig scanConfig;
        pScanStateMachine->GetCommonScanRequestInfo(&interMessage, requestIndex, scanConfig);
    }

    void GetCommonScanRequestInfoTest2()
    {
        int requestIndex = 0;
        InterScanConfig scanConfig;
        pScanStateMachine->GetCommonScanRequestInfo(nullptr, requestIndex, scanConfig);
    }

    void GetCommonScanConfigSuccess()
    {
        InternalMessage msg;
        msg.AddIntMessageBody(1);
        msg.AddStringMessageBody("hmwifi1");
        msg.AddIntMessageBody(1);
        msg.AddStringMessageBody("hmwifi2");
        InterScanConfig scanConfig;
        pScanStateMachine->GetCommonScanConfig(&msg, scanConfig);
    }

    void GetCommonScanConfigFail1()
    {
        InterScanConfig scanConfig;
        pScanStateMachine->GetCommonScanConfig(nullptr, scanConfig);
    }

    void GetCommonScanConfigFail2()
    {
        InternalMessage msg;
        msg.AddIntMessageBody(1);
        InterScanConfig scanConfig;
        pScanStateMachine->GetCommonScanConfig(&msg, scanConfig);
    }

    void GetCommonScanConfigFail3()
    {
        InternalMessage msg;
        msg.AddIntMessageBody(1);
        msg.AddStringMessageBody("hmwifi1");
        msg.AddIntMessageBody(1);
        InterScanConfig scanConfig;
        pScanStateMachine->GetCommonScanConfig(&msg, scanConfig);
    }

    void StartNewCommonScanTest1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InterScanConfig interScanConfig;
        pScanStateMachine->waitingScans.emplace(0, interScanConfig);
        pScanStateMachine->StartNewCommonScan();
    }

    void StartNewCommonScanTest2()
    {
        pScanStateMachine->StartNewCommonScan();
    }

    void ClearRunningScanSettingsTest()
    {
        pScanStateMachine->ClearRunningScanSettings();
    }

    void StartSingleCommonScanSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        WifiScanParam scanParam;
        pScanStateMachine->StartSingleCommonScan(scanParam);
    }

    void StartSingleCommonScanFail()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        WifiScanParam scanParam;
        pScanStateMachine->StartSingleCommonScan(scanParam);
    }

    void CommonScanWhenRunningSuccess()
    {
        InternalMessage msg;
        pScanStateMachine->CommonScanWhenRunning(&msg);
    }

    void CommonScanWhenRunningFail()
    {
        pScanStateMachine->CommonScanWhenRunning(nullptr);
    }

    void ActiveCoverNewScanSuccess()
    {
        pScanStateMachine->runningFullScanFlag = true;
        InterScanConfig interScanConfig;
        interScanConfig.scanStyle = SCAN_TYPE_HIGH_ACCURACY;
        pScanStateMachine->ActiveCoverNewScan(interScanConfig);
    }

    void ActiveCoverNewScanFail()
    {
        pScanStateMachine->runningFullScanFlag = true;
        InterScanConfig interScanConfig;
        interScanConfig.scanStyle = SCAN_TYPE_INVALID;
        pScanStateMachine->ActiveCoverNewScan(interScanConfig);
    }

    void CommonScanInfoProcessTest()
    {
        pScanStateMachine->CommonScanInfoProcess();
    }

    void ReportStatusChangeTest()
    {
        ScanStatus status = SCAN_STARTED_STATUS;
        pScanStateMachine->ReportStatusChange(status);
    }

    void ReportScanInnerEventTest()
    {
        ScanInnerEventType innerEvent = WAIT_SCAN_RESULT_TIMER;
        pScanStateMachine->ReportScanInnerEvent(innerEvent);
    }

    void ReportCommonScanFailedTest()
    {
        pScanStateMachine->ReportCommonScanFailed(0);
    }

    void ReportCommonScanFailedAndClearTest1()
    {
        pScanStateMachine->ReportCommonScanFailedAndClear(true);
    }

    void ReportCommonScanFailedAndClearTest2()
    {
        pScanStateMachine->ReportCommonScanFailedAndClear(false);
    }

    void GetRunningIndexListTest()
    {
        InterScanConfig interScanConfig;
        pScanStateMachine->runningScans.emplace(0, interScanConfig);
        std::vector<int> runningIndexList;
        pScanStateMachine->GetRunningIndexList(runningIndexList);
    }

    void GetWaitingIndexListTest()
    {
        InterScanConfig interScanConfig;
        pScanStateMachine->waitingScans.emplace(0, interScanConfig);
        std::vector<int> waitingIndexList;
        pScanStateMachine->GetRunningIndexList(waitingIndexList);
    }

    void VerifyScanStyleSuccess()
    {
        int scanStyle = SCAN_TYPE_HIGH_ACCURACY;
        EXPECT_EQ(true, pScanStateMachine->VerifyScanStyle(scanStyle));
    }

    void VerifyScanStyleFail()
    {
        int scanStyle = SCAN_TYPE_INVALID;
        EXPECT_EQ(false, pScanStateMachine->VerifyScanStyle(scanStyle));
    }

    void ActiveScanStyleTest1()
    {
        pScanStateMachine->runningScanSettings.scanStyle = SCAN_TYPE_LOW_SPAN;
        int scanStyle = SCAN_TYPE_LOW_POWER;
        pScanStateMachine->ActiveScanStyle(scanStyle);
    }

    void ActiveScanStyleTest2()
    {
        pScanStateMachine->runningScanSettings.scanStyle = SCAN_TYPE_HIGH_ACCURACY;
        int scanStyle = SCAN_TYPE_LOW_POWER;
        pScanStateMachine->ActiveScanStyle(scanStyle);
    }

    void ActiveScanStyleTest3()
    {
        pScanStateMachine->runningScanSettings.scanStyle = SCAN_TYPE_INVALID;
        int scanStyle = SCAN_TYPE_LOW_POWER;
        pScanStateMachine->ActiveScanStyle(scanStyle);
    }

    void MergeScanStyleTest1()
    {
        int currentScanStyle = SCAN_TYPE_LOW_SPAN;
        int newScanStyle = SCAN_TYPE_HIGH_ACCURACY;
        pScanStateMachine->MergeScanStyle(currentScanStyle, newScanStyle);
    }

    void MergeScanStyleTest2()
    {
        int currentScanStyle = SCAN_TYPE_HIGH_ACCURACY;
        int newScanStyle = SCAN_TYPE_HIGH_ACCURACY;
        pScanStateMachine->MergeScanStyle(currentScanStyle, newScanStyle);
    }

    void MergeScanStyleTest3()
    {
        int currentScanStyle = SCAN_TYPE_INVALID;
        int newScanStyle = SCAN_TYPE_HIGH_ACCURACY;
        pScanStateMachine->MergeScanStyle(currentScanStyle, newScanStyle);
    }

    void RemoveCommonScanRequestTest()
    {
        InterScanConfig interScanConfig;
        pScanStateMachine->runningScans.emplace(0, interScanConfig);
        pScanStateMachine->waitingScans.emplace(0, interScanConfig);
        pScanStateMachine->RemoveCommonScanRequest(0);
    }

    void PnoScanRequestProcessTest()
    {
        InternalMessage msg;
        pScanStateMachine->PnoScanRequestProcess(&msg);
    }

    void ContinuePnoScanProcess()
    {
        pScanStateMachine->pnoConfigStoredFlag = true;
        pScanStateMachine->ContinuePnoScanProcess();
    }

    void PnoScanHardwareProcessTest1()
    {
        pScanStateMachine->runningHwPnoFlag = false;
        InternalMessage msg;
        pScanStateMachine->PnoScanHardwareProcess(&msg);
    }

    void PnoScanHardwareProcessTest2()
    {
        pScanStateMachine->runningHwPnoFlag = true;
        InternalMessage msg;
        pScanStateMachine->PnoScanHardwareProcess(&msg);
    }

    void StartPnoScanHardwareSuccess1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartPnoScan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->runningHwPnoFlag = false;
        pScanStateMachine->pnoConfigStoredFlag = true;
        EXPECT_EQ(true, pScanStateMachine->StartPnoScanHardware());
    }

    void StartPnoScanHardwareSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartPnoScan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->runningHwPnoFlag = true;
        pScanStateMachine->pnoConfigStoredFlag = true;
        EXPECT_EQ(true, pScanStateMachine->StartPnoScanHardware());
    }

    void StartPnoScanHardwareSuccess3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartPnoScan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->runningHwPnoFlag = false;
        pScanStateMachine->pnoConfigStoredFlag = false;
        EXPECT_EQ(true, pScanStateMachine->StartPnoScanHardware());
    }

    void StopPnoScanHardwareTest1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->supportHwPnoFlag = true;
        pScanStateMachine->runningHwPnoFlag = true;
        pScanStateMachine->StopPnoScanHardware();
    }

    void StopPnoScanHardwareTest2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->supportHwPnoFlag = false;
        pScanStateMachine->runningHwPnoFlag = true;
        pScanStateMachine->StopPnoScanHardware();
    }

    void StopPnoScanHardwareTest3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pScanStateMachine->supportHwPnoFlag = true;
        pScanStateMachine->runningHwPnoFlag = true;
        pScanStateMachine->StopPnoScanHardware();
    }

    void UpdatePnoScanRequestTest()
    {
        InternalMessage msg;
        pScanStateMachine->UpdatePnoScanRequest(&msg);
    }

    void GetPnoScanRequestInfoTest1()
    {
        InternalMessage msg;
        pScanStateMachine->GetPnoScanRequestInfo(&msg);
    }

    void GetPnoScanRequestInfoTest2()
    {
        pScanStateMachine->GetPnoScanRequestInfo(nullptr);
    }

    void GetPnoScanConfigSuccess()
    {
        InternalMessage msg;
        msg.AddIntMessageBody(0);
        msg.AddIntMessageBody(0);
        msg.AddIntMessageBody(0);
        msg.AddIntMessageBody(1);
        msg.AddStringMessageBody("hmwifi1");
        msg.AddIntMessageBody(0);
        msg.AddStringMessageBody("hmwifi2");
        msg.AddIntMessageBody(1);
        msg.AddIntMessageBody(FREQ_2_DOT_4_GHZ_VALUE);
        PnoScanConfig pnoScanConfig;
        EXPECT_EQ(true, pScanStateMachine->GetPnoScanConfig(&msg, pnoScanConfig));
    }

    void GetPnoScanConfigFail1()
    {
        PnoScanConfig pnoScanConfig;
        EXPECT_EQ(false, pScanStateMachine->GetPnoScanConfig(nullptr, pnoScanConfig));
    }

    void GetPnoScanConfigFail2()
    {
        InternalMessage msg;
        msg.AddIntMessageBody(0);
        msg.AddIntMessageBody(0);
        msg.AddIntMessageBody(0);
        msg.AddIntMessageBody(1);
        PnoScanConfig pnoScanConfig;
        EXPECT_EQ(false, pScanStateMachine->GetPnoScanConfig(&msg, pnoScanConfig));
    }

    void HwPnoScanInfoProcessTest1()
    {
        std::vector<InterScanInfo> scanInfos = { InterScanInfo() };
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(scanInfos), Return(WIFI_IDL_OPT_OK)));
        pScanStateMachine->runningHwPnoFlag = true;
        pScanStateMachine->HwPnoScanInfoProcess();
    }

    void HwPnoScanInfoProcessTest2()
    {
        std::vector<InterScanInfo> scanInfos = { InterScanInfo() };
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(scanInfos), Return(WIFI_IDL_OPT_OK)));
        pScanStateMachine->runningHwPnoFlag = false;
        pScanStateMachine->HwPnoScanInfoProcess();
    }

    void HwPnoScanInfoProcessTest3()
    {
        std::vector<InterScanInfo> scanInfos = { InterScanInfo() };
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(scanInfos), Return(WIFI_IDL_OPT_FAILED)));
        pScanStateMachine->runningHwPnoFlag = true;
        pScanStateMachine->HwPnoScanInfoProcess();
    }

    void ReportPnoScanInfosTest()
    {
        std::vector<InterScanInfo> scanInfos = { InterScanInfo() };
        pScanStateMachine->ReportPnoScanInfos(scanInfos);
    }

    void NeedCommonScanAfterPnoTest()
    {
        std::vector<InterScanInfo> scanInfos = { InterScanInfo() };
        EXPECT_EQ(false, pScanStateMachine->NeedCommonScanAfterPno(scanInfos));
    }

    void CommonScanAfterPnoProcessTest1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->CommonScanAfterPnoProcess();
    }

    void CommonScanAfterPnoProcessTest2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pScanStateMachine->CommonScanAfterPnoProcess();
    }

    void CommonScanAfterPnoResultTest1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->CommonScanAfterPnoResult();
    }

    void CommonScanAfterPnoResultTest2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pScanStateMachine->CommonScanAfterPnoResult();
    }

    void PnoScanFailedProcessSuccessTest()
    {
        pScanStateMachine->PnoScanFailedProcess();
    }

    void ClearPnoScanConfigTest()
    {
        pScanStateMachine->ClearPnoScanConfig();
    }

    void GetScanInfosSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        std::vector<InterScanInfo> scanInfos;
        EXPECT_EQ(true, pScanStateMachine->GetScanInfos(scanInfos));
    }

    void GetScanInfosFail()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        std::vector<InterScanInfo> scanInfos;
        EXPECT_EQ(false, pScanStateMachine->GetScanInfos(scanInfos));
    }

    void GetSecurityTypeAndBandTest()
    {
        std::vector<InterScanInfo> scanInfos;
        InterScanInfo interScanInfo;
        interScanInfo.frequency = FREQ_2_DOT_4_GHZ_VALUE;
        interScanInfo.capabilities = "WAPI-PSK";
        scanInfos.push_back(interScanInfo);
        interScanInfo.frequency = FREQ_5_GHZ_VALUE;
        interScanInfo.capabilities = "PSK";
        scanInfos.push_back(interScanInfo);
        interScanInfo.frequency = FREQ_5_GHZ_VALUE;
        interScanInfo.capabilities = "WEP";
        scanInfos.push_back(interScanInfo);
        interScanInfo.frequency = FREQ_5_GHZ_VALUE;
        interScanInfo.capabilities = "EAP_SUITE_B_192";
        scanInfos.push_back(interScanInfo);
        interScanInfo.frequency = FREQ_5_GHZ_VALUE;
        interScanInfo.capabilities = "EAP";
        scanInfos.push_back(interScanInfo);
        interScanInfo.frequency = FREQ_5_GHZ_VALUE;
        interScanInfo.capabilities = "SAE";
        scanInfos.push_back(interScanInfo);
        interScanInfo.frequency = FREQ_5_GHZ_VALUE;
        interScanInfo.capabilities = "OWE";
        scanInfos.push_back(interScanInfo);
        interScanInfo.frequency = FREQ_5_GHZ_VALUE;
        interScanInfo.capabilities = "CERT";
        scanInfos.push_back(interScanInfo);
        pScanStateMachine->GetSecurityTypeAndBand(scanInfos);
    }

    void StartNewSoftwareScanTest()
    {
        pScanStateMachine->StartNewSoftwareScan();
    }

    void RepeatStartCommonScanTest1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->pnoConfigStoredFlag = true;
        EXPECT_EQ(true, pScanStateMachine->RepeatStartCommonScan());
    }

    void RepeatStartCommonScanTest2()
    {
        pScanStateMachine->pnoConfigStoredFlag = false;
        EXPECT_EQ(false, pScanStateMachine->RepeatStartCommonScan());
    }

    void RepeatStartCommonScanTest3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pScanStateMachine->pnoConfigStoredFlag = true;
        EXPECT_EQ(false, pScanStateMachine->RepeatStartCommonScan());
    }

    void StopPnoScanSoftwareTest()
    {
        pScanStateMachine->StopPnoScanSoftware();
    }

    void PnoScanSoftwareProcessTest1()
    {
        InternalMessage msg;
        pScanStateMachine->runningSwPnoFlag = false;
        pScanStateMachine->PnoScanSoftwareProcess(&msg);
    }

    void PnoScanSoftwareProcessTest2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        InternalMessage msg;
        pScanStateMachine->runningSwPnoFlag = true;
        pScanStateMachine->PnoScanSoftwareProcess(&msg);
    }

    void PnoScanSoftwareProcessTest3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), Scan(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        InternalMessage msg;
        pScanStateMachine->runningSwPnoFlag = true;
        pScanStateMachine->PnoScanSoftwareProcess(&msg);
    }

    void SoftwareScanInfoProcessTest1()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanStateMachine->SoftwareScanInfoProcess();
    }

    void SoftwareScanInfoProcessTest2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), QueryScanInfos(_)).WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        pScanStateMachine->SoftwareScanInfoProcess();
    }

    void InitCommonScanStateTest()
    {
        pScanStateMachine->InitCommonScanState();
    }

    void InitPnoScanState()
    {
        pScanStateMachine->InitPnoScanState();
    }

    void BuildScanStateTreeTest()
    {
        pScanStateMachine->BuildScanStateTree();
    }
};

HWTEST_F(ScanStateMachineTest, InitGoInStateTest, TestSize.Level1)
{
    InitGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, InitGoOutStateTest, TestSize.Level1)
{
    InitGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess1, TestSize.Level1)
{
    InitExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess2, TestSize.Level1)
{
    InitExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess3, TestSize.Level1)
{
    InitExeMsgSuccess3();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess4, TestSize.Level1)
{
    InitExeMsgSuccess4();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess5, TestSize.Level1)
{
    InitExeMsgSuccess5();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess6, TestSize.Level1)
{
    InitExeMsgSuccess6();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess7, TestSize.Level1)
{
    InitExeMsgSuccess7();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess8, TestSize.Level1)
{
    InitExeMsgSuccess8();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess9, TestSize.Level1)
{
    InitExeMsgSuccess9();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgSuccess10, TestSize.Level1)
{
    InitExeMsgSuccess10();
}

HWTEST_F(ScanStateMachineTest, InitExeMsgFail, TestSize.Level1)
{
    InitExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, HardwareReadyGoInStateTest, TestSize.Level1)
{
    HardwareReadyGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, HardwareReadyGoOutStateMsgTest, TestSize.Level1)
{
    HardwareReadyGoOutStateMsgTest();
}

HWTEST_F(ScanStateMachineTest, HardwareReadyExeMsgSuccess1, TestSize.Level1)
{
    HardwareReadyExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, HardwareReadyExeMsgSuccess2, TestSize.Level1)
{
    HardwareReadyExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, HardwareReadyExeMsgFail, TestSize.Level1)
{
    HardwareReadyExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, CommonScanGoInStateTest, TestSize.Level1)
{
    CommonScanGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanGoOutStateTest, TestSize.Level1)
{
    CommonScanGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanExeMsgSuccess, TestSize.Level1)
{
    CommonScanExeMsgSuccess();
}

HWTEST_F(ScanStateMachineTest, CommonScanExeMsgFail, TestSize.Level1)
{
    CommonScanExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, CommonScanUnworkedGoInStateTest, TestSize.Level1)
{
    CommonScanUnworkedGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanUnworkedGoOutStateTest, TestSize.Level1)
{
    CommonScanUnworkedGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanUnworkedExeMsgSuccess1, TestSize.Level1)
{
    CommonScanUnworkedExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, CommonScanUnworkedExeMsgSuccess2, TestSize.Level1)
{
    CommonScanUnworkedExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, CommonScanUnworkedExeMsgSuccess3, TestSize.Level1)
{
    CommonScanUnworkedExeMsgSuccess3();
}

HWTEST_F(ScanStateMachineTest, CommonScanUnworkedExeMsgFail, TestSize.Level1)
{
    CommonScanUnworkedExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, CommonScanningGoInStateTest, TestSize.Level1)
{
    CommonScanningGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanningGoOutStateTest, TestSize.Level1)
{
    CommonScanningGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanningExeMsgSuccess1, TestSize.Level1)
{
    CommonScanningExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, CommonScanningExeMsgSuccess2, TestSize.Level1)
{
    CommonScanningExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, CommonScanningExeMsgSuccess3, TestSize.Level1)
{
    CommonScanningExeMsgSuccess3();
}

HWTEST_F(ScanStateMachineTest, CommonScanningExeMsgSuccess4, TestSize.Level1)
{
    CommonScanningExeMsgSuccess4();
}

HWTEST_F(ScanStateMachineTest, CommonScanningExeMsgSuccess5, TestSize.Level1)
{
    CommonScanningExeMsgSuccess5();
}

HWTEST_F(ScanStateMachineTest, CommonScanningExeMsgSuccess6, TestSize.Level1)
{
    CommonScanningExeMsgSuccess6();
}

HWTEST_F(ScanStateMachineTest, CommonScanningExeMsgFail, TestSize.Level1)
{
    CommonScanningExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, PnoScanGoInStateTest, TestSize.Level1)
{
    PnoScanGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanGoOutStateTest, TestSize.Level1)
{
    PnoScanGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanExeMsgSuccess, TestSize.Level1)
{
    PnoScanExeMsgSuccess();
}

HWTEST_F(ScanStateMachineTest, PnoScanExeMsgFail, TestSize.Level1)
{
    PnoScanExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareGoInStateTest, TestSize.Level1)
{
    PnoScanHardwareGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareGoOutStateTest, TestSize.Level1)
{
    PnoScanHardwareGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareExeMsgSuccess1, TestSize.Level1)
{
    PnoScanHardwareExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareExeMsgSuccess2, TestSize.Level1)
{
    PnoScanHardwareExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareExeMsgSuccess3, TestSize.Level1)
{
    PnoScanHardwareExeMsgSuccess3();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareExeMsgSuccess4, TestSize.Level1)
{
    PnoScanHardwareExeMsgSuccess4();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareExeMsgSuccess5, TestSize.Level1)
{
    PnoScanHardwareExeMsgSuccess5();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareExeMsgSuccess6, TestSize.Level1)
{
    PnoScanHardwareExeMsgSuccess6();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareExeMsgFail, TestSize.Level1)
{
    PnoScanHardwareExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoGoInStateTest, TestSize.Level1)
{
    CommonScanAfterPnoGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoGoOutStateTest, TestSize.Level1)
{
    CommonScanAfterPnoGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoExeMsgSuccess1, TestSize.Level1)
{
    CommonScanAfterPnoExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoExeMsgSuccess2, TestSize.Level1)
{
    CommonScanAfterPnoExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoExeMsgSuccess3, TestSize.Level1)
{
    CommonScanAfterPnoExeMsgSuccess3();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoExeMsgSuccess4, TestSize.Level1)
{
    CommonScanAfterPnoExeMsgSuccess4();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoExeMsgSuccess5, TestSize.Level1)
{
    CommonScanAfterPnoExeMsgSuccess5();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoExeMsgSuccess6, TestSize.Level1)
{
    CommonScanAfterPnoExeMsgSuccess6();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoExeMsgFail, TestSize.Level1)
{
    CommonScanAfterPnoExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareGoInStateTest, TestSize.Level1)
{
    PnoScanSoftwareGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareGoOutStateTest, TestSize.Level1)
{
    PnoScanSoftwareGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareExeMsgSuccess1, TestSize.Level1)
{
    PnoScanSoftwareExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareExeMsgSuccess2, TestSize.Level1)
{
    PnoScanSoftwareExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareExeMsgFail, TestSize.Level1)
{
    PnoScanSoftwareExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeGoInStateTest, TestSize.Level1)
{
    PnoSwScanFreeGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeGoOutStateTest, TestSize.Level1)
{
    PnoSwScanFreeGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeExeMsgSuccess1, TestSize.Level1)
{
    PnoSwScanFreeExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeExeMsgSuccess2, TestSize.Level1)
{
    PnoSwScanFreeExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeExeMsgSuccess3, TestSize.Level1)
{
    PnoSwScanFreeExeMsgSuccess3();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeExeMsgSuccess4, TestSize.Level1)
{
    PnoSwScanFreeExeMsgSuccess4();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeExeMsgSuccess5, TestSize.Level1)
{
    PnoSwScanFreeExeMsgSuccess5();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanFreeExeMsgFail, TestSize.Level1)
{
    PnoSwScanFreeExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningGoInStateTest, TestSize.Level1)
{
    PnoSwScanningGoInStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningGoOutStateTest, TestSize.Level1)
{
    PnoSwScanningGoOutStateTest();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgSuccess1, TestSize.Level1)
{
    PnoSwScanningExeMsgSuccess1();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgSuccess2, TestSize.Level1)
{
    PnoSwScanningExeMsgSuccess2();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgSuccess3, TestSize.Level1)
{
    PnoSwScanningExeMsgSuccess3();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgSuccess4, TestSize.Level1)
{
    PnoSwScanningExeMsgSuccess4();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgSuccess5, TestSize.Level1)
{
    PnoSwScanningExeMsgSuccess5();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgSuccess6, TestSize.Level1)
{
    PnoSwScanningExeMsgSuccess6();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgSuccess7, TestSize.Level1)
{
    PnoSwScanningExeMsgSuccess7();
}

HWTEST_F(ScanStateMachineTest, PnoSwScanningExeMsgFail, TestSize.Level1)
{
    PnoSwScanningExeMsgFail();
}

HWTEST_F(ScanStateMachineTest, CommonScanRequestProcessTest, TestSize.Level1)
{
    CommonScanRequestProcessTest();
}

HWTEST_F(ScanStateMachineTest, GetCommonScanRequestInfoTest1, TestSize.Level1)
{
    GetCommonScanRequestInfoTest1();
}

HWTEST_F(ScanStateMachineTest, GetCommonScanRequestInfoTest2, TestSize.Level1)
{
    GetCommonScanRequestInfoTest2();
}

HWTEST_F(ScanStateMachineTest, GetCommonScanConfigSuccess, TestSize.Level1)
{
    GetCommonScanConfigSuccess();
}

HWTEST_F(ScanStateMachineTest, GetCommonScanConfigFail1, TestSize.Level1)
{
    GetCommonScanConfigFail1();
}

HWTEST_F(ScanStateMachineTest, GetCommonScanConfigFail2, TestSize.Level1)
{
    GetCommonScanConfigFail2();
}

HWTEST_F(ScanStateMachineTest, GetCommonScanConfigFail3, TestSize.Level1)
{
    GetCommonScanConfigFail3();
}

HWTEST_F(ScanStateMachineTest, StartNewCommonScanTest1, TestSize.Level1)
{
    StartNewCommonScanTest1();
}

HWTEST_F(ScanStateMachineTest, StartNewCommonScanTest2, TestSize.Level1)
{
    StartNewCommonScanTest2();
}

HWTEST_F(ScanStateMachineTest, ClearRunningScanSettingsTest, TestSize.Level1)
{
    ClearRunningScanSettingsTest();
}

HWTEST_F(ScanStateMachineTest, StartSingleCommonScanSuccess, TestSize.Level1)
{
    StartSingleCommonScanSuccess();
}

HWTEST_F(ScanStateMachineTest, StartSingleCommonScanFail, TestSize.Level1)
{
    StartSingleCommonScanFail();
}

HWTEST_F(ScanStateMachineTest, CommonScanWhenRunningSuccess, TestSize.Level1)
{
    CommonScanWhenRunningSuccess();
}

HWTEST_F(ScanStateMachineTest, CommonScanWhenRunningFail, TestSize.Level1)
{
    CommonScanWhenRunningFail();
}

HWTEST_F(ScanStateMachineTest, ActiveCoverNewScanSuccess, TestSize.Level1)
{
    ActiveCoverNewScanSuccess();
}

HWTEST_F(ScanStateMachineTest, ActiveCoverNewScanFail, TestSize.Level1)
{
    ActiveCoverNewScanFail();
}

HWTEST_F(ScanStateMachineTest, CommonScanInfoProcessTest, TestSize.Level1)
{
    CommonScanInfoProcessTest();
}

HWTEST_F(ScanStateMachineTest, ReportStatusChangeTest, TestSize.Level1)
{
    ReportStatusChangeTest();
}

HWTEST_F(ScanStateMachineTest, ReportScanInnerEventTest, TestSize.Level1)
{
    ReportScanInnerEventTest();
}

HWTEST_F(ScanStateMachineTest, ReportCommonScanFailedTest, TestSize.Level1)
{
    ReportCommonScanFailedTest();
}

HWTEST_F(ScanStateMachineTest, ReportCommonScanFailedAndClearTest1, TestSize.Level1)
{
    ReportCommonScanFailedAndClearTest1();
}

HWTEST_F(ScanStateMachineTest, ReportCommonScanFailedAndClearTest2, TestSize.Level1)
{
    ReportCommonScanFailedAndClearTest2();
}

HWTEST_F(ScanStateMachineTest, GetRunningIndexListTest, TestSize.Level1)
{
    GetRunningIndexListTest();
}

HWTEST_F(ScanStateMachineTest, GetWaitingIndexListTest, TestSize.Level1)
{
    GetWaitingIndexListTest();
}

HWTEST_F(ScanStateMachineTest, VerifyScanStyleSuccess, TestSize.Level1)
{
    VerifyScanStyleSuccess();
}

HWTEST_F(ScanStateMachineTest, VerifyScanStyleFail, TestSize.Level1)
{
    VerifyScanStyleFail();
}

HWTEST_F(ScanStateMachineTest, ActiveScanStyleTest1, TestSize.Level1)
{
    ActiveScanStyleTest1();
}

HWTEST_F(ScanStateMachineTest, ActiveScanStyleTest2, TestSize.Level1)
{
    ActiveScanStyleTest2();
}

HWTEST_F(ScanStateMachineTest, ActiveScanStyleTest3, TestSize.Level1)
{
    ActiveScanStyleTest3();
}

HWTEST_F(ScanStateMachineTest, MergeScanStyleTest1, TestSize.Level1)
{
    MergeScanStyleTest1();
}

HWTEST_F(ScanStateMachineTest, MergeScanStyleTest2, TestSize.Level1)
{
    MergeScanStyleTest2();
}

HWTEST_F(ScanStateMachineTest, MergeScanStyleTest3, TestSize.Level1)
{
    MergeScanStyleTest3();
}

HWTEST_F(ScanStateMachineTest, RemoveCommonScanRequestTest, TestSize.Level1)
{
    RemoveCommonScanRequestTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanRequestProcessTest, TestSize.Level1)
{
    PnoScanRequestProcessTest();
}

HWTEST_F(ScanStateMachineTest, ContinuePnoScanProcess, TestSize.Level1)
{
    ContinuePnoScanProcess();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareProcessTest1, TestSize.Level1)
{
    PnoScanHardwareProcessTest1();
}

HWTEST_F(ScanStateMachineTest, PnoScanHardwareProcessTest2, TestSize.Level1)
{
    PnoScanHardwareProcessTest2();
}

HWTEST_F(ScanStateMachineTest, StartPnoScanHardwareSuccess1, TestSize.Level1)
{
    StartPnoScanHardwareSuccess1();
}

HWTEST_F(ScanStateMachineTest, StartPnoScanHardwareSuccess2, TestSize.Level1)
{
    StartPnoScanHardwareSuccess2();
}

HWTEST_F(ScanStateMachineTest, StartPnoScanHardwareSuccess3, TestSize.Level1)
{
    StartPnoScanHardwareSuccess3();
}

HWTEST_F(ScanStateMachineTest, StopPnoScanHardwareTest1, TestSize.Level1)
{
    StopPnoScanHardwareTest1();
}

HWTEST_F(ScanStateMachineTest, StopPnoScanHardwareTest2, TestSize.Level1)
{
    StopPnoScanHardwareTest2();
}

HWTEST_F(ScanStateMachineTest, StopPnoScanHardwareTest3, TestSize.Level1)
{
    StopPnoScanHardwareTest3();
}

HWTEST_F(ScanStateMachineTest, UpdatePnoScanRequestTest, TestSize.Level1)
{
    UpdatePnoScanRequestTest();
}

HWTEST_F(ScanStateMachineTest, GetPnoScanRequestInfoTest1, TestSize.Level1)
{
    GetPnoScanRequestInfoTest1();
}

HWTEST_F(ScanStateMachineTest, GetPnoScanRequestInfoTest2, TestSize.Level1)
{
    GetPnoScanRequestInfoTest2();
}

HWTEST_F(ScanStateMachineTest, GetPnoScanConfigSuccess, TestSize.Level1)
{
    GetPnoScanConfigSuccess();
}

HWTEST_F(ScanStateMachineTest, GetPnoScanConfigFail1, TestSize.Level1)
{
    GetPnoScanConfigFail1();
}

HWTEST_F(ScanStateMachineTest, GetPnoScanConfigFail2, TestSize.Level1)
{
    GetPnoScanConfigFail2();
}

HWTEST_F(ScanStateMachineTest, HwPnoScanInfoProcessTest1, TestSize.Level1)
{
    HwPnoScanInfoProcessTest1();
}

HWTEST_F(ScanStateMachineTest, HwPnoScanInfoProcessTest2, TestSize.Level1)
{
    HwPnoScanInfoProcessTest2();
}

HWTEST_F(ScanStateMachineTest, HwPnoScanInfoProcessTest3, TestSize.Level1)
{
    HwPnoScanInfoProcessTest3();
}

HWTEST_F(ScanStateMachineTest, ReportPnoScanInfosTest, TestSize.Level1)
{
    ReportPnoScanInfosTest();
}

HWTEST_F(ScanStateMachineTest, NeedCommonScanAfterPnoTest, TestSize.Level1)
{
    NeedCommonScanAfterPnoTest();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoProcessTest1, TestSize.Level1)
{
    CommonScanAfterPnoProcessTest1();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoProcessTest2, TestSize.Level1)
{
    CommonScanAfterPnoProcessTest2();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoResultTest1, TestSize.Level1)
{
    CommonScanAfterPnoResultTest1();
}

HWTEST_F(ScanStateMachineTest, CommonScanAfterPnoResultTest2, TestSize.Level1)
{
    CommonScanAfterPnoResultTest2();
}

HWTEST_F(ScanStateMachineTest, PnoScanFailedProcessSuccessTest, TestSize.Level1)
{
    PnoScanFailedProcessSuccessTest();
}

HWTEST_F(ScanStateMachineTest, ClearPnoScanConfigTest, TestSize.Level1)
{
    ClearPnoScanConfigTest();
}

HWTEST_F(ScanStateMachineTest, GetScanInfosSuccess, TestSize.Level1)
{
    GetScanInfosSuccess();
}

HWTEST_F(ScanStateMachineTest, GetScanInfosFail, TestSize.Level1)
{
    GetScanInfosFail();
}

HWTEST_F(ScanStateMachineTest, GetSecurityTypeAndBandTest, TestSize.Level1)
{
    GetSecurityTypeAndBandTest();
}

HWTEST_F(ScanStateMachineTest, StartNewSoftwareScanTest, TestSize.Level1)
{
    StartNewSoftwareScanTest();
}

HWTEST_F(ScanStateMachineTest, RepeatStartCommonScanTest1, TestSize.Level1)
{
    RepeatStartCommonScanTest1();
}

HWTEST_F(ScanStateMachineTest, RepeatStartCommonScanTest2, TestSize.Level1)
{
    RepeatStartCommonScanTest2();
}

HWTEST_F(ScanStateMachineTest, RepeatStartCommonScanTest3, TestSize.Level1)
{
    RepeatStartCommonScanTest3();
}

HWTEST_F(ScanStateMachineTest, StopPnoScanSoftwareTest, TestSize.Level1)
{
    StopPnoScanSoftwareTest();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareProcessTest1, TestSize.Level1)
{
    PnoScanSoftwareProcessTest1();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareProcessTest2, TestSize.Level1)
{
    PnoScanSoftwareProcessTest2();
}

HWTEST_F(ScanStateMachineTest, PnoScanSoftwareProcessTest3, TestSize.Level1)
{
    PnoScanSoftwareProcessTest3();
}

HWTEST_F(ScanStateMachineTest, SoftwareScanInfoProcessTest1, TestSize.Level1)
{
    SoftwareScanInfoProcessTest1();
}

HWTEST_F(ScanStateMachineTest, SoftwareScanInfoProcessTest2, TestSize.Level1)
{
    SoftwareScanInfoProcessTest2();
}

HWTEST_F(ScanStateMachineTest, InitCommonScanStateTest, TestSize.Level1)
{
    InitCommonScanStateTest();
}

HWTEST_F(ScanStateMachineTest, InitPnoScanState, TestSize.Level1)
{
    InitPnoScanState();
}

HWTEST_F(ScanStateMachineTest, BuildScanStateTreeTest, TestSize.Level1)
{
    BuildScanStateTreeTest();
}

HWTEST_F(ScanStateMachineTest, LoadDriverTest, TestSize.Level1)
{
    LoadDriverTest();
}

HWTEST_F(ScanStateMachineTest, UnLoadDriverTest, TestSize.Level1)
{
    UnLoadDriverTest();
}
} // namespace Wifi
} // namespace OHOS
