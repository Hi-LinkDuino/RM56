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
#ifndef OHOS_SCAN_STATE_MACHINE_H
#define OHOS_SCAN_STATE_MACHINE_H

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "wifi_log.h"
#include "wifi_error_no.h"
#include "wifi_scan_param.h"
#include "scan_common.h"
#include "state_machine.h"
#include "wifi_msg.h"
#include "wifi_errcode.h"

namespace OHOS {
namespace Wifi {
const int SCAN_TYPE_LOW_SPAN = 0;
const int SCAN_TYPE_LOW_POWER = 1;
const int SCAN_TYPE_HIGH_ACCURACY = 2;
const int SCAN_TYPE_INVALID = 0xFF;
const int MAX_WAIT_SCAN_RESULT_TIME = 60 * 1000;
const int SCAN_24GHZ_MAX_FREQUENCY = 2500;
const int SCAN_5GHZ_MIN_FREQUENCY = 5000;
const int SCAN_24GHZ_BAND = 1;
const int SCAN_5GHZ_BAND = 2;

class ScanStateMachine : public StateMachine {
    FRIEND_GTEST(ScanStateMachine);
public:
    ScanStateMachine();
    ~ScanStateMachine();
    /**
    * @Description  Initialize the scanning state machine,construct the state tree,
                    set the initialization state, and start the state machine.
    *
    * @return success: true, failed: false
    */
    bool InitScanStateMachine();
    /**
     * @Description  Registers the scan status change callback function for ScanService.
     *
     * @param handler - Function handle[in]
     * @return success: true, failed: false
     */
    bool EnrollScanStatusListener(ScanStatusReportHandler handler);

    class InitState : public State {
        FRIEND_GTEST(ScanStateMachine);
    public:
        /**
         * @Description  Initial state of the scanning state machine
         *
         */
        explicit InitState(ScanStateMachine *paraScanStateMachine);
        ~InitState();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
        /**
         * @Description  load hardware
         *
         */
        void LoadDriver();
        /**
         * @Description  unload hardware
         *
         */
        void UnLoadDriver();
    };

