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

#ifndef HOS_CAMERA_OFFLINE_STREAM_CONTEXT_H
#define HOS_CAMERA_OFFLINE_STREAM_CONTEXT_H

#include "camera.h"
#include "capture_message.h"
#include "capture_request.h"
#include "ibuffer_pool.h"
#include "istream_pipeline_core.h"
#include "stream_tunnel.h"
#include "utils.h"
#include <list>
#include <memory>

namespace OHOS::Camera {
class CaptureRequest;
struct OfflineStreamContext {
    StreamConfiguration streamInfo;
    std::shared_ptr<StreamTunnel> tunnel;
    std::shared_ptr<IBufferPool> bufferPool;
    std::weak_ptr<IStreamPipelineCore> pipeline;
    std::list<std::shared_ptr<CaptureRequest>> restRequests;
};
} // namespace OHOS::Camera
#endif
