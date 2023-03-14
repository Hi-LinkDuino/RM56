/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_CONSTANTS_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_CONSTANTS_H

#include <string>

namespace OHOS {
namespace AppExecFwk {
namespace AgingConstants {
const std::string SYSTEM_PARAM_DATA_SIZE_THRESHOLD = "persist.sys.bms.aging.policy.data.size.threshold";
const std::string SYSTEM_PARAM_RECENILY_USED_THRESHOLD = "persist.sys.bms.aging.policy.recently.used.threshold";
const std::string SYSTEM_PARAM_AGING_TIMER_INTERVAL = "persist.sys.bms.aging.policy.timer.interval";
const std::string SYSTEM_PARAM_AGING_BATTER_THRESHOLD = "persist.sys.bms.aging.policy.battery.threshold";
const int64_t ONE_SECOND_MS = 1000;
const float AGING_SIZE_RATIO = 0.8F;
const int64_t ONE_HOUR_MS = (int64_t)60 * 60 * ONE_SECOND_MS;
const int64_t DEFAULT_AGING_TIMER_INTERVAL = (int64_t)8 * ONE_HOUR_MS;
const int64_t ONE_DAYS_MS = (int64_t)24 * ONE_HOUR_MS;
const int64_t ONE_KB = (int64_t)1024;
const int64_t ONE_MB = (int64_t)1024 * ONE_KB;
const int64_t DEFAULT_AGING_DATA_SIZE_THRESHOLD = (int64_t)500 * ONE_MB;
const int64_t DEFAULT_AGING_BATTERY_THRESHOLD = (int64_t)10;
const int64_t THRESHOLD_VAL_LEN = 20;
const int32_t TIME_30_DAYS = 30;
const int32_t TIME_20_DAYS = 20;
const int32_t TIME_10_DAYS = 10;
const int32_t COUNT_MODULE_RECODES_GET = 1000;

const std::string AGING_THREAD = "aging_thread";

const std::string UNUSED_FOR_30_DAYS_BUNDLE_AGING_HANDLER = "UnusedFor30DaysBundleAgingHandler";
const std::string UNUSED_FOR_20_DAYS_BUNDLE_AGING_HANDLER = "UnusedFor20DaysBundleAgingHandler";
const std::string UNUSED_FOR_10_DAYS_BUNDLE_AGING_HANDLER = "UnusedFor10DaysBundleAgingHandler";
const std::string BUNDLE_DATA_SIZE_AGING_HANDLER = "BundleDataSizeAgingHandler";
}  // namespace AgingConstants
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_CONSTANTS_H
