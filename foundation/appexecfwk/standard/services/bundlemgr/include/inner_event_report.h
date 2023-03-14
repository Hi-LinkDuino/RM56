/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_EVENT_REPORT_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_EVENT_REPORT_H

#include <unordered_map>

#include "event_report.h"

namespace OHOS {
namespace AppExecFwk {
class InnerEventReport {
public:
    /**
     * @brief Send system events.
     * @param eventType Indicates the bms eventInfo.
     * @param eventInfo Indicates the eventInfo.
     */
    static void SendSystemEvent(BMSEventType eventType, const EventInfo& eventInfo);

private:
    // fault event
    static void InnerSendBundleInstallExceptionEvent(const EventInfo& eventInfo);
    static void InnerSendBundleUninstallExceptionEvent(const EventInfo& eventInfo);
    static void InnerSendBundleUpdateExceptionEvent(const EventInfo& eventInfo);
    static void InnerSendPreBundleRecoverExceptionEvent(const EventInfo& eventInfo);
    static void InnerSendBundleStateChangeExceptionEvent(const EventInfo& eventInfo);
    static void InnerSendBundleCleanCacheExceptionEvent(const EventInfo& eventInfo);

    // behavior event
    static void InnerSendBootScanStartEvent(const EventInfo& eventInfo);
    static void InnerSendBootScanEndEvent(const EventInfo& eventInfo);
    static void InnerSendBundleInstallEvent(const EventInfo& eventInfo);
    static void InnerSendBundleUninstallEvent(const EventInfo& eventInfo);
    static void InnerSendBundleUpdateEvent(const EventInfo& eventInfo);
    static void InnerSendPreBundleRecoverEvent(const EventInfo& eventInfo);
    static void InnerSendBundleStateChangeEvent(const EventInfo& eventInfo);
    static void InnerSendBundleCleanCacheEvent(const EventInfo& eventInfo);

    template<typename... Types>
    static void InnerEventWrite(const std::string &eventName,
        HiSysEventType type, Types... keyValues);

    static std::unordered_map<BMSEventType, void (*)(const EventInfo& eventInfo)> bmsSysEventMap_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INNER_EVENT_REPORT_H
