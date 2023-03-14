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
#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_UTIL_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_UTIL_H

#include <chrono>
#include <vector>

#include "aging/aging_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
class AgingUtil {
public:
    static inline int64_t GetNowSysTimeMs()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
    static void SortAgingBundles(std::vector<AgingBundleInfo> &bundles);
    static bool SortTwoAgingBundleInfos(AgingBundleInfo &bundle1, AgingBundleInfo &bundle2);
    static int64_t GetUnusedTimeMsBaseOnCurrentTime(int64_t currentTimeMs, int32_t days);
};
}  //  namespace AppExecFwk
}  //  namespace OHOS
#endif  //  FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_UTIL_H
