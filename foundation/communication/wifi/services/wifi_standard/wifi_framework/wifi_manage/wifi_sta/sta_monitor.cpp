/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include "sta_monitor.h"
#include "wifi_idl_define.h"
#include "sta_define.h"
#include "wifi_logger.h"
#include "wifi_supplicant_hal_interface.h"
#include "wifi_sta_hal_interface.h"

DEFINE_WIFILOG_LABEL("StaMonitor");

namespace OHOS {
namespace Wifi {
StaMonitor::StaMonitor() : pStaStateMachine(nullptr)
{}

StaMonitor::~StaMonitor()
{
    WIFI_LOGI("StaMonitor::~StaMonitor");
}

ErrCode StaMonitor::InitStaMonitor()
{
    WIFI_LOGI("Enter StaMonitor::InitStaMonitor.\n");
    using namespace std::placeholders;
    WifiEventCallback callBack = {
        std::bind(&StaMonitor::OnConnectChangedCallBack, this, _1, _2, _3),
        std::bind(&StaMonitor::OnWpaStateChangedCallBack, this, _1),
        std::bind(&StaMonitor::OnWpaSsidWrongKeyCallBack, this, _1),
        std::bind(&StaMonitor::OnWpsPbcOverlapCallBack, this, _1),
        std::bind(&StaMonitor::OnWpsTimeOutCallBack, this, _1),
        std::bind(&StaMonitor::onWpaConnectionFullCallBack, this, _1),
        std::bind(&StaMonitor::onWpaConnectionRejectCallBack, this, _1)
    };

    if (WifiStaHalInterface::GetInstance().RegisterStaEventCallback(callBack) != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("StaMonitor::InitStaMonitor RegisterStaEventCallback failed!");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode StaMonitor::UnInitStaMonitor() const
{
    WIFI_LOGI("Enter StaMonitor::UnInitStaMonitor.\n");
    WifiEventCallback callBack;
    if (WifiStaHalInterface::GetInstance().RegisterStaEventCallback(callBack) != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("StaMonitor::~StaMonitor RegisterStaEventCallback failed!");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

void StaMonitor::SetStateMachine(StaStateMachine *paraStaStateMachine)
{
    if (paraStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }
    pStaStateMachine = paraStaStateMachine;
    return;
}

void StaMonitor::OnConnectChangedCallBack(int status, int networkId, const std::string &bssid)
{
    WIFI_LOGI("OnConnectChangedCallBack() status:%{public}d,networkId=%{public}d,bssid=%{private}s",
        status,
        networkId,
        bssid.c_str());
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }

    WifiLinkedInfo linkedInfo;
    pStaStateMachine->GetLinkedInfo(linkedInfo);
    /* P2P affects STA, causing problems or incorrect data updates */
    if ((linkedInfo.connState == ConnState::CONNECTED) && (linkedInfo.bssid != bssid)) {
        WIFI_LOGI("Sta ignored the event for bssid is mismatch!");
        return;
    }
    switch (status) {
        case WPA_CB_CONNECTED: {
            pStaStateMachine->OnNetworkConnectionEvent(networkId, bssid);
            break;
        }
        case WPA_CB_DISCONNECTED: {
            pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_NETWORK_DISCONNECTION_EVENT);
            break;
        }
        default:
            break;
    }
}

void StaMonitor::OnWpaStateChangedCallBack(int status)
{
    WIFI_LOGI("OnWpaStateChangedCallBack() status:%{public}d\n", status);
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }
    /* Notification state machine wpa state changed event. */
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_WPA_STATE_CHANGE_EVENT, status);
}

void StaMonitor::OnWpaSsidWrongKeyCallBack(int status)
{
    WIFI_LOGI("OnWpaSsidWrongKeyCallBack() status:%{public}d\n", status);
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }

    if (status != 1) {
        WIFI_LOGE("OnWpaSsidWrongKeyCallBack error.");
        return;
    }
    /* Notification state machine wpa password wrong event. */
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_WPA_PASSWD_WRONG_EVENT);
}

void StaMonitor::onWpaConnectionFullCallBack(int status)
{
    LOGI("onWpaConnectionFullCallBack() status:%d.\n", status);
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }

    /* Notification state machine wpa password wrong event. */
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_WPA_FULL_CONNECT_EVENT);
}

void StaMonitor::onWpaConnectionRejectCallBack(int status)
{
    LOGI("onWpsConnectionRejectCallBack() status:%d.\n", status);
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }

    /* Notification state machine wpa password wrong event. */
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_WPA_ASSOC_REJECT_EVENT);
}

void StaMonitor::OnWpsPbcOverlapCallBack(int status)
{
    WIFI_LOGI("OnWpsPbcOverlapCallBack() status:%{public}d\n", status);
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }
    /* Notification state machine WPS overlap event. */
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_WPS_OVERLAP_EVENT);
}

void StaMonitor::OnWpsTimeOutCallBack(int status)
{
    WIFI_LOGI("OnWpsTimeOutCallBack() status:%{public}d\n", status);
    if (pStaStateMachine == nullptr) {
        WIFI_LOGE("The statemachine pointer is null.");
        return;
    }
    /* Notification state machine WPS timeout event */
    pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_WPS_TIMEOUT_EVNET);
}
}  // namespace Wifi
}  // namespace OHOS