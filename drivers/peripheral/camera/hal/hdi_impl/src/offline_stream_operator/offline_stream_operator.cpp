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

#include "offline_stream_operator.h"
#include "watchdog.h"

namespace OHOS::Camera {
OfflineStreamOperator::OfflineStreamOperator()
{
    CAMERA_LOGV("ctor, instance = %{public}p", this);
}

OfflineStreamOperator::~OfflineStreamOperator()
{
    CAMERA_LOGV("dtor, instance = %{public}p", this);
    offlineStreamMap_.clear();
}

CamRetCode OfflineStreamOperator::CancelCapture(int captureId)
{
    CHECK_IF_EQUAL_RETURN_VALUE(captureId < 0, true, INVALID_ARGUMENT);

    PLACE_A_SELFKILL_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;

    std::shared_ptr<OfflineStream> stream = FindStreamByCaptureId(captureId);
    if (stream == nullptr) {
        CAMERA_LOGD("can't find stream by captureId %{public}d, buffer all returned.", captureId);
        return NO_ERROR;
    }
    RetCode ret = stream->CancelCapture(captureId);
    if (ret != RC_OK) {
        CAMERA_LOGE("cancel captureId %{public}d failed", captureId);
        return DEVICE_ERROR;
    }

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode OfflineStreamOperator::ReleaseStreams(const std::vector<int>& streamIds)
{
    PLACE_A_SELFKILL_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;

    for (auto it : streamIds) {
        RetCode ret = offlineStreamMap_[it]->Release();
        if (ret != RC_OK) {
            CAMERA_LOGE("release stream %{public}d failed", it);
        }

        {
            std::lock_guard<std::mutex> l(lock_);
            offlineStreamMap_.erase(it);
        }
    }

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

CamRetCode OfflineStreamOperator::Release()
{
    PLACE_A_SELFKILL_WATCHDOG;
    DFX_LOCAL_HITRACE_BEGIN;

    {
        std::lock_guard<std::mutex> l(lock_);
        for (auto it = offlineStreamMap_.begin(); it != offlineStreamMap_.end(); it++) {
            it->second->Release();
        }

        offlineStreamMap_.clear();
    }

    DFX_LOCAL_HITRACE_END;
    return NO_ERROR;
}

RetCode OfflineStreamOperator::CommitOfflineStream(const std::shared_ptr<OfflineStream>& of)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(of, RC_ERROR);
    {
        std::lock_guard<std::mutex> l(lock_);
        offlineStreamMap_[of->GetStreamId()] = of;
    }
    return RC_OK;
}

std::shared_ptr<OfflineStream> OfflineStreamOperator::FindStreamByCaptureId(int32_t captureId)
{
    std::shared_ptr<OfflineStream> stream = nullptr;
    {
        std::lock_guard<std::mutex> l(lock_);
        for (auto it = offlineStreamMap_.begin(); it != offlineStreamMap_.end(); it++) {
            if (it->second->CheckCaptureIdExist(captureId)) {
                stream = it->second;
            }
        }
    }
    return stream;
}
} // end namespace OHOS::Camera
