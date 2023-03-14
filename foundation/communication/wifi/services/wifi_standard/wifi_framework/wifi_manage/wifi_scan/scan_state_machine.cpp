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
#include "scan_state_machine.h"
#include "wifi_logger.h"
#include "wifi_settings.h"
#include "wifi_sta_hal_interface.h"

DEFINE_WIFILOG_SCAN_LABEL("ScanStateMachine");

namespace OHOS {
namespace Wifi {
ScanStateMachine::ScanStateMachine()
    : StateMachine("ScanStateMachine"),
      quitFlag(false),
      initState(nullptr),
      hardwareReadyState(nullptr),
      commonScanState(nullptr),
      commonScanUnworkedState(nullptr),
      commonScanningState(nullptr),
      pnoScanState(nullptr),
      pnoScanHardwareState(nullptr),
      commonScanAfterPnoState(nullptr),
      pnoScanSoftwareState(nullptr),
      pnoSwScanFreeState(nullptr),
      pnoSwScanningState(nullptr),
      runningFullScanFlag(false),
      supportHwPnoFlag(true),
      pnoConfigStoredFlag(false),
      runningHwPnoFlag(false),
      remainWaitResultTimer(false),
      runningSwPnoFlag(false)
{}

ScanStateMachine::~ScanStateMachine()
{
    WIFI_LOGI("Enter ScanStateMachine::~ScanStateMachine.\n");

    /* Stop the thread. Otherwise, a problem occurs */
    StopHandlerThread();

    if (initState != nullptr) {
        delete initState;
        initState = nullptr;
    }

    if (hardwareReadyState != nullptr) {
        delete hardwareReadyState;
        hardwareReadyState = nullptr;
    }

    if (commonScanState != nullptr) {
        delete commonScanState;
        commonScanState = nullptr;
    }

    if (commonScanUnworkedState != nullptr) {
        delete commonScanUnworkedState;
        commonScanUnworkedState = nullptr;
    }

    if (commonScanningState != nullptr) {
        delete commonScanningState;
        commonScanningState = nullptr;
    }

    if (pnoScanState != nullptr) {
        delete pnoScanState;
        pnoScanState = nullptr;
    }

    if (pnoScanHardwareState != nullptr) {
        delete pnoScanHardwareState;
        pnoScanHardwareState = nullptr;
    }
    StopPnoScanHardware();

    if (commonScanAfterPnoState != nullptr) {
        delete commonScanAfterPnoState;
        commonScanAfterPnoState = nullptr;
    }

    if (pnoScanSoftwareState != nullptr) {
        delete pnoScanSoftwareState;
        pnoScanSoftwareState = nullptr;
    }

    if (pnoSwScanFreeState != nullptr) {
        delete pnoSwScanFreeState;
        pnoSwScanFreeState = nullptr;
    }

    if (pnoSwScanningState != nullptr) {
        delete pnoSwScanningState;
        pnoSwScanningState = nullptr;
    }
}

bool ScanStateMachine::InitScanStateMachine()
{
    WIFI_LOGI("Enter InitScanStateMachine.\n");

    /* init supportHwPnoFlag value */
    supportHwPnoFlag = WifiSettings::GetInstance().GetSupportHwPnoFlag();

    if (!InitialStateMachine()) {
        WIFI_LOGE("Initial StateMachine failed.\n");
        return false;
    }

    if (InitCommonScanState() != true) {
        return false;
    };

    if (InitPnoScanState() != true) {
        return false;
    };

    BuildScanStateTree();
    SetFirstState(initState);
    StartStateMachine();
    return true;
}

bool ScanStateMachine::EnrollScanStatusListener(ScanStatusReportHandler handler)
{
    WIFI_LOGI("Enter ScanStateMachine::EnrollScanStatusListener.\n");

    if (!handler) {
        WIFI_LOGE("handler is null.\n");
        return false;
    }

    scanStatusReportHandler = handler;
    return true;
}

ScanStateMachine::InitState::InitState(ScanStateMachine *paraScanStateMachine) : State("InitState")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::InitState::~InitState()
{}

void ScanStateMachine::InitState::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::InitState::GoInState.\n");
    pScanStateMachine->runningScans.clear();
    pScanStateMachine->waitingScans.clear();

    if (pScanStateMachine->quitFlag) {
        WIFI_LOGI("Notify finish ScanStateMachine.\n");
        pScanStateMachine->ReportStatusChange(SCAN_FINISHED_STATUS);
    }
    return;
}

void ScanStateMachine::InitState::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::InitState::GoOutState.\n");
    return;
}

bool ScanStateMachine::InitState::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("Enter ScanStateMachine::InitState::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_SCAN_PREPARE:
            LoadDriver();
            return true;

        case CMD_SCAN_FINISH:
            UnLoadDriver();
            return true;

        case CMD_START_COMMON_SCAN:
            pScanStateMachine->ReportCommonScanFailed(msg->GetParam1());
            return true;

        case CMD_START_PNO_SCAN:
            pScanStateMachine->ReportStatusChange(PNO_SCAN_FAILED);
            return true;

        case CMD_STOP_PNO_SCAN:
            pScanStateMachine->ClearPnoScanConfig();
            pScanStateMachine->StopPnoScanHardware();
            return true;

        case HARDWARE_LOAD_EVENT:
            pScanStateMachine->SwitchState(pScanStateMachine->hardwareReadyState);
            pScanStateMachine->ReportStatusChange(SCAN_STARTED_STATUS);
            return true;

        case HARDWARE_UNLOAD_EVENT:
            pScanStateMachine->SwitchState(pScanStateMachine->initState);
            pScanStateMachine->quitFlag = true;
            return true;

        case CMD_STOP_COMMON_SCAN:
        case SCAN_RESULT_EVENT:
        case PNO_SCAN_RESULT_EVENT:
        case SCAN_FAILED_EVENT:
            WIFI_LOGE("ignored scan results event.\n");
            return true;

        case SYSTEM_SCAN_TIMER:
        case DISCONNECTED_SCAN_TIMER:
        case RESTART_PNO_SCAN_TIMER:
            pScanStateMachine->ReportScanInnerEvent((ScanInnerEventType)msg->GetMessageName());
            return true;

        default:
            return false;
    }
}

ScanStateMachine::HardwareReady::HardwareReady(ScanStateMachine *paraScanStateMachine) : State("HardwareReady")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::HardwareReady::~HardwareReady()
{}

void ScanStateMachine::HardwareReady::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::HardwareReady::GoInState.\n");
    return;
}

void ScanStateMachine::HardwareReady::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::HardwareReady::GoOutState.\n");
    return;
}

bool ScanStateMachine::HardwareReady::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("ScanStateMachine::HardwareReady::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_START_COMMON_SCAN:
            pScanStateMachine->CommonScanRequestProcess(msg);
            return true;

        case CMD_START_PNO_SCAN:
            pScanStateMachine->PnoScanRequestProcess(msg);
            return true;

        default:
            return false;
    }
}

