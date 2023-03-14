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

#ifndef HDI_TEST_LAYER_H
#define HDI_TEST_LAYER_H
#include <queue>
#include "display_gralloc.h"
#include "display_layer.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
class HdiGrallocBuffer {
public:
    HdiGrallocBuffer(uint32_t w, uint32_t h, PixelFormat fmt);
    ~HdiGrallocBuffer();
    BufferHandle *Get() const
    {
        return mBufferHandle;
    }
    void SetReleaseFence(int fd);
    void SetAcquirceFence(int fd);
    int GetAcquireFence() const
    {
        return mAcquireFence;
    }
    int GetReleaseFence() const
    {
        return mReleaseFence;
    }

private:
    BufferHandle *mBufferHandle = nullptr;
    int mAcquireFence = -1;
    int mReleaseFence = -1;
};

class HdiTestLayer {
public:
    HdiTestLayer(LayerInfo &info, uint32_t id, uint32_t displayId);
    virtual ~HdiTestLayer();
    int32_t Init();
    int32_t PreparePresent();

    uint32_t GetId() const
    {
        return mId;
    }
    CompositionType GetCompType() const
    {
        return mCompType;
    }

    HdiGrallocBuffer *GetFrontBuffer();
    HdiGrallocBuffer *GetBackBuffer();
    HdiGrallocBuffer *AcquireBackBuffer();

    int32_t SwapFrontToBackQ();
    int32_t SwapBackToFrontQ();

    void SetLayerSize(IRect &rect);
    void SetLayerCrop(IRect &rect);
    void SetZorder(uint32_t zorder);
    void SetCompType(CompositionType type);
    void SetReleaseFence(int fd);
    void SetAlpha(LayerAlpha alpha);
    void SetBlendType(BlendType type);
    void SetTransform(TransformType transform);

private:
    uint32_t mId;
    uint32_t mDisplayID;
    std::queue<std::unique_ptr<HdiGrallocBuffer>> mFrontBuffers;
    std::queue<std::unique_ptr<HdiGrallocBuffer>> mBackBuffers;
    LayerInfo mLayerInfo = { 0 };

    CompositionType mCompType = COMPOSITION_CLIENT;
    IRect mDisplayRect = { 0 };
    IRect mCropRect = { 0 };
    uint32_t mZorder = 0;
    LayerAlpha mAlpha = { 0 };
    BlendType mBlendType = BLEND_SRC;
    std::unique_ptr<HdiGrallocBuffer> mCurrentBuffer;
    TransformType mTransform = ROTATE_NONE;
};
} // OHOS
} // HDI
} // DISPLAY
} // TEST

#endif // HDI_TEST_LAYER_H