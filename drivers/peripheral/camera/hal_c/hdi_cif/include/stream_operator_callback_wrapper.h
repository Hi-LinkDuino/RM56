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

#ifndef HOS_CAMERA_STREAM_OPERATOR_CALLBACK_WRAPPER_H
#define HOS_CAMERA_STREAM_OPERATOR_CALLBACK_WRAPPER_H

#include <memory>
#include "types.h"
#include "camera_types_c_if.h"
#include "istream_operator_callback.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void BindStreamOperatorCallback(const OHOS::sptr<OHOS::Camera::IStreamOperatorCallback>& callback);
extern void StreamCBOnCaptureStarted(int captureId, int* streamId, int count);
extern void StreamCBOnCaptureEnded(int captureId, CaptureEndedInfoCIF* info, int count);
extern void StreamCBOnCaptureError(int captureId, CaptureErrorInfoCIF* info, int count);
extern void StreamCBOnFrameShutter(int captureId, int* streamId, int count, long timestamp);

#ifdef __cplusplus
}
#endif

#endif