ScanStateMachine::CommonScan::CommonScan(ScanStateMachine *paraScanStateMachine) : State("CommonScan")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::CommonScan::~CommonScan()
{}

void ScanStateMachine::CommonScan::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScan::GoInState.\n");
    return;
}

void ScanStateMachine::CommonScan::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScan::GoOutState.\n");
    pScanStateMachine->ReportCommonScanFailedAndClear(false);
    return;
}

bool ScanStateMachine::CommonScan::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("ScanStateMachine::CommonScan::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_STOP_COMMON_SCAN:
            pScanStateMachine->RemoveCommonScanRequest(msg->GetParam1());
            return true;

        default:
            return false;
    }
}

ScanStateMachine::CommonScanUnworked::CommonScanUnworked(ScanStateMachine *paraScanStateMachine)
    : State("CommonScanUnworked")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::CommonScanUnworked::~CommonScanUnworked()
{}

void ScanStateMachine::CommonScanUnworked::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanUnworked::GoInState.\n");
    pScanStateMachine->StartNewCommonScan();
    return;
}

void ScanStateMachine::CommonScanUnworked::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanUnworked::GoOutState.\n");
    return;
}

bool ScanStateMachine::CommonScanUnworked::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("ScanStateMachine::CommonScanUnworked::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_START_COMMON_SCAN:
            pScanStateMachine->CommonScanRequestProcess(msg);
            return true;

        case CMD_START_PNO_SCAN:
        case CMD_RESTART_PNO_SCAN:
            pScanStateMachine->PnoScanRequestProcess(msg);
            return true;

        default:
            return false;
    }
}

ScanStateMachine::CommonScanning::CommonScanning(ScanStateMachine *paraScanStateMachine) : State("CommonScanning")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::CommonScanning::~CommonScanning()
{}

void ScanStateMachine::CommonScanning::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanning::GoInState.\n");
    return;
}

void ScanStateMachine::CommonScanning::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanning::GoOutState.\n");
    pScanStateMachine->ClearRunningScanSettings();
    pScanStateMachine->ReportCommonScanFailedAndClear(true);
    pScanStateMachine->StopTimer(int(WAIT_SCAN_RESULT_TIMER));
    return;
}

/**
 * @Description  Function for processing messages when common scanning is in progress.
 * @param msg - Internal message class, which is used to send messages to the state machine.[in]
 * @return success: true, failed: false
 */
bool ScanStateMachine::CommonScanning::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanning::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_START_COMMON_SCAN:
            pScanStateMachine->CommonScanWhenRunning(msg);
            return true;

        case SCAN_RESULT_EVENT:
            pScanStateMachine->CommonScanInfoProcess();
            pScanStateMachine->SwitchState(pScanStateMachine->commonScanUnworkedState);
            return true;

        case SCAN_FAILED_EVENT:
            WIFI_LOGE("scan failed.");
            pScanStateMachine->ReportCommonScanFailedAndClear(true);
            pScanStateMachine->SwitchState(pScanStateMachine->commonScanUnworkedState);
            return true;

        case WAIT_SCAN_RESULT_TIMER:
            WIFI_LOGE("get scan result time out.");
            pScanStateMachine->ReportCommonScanFailedAndClear(true);
            pScanStateMachine->SwitchState(pScanStateMachine->commonScanUnworkedState);
            return true;

        /*
         * Receive a PNO scanning request and wait until the scanning is complete and
         * enter the idle state
         */
        case CMD_START_PNO_SCAN:
        case CMD_RESTART_PNO_SCAN:
            pScanStateMachine->DelayMessage(msg);
            return true;

        default:
            return false;
    }
}

ScanStateMachine::PnoScan::PnoScan(ScanStateMachine *paraScanStateMachine) : State("PnoScan")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::PnoScan::~PnoScan()
{}

void ScanStateMachine::PnoScan::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScan::GoInState.\n");
    return;
}

void ScanStateMachine::PnoScan::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScan::GoOutState.\n");
    pScanStateMachine->StopPnoScanHardware();
    return;
}

bool ScanStateMachine::PnoScan::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("ScanStateMachine::PnoScan::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
    }
    return false;
}

ScanStateMachine::PnoScanHardware::PnoScanHardware(ScanStateMachine *paraScanStateMachine) : State("PnoScanHardware")
{
    pScanStateMachine = paraScanStateMachine;
    return;
}

ScanStateMachine::PnoScanHardware::~PnoScanHardware()
{}

void ScanStateMachine::PnoScanHardware::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScanHardware::GoInState.\n");
    if (!pScanStateMachine->StartPnoScanHardware()) {
        WIFI_LOGE("StartPnoScanHardware failed.");
        return;
    }
    return;
}

void ScanStateMachine::PnoScanHardware::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScanHardware::GoOutState.\n");
    return;
}

bool ScanStateMachine::PnoScanHardware::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("ScanStateMachine::PnoScanHardware::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_START_PNO_SCAN:
            pScanStateMachine->PnoScanHardwareProcess(msg);
            return true;

        case CMD_STOP_PNO_SCAN:
            pScanStateMachine->ClearPnoScanConfig();
            pScanStateMachine->StopPnoScanHardware();
            return true;

        case CMD_RESTART_PNO_SCAN:
            pScanStateMachine->StopPnoScanHardware();
            pScanStateMachine->PnoScanHardwareProcess(msg);
            return true;

        case PNO_SCAN_RESULT_EVENT:
            pScanStateMachine->HwPnoScanInfoProcess();
            return true;

        case CMD_START_COMMON_SCAN:
            pScanStateMachine->DelayMessage(msg);
            pScanStateMachine->SwitchState(pScanStateMachine->hardwareReadyState);
            return true;

        default:
            return false;
    }
}

ScanStateMachine::CommonScanAfterPno::CommonScanAfterPno(ScanStateMachine *paraScanStateMachine)
    : State("CommonScanAfterPno")
{
    pScanStateMachine = paraScanStateMachine;
    return;
}

ScanStateMachine::CommonScanAfterPno::~CommonScanAfterPno()
{}

void ScanStateMachine::CommonScanAfterPno::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanAfterPno::GoInState.\n");
    pScanStateMachine->CommonScanAfterPnoProcess();
    return;
}

void ScanStateMachine::CommonScanAfterPno::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanAfterPno::GoOutState.\n");
    if (!pScanStateMachine->remainWaitResultTimer) {
        pScanStateMachine->StopTimer(static_cast<int>(WAIT_SCAN_RESULT_TIMER));
    }
    pScanStateMachine->remainWaitResultTimer = false;

    return;
}

