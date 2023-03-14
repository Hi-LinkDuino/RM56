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

#ifndef OHOS_SCAN_MONITOR_H
#define OHOS_SCAN_MONITOR_H

#include "wifi_log.h"
#include "wifi_error_no.h"
#include "wifi_idl_define.h"
#include "scan_common.h"
#include "scan_state_machine.h"

namespace OHOS {
namespace Wifi {
class ScanMonitor {
    FRIEND_GTEST(ScanMonitor);
public:
    ScanMonitor();
    ~ScanMonitor();
    /**
     * @Description  Scan monitor initialization function.
     *
     * @return success: true, failed: false
     */
    bool InitScanMonitor();
    /**
     * @Description  scanMonitor needs to send messages to the scan state machine.
     *
     * @param paraScanStateMachine - scan state machine[in]
     */
    void SetScanStateMachine(ScanStateMachine *paraScanStateMachine);
    /**
     * @Description  Receive an event notification.
     *
     * @param result - Received events[in]
     */
    void ReceiveScanEventFromIdl(int result);

private:
    ScanStateMachine *pScanStateMachine;

    /**
     * @Description  Send Scan Info.
     *
     */
    void SendScanInfoEvent();
    /**
     * @Description  Send Pno scanning results.
     *
     */
    void SendPnoScanInfoEvent();
    /**
     * @Description  Sending Scan Failure Events.
     *
     */
    void SendScanFailedEvent();
    /**
     * @Description  Processing Event Notifications.
     *
     * @param result - Received events[in]
     */
    void ProcessReceiveScanEvent(int result);
};
}  // namespace Wifi
}  // namespace OHOS
#endif