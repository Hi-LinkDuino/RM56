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

#include "hdi_test_layer.h"
#include "hdi_test_device.h"
namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
HdiGrallocBuffer::HdiGrallocBuffer(uint32_t w, uint32_t h, PixelFormat fmt)
{
    GrallocFuncs &gralloc = HdiTestDevice::GetInstance().GetGrallocFuncs();
    AllocInfo info = { 0 };
    info.width = w;
    info.height = h;
    info.usage = HBM_USE_MEM_DMA | HBM_USE_CPU_READ | HBM_USE_CPU_WRITE;
    info.format = fmt;

    int ret = gralloc.AllocMem(&info, &mBufferHandle);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_TEST_LOGE("can not alloc memory");
    }
    void *vaddr = gralloc.Mmap(mBufferHandle);
    if (vaddr == nullptr) {
        DISPLAY_TEST_LOGE("mmap failed");
    }
}

HdiGrallocBuffer::~HdiGrallocBuffer()
{
    if (mBufferHandle != nullptr) {
        GrallocFuncs &gralloc = HdiTestDevice::GetInstance().GetGrallocFuncs();
        if (mBufferHandle->virAddr != nullptr) {
            int ret = gralloc.Unmap(mBufferHandle);
            if (ret != DISPLAY_SUCCESS) {
                DISPLAY_TEST_LOGE("can not ummap buffer handle");
            }
        }
        gralloc.FreeMem(mBufferHandle);
        mBufferHandle = nullptr;
    }
    if (mReleaseFence != -1) {
        close(mReleaseFence);
    }
}

void HdiGrallocBuffer::SetReleaseFence(int fd)
{
    DISPLAY_TEST_LOGD("the fd is %d", fd);
    if (mReleaseFence != -1) {
        close(mReleaseFence);
        mReleaseFence = -1;
    }
    mReleaseFence = fd;
}

void HdiGrallocBuffer::SetAcquirceFence(int fd)
{
    DISPLAY_TEST_LOGD("the fd is %d", fd);
    mAcquireFence = fd;
}

HdiGrallocBuffer *HdiTestLayer::AcquireBackBuffer()
{
    if (!mBackBuffers.empty()) {
        if (mCurrentBuffer != nullptr) {
            mFrontBuffers.emplace(std::move(mCurrentBuffer));
        }
        mCurrentBuffer = std::move(mBackBuffers.front());
        mBackBuffers.pop();
    }
    return mCurrentBuffer.get();
}

HdiGrallocBuffer *HdiTestLayer::GetFrontBuffer()
{
    HdiGrallocBuffer *buffer = nullptr;
    if (!mFrontBuffers.empty()) {
        buffer = mFrontBuffers.front().get();
    }
    return buffer;
}

HdiGrallocBuffer *HdiTestLayer::GetBackBuffer()
{
    HdiGrallocBuffer *buffer = nullptr;
    if (!mBackBuffers.empty()) {
        buffer = mBackBuffers.front().get();
    }
    return buffer;
}

HdiTestLayer::HdiTestLayer(LayerInfo &info, uint32_t id, uint32_t displayId)
    : mId(id), mDisplayID(displayId), mLayerInfo(info)
{}

int32_t HdiTestLayer::Init()
{
    // init the font queue
    DISPLAY_TEST_LOGD();
    const int maxBufferCount = 3;
    for (int i = 0; i < maxBufferCount; i++) {
        std::unique_ptr<HdiGrallocBuffer> buffer =
            std::make_unique<HdiGrallocBuffer>(mLayerInfo.width, mLayerInfo.height, mLayerInfo.pixFormat);
        DISPLAY_TEST_CHK_RETURN((buffer->Get() == nullptr), DISPLAY_FAILURE,
            DISPLAY_TEST_LOGE("buffer handle is null"));
        mFrontBuffers.emplace(std::move(buffer));
    }
    mDisplayRect.w = mLayerInfo.width;
    mDisplayRect.h = mLayerInfo.height;
    mCropRect = mDisplayRect;
    return DISPLAY_SUCCESS;
}


int32_t HdiTestLayer::SwapFrontToBackQ()
{
    DISPLAY_TEST_CHK_RETURN((mFrontBuffers.empty()), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("the font buffer is empty"));
    mBackBuffers.emplace(std::move(mFrontBuffers.front()));
    mFrontBuffers.pop();
    return DISPLAY_SUCCESS;
}

int32_t HdiTestLayer::SwapBackToFrontQ()
{
    DISPLAY_TEST_CHK_RETURN((mBackBuffers.empty()), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("the font buffer is empty"));
    mFrontBuffers.emplace(std::move(mBackBuffers.front()));
    mBackBuffers.pop();
    return DISPLAY_SUCCESS;
}

void HdiTestLayer::SetLayerSize(IRect &rect)
{
    DISPLAY_TEST_LOGD("x : %d y : %d w : %d h : %d", rect.x, rect.y, rect.w, rect.h);
    mDisplayRect = rect;
}

void HdiTestLayer::SetLayerCrop(IRect &rect)
{
    DISPLAY_TEST_LOGD("x : %d y : %d w : %d h : %d", rect.x, rect.y, rect.w, rect.h);
    mCropRect = rect;
}

int32_t HdiTestLayer::PreparePresent()
{
    int ret;
    DISPLAY_TEST_LOGD();
    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetLayerSize(mDisplayID, mId, &mDisplayRect);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("set display rect failed"));

    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetLayerCrop(mDisplayID, mId, &mCropRect);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("set display crop failed"));

    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetLayerZorder(mDisplayID, mId, mZorder);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("set display zorder failed"));

    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetLayerCompositionType(mDisplayID, mId, mCompType);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE,
        DISPLAY_TEST_LOGE("set display composition type failed"));

    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetTransformMode(mDisplayID, mId, mTransform);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("set transform mode failed"));

    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetLayerAlpha(mDisplayID, mId, &mAlpha);
    HdiGrallocBuffer *buffer = AcquireBackBuffer();
    DISPLAY_TEST_CHK_RETURN((buffer == nullptr), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("can not get back buffer"));

    BufferHandle *handle = buffer->Get();
    DISPLAY_TEST_CHK_RETURN((handle == nullptr), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("BufferHandle is null"));
    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetLayerBuffer(mDisplayID, mId, handle, -1);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("set buffer handle failed"));

    ret = HdiTestDevice::GetInstance().GetLayerFuncs().SetLayerBlendType(mDisplayID, mId, mBlendType);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("set blend type failed"));
    return DISPLAY_SUCCESS;
}

void HdiTestLayer::SetZorder(uint32_t zorder)
{
    DISPLAY_TEST_LOGD("the zorder is %d", zorder);
    mZorder = zorder;
}

void HdiTestLayer::SetCompType(CompositionType type)
{
    DISPLAY_TEST_LOGD("layer id %d ,the type is : %d", mId, type);
    mCompType = type;
}

void HdiTestLayer::SetTransform(TransformType transform)
{
    mTransform = transform;
}

void HdiTestLayer::SetAlpha(LayerAlpha alpha)
{
    DISPLAY_TEST_LOGD();
    mAlpha = alpha;
}

void HdiTestLayer::SetBlendType(BlendType type)
{
    DISPLAY_TEST_LOGD("type %d", type);
    mBlendType = type;
}

void HdiTestLayer::SetReleaseFence(int fd)
{
    DISPLAY_TEST_LOGD("layer id %d , fd %d", mId, fd);
}

HdiTestLayer::~HdiTestLayer() {}
} // OHOS
} // HDI
} // DISPLAY
} // TEST