bool ScanStateMachine::CommonScanAfterPno::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("ScanStateMachine::CommonScanAfterPno::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case SCAN_RESULT_EVENT:
            pScanStateMachine->CommonScanAfterPnoResult();
            pScanStateMachine->SwitchState(pScanStateMachine->pnoScanHardwareState);
            return true;

        case SCAN_FAILED_EVENT:
        case WAIT_SCAN_RESULT_TIMER:
            pScanStateMachine->SwitchState(pScanStateMachine->pnoScanHardwareState);
            return true;

        case CMD_START_PNO_SCAN:
        case PNO_SCAN_RESULT_EVENT:
            WIFI_LOGE("Ignore the message.\n");
            return true;

        /*
         * After receiving the scanning start message,
         * wait until the current scanning is complete and process the message after
         * the status is changed
         */
        case CMD_START_COMMON_SCAN:
            pScanStateMachine->DelayMessage(msg);
            pScanStateMachine->SwitchState(pScanStateMachine->commonScanningState);
            pScanStateMachine->remainWaitResultTimer = true;
            return true;

        case CMD_RESTART_PNO_SCAN:
            pScanStateMachine->UpdatePnoScanRequest(msg);
            return true;

        default:
            return false;
    }
}

ScanStateMachine::PnoScanSoftware::PnoScanSoftware(ScanStateMachine *paraScanStateMachine) : State("PnoScanSoftware")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::PnoScanSoftware::~PnoScanSoftware()
{}

void ScanStateMachine::PnoScanSoftware::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScanSoftware::GoInState.\n");
    WIFI_LOGI("Start scan first!");

    if (!pScanStateMachine->StartNewSoftwareScan()) {
        WIFI_LOGE("failed to start new softwareScan");
    }
    return;
}

void ScanStateMachine::PnoScanSoftware::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScanSoftware::GoOutState.\n");
    pScanStateMachine->StopTimer(static_cast<int>(SOFTWARE_PNO_SCAN_TIMER));
    return;
}

bool ScanStateMachine::PnoScanSoftware::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScanSoftware::ExecuteStateMsg.\n");

    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_STOP_PNO_SCAN:
            pScanStateMachine->ClearPnoScanConfig();
            pScanStateMachine->StopPnoScanSoftware();
            return true;
        default:
            return false;
    }
}

ScanStateMachine::PnoSwScanFree::PnoSwScanFree(ScanStateMachine *paraScanStateMachine) : State("PnoSwScanFree")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::PnoSwScanFree::~PnoSwScanFree()
{}

void ScanStateMachine::PnoSwScanFree::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoSwScanFree::GoInState.\n");
    return;
}

void ScanStateMachine::PnoSwScanFree::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoSwScanFree::GoOutState.\n");
    return;
}

bool ScanStateMachine::PnoSwScanFree::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("Enter ScanStateMachine::PnoSwScanFree::ExecuteStateMsg.\n");

    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case CMD_START_PNO_SCAN:
            pScanStateMachine->PnoScanSoftwareProcess(msg);
            return true;
        case CMD_RESTART_PNO_SCAN:
            pScanStateMachine->StopPnoScanSoftware();
            pScanStateMachine->PnoScanSoftwareProcess(msg);
            return true;
        case CMD_START_COMMON_SCAN:
            pScanStateMachine->DelayMessage(msg);
            pScanStateMachine->SwitchState(pScanStateMachine->hardwareReadyState);
            return true;
        case SOFTWARE_PNO_SCAN_TIMER:
            WIFI_LOGI(
                "softwarePno scanscanInterval is %{public}d.\n", pScanStateMachine->runningPnoScanConfig.scanInterval);

            if (!pScanStateMachine->RepeatStartCommonScan()) {
                WIFI_LOGE("Failed to start scan");
            }
            pScanStateMachine->StartTimer(static_cast<int>(SOFTWARE_PNO_SCAN_TIMER),
                (pScanStateMachine->runningPnoScanConfig.scanInterval) * SECOND_TO_MILLI_SECOND);

            return true;
        default:
            return false;
    }
}

ScanStateMachine::PnoSwScanning::PnoSwScanning(ScanStateMachine *paraScanStateMachine) : State("PnoSwScanning")
{
    pScanStateMachine = paraScanStateMachine;
}

ScanStateMachine::PnoSwScanning::~PnoSwScanning()
{}

void ScanStateMachine::PnoSwScanning::GoInState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoSwScanning::GoInState.\n");
    return;
}

void ScanStateMachine::PnoSwScanning::GoOutState()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoSwScanning::GoOutState.\n");
    pScanStateMachine->StopTimer(static_cast<int>(WAIT_SCAN_RESULT_TIMER));
    return;
}

bool ScanStateMachine::PnoSwScanning::ExecuteStateMsg(InternalMessage *msg)
{
    WIFI_LOGI("Enter ScanStateMachine::PnoSwScanning::ExecuteStateMsg.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null.\n");
        return true;
    }

    switch (msg->GetMessageName()) {
        case SCAN_RESULT_EVENT:
            pScanStateMachine->SoftwareScanInfoProcess();
            pScanStateMachine->SwitchState(pScanStateMachine->pnoSwScanFreeState);
            return true;

        case SCAN_FAILED_EVENT:
            WIFI_LOGE("scan failed");
            pScanStateMachine->SwitchState(pScanStateMachine->pnoSwScanFreeState);
            return true;

        case WAIT_SCAN_RESULT_TIMER:
            WIFI_LOGE("get scan result timed out");
            pScanStateMachine->SwitchState(pScanStateMachine->pnoSwScanFreeState);
            return true;

        case CMD_START_PNO_SCAN:
            WIFI_LOGE("The SwPnoScan is in progress and cannot be performed repeatedly.");
            pScanStateMachine->PnoScanFailedProcess();
            return true;
        case CMD_RESTART_PNO_SCAN:
        case CMD_START_COMMON_SCAN:
            pScanStateMachine->DelayMessage(msg);
            return true;

        case SOFTWARE_PNO_SCAN_TIMER:
            WIFI_LOGI("Scanning is in progress. Please wait for the scan result.");
            pScanStateMachine->DelayMessage(msg);
            return true;

        default:
            return false;
    }
}

void ScanStateMachine::CommonScanRequestProcess(InternalMessage *interMessage)
{
    WIFI_LOGI("ScanStateMachine::CommonScanRequestProcess.\n");

    int requestIndex = 0;
    InterScanConfig scanConfig;
    if (!GetCommonScanRequestInfo(interMessage, requestIndex, scanConfig)) {
        ReportCommonScanFailed(requestIndex);
        return;
    }
    if (!VerifyScanStyle(scanConfig.scanStyle)) {
        WIFI_LOGE("invalid scan type");
        return;
    }
    waitingScans.insert(std::pair<int, InterScanConfig>(requestIndex, scanConfig));
    StartNewCommonScan();

    return;
}

