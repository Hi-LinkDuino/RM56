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

#ifndef HDI_DRM_LAYER_H
#define HDI_DRM_LAYER_H
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "buffer_handle.h"
#include "display_common.h"
#include "drm_fourcc.h"
#include "hdi_layer.h"
#include "hdi_device_common.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
const int INVALID_DRM_ID = 0;
class DrmGemBuffer {
public:
    DrmGemBuffer(int drmFd, HdiLayerBuffer &hdl);
    virtual ~DrmGemBuffer();
    uint32_t GetFbId() const
    {
        return mFdId;
    }
    bool IsValid();

private:
    void Init(int drmFd, HdiLayerBuffer &hdl);
    uint32_t mGemHandle = 0;
    uint32_t mFdId = 0;
    int mDrmFd = -1; // the fd can not close. the other module will close it.
    uint32_t mDrmFormat = DRM_FORMAT_INVALID;
};

class HdiDrmLayer : public HdiLayer {
public:
    explicit HdiDrmLayer(LayerType type) : HdiLayer(type) {}
    virtual ~HdiDrmLayer() {}
    // Return value optimization
    DrmGemBuffer *GetGemBuffer();

private:
    std::unique_ptr<DrmGemBuffer> mCurrentBuffer;
    std::unique_ptr<DrmGemBuffer> lastBuffer_;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // HDI_DRM_LAYER_H
