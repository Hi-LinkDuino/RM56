/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef HOS_CAMERA_STREAM_CUSTOMER_H
#define HOS_CAMERA_STREAM_CUSTOMER_H

#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <hdf_log.h>
#include <surface.h>
#include <display_type.h>
#include "camera_metadata_info.h"
#include "ibuffer.h"
#include "ioffline_stream_operator.h"
#include "camera.h"
#ifndef CAMERA_BUILT_ON_OHOS_LITE
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#endif

namespace OHOS::Camera {
class StreamCustomer {
public:
    StreamCustomer();
    ~StreamCustomer();
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> CreateProducer();
#else
    sptr<OHOS::IBufferProducer> CreateProducer();
#endif
    void CamFrame(const std::function<void(void*, const uint32_t)> callback);

    RetCode ReceiveFrameOn(const std::function<void(void*, const uint32_t)> callback);
    void ReceiveFrameOff();

#ifndef CAMERA_BUILT_ON_OHOS_LITE
    class TestBuffersConsumerListener: public IBufferConsumerListener {
    public:
        TestBuffersConsumerListener()
        {
        }

        ~TestBuffersConsumerListener()
        {
        }

        void OnBufferAvailable()
        {
        }
    };
#endif

private:
    unsigned int camFrameExit_ = 1;

#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> consumer_ = nullptr;
#else
    sptr<OHOS::Surface> consumer_ = nullptr;
#endif
    std::thread* previewThreadId_ = nullptr;
};
} // namespace OHOS::Camera
#endif // HOS_CAMERA_STREAM_CUSTOMER_H