bool ScanStateMachine::GetCommonScanRequestInfo(
    InternalMessage *interMessage, int &requestIndex, InterScanConfig &scanConfig)
{
    WIFI_LOGI("Enter ScanStateMachine::GetRequestMsgInfo.\n");

    if (interMessage == nullptr) {
        WIFI_LOGE("interMessage is null.");
        return false;
    }

    requestIndex = interMessage->GetParam1();
    if (!GetCommonScanConfig(interMessage, scanConfig)) {
        WIFI_LOGE("GetCommonScanConfig failed.");
        return false;
    }

    return true;
}

bool ScanStateMachine::GetCommonScanConfig(InternalMessage *interMessage, InterScanConfig &scanConfig)
{
    WIFI_LOGI("Enter ScanStateMachine::GetCommonScanConfig.\n");

    if (interMessage == nullptr) {
        WIFI_LOGE("interMessage is null.");
        return false;
    }

    /* Obtaining the Hidden Network List */
    int hiddenSize = interMessage->GetIntFromMessage();
    for (int i = 0; i < hiddenSize; i++) {
        std::string hiddenSsid = interMessage->GetStringFromMessage();
        if (hiddenSsid.empty()) {
            WIFI_LOGE("Message body is error.");
            return false;
        }
        scanConfig.hiddenNetworkSsid.push_back(hiddenSsid);
    }

    /* Obtains the frequency list */
    int freqSize = interMessage->GetIntFromMessage();
    for (int i = 0; i < freqSize; i++) {
        int freq = interMessage->GetIntFromMessage();
        if (freq == 0) {
            WIFI_LOGE("Message body is error.");
            return false;
        }
        scanConfig.scanFreqs.push_back(freq);
    }

    scanConfig.fullScanFlag = (bool)interMessage->GetIntFromMessage();
    scanConfig.backScanPeriod = interMessage->GetIntFromMessage();
    scanConfig.bssidsNumPerScan = interMessage->GetIntFromMessage();
    scanConfig.maxScansCache = interMessage->GetIntFromMessage();
    scanConfig.maxBackScanPeriod = interMessage->GetIntFromMessage();
    scanConfig.scanStyle = interMessage->GetIntFromMessage();

    return true;
}

void ScanStateMachine::StartNewCommonScan()
{
    WIFI_LOGI("Enter ScanStateMachine::StartNewCommonScan.\n");

    if (waitingScans.size() == 0) {
        ContinuePnoScanProcess();
        return;
    }

    ClearRunningScanSettings();
    bool hasFullScan = false;
    /* Traverse the request list and combine parameters */
    std::map<int, InterScanConfig>::iterator configIter = waitingScans.begin();
    for (; configIter != waitingScans.end(); ++configIter) {
        runningScanSettings.scanStyle = MergeScanStyle(runningScanSettings.scanStyle, configIter->second.scanStyle);

        std::vector<std::string>::iterator hiddenIter = configIter->second.hiddenNetworkSsid.begin();
        /* Remove duplicate hidden list */
        for (; hiddenIter != configIter->second.hiddenNetworkSsid.end(); ++hiddenIter) {
            if (std::find(runningScanSettings.hiddenNetworkSsid.begin(),
                runningScanSettings.hiddenNetworkSsid.end(),
                *hiddenIter) != runningScanSettings.hiddenNetworkSsid.end()) {
                continue;
            }
            runningScanSettings.hiddenNetworkSsid.push_back(*hiddenIter);
        }

        if (!hasFullScan) {
            /* When scanFreqs is empty, it means that scan all frequenties */
            if (configIter->second.scanFreqs.empty()) {
                runningScanSettings.scanFreqs.clear();
                runningFullScanFlag = true;
                hasFullScan = true;
            } else {
                std::vector<int>::iterator freqIter = configIter->second.scanFreqs.begin();
                /* Repetitions are eliminated */
                for (; freqIter != configIter->second.scanFreqs.end(); ++freqIter) {
                    if (std::find(runningScanSettings.scanFreqs.begin(),
                        runningScanSettings.scanFreqs.end(),
                        *freqIter) != runningScanSettings.scanFreqs.end()) {
                        continue;
                    }
                    runningScanSettings.scanFreqs.push_back(*freqIter);
                }
            }
        }
    }

    if (!StartSingleCommonScan(runningScanSettings)) {
        ReportCommonScanFailedAndClear(false);
        ContinuePnoScanProcess();
        return;
    }

    runningScans.swap(waitingScans);
    waitingScans.clear();
    SwitchState(commonScanningState);
    WIFI_LOGI("StartNewCommonScan success.\n");

    return;
}

void ScanStateMachine::ClearRunningScanSettings()
{
    runningScanSettings.hiddenNetworkSsid.clear();
    runningScanSettings.scanFreqs.clear();
    runningFullScanFlag = false;
    return;
}

bool ScanStateMachine::StartSingleCommonScan(WifiScanParam &scanParam)
{
    WIFI_LOGI("Enter ScanStateMachine::StartSingleCommonScan.\n");

    for (auto freqIter = scanParam.scanFreqs.begin(); freqIter != scanParam.scanFreqs.end(); ++freqIter) {
        WIFI_LOGI("freq is %{public}d.\n", *freqIter);
    }

    for (auto hiddenIter = scanParam.hiddenNetworkSsid.begin(); hiddenIter != scanParam.hiddenNetworkSsid.end();
         ++hiddenIter) {
        WIFI_LOGI("hidden ssid is %{public}s.\n", hiddenIter->c_str());
    }

    WIFI_LOGI("Begin call Scan.\n");
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().Scan(scanParam);
    if ((ret != WIFI_IDL_OPT_OK) && (ret != WIFI_IDL_OPT_SCAN_BUSY)) {
        WIFI_LOGE("WifiStaHalInterface::GetInstance().scan failed.");
        return false;
    }
    WIFI_LOGI("End call Scan.\n");

    /*
     * Start the timer. If no result is returned for a long time, the scanning
     * fails
     */
    StartTimer(static_cast<int>(WAIT_SCAN_RESULT_TIMER), MAX_WAIT_SCAN_RESULT_TIME);

    return true;
}

void ScanStateMachine::CommonScanWhenRunning(InternalMessage *interMessage)
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanWhenRunning.\n");

    int requestIndex = MAX_SCAN_CONFIG_STORE_INDEX;
    InterScanConfig scanConfig;
    if (!GetCommonScanRequestInfo(interMessage, requestIndex, scanConfig)) {
        ReportCommonScanFailed(requestIndex);
        return;
    }

    if (ActiveCoverNewScan(scanConfig)) {
        runningScans.insert(std::pair<int, InterScanConfig>(requestIndex, scanConfig));
    } else {
        waitingScans.insert(std::pair<int, InterScanConfig>(requestIndex, scanConfig));
    }

    return;
}

