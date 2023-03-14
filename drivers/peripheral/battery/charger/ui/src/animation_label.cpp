/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "animation_label.h"
#include <cerrno>
#include <cstdio>
#include <string>
#include <sys/epoll.h>
#include "frame.h"
#include "png.h"
#include "securec.h"
#include "view.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
constexpr int32_t PNG_HEADER_SIZE = 8;
constexpr int32_t MAX_PICTURE_CHANNELS = 3;
constexpr int32_t MAX_BIT_DEPTH = 8;
constexpr useconds_t SECOND_PER_MS = 1000;
bool AnimationLabel::isVisible_ = true;
bool AnimationLabel::needStop_ = false;

AnimationLabel::AnimationLabel(int32_t startX, int32_t startY, int32_t w, int32_t h, Frame* mParent)
{
    startX_ = startX;
    startY_ = startY;
    this->CreateBuffer(w, h, View::PixelFormat::BGRA888);
    parent_ = mParent;
    SetFocusAble(false);
    needStop_ = false;
    parent_->ViewRegister(this);
}

AnimationLabel::~AnimationLabel()
{
    needStop_ = true;
    FreeBuffer();
    size_t imgSize = imgList_.size();

    for (size_t i = 0; i < imgSize; i++) {
        free(imgList_[i]);
    }
    imgList_.clear();
}

void AnimationLabel::SetStaticImg(int32_t picId)
{
    staticShowId_ = picId;
}

void AnimationLabel::SetIsVisible(bool visible)
{
    isVisible_ = visible;
}

void AnimationLabel::SetPlayMode(AnimationLabel::PlayMode mode)
{
    if (mode == AnimationLabel::PlayMode::ANIMATION_MODE) {
        showStatic_ = false;
    } else if (mode == AnimationLabel::PlayMode::STATIC_MODE) {
        showStatic_ = true;
    }
}

void AnimationLabel::UpdateLoop()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start update animation loop");
    uint32_t index = 0;

    while (!needStop_) {
        if (showStatic_) {
            usleep(SECOND_PER_MS * SECOND_PER_MS);
        } else {
            usleep(intervalMs_ * SECOND_PER_MS);
        }
        if (imgList_.size() <= 0) {
            continue;
        }

        BATTERY_HILOGD(FEATURE_CHARGING, "isVisible_ = %{public}d", isVisible_);
        if (!isVisible_) {
            continue;
        }

        if (imgList_.size() <= index) {
            index = 0;
            needStop_ = true;
        }
        SyncBuffer();
        mutex_.lock();

        if (showStatic_) {
            if (staticShowId_ < staticImgSize_) {
                DrawSubView(0, 0, viewWidth_, viewHeight_, staticImgList_[staticShowId_]);
            }
        } else {
            DrawSubView(0, 0, viewWidth_, viewHeight_, imgList_[index]);
        }
        mutex_.unlock();
        if (parent_ != nullptr) {
            parent_->OnDraw();
        }
        index++;
    }
    BATTERY_HILOGD(FEATURE_CHARGING, "finish update animation loop");
}

void AnimationLabel::AddImg(const std::string& imgFileName)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "add img, name=%{public}s", imgFileName.c_str());
    mutex_.lock();
    char* buf = static_cast<char*>(LoadPng(imgFileName));
    imgList_.push_back(buf);
    mutex_.unlock();
}

int32_t AnimationLabel::AddStaticImg(const std::string& imgFileName)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "add static img, name=%{public}s", imgFileName.c_str());
    int32_t id = staticImgSize_;
    mutex_.lock();
    staticImgList_[id] = static_cast<char*>(LoadPng(imgFileName));
    staticImgSize_++;
    mutex_.unlock();
    return id;
}

int32_t AnimationLabel::LoadPngInternalWithFile(FILE* fp, png_structpp pngPtr, png_infopp pngInfoPtr,
    struct PictureAttr& attr)
{
    if (fp == nullptr) {
        return -1;
    }
    uint8_t header[PNG_HEADER_SIZE];
    size_t bytesRead = fread(header, 1, sizeof(header), fp);
    if (bytesRead != sizeof(header)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "read header from file failed, errno=%{public}d", errno);
        return -1;
    }
    if (png_sig_cmp(header, 0, sizeof(header))) {
        BATTERY_HILOGE(FEATURE_CHARGING, "png file header is not valid.");
        return -1;
    }

    *pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (*pngPtr == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "create png struct failed.");
        return -1;
    }

    *pngInfoPtr = png_create_info_struct(*pngPtr);
    if (*pngInfoPtr == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "create png info failed.");
        return -1;
    }
    png_init_io(*pngPtr, fp);
    png_set_sig_bytes(*pngPtr, sizeof(header));
    png_read_info(*pngPtr, *pngInfoPtr);
    png_get_IHDR(*pngPtr, *pngInfoPtr, &attr.pictureWidth, &attr.pictureHeight, &attr.bitDepth, &attr.colorType,
        nullptr, nullptr, nullptr);
    attr.pictureChannels = png_get_channels(*pngPtr, *pngInfoPtr);
    if (attr.bitDepth <= MAX_BIT_DEPTH && attr.pictureChannels == 1 && attr.colorType == PNG_COLOR_TYPE_PALETTE) {
        // paletted images: expand to 8-bit RGB.  Note that we DON'T
        // currently expand the tRNS chunk (if any) to an alpha
        // channel, because minui doesn't support alpha channels in
        // general.
        png_set_palette_to_rgb(*pngPtr);
        attr.pictureChannels = MAX_PICTURE_CHANNELS;
    }

    if (attr.pictureChannels < MAX_PICTURE_CHANNELS) {
        BATTERY_HILOGE(FEATURE_CHARGING, "need rgb format pic.");
        return -1;
    }
    return 0;
}

