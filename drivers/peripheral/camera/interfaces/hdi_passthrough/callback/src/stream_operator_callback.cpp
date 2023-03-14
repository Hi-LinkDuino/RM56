/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <hdf_log.h>
#include "stream_operator_callback.h"

namespace OHOS::Camera {
void StreamOperatorCallback::OnCaptureStarted(int32_t captureId,
    const std::vector<int32_t> &streamId)
{
    HDF_LOGV("%{public}s, enter.", __func__);
}

void StreamOperatorCallback::OnCaptureEnded(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureEndedInfo>> &info)
{
    HDF_LOGV("%{public}s, enter.", __func__);
}

void StreamOperatorCallback::OnCaptureError(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureErrorInfo>> &info)
{
    HDF_LOGV("%{public}s, enter.", __func__);
}

void StreamOperatorCallback::OnFrameShutter(int32_t captureId,
    const std::vector<int32_t> &streamId, uint64_t timestamp)
{
    HDF_LOGV("%{public}s, enter.", __func__);
}
}