bool ScanStateMachine::ActiveCoverNewScan(InterScanConfig &interScanConfig)
{
    WIFI_LOGI("Enter ScanStateMachine::ActiveCoverNewScan.\n");

    if (!ActiveScanStyle(interScanConfig.scanStyle)) {
        return false;
    }

    /*
     * Determine if the frequency of new requests is included in the ongoing scan
     * settings
     */
    if (!runningFullScanFlag) {
        /* When scanFreqs is empty, it means that scan all frequenties */
        if (interScanConfig.scanFreqs.size() == 0) {
            return false;
        }

        for (auto freqIter = interScanConfig.scanFreqs.begin(); freqIter != interScanConfig.scanFreqs.end();
             ++freqIter) {
            if (std::find(runningScanSettings.scanFreqs.begin(), runningScanSettings.scanFreqs.end(), *freqIter) ==
                runningScanSettings.scanFreqs.end()) {
                return false;
            }
        }
    }

    /*
     * Determines whether the newly requested hidden network list is included in
     * the ongoing scan settings
     */
    if ((runningScanSettings.hiddenNetworkSsid.size() == 0) && (interScanConfig.hiddenNetworkSsid.size() != 0)) {
        return false;
    }

    for (auto hiddenIter = interScanConfig.hiddenNetworkSsid.begin();
         hiddenIter != interScanConfig.hiddenNetworkSsid.end();
         ++hiddenIter) {
        if (std::find(runningScanSettings.hiddenNetworkSsid.begin(),
            runningScanSettings.hiddenNetworkSsid.end(),
            *hiddenIter) == runningScanSettings.hiddenNetworkSsid.end()) {
            return false;
        }
    }

    return true;
}

void ScanStateMachine::CommonScanInfoProcess()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanInfoProcess.\n");

    ScanStatusReport scanStatusReport;
    if (!GetScanInfos(scanStatusReport.scanInfoList)) {
        WIFI_LOGE("GetScanInfos failed.");
        ReportCommonScanFailedAndClear(true);
        return;
    }
    GetRunningIndexList(scanStatusReport.requestIndexList);

    scanStatusReport.status = COMMON_SCAN_SUCCESS;
    if (scanStatusReportHandler) {
        scanStatusReportHandler(scanStatusReport);
    }
    runningScans.clear();

    return;
}

void ScanStateMachine::GetSecurityTypeAndBand(std::vector<InterScanInfo> &scanInfos)
{
    WIFI_LOGI("Enter ScanStateMachine::GetSecurityTypeAndBand.\n");

    for (auto iter = scanInfos.begin(); iter != scanInfos.end(); ++iter) {
        if (iter->frequency < SCAN_24GHZ_MAX_FREQUENCY) {
            iter->band = SCAN_24GHZ_BAND;
        } else if (iter->frequency > SCAN_5GHZ_MIN_FREQUENCY) {
            iter->band = SCAN_5GHZ_BAND;
        } else {
            WIFI_LOGE("invalid frequency value: %{public}d", iter->frequency);
            iter->band = 0;
        }

        iter->securityType = WifiSecurity::OPEN;
        if (iter->capabilities.find("WAPI-PSK") != std::string::npos) {
            iter->securityType = WifiSecurity::WAPI_PSK;
            continue;
        }
        if (iter->capabilities.find("PSK") != std::string::npos) {
            iter->securityType = WifiSecurity::PSK;
            continue;
        }
        if (iter->capabilities.find("WEP") != std::string::npos) {
            iter->securityType = WifiSecurity::WEP;
            continue;
        }
        if (iter->capabilities.find("EAP_SUITE_B_192") != std::string::npos) {
            iter->securityType = WifiSecurity::EAP_SUITE_B;
            continue;
        }
        if (iter->capabilities.find("EAP") != std::string::npos) {
            iter->securityType = WifiSecurity::EAP;
            continue;
        }
        if (iter->capabilities.find("SAE") != std::string::npos) {
            iter->securityType = WifiSecurity::SAE;
            continue;
        }
        if (iter->capabilities.find("OWE") != std::string::npos) {
            iter->securityType = WifiSecurity::OWE;
            continue;
        }
        if (iter->capabilities.find("CERT") != std::string::npos) {
            iter->securityType = WifiSecurity::WAPI_CERT;
            continue;
        }
    }
    return;
}

void ScanStateMachine::ReportStatusChange(ScanStatus status)
{
    WIFI_LOGI("Enter ScanStateMachine::ReportStatusChange.\n");

    ScanStatusReport scanStatusReport;
    scanStatusReport.status = status;
    if (scanStatusReportHandler) {
        scanStatusReportHandler(scanStatusReport);
    }

    return;
}

void ScanStateMachine::ReportScanInnerEvent(ScanInnerEventType innerEvent)
{
    WIFI_LOGI("Enter ScanStateMachine::ReportScanInnerEvent, event is %{public}d.\n", innerEvent);

    ScanStatusReport scanStatusReport;
    scanStatusReport.status = SCAN_INNER_EVENT;
    scanStatusReport.innerEvent = innerEvent;
    if (scanStatusReportHandler) {
        scanStatusReportHandler(scanStatusReport);
    }

    return;
}

void ScanStateMachine::ReportCommonScanFailed(int requestIndex)
{
    WIFI_LOGI("Enter ScanStateMachine::ReportCommonScanFailed.\n");

    if (requestIndex == MAX_SCAN_CONFIG_STORE_INDEX) {
        return;
    }

    ScanStatusReport scanStatusReport;
    scanStatusReport.status = COMMON_SCAN_FAILED;
    scanStatusReport.requestIndexList.push_back(requestIndex);
    if (scanStatusReportHandler) {
        scanStatusReportHandler(scanStatusReport);
    }

    return;
}

void ScanStateMachine::ReportCommonScanFailedAndClear(bool runningFlag)
{
    WIFI_LOGI("Enter ScanStateMachine::ReportCommonScanFailedAndClear.\n");

    ScanStatusReport scanStatusReport;
    if (runningFlag) {
        GetRunningIndexList(scanStatusReport.requestIndexList);
        runningScans.clear();
    } else {
        GetWaitingIndexList(scanStatusReport.requestIndexList);
        waitingScans.clear();
    }

    if (scanStatusReport.requestIndexList.size() == 0) {
        return;
    }

    scanStatusReport.status = COMMON_SCAN_FAILED;
    if (scanStatusReportHandler) {
        scanStatusReportHandler(scanStatusReport);
    }

    return;
}

void ScanStateMachine::GetRunningIndexList(std::vector<int> &runningIndexList)
{
    std::map<int, InterScanConfig>::iterator iter = runningScans.begin();
    for (; iter != runningScans.end(); ++iter) {
        runningIndexList.push_back(iter->first);
    }

    return;
}

void ScanStateMachine::GetWaitingIndexList(std::vector<int> &waitingIndexList)
{
    std::map<int, InterScanConfig>::iterator iter = waitingScans.begin();
    for (; iter != waitingScans.end(); ++iter) {
        waitingIndexList.push_back(iter->first);
    }

    return;
}

