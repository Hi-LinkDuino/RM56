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

#include "stream_operator_callback_wrapper.h"
#ifdef __cplusplus
extern "C" {
#endif

static OHOS::sptr<OHOS::Camera::IStreamOperatorCallback> g_streamCallback;

void BindStreamOperatorCallback(const OHOS::sptr<OHOS::Camera::IStreamOperatorCallback>& callback)
{
    g_streamCallback = callback;
}

void StreamCBOnCaptureStarted(int captureId, int* streamId, int count)
{
    if (g_streamCallback == nullptr) {
        return;
    }
    std::vector<int> ids = {};
    for (int i = 0; i < count; i++) {
        ids.push_back(streamId[i]);
    }

    g_streamCallback->OnCaptureStarted(captureId, ids);
}

void StreamCBOnCaptureEnded(int captureId, CaptureEndedInfoCIF* info, int count)
{
    if (g_streamCallback == nullptr) {
        return;
    }

    std::vector<std::shared_ptr<OHOS::Camera::CaptureEndedInfo>> ends;
    for (int i = 0; i < count; i++) {
        std::shared_ptr<OHOS::Camera::CaptureEndedInfo> it = std::make_shared<OHOS::Camera::CaptureEndedInfo>();
        it->streamId_ = info[i].streamId;
        it->frameCount_ = info[i].frameCount;
        ends.push_back(it);
    }

    g_streamCallback->OnCaptureEnded(captureId, ends);

    return;
}

void StreamCBOnCaptureError(int captureId, CaptureErrorInfoCIF* info, int count)
{
    if (g_streamCallback == nullptr) {
        return;
    }

    std::vector<std::shared_ptr<OHOS::Camera::CaptureErrorInfo>> errors = {};
    for (int i = 0; i < count; i++) {
        std::shared_ptr<OHOS::Camera::CaptureErrorInfo> it = std::make_shared<OHOS::Camera::CaptureErrorInfo>();
        it->streamId_ = info[i].streamId;
        it->error_ = static_cast<OHOS::Camera::StreamError>(info[i].error);
        errors.push_back(it);
    }

    g_streamCallback->OnCaptureError(captureId, errors);

    return;
}

void StreamCBOnFrameShutter(int captureId, int* streamId, int count, long timestamp)
{
    if (g_streamCallback == nullptr) {
        return;
    }

    if (streamId == nullptr) {
        return;
    }

    std::vector<int> ids = {};
    for (int i = 0; i < count; i++) {
        ids.push_back(streamId[i]);
    }
    g_streamCallback->OnFrameShutter(captureId, ids, timestamp);

    return;
}

#ifdef __cplusplus
}
#endif
