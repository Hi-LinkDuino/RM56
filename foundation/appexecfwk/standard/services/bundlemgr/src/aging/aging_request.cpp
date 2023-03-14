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

#include "aging/aging_request.h"

#include <cinttypes>

#include "aging/aging_constants.h"
#include "app_log_wrapper.h"
#include "parameter.h"

namespace OHOS {
namespace AppExecFwk {
int64_t AgingRequest::totalDataBytesThreshold = AgingConstants::DEFAULT_AGING_DATA_SIZE_THRESHOLD;
int64_t AgingRequest::oneDayTimeMs = AgingConstants::ONE_DAYS_MS;
AgingRequest::AgingRequest()
{
    InitAgingPolicySystemParameters();
}

void AgingRequest::InitAgingPolicySystemParameters()
{
    char szDatasizeThreshold[AgingConstants::THRESHOLD_VAL_LEN] = {0};
    int32_t ret = GetParameter(AgingConstants::SYSTEM_PARAM_DATA_SIZE_THRESHOLD.c_str(), "", szDatasizeThreshold,
        AgingConstants::THRESHOLD_VAL_LEN);
    if (strcmp(szDatasizeThreshold, "") != 0) {
        totalDataBytesThreshold = atoi(szDatasizeThreshold);
    }

    char szOneDayTimeMs[AgingConstants::THRESHOLD_VAL_LEN] = {0};
    ret = GetParameter(AgingConstants::SYSTEM_PARAM_RECENILY_USED_THRESHOLD.c_str(), "", szOneDayTimeMs,
        AgingConstants::THRESHOLD_VAL_LEN);
    if (strcmp(szOneDayTimeMs, "") != 0)
        oneDayTimeMs = atoi(szOneDayTimeMs);
}

bool AgingRequest::IsReachStartAgingThreshold() const
{
    APP_LOGD("tatalDataBytes: %{public}" PRId64 ", totalDataBytesThreshold: %{public}" PRId64, tatalDataBytes,
        totalDataBytesThreshold);
    return tatalDataBytes > totalDataBytesThreshold;
}

bool AgingRequest::IsReachEndAgingThreshold() const
{
    APP_LOGD("tatalDataBytes: %{public}" PRId64 ", totalDataBytesThreshold: %{public}" PRId64, tatalDataBytes,
        totalDataBytesThreshold);
    return tatalDataBytes < (int64_t)(totalDataBytesThreshold * AgingConstants::AGING_SIZE_RATIO);
}

void AgingRequest::AddAgingBundle(AgingBundleInfo &bundleInfo)
{
    agingBundles.emplace_back(bundleInfo);
}

void AgingRequest::RequestReset()
{
    agingBundles.clear();
    tatalDataBytes = 0;
    InitAgingPolicySystemParameters();
}
}  //  namespace AppExecFwk
}  //  namespace OHOS