bool ScanStateMachine::VerifyScanStyle(int scanStyle)
{
    return (
        scanStyle == SCAN_TYPE_LOW_SPAN || scanStyle == SCAN_TYPE_LOW_POWER || scanStyle == SCAN_TYPE_HIGH_ACCURACY);
}

bool ScanStateMachine::ActiveScanStyle(int scanStyle)
{
    switch (runningScanSettings.scanStyle) {
        case SCAN_TYPE_LOW_SPAN:
        case SCAN_TYPE_LOW_POWER:
            return scanStyle != SCAN_TYPE_HIGH_ACCURACY;
        case SCAN_TYPE_HIGH_ACCURACY:
            return true;
        default:
            WIFI_LOGE("invalid scan style.");
            return false;
    }
}

int ScanStateMachine::MergeScanStyle(int currentScanStyle, int newScanStyle)
{
    switch (currentScanStyle) {
        case SCAN_TYPE_LOW_SPAN:
        case SCAN_TYPE_LOW_POWER:
            return newScanStyle;
        case SCAN_TYPE_HIGH_ACCURACY:
            return currentScanStyle;
        default:
            WIFI_LOGE("invalid scan style.");
            return newScanStyle;
    }
}

void ScanStateMachine::RemoveCommonScanRequest(int requestIndex)
{
    WIFI_LOGI("Enter ScanStateMachine::RemoveCommonScanRequest.\n");

    if (runningScans.count(requestIndex) == 1) {
        runningScans.erase(requestIndex);
    }

    if (waitingScans.count(requestIndex) == 1) {
        waitingScans.erase(requestIndex);
    }

    return;
}

void ScanStateMachine::PnoScanRequestProcess(InternalMessage *interMessage)
{
    WIFI_LOGI("ScanStateMachine::PnoScanRequestProcess.\n");

    if (!GetPnoScanRequestInfo(interMessage)) {
        WIFI_LOGE("GetPnoScanRequestInfo failed.\n");
        return;
    }

    if (supportHwPnoFlag) {
        SwitchState(pnoScanHardwareState);
    } else {
        SwitchState(pnoScanSoftwareState);
    }

    return;
}

void ScanStateMachine::ContinuePnoScanProcess()
{
    WIFI_LOGI("ScanStateMachine::ContinuePnoScanProcess.\n");

    if (!pnoConfigStoredFlag) {
        return;
    }

    if (supportHwPnoFlag) {
        SwitchState(pnoScanHardwareState);
    } else {
        SwitchState(pnoScanSoftwareState);
    }

    return;
}

void ScanStateMachine::PnoScanHardwareProcess(InternalMessage *interMessage)
{
    WIFI_LOGI("ScanStateMachine::PnoScanHardwareProcess.\n");
    if (runningHwPnoFlag) {
        WIFI_LOGE("Hardware Pno scan is running.");
        return;
    }

    if (!GetPnoScanRequestInfo(interMessage)) {
        WIFI_LOGE("GetPnoScanRequestInfo failed.");
        return;
    }

    if (!StartPnoScanHardware()) {
        WIFI_LOGE("StartPnoScanHardware failed.");
        return;
    }

    return;
}

bool ScanStateMachine::StartPnoScanHardware()
{
    WIFI_LOGI("ScanStateMachine::StartPnoScanHardware.\n");
    if (runningHwPnoFlag) {
        WIFI_LOGE("Hardware Pno scan is running.");
        return true;
    }

    if (!pnoConfigStoredFlag) {
        WIFI_LOGE("Pno config has not stored.");
        return true;
    }

    /* Invoke the IDL interface to start PNO scanning */
    WifiPnoScanParam pnoScanParam;
    pnoScanParam.scanInterval = runningPnoScanConfig.scanInterval;
    pnoScanParam.minRssi2Dot4Ghz = runningPnoScanConfig.minRssi2Dot4Ghz;
    pnoScanParam.minRssi5Ghz = runningPnoScanConfig.minRssi5Ghz;
    pnoScanParam.hiddenSsid.assign(
        runningPnoScanConfig.hiddenNetworkSsid.begin(), runningPnoScanConfig.hiddenNetworkSsid.end());
    pnoScanParam.savedSsid.assign(
        runningPnoScanConfig.savedNetworkSsid.begin(), runningPnoScanConfig.savedNetworkSsid.end());
    pnoScanParam.scanFreqs.assign(runningPnoScanConfig.freqs.begin(), runningPnoScanConfig.freqs.end());
    WIFI_LOGI("pnoScanParam.scanInterval is %{public}d.\n", pnoScanParam.scanInterval);
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StartPnoScan(pnoScanParam);
    if ((ret != WIFI_IDL_OPT_OK) && (ret != WIFI_IDL_OPT_SCAN_BUSY)) {
        WIFI_LOGE("WifiStaHalInterface::GetInstance().StartPnoScan failed.");
        PnoScanFailedProcess();
        return false;
    }
    runningHwPnoFlag = true;

    return true;
}

void ScanStateMachine::StopPnoScanHardware()
{
    WIFI_LOGI("ScanStateMachine::StopPnoScanHardware.\n");

    if (!supportHwPnoFlag) {
        return;
    }
    if (!runningHwPnoFlag) {
        WIFI_LOGE("Hardware Pno scan is not running.");
    }

    /* Invoke the IDL interface to stop PNO scanning */
    if (WifiStaHalInterface::GetInstance().StopPnoScan() != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("WifiStaHalInterface::GetInstance().StopPnoScan failed.");
    }

    runningHwPnoFlag = false;
    return;
}

void ScanStateMachine::UpdatePnoScanRequest(InternalMessage *interMessage)
{
    WIFI_LOGI("Enter ScanStateMachine::UpdatePnoScanRequest.\n");

    if (!GetPnoScanRequestInfo(interMessage)) {
        WIFI_LOGE("GetPnoScanRequestInfo failed.");
        return;
    }

    return;
}

bool ScanStateMachine::GetPnoScanRequestInfo(InternalMessage *interMessage)
{
    WIFI_LOGI("Enter ScanStateMachine::GetPnoScanRequestInfo.\n");

    if (interMessage == nullptr) {
        WIFI_LOGE("interMessage is null.");
        PnoScanFailedProcess();
        return false;
    }

    ClearPnoScanConfig();
    if (!GetPnoScanConfig(interMessage, runningPnoScanConfig)) {
        WIFI_LOGE("GetPnoScanConfig failed.");
        PnoScanFailedProcess();
        return false;
    }

    if (!GetCommonScanConfig(interMessage, runningScanConfigForPno)) {
        WIFI_LOGE("GetCommonScanConfig failed.");
        PnoScanFailedProcess();
        return false;
    }

    if ((runningScanConfigForPno.hiddenNetworkSsid.size() == 0) &&
        (runningPnoScanConfig.hiddenNetworkSsid.size() != 0)) {
        runningScanConfigForPno.hiddenNetworkSsid.assign(
            runningPnoScanConfig.hiddenNetworkSsid.begin(), runningPnoScanConfig.hiddenNetworkSsid.end());
    }

    pnoConfigStoredFlag = true;
    return true;
}

