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

#include "wifi_scan_callback_stub.h"
#include "define.h"
#include "wifi_hisysevent.h"
#include "wifi_logger.h"
#include "wifi_msg.h"

DEFINE_WIFILOG_SCAN_LABEL("WifiScanCallbackStub");
namespace OHOS {
namespace Wifi {
WifiScanCallbackStub::WifiScanCallbackStub() : userCallback_(nullptr), mRemoteDied(false)
{}

WifiScanCallbackStub::~WifiScanCallbackStub()
{}

int WifiScanCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("WifiScanCallbackStub::OnRemoteRequest code:%{public}u!", code);
    if (mRemoteDied) {
        WIFI_LOGD("Failed to `%{public}s`,remote service is died!", __func__);
        return -1;
    }

    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WIFI_LOGE("Scan callback stub token verification error");
        return WIFI_OPT_FAILED;
    }

    int exception = data.ReadInt32();
    if (exception) {
        WIFI_LOGD("WifiScanCallbackStub::OnRemoteRequest exception! %{public}d!", exception);
        return WIFI_OPT_FAILED;
    }
    int ret = -1;
    switch (code) {
        case WIFI_CBK_CMD_SCAN_STATE_CHANGE: {
            WIFI_LOGD("WifiScanCallbackStub::OnRemoteRequest code:%{public}u line:%{public}d!", code, __LINE__);
            ret = RemoteOnWifiScanStateChanged(code, data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

void WifiScanCallbackStub::RegisterCallBack(const sptr<IWifiScanCallback> &userCallback)
{
    if (userCallback_ != nullptr) {
        WIFI_LOGD("Callback has registered!");
        return;
    }
    userCallback_ = userCallback;
}

bool WifiScanCallbackStub::IsRemoteDied() const
{
    return mRemoteDied;
}

void WifiScanCallbackStub::SetRemoteDied(bool val)
{
    mRemoteDied = val;
}

void WifiScanCallbackStub::OnWifiScanStateChanged(int state)
{
    WIFI_LOGD("WifiScanCallbackStub::OnWifiScanStateChanged,state:%{public}d", state);

    if (userCallback_) {
        userCallback_->OnWifiScanStateChanged(state);
    }
    WriteWifiEventReceivedHiSysEvent(HISYS_STA_SCAN_STATE_CHANGE, state);
}

int WifiScanCallbackStub::RemoteOnWifiScanStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int stateCode = data.ReadInt32();
    OnWifiScanStateChanged(stateCode);
    reply.WriteInt32(0);
    return 0;
}
}  // namespace Wifi
}  // namespace OHOS