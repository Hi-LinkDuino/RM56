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

#include "wifi_scan_callback_proxy.h"
#include "wifi_logger.h"
#include "define.h"
#include "wifi_internal_event_dispatcher.h"

DEFINE_WIFILOG_SCAN_LABEL("WifiScanCallbackProxy");

namespace OHOS {
namespace Wifi {
WifiScanCallbackProxy::WifiScanCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IWifiScanCallback>(impl)
{}

WifiScanCallbackProxy::~WifiScanCallbackProxy()
{}

void WifiScanCallbackProxy::OnWifiScanStateChanged(int state)
{
    WIFI_LOGD("OnWifiScanStateChanged, state:%{public}d", state);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(state);
    int error = Remote()->SendRequest(WIFI_CBK_CMD_SCAN_STATE_CHANGE, data, reply, option);
    switch (error) {
        case NO_ERROR:
            WIFI_LOGD("OnWifiScanStateChanged callback sucessed!");
            break;
        case DEAD_OBJECT: {
            WIFI_LOGE("Failed to SendRequest, remote object has dead!");
            if (WifiInternalEventDispatcher::GetInstance().HasScanRemote(Remote())) {
                WifiInternalEventDispatcher::GetInstance().RemoveScanCallback(Remote());
            }
            break;
        }
        default: {
            WIFI_LOGE("OnWifiScanStateChanged,connect done failed, error: %{public}d!", error);
            break;
        }
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi scan state failed!");
    }
    return;
}
}  // namespace Wifi
}  // namespace OHOS