bool ScanStateMachine::GetPnoScanConfig(InternalMessage *interMessage, PnoScanConfig &pnoScanConfig)
{
    WIFI_LOGI("Enter ScanStateMachine::GetPnoScanConfig.\n");

    if (interMessage == nullptr) {
        WIFI_LOGE("interMessage is null.");
        return false;
    }

    pnoScanConfig.scanInterval = interMessage->GetIntFromMessage();
    pnoScanConfig.minRssi2Dot4Ghz = interMessage->GetIntFromMessage();
    pnoScanConfig.minRssi5Ghz = interMessage->GetIntFromMessage();

    /* Obtaining the Hidden Network List */
    int hiddenSize = interMessage->GetIntFromMessage();
    for (int i = 0; i < hiddenSize; i++) {
        std::string hiddenSsid = interMessage->GetStringFromMessage();
        if (hiddenSsid.empty()) {
            WIFI_LOGE("Message body is error.");
            return false;
        }
        pnoScanConfig.hiddenNetworkSsid.push_back(hiddenSsid);
    }

    /* Obtains the saved network list. */
    int iSavedSize = interMessage->GetIntFromMessage();
    for (int i = 0; i < iSavedSize; i++) {
        std::string savedSizeStr = interMessage->GetStringFromMessage();
        if (savedSizeStr.empty()) {
            WIFI_LOGE("Message body is error.");
            return false;
        }
        pnoScanConfig.savedNetworkSsid.push_back(savedSizeStr);
    }

    int freqsSize = interMessage->GetIntFromMessage();
    for (int i = 0; i < freqsSize; i++) {
        int freqs = interMessage->GetIntFromMessage();
        if (freqs == 0) {
            WIFI_LOGE("Message body is error.");
            return false;
        }
        pnoScanConfig.freqs.push_back(freqs);
    }

    return true;
}

void ScanStateMachine::HwPnoScanInfoProcess()
{
    WIFI_LOGI("Enter ScanStateMachine::HwPnoScanInfoProcess.\n");

    if (!runningHwPnoFlag) {
        WIFI_LOGE("Hardware pno scan is not running.");
        return;
    }

    std::vector<InterScanInfo> scanInfos;
    if (!GetScanInfos(scanInfos)) {
        WIFI_LOGE("GetScanInfos failed.");
        return;
    }

    if (NeedCommonScanAfterPno(scanInfos)) {
        SwitchState(commonScanAfterPnoState);
        return;
    }

    ReportPnoScanInfos(scanInfos);
    return;
}

void ScanStateMachine::ReportPnoScanInfos(std::vector<InterScanInfo> &scanInfos)
{
    WIFI_LOGI("Enter ScanStateMachine::ReportPnoScanInfos.\n");

    ScanStatusReport scanStatusReport;
    scanStatusReport.status = PNO_SCAN_INFO;
    scanStatusReport.scanInfoList.assign(scanInfos.begin(), scanInfos.end());
    if (scanStatusReportHandler) {
        scanStatusReportHandler(scanStatusReport);
    }
    return;
}

bool ScanStateMachine::NeedCommonScanAfterPno(std::vector<InterScanInfo> &scanInfos)
{
    WIFI_LOGI("Enter ScanStateMachine::NeedCommonScanAfterPno.\n");
    if (scanInfos.size() > 0) {
        WIFI_LOGI("Enter UpdateNetworkScoreCache.[%{public}s]\n", scanInfos[0].bssid.c_str());
    }
    return false;
}

void ScanStateMachine::CommonScanAfterPnoProcess()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanAfterPnoProcess.\n");

    StopPnoScanHardware();
    WifiScanParam scanParam;
    scanParam.hiddenNetworkSsid.assign(
        runningScanConfigForPno.hiddenNetworkSsid.begin(), runningScanConfigForPno.hiddenNetworkSsid.end());
    scanParam.scanFreqs.assign(runningScanConfigForPno.scanFreqs.begin(), runningScanConfigForPno.scanFreqs.end());
    if (!StartSingleCommonScan(scanParam)) {
        WIFI_LOGE("StartSingleCommonScan failed.\n");
        SwitchState(pnoScanHardwareState);
        return;
    }

    return;
}

void ScanStateMachine::CommonScanAfterPnoResult()
{
    WIFI_LOGI("Enter ScanStateMachine::CommonScanAfterPnoResult.\n");

    std::vector<InterScanInfo> scanInfos;
    if (!GetScanInfos(scanInfos)) {
        WIFI_LOGE("GetScanInfos failed.");
        return;
    }

    ReportPnoScanInfos(scanInfos);
    return;
}

void ScanStateMachine::PnoScanFailedProcess()
{
    WIFI_LOGI("Enter ScanStateMachine::PnoScanFailedProcess.\n");

    runningHwPnoFlag = false;
    runningSwPnoFlag = false;
    ClearPnoScanConfig();
    ReportStatusChange(PNO_SCAN_FAILED);

    return;
}

void ScanStateMachine::ClearPnoScanConfig()
{
    pnoConfigStoredFlag = false;
    runningPnoScanConfig.scanInterval = 0;
    runningPnoScanConfig.minRssi2Dot4Ghz = 0;
    runningPnoScanConfig.minRssi5Ghz = 0;
    runningPnoScanConfig.hiddenNetworkSsid.clear();
    runningPnoScanConfig.savedNetworkSsid.clear();
    runningPnoScanConfig.freqs.clear();

    runningScanConfigForPno.fullScanFlag = 0;
    runningScanConfigForPno.backScanPeriod = 0;
    runningScanConfigForPno.bssidsNumPerScan = 0;
    runningScanConfigForPno.maxScansCache = 0;
    runningScanConfigForPno.maxBackScanPeriod = 0;
    runningScanConfigForPno.hiddenNetworkSsid.clear();
    runningScanConfigForPno.scanFreqs.clear();

    return;
}

bool ScanStateMachine::GetScanInfos(std::vector<InterScanInfo> &scanInfos)
{
    WIFI_LOGI("Enter ScanStateMachine::GetScanInfos.\n");

    WIFI_LOGI("Begin: QueryScanInfos.");
    if (WifiStaHalInterface::GetInstance().QueryScanInfos(scanInfos) != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("WifiStaHalInterface::GetInstance().GetScanInfos failed.");
        return false;
    }
    WIFI_LOGI("End: QueryScanInfos.");
    GetSecurityTypeAndBand(scanInfos);

    return true;
}

bool ScanStateMachine::StartNewSoftwareScan()
{
    WIFI_LOGI("Enter ScanStateMachine::StartNewSoftwareScan.\n");

    if (!RepeatStartCommonScan()) {
        WIFI_LOGE("failed to start common single scan");
        return false;
    }
    StartTimer(int(SOFTWARE_PNO_SCAN_TIMER), (runningPnoScanConfig.scanInterval) * SECOND_TO_MILLI_SECOND);

    return true;
}