    class HardwareReady : public State {
    public:
        /**
         * @Description  The hardware is successfully loaded. The scanning request can be processed.
         *
         */
        explicit HardwareReady(ScanStateMachine *paraScanStateMachine);
        ~HardwareReady();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class CommonScan : public State {
    public:
        /**
         * @Description  Normal Scan Status.
         *
         */
        explicit CommonScan(ScanStateMachine *paraScanStateMachine);
        ~CommonScan();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class CommonScanUnworked : public State {
    public:
        /**
         * @Description  Normal Scan Idle State.
         *
         */
        explicit CommonScanUnworked(ScanStateMachine *paraScanStateMachine);
        ~CommonScanUnworked();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class CommonScanning : public State {
    public:
        /**
         * @Description  Normal scan in progress.
         *
         */
        explicit CommonScanning(ScanStateMachine *paraScanStateMachine);
        ~CommonScanning();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class PnoScan : public State {
    public:
        /**
         * @Description  PNO scanning status
         *
         */
        explicit PnoScan(ScanStateMachine *paraScanStateMachine);
        ~PnoScan();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class PnoScanHardware : public State {
    public:
        /**
         * @Description  PNO hardware scanning status.
         *
         */
        explicit PnoScanHardware(ScanStateMachine *paraScanStateMachine);
        ~PnoScanHardware();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class CommonScanAfterPno : public State {
    public:
        /**
         * @Description  Single scanning status after PNO scanning.
         *
         */
        explicit CommonScanAfterPno(ScanStateMachine *paraScanStateMachine);
        ~CommonScanAfterPno();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class PnoScanSoftware : public State {
    public:
        /**
         * @Description  PNO software scanning status.
         *
         */
        explicit PnoScanSoftware(ScanStateMachine *paraScanStateMachine);
        ~PnoScanSoftware();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class PnoSwScanFree : public State {
    public:
        /**
         * @Description  PNO software scans the idle state.
         *
         */
        explicit PnoSwScanFree(ScanStateMachine *paraScanStateMachine);
        ~PnoSwScanFree();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

    class PnoSwScanning : public State {
    public:
        /**
         * @Description  PNO software scanning status.
         *
         */
        explicit PnoSwScanning(ScanStateMachine *paraScanStateMachine);
        ~PnoSwScanning();
        void GoInState();
        void GoOutState();
        bool ExecuteStateMsg(InternalMessage *msg);

    private:
        ScanStateMachine *pScanStateMachine;
    };

private:
    bool quitFlag;                               /* Scanning state machine exit flag */
    InitState *initState;                        /* Scanning initial status pointer */
    HardwareReady *hardwareReadyState;           /* Pointer to the hardware startup completion status */
    CommonScan *commonScanState;                 /* Pointer to the common scanning status */
    CommonScanUnworked *commonScanUnworkedState; /* Pointer to the common scanning idle state */
    CommonScanning *commonScanningState;         /* Pointer to the common scanning status */
    PnoScan *pnoScanState;                       /* PNO scanning status */
    PnoScanHardware *pnoScanHardwareState;       /* PNO hardware scanning status */
    CommonScanAfterPno *commonScanAfterPnoState; /*
                                                  * After obtaining the scanning result,
                                                  * determine whether to perform a single scan
                                                  */
    PnoScanSoftware *pnoScanSoftwareState;       /* PNO software scanning status */
    PnoSwScanFree *pnoSwScanFreeState;           /* PNO software scans the idle state */
    PnoSwScanning *pnoSwScanningState;           /* PNO software scanning status */

    std::map<int, InterScanConfig> runningScans;     /*
                                                      * Saves the parameter information about the scan that is
                                                      * being performed
                                                      */
    std::map<int, InterScanConfig> waitingScans;     /* Saves the parameters to be scanned */
    WifiScanParam runningScanSettings;               /*
                                                      * Parameter information about the scan
                                                      * that is being performed
                                                      */
    bool runningFullScanFlag;                        /* Full scan is in progress. */
    ScanStatusReportHandler scanStatusReportHandler; /* Status reporting callback function */

    bool supportHwPnoFlag;                   /* Whether to support hardware PNO scanning */
    bool pnoConfigStoredFlag;                /* The PNO configuration information has been saved */
    bool runningHwPnoFlag;                   /* PNO scanning is in progress. */
    bool remainWaitResultTimer;              /* Waiting for scanning results timer */
    PnoScanConfig runningPnoScanConfig;      /* PNO scan configuration in progress */
    InterScanConfig runningScanConfigForPno; /*
                                              * Common scan configuration for
                                              * ongoing PNO scans
                                              */
    bool runningSwPnoFlag;                   /* Software PNO scanning is in progress. */

    /**
     * @Description  Processing of Scan Requests Received in Idle State.
     *
     * @param interMessage - internal message
     */
    void CommonScanRequestProcess(InternalMessage *interMessage);
    /**
     * @Description  Obtains the scanning request parameters carried in an internal message.
     *
     * @param interMessage - internal message[in]
     * @param requestIndex - Request index carried in the message[out]
     * @param scanConfig - Scan configuration carried in the message[out]
     * @return success - true, failed - false
     */
    bool GetCommonScanRequestInfo(InternalMessage *interMessage, int &requestIndex, InterScanConfig &scanConfig);
    /**
     * @Description  Obtains the scanning configuration carried in an internal message.
     *
     * @param interMessage - internal message[in]
     * @param scanConfig - Scan configuration carried in the message[out]
     * @return success - true, failed - false
     */
    bool GetCommonScanConfig(InternalMessage *interMessage, InterScanConfig &scanConfig);
    /**
     * @Description  Start a common scan.
     *
     */
    void StartNewCommonScan();
    /**
     * @Description  Clears the scanning information in progress.
     *
     */
    void ClearRunningScanSettings();
    /**
     * @Description  Starting a Single Common Scan.
     *
     * @param scanParam - Scanning parameters[in]
     * @return success: true, failed: false
     */
    bool StartSingleCommonScan(WifiScanParam &scanParam);
    /**
     * @Description  Processing of new scan requests received while scanning.
     *
     * @param interMessage - internal Message[in]
     */
    void CommonScanWhenRunning(InternalMessage *interMessage);
    /**
    * @Description  Compare the parameters of the current scan with those carried in
                    the new scan request. If the current scan can overwrite the new
                    scan, obtain the result from the current scan. Otherwise, start
                    the new scan.
    *
    * @param interScanConfig - Parameters in the scan request[in]
    * @return success: true, failed: false
    */
    bool ActiveCoverNewScan(InterScanConfig &interScanConfig);
    /**
     * @Description  Processing after the ScanMonitor scan success message is received.
     *
     */
    void CommonScanInfoProcess();
    /**
     * @Description The band and security type are parsed from the scanning result obtained by the IDL.
     * 
     * @param scanInfo - scan result[inout]
     */
    void GetSecurityTypeAndBand(std::vector<InterScanInfo> &scanInfo);
    /**
     * @Description  Reporting Status to ScanService.
     *
     * @param status - status code[in]
     */
    void ReportStatusChange(ScanStatus status);
    /**
     * @Description  Reports internal events to the ScanService. This parameter is used only for timers.
     *
     * @param innerEvent - Internal Event Name[in]
     */
    void ReportScanInnerEvent(ScanInnerEventType innerEvent);
    /**
     * @Description  Reporting Common Scan Failures to ScanService.
     *
     * @param requestIndex - Request index included in the failure[in]
     */
    void ReportCommonScanFailed(int requestIndex);
    /**
    * @Description  Replying the operation failure message and clearing the request list.
    *
    * @param runningFlag - If the value is true, the running list is cleared.
                        If the value is false, the waiting list is cleared.[in]
    */
    void ReportCommonScanFailedAndClear(bool runningFlag);
    /**
    * @Description  delete runningScans and waitingScans corresponding request.
    *
    * @param requestIndex - The specified element in the map set is deleted
                            based on the transferred key[in]
    */
    void RemoveCommonScanRequest(int requestIndex);
    /**
     * @Description  Gets the index of an ongoing scan request.
     *
     * @param runningIndexList - List of scan requests in progress[out]
     */
    void GetRunningIndexList(std::vector<int> &runningIndexList);
    /**
     * @Description  Gets the index of the scanning request that is waiting.
     *
     * @param waitingIndexList - List of pending scan request indexes[out]
     */
    void GetWaitingIndexList(std::vector<int> &waitingIndexList);
    /**
     * @Description  Check whether the scanstyle is valid.
     *
     * @param scanStyle - style of scan[in]
     * @return success:true, failed:false
     */
    bool VerifyScanStyle(int scanStyle);
    /**
    * @Description  If the new scan request is SCAN_TYPE_HIGH_ACCURACY and the
                    current scan request is not, a new scan needs to be started.
    *
    * @param scanStyle - style of scan[in]
    * @return success:true, failed:false
    */
    bool ActiveScanStyle(int scanStyle);
    /**
    * @Description  If there are SCAN_TYPE_HIGH_ACCURACY requests,
                    the combined requests are SCAN_TYPE_HIGH_ACCURACY requests.
    *
    * @param currentScanStyle - current style of scan[in]
    * @param newScanStyle - new style of scan[in]
    * @return scantyle
    */
    int MergeScanStyle(int currentScanStyle, int newScanStyle);
    /**
     * @Description  Processing the received PNO scanning request.
     *
     * @param msg - internal message[in]
     */
    void PnoScanRequestProcess(InternalMessage *interMessage);
    /**
    * @Description  If the PNO scanning is interrupted by a single scan,
                    resume the PNO scanning after the single scan is complete.
    *
    */
    void ContinuePnoScanProcess();
    /**
     * @Description  Hardware PNO scanning.
     *
     * @param msg - internal message[in]
     */
    void PnoScanHardwareProcess(InternalMessage *interMessage);
    /**
     * @Description  Start hardware PNO scanning.
     *
     * @return success: true, failed: false
     */
    bool StartPnoScanHardware();
    /**
     * @Description  Stop hardware PNO scanning.
     *
     */
    void StopPnoScanHardware();
    /**
    * @Description  Obtain the PNO scanning request parameters carried in the internal message
                    and update the parameters to the saved configuration.
    *
    * @param interMessage - internal message[in]
    */
    void UpdatePnoScanRequest(InternalMessage *interMessage);
    /**
    * @Description  Obtain the PNO scanning request parameters carried in the internal message
                    and update the parameters to the saved configuration.
    *
    * @param interMessage - internal message[in]
    * @return success: true, failed: false
    */
    bool GetPnoScanRequestInfo(InternalMessage *interMessage);
    /**
     * @Description  Obtains the scanning configuration carried in an internal message.
     *
     * @param interMessage - internal message[in]
     * @param pnoScanConfig - PNO scanning configuration carried in the message[out]
     * @return success: true, failed: false
     */
    bool GetPnoScanConfig(InternalMessage *interMessage, PnoScanConfig &pnoScanConfig);
    /**
     * @Description  Processing after receiving a PNO scan success message from the ScanMonitor.
     *
     */
    void HwPnoScanInfoProcess();
    /**
     * @Description  Send the scanning result to ScanService.
     *
     * @param scanInfos - pno scan result
     */
    void ReportPnoScanInfos(std::vector<InterScanInfo> &scanInfos);
    /**
     * @Description  Determine whether to start a common scan after hardware PNO.
     *
     * @param scanInfos - PNO scanning result
     * @return success:true, failed:false
     */
    bool NeedCommonScanAfterPno(std::vector<InterScanInfo> &scanInfos);
    /**
     * @Description  Common scanning after hardware PNO.
     *
     */
    void CommonScanAfterPnoProcess();
    /**
     * @Description  Processing of a success message received after PNO scanning.
     *
     */
    void CommonScanAfterPnoResult();
    /**
     * @Description  Clear PNO Scan Configuration.
     *
     */
    void ClearPnoScanConfig();
    /**
     * @Description  PNO Scan Failure Handling.
     *
     */
    void PnoScanFailedProcess();
    /**
     * @Description  Obtain the scanning result from the IDL.
     *
     * @param scanInfos - scan result[out]
     * @return success:true, failed:false
     */
    bool GetScanInfos(std::vector<InterScanInfo> &scanInfos);
    /**
     * @Description  start PNO Software Scanning.
     *
     * @return success:true, failed:false
     */
    bool StartNewSoftwareScan();
    /**
     * @Description  This function is called periodically during PNO software scanning.
     *
     * @return success:true, failed:false
     */
    bool RepeatStartCommonScan();
    /**
     * @Description  Stopping PNO Software Scanning.
     *
     */
    void StopPnoScanSoftware();
    /**
     * @Description  When the PNO software scanning is started, another PNO software
     *               scanning request is sent.
     *
     * @param interMessage - internal message
     */
    void PnoScanSoftwareProcess(InternalMessage *interMessage);
    /**
     * @Description  Invoke the IDL client to obtain the software scanning result.
     *
     */
    void SoftwareScanInfoProcess();
    /**
     * @Description  Constructing the State Tree of the Scan State Machine.
     *
     * @return success: true, failed: false
     */
    bool InitCommonScanState();
    /**
     * @Description  Constructing the State Tree of the Scan State Machine.
     *
     * @return success: true, failed: false
     */
    bool InitPnoScanState();
    /**
     * @Description  Constructing the State Tree of the Scan State Machine.
     *
     */
    void BuildScanStateTree();
};
}  // namespace Wifi
}  // namespace OHOS

#endif