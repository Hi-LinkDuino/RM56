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
#include "dhcp_api.h"

#include "securec.h"
#include "common_event.h"
#include "common_event_data.h"
#include "common_event_manager.h"

#undef LOG_TAG
#define LOG_TAG "WifiDhcpApi"

using namespace OHOS::EventFwk;

static bool PublishDhcpEvent(const char *action, const int code, const char *data)
{
    Want want;
    want.SetAction(action);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    if (!CommonEventManager::PublishCommonEvent(commonData)) {
        LOGE("PublishDhcpEvent() PublishCommonEvent failed, action:%{public}s, code:%{public}d, data:%{public}s.",
            action, code, data);
        return false;
    }
    LOGI("PublishDhcpEvent() PublishCommonEvent success, action:%{public}s, code:%{public}d, data:%{private}s.",
        action, code, data);
    return true;
}

bool PublishDhcpIpv4ResultEvent(const int code, const char *data, const char *ifname)
{
    char strAction[STRING_MAX_LEN] = {0};
    if (ifname == NULL) {
        if (strncpy_s(strAction, sizeof(strAction), EVENT_GET_IPV4, strlen(EVENT_GET_IPV4)) != EOK) {
            LOGE("PublishDhcpIpv4ResultEvent() strncpy_s %{public}s failed!", EVENT_GET_IPV4);
            return false;
        }
    } else {
        if (snprintf_s(strAction, STRING_MAX_LEN, STRING_MAX_LEN - 1, "%s.%s", EVENT_GET_IPV4, ifname) < 0) {
            LOGE("PublishDhcpIpv4ResultEvent() snprintf_s %{public}s failed!", ifname);
            return false;
        }
    }
    return PublishDhcpEvent(strAction, code, data);
}