bool ScanStateMachine::RepeatStartCommonScan()
{
    WIFI_LOGI("Enter ScanStateMachine::RepeatStartCommonScan.\n");

    if (!pnoConfigStoredFlag) {
        WIFI_LOGE("Pno config has not stored.");
        return false;
    }

    WifiScanParam scanParam;
    scanParam.scanFreqs.assign(runningScanConfigForPno.scanFreqs.begin(), runningScanConfigForPno.scanFreqs.end());
    scanParam.hiddenNetworkSsid.assign(
        runningScanConfigForPno.hiddenNetworkSsid.begin(), runningScanConfigForPno.hiddenNetworkSsid.end());

    if (!StartSingleCommonScan(scanParam)) {
        PnoScanFailedProcess();
        return false;
    }

    runningSwPnoFlag = true;
    SwitchState(pnoSwScanningState);

    return true;
}

void ScanStateMachine::StopPnoScanSoftware()
{
    WIFI_LOGI("ScanStateMachine::StopPnoScanSoftware.\n");

    if (!runningSwPnoFlag) {
        WIFI_LOGE("Software Pno scan is not running.");
        return;
    }

    StopTimer(int(WAIT_SCAN_RESULT_TIMER));
    /* Stop the PNO software scanning timer. */
    StopTimer(int(SOFTWARE_PNO_SCAN_TIMER));

    runningSwPnoFlag = false;
    return;
}

void ScanStateMachine::PnoScanSoftwareProcess(InternalMessage *interMessage)
{
    WIFI_LOGI("ScanStateMachine::PnoScanSoftwareProcess.\n");

    if (runningSwPnoFlag) {
        WIFI_LOGE("Software Pno scan is running.");
        return;
    }

    if (!GetPnoScanRequestInfo(interMessage)) {
        WIFI_LOGE("GetPnoScanRequestInfo failed.");
        return;
    }

    if (!StartNewSoftwareScan()) {
        WIFI_LOGE("StartPnoScanSoftware failed.");
        return;
    }

    return;
}

void ScanStateMachine::SoftwareScanInfoProcess()
{
    WIFI_LOGI("Enter ScanStateMachine::SoftwareScanInfoProcess.\n");

    std::vector<InterScanInfo> scanInfos;
    if (!GetScanInfos(scanInfos)) {
        WIFI_LOGE("GetScanInfos failed.");
    }

    ReportPnoScanInfos(scanInfos);
    return;
}

bool ScanStateMachine::InitCommonScanState()
{
    WIFI_LOGI("Enter ScanStateMachine::InitCommonScanState.\n");

    initState = new (std::nothrow) InitState(this);
    if (initState == nullptr) {
        WIFI_LOGE("Alloc initState failed.\n");
        return false;
    }

    hardwareReadyState = new (std::nothrow) HardwareReady(this);
    if (hardwareReadyState == nullptr) {
        WIFI_LOGE("Alloc hardwareReadyState failed.\n");
        return false;
    }

    commonScanState = new (std::nothrow) CommonScan(this);
    if (commonScanState == nullptr) {
        WIFI_LOGE("Alloc commonScanState failed.\n");
        return false;
    }

    commonScanUnworkedState = new (std::nothrow) CommonScanUnworked(this);
    if (commonScanUnworkedState == nullptr) {
        WIFI_LOGE("Alloc commonScanUnworkedState failed.\n");
        return false;
    }

    commonScanningState = new (std::nothrow) CommonScanning(this);
    if (commonScanningState == nullptr) {
        WIFI_LOGE("Alloc commonScanningState failed.\n");
        return false;
    }

    return true;
}

bool ScanStateMachine::InitPnoScanState()
{
    WIFI_LOGI("Enter ScanStateMachine::InitPnoScanState.\n");

    pnoScanState = new (std::nothrow) PnoScan(this);
    if (pnoScanState == nullptr) {
        WIFI_LOGE("Alloc pnoScanState failed.\n");
        return false;
    }

    pnoScanHardwareState = new (std::nothrow) PnoScanHardware(this);
    if (pnoScanHardwareState == nullptr) {
        WIFI_LOGE("Alloc pnoScanHardwareState failed.\n");
        return false;
    }

    commonScanAfterPnoState = new (std::nothrow) CommonScanAfterPno(this);
    if (commonScanAfterPnoState == nullptr) {
        WIFI_LOGE("Alloc commonScanAfterPnoState failed.\n");
        return false;
    }

    pnoScanSoftwareState = new (std::nothrow) PnoScanSoftware(this);
    if (pnoScanSoftwareState == nullptr) {
        WIFI_LOGE("Alloc pnoScanSoftwareState failed.\n");
        return false;
    }

    pnoSwScanFreeState = new (std::nothrow) PnoSwScanFree(this);
    if (pnoSwScanFreeState == nullptr) {
        WIFI_LOGE("Alloc pnoSwScanFreeState failed.\n");
        return false;
    }

    pnoSwScanningState = new (std::nothrow) PnoSwScanning(this);
    if (pnoSwScanningState == nullptr) {
        WIFI_LOGE("Alloc pnoSwScanningState failed.\n");
        return false;
    }

    return true;
}

void ScanStateMachine::BuildScanStateTree()
{
    WIFI_LOGI("Enter ScanStateMachine::BuildScanStateTree.\n");

    StatePlus(initState, nullptr);
    StatePlus(hardwareReadyState, initState);
    StatePlus(commonScanState, hardwareReadyState);
    StatePlus(commonScanUnworkedState, commonScanState);
    StatePlus(commonScanningState, commonScanState);
    StatePlus(pnoScanState, hardwareReadyState);
    StatePlus(pnoScanHardwareState, pnoScanState);
    StatePlus(commonScanAfterPnoState, pnoScanHardwareState);
    StatePlus(pnoScanSoftwareState, pnoScanState);
    StatePlus(pnoSwScanFreeState, pnoScanSoftwareState);
    StatePlus(pnoSwScanningState, pnoScanSoftwareState);
}

void ScanStateMachine::InitState::LoadDriver()
{
    WIFI_LOGI("Enter ScanStateMachine::LoadDriver.\n");
    pScanStateMachine->SwitchState(pScanStateMachine->hardwareReadyState);
    pScanStateMachine->ReportStatusChange(SCAN_STARTED_STATUS);
    WIFI_LOGI("Start Scan Service Success.\n");
}

void ScanStateMachine::InitState::UnLoadDriver()
{
    WIFI_LOGI("Enter ScanStateMachine::UnLoadDriver.\n");
    pScanStateMachine->SwitchState(pScanStateMachine->initState);
    pScanStateMachine->quitFlag = true;
    WIFI_LOGI("Stop Scan Service Success.\n");
}
}  // namespace Wifi
}  // namespace OHOS