void AnimationLabel::CopyPictureBuffer(struct PictureAttr& attr, char* pictureBufferTmp,
    BRGA888Pixel* pictureBuffer) const
{
    auto pictureHeight = static_cast<int32_t>(attr.pictureHeight);
    auto pictureWidth = static_cast<int32_t>(attr.pictureWidth);
    int32_t copyHeight = (viewHeight_ < pictureHeight) ? viewHeight_ : pictureHeight;
    int32_t copyWidth = (viewWidth_ < pictureWidth) ? viewWidth_ : pictureWidth;
    auto* rgb = reinterpret_cast<RGB888Pixel*>(pictureBufferTmp);
    for (int32_t y = 0; y < copyHeight; y++) {
        for (int32_t x = 0; x < copyWidth; x++) {
            uint32_t colorValue = rgb[x + y * pictureWidth].r +
            rgb[x + y * pictureWidth].g + rgb[x + y * pictureWidth].b;
            if (colorValue > 0) {
                pictureBuffer[x + y * viewWidth_].r = rgb[x + y * pictureWidth].r;
                pictureBuffer[x + y * viewWidth_].g = rgb[x + y * pictureWidth].g;
                pictureBuffer[x + y * viewWidth_].b = rgb[x + y * pictureWidth].b;
                pictureBuffer[x + y * viewWidth_].a = 0xff;
            }
        }
    }
}

void* AnimationLabel::LoadPng(const std::string& imgFileName)
{
    png_structp pngPtr = nullptr;
    png_infop pngInfoPtr = nullptr;
    struct PictureAttr attr {};
    char* pictureBufferTmp = nullptr;
    uint8_t* pictureRow = nullptr;

    FILE* fp = fopen(imgFileName.c_str(), "rb");
    if (fp == nullptr) {
        BATTERY_HILOGD(FEATURE_CHARGING, "open img file failed.");
        return nullptr;
    }
    if (LoadPngInternalWithFile(fp, &pngPtr, &pngInfoPtr, attr) < 0) {
        png_destroy_read_struct(&pngPtr, &pngInfoPtr, 0);
        fclose(fp);
        fp = nullptr;
        return nullptr;
    }
    uint32_t pictureRowSize = attr.pictureWidth * attr.pictureChannels;
    pictureBufferTmp = static_cast<char*>(malloc(pictureRowSize * attr.pictureHeight));
    if (pictureBufferTmp == nullptr) {
        BATTERY_HILOGD(FEATURE_CHARGING, "malloc memory failed.");
        if (fp != nullptr) {
            fclose(fp);
            fp = nullptr;
        }
        return nullptr;
    }

    for (uint32_t y = 0; y < attr.pictureHeight; y++) {
        pictureRow = reinterpret_cast<uint8_t*>((pictureBufferTmp) + y * pictureRowSize);
        png_read_row(pngPtr, pictureRow, nullptr);
    }

    BRGA888Pixel* pictureBuffer = HandleLoadPng(&fp, &pictureBufferTmp, attr);
    return static_cast<void*>(pictureBuffer);
}

View::BRGA888Pixel* AnimationLabel::HandleLoadPng(FILE** fp, char** pictureBufferTmp, struct PictureAttr& attr)
{
    int32_t pictureBufferSize = viewHeight_ * viewWidth_ * sizeof(BRGA888Pixel);
    BRGA888Pixel* pictureBuffer = nullptr;
    char* backgroundBuffer = static_cast<char*>(GetRawBuffer());

    pictureBuffer = static_cast<BRGA888Pixel*>(malloc(pictureBufferSize));
    if (pictureBuffer == nullptr) {
        BATTERY_HILOGD(FEATURE_CHARGING, "malloc memory failed.");
        if (*pictureBufferTmp != nullptr) {
            free(*pictureBufferTmp);
            *pictureBufferTmp = nullptr;
        }
        if (*fp != nullptr) {
            fclose(*fp);
            *fp = nullptr;
        }
        return nullptr;
    }

    if (memcpy_s(reinterpret_cast<char*>(pictureBuffer), pictureBufferSize,
        backgroundBuffer, pictureBufferSize) != EOK) {
        if (*pictureBufferTmp != nullptr) {
            free(*pictureBufferTmp);
            *pictureBufferTmp = nullptr;
        }
        if (pictureBuffer != nullptr) {
            free(pictureBuffer);
            pictureBuffer = nullptr;
        }
        if (*fp != nullptr) {
            fclose(*fp);
            *fp = nullptr;
        }
        return nullptr;
    }
    CopyPictureBuffer(attr, *pictureBufferTmp, pictureBuffer);
    free(*pictureBufferTmp);
    *pictureBufferTmp = nullptr;
    int32_t ret = fclose(*fp);
    if (ret < 0) {
        BATTERY_HILOGD(FEATURE_CHARGING, "fp file close failed.");
        return nullptr;
    }
    *fp = nullptr;
    return pictureBuffer;
}

void AnimationLabel::SetInterval(int32_t ms)
{
    intervalMs_ = static_cast<uint32_t>(ms);
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
