/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "frame_config.h"
#include "media_log.h"
#include "securec.h"

using namespace std;

namespace OHOS {
namespace Media {
const int32_t DEFAULT_FPS = 30;
int32_t FrameConfig::GetFrameConfigType()
{
    return type_;
}

list<Surface *> FrameConfig::GetSurfaces()
{
    return surfaceList_;
}

void FrameConfig::AddSurface(Surface &surface)
{
    surfaceList_.remove(&surface);
    surfaceList_.emplace_back(&surface);
}

void FrameConfig::RemoveSurface(Surface &surface)
{
    surfaceList_.remove(&surface);
}

void *FrameConfig::GetValue(uint32_t key)
{
    if (key == CAM_IMAGE_CROP_RECT) {
        return &crop;
    }
    auto p = keyMap_.find(key);
    return (p == keyMap_.end()) ? nullptr : (&p->second);
}

void FrameConfig::SetVendorParameter(uint8_t *value, uint32_t len)
{
    if (value == nullptr) {
        MEDIA_ERR_LOG("value is a nullptr");
        return;
    }
    errno_t ret = memset_s(privateTag_, sizeof(privateTag_), 0, sizeof(privateTag_));
    if (ret != EOK) {
        MEDIA_ERR_LOG("memset failed when set private tag, ret(%d)", ret);
        return;
    }
    ret = memcpy_s(privateTag_, sizeof(privateTag_), value, len);
    if (ret != EOK) {
        MEDIA_ERR_LOG("memcpy failed when set private tag, ret(%d)", ret);
    }
}

void FrameConfig::GetVendorParameter(uint8_t *value, uint32_t len)
{
    if (value == nullptr) {
        MEDIA_ERR_LOG("value is a nullptr");
        return;
    }
    uint32_t realLength = (len > PRIVATE_TAG_LEN) ? PRIVATE_TAG_LEN : len;
    errno_t ret = memcpy_s(value, realLength, privateTag_, realLength);
    if (ret != EOK) {
        MEDIA_ERR_LOG("memcpy failed when get private tag, ret(%d)", ret);
    }
}

void FrameConfig::SetValue(uint32_t key, const void *value)
{
    if (value == nullptr) {
        MEDIA_ERR_LOG("value is a nullptr");
        return;
    }
    switch (key) {
        case PARAM_KEY_IMAGE_ENCODE_QFACTOR:
        case CAM_FRAME_FPS:
        case CAM_IMAGE_INVERT_MODE:
        case CAM_IMAGE_FORMAT:
            keyMap_[key] = *(static_cast<const int32_t *>(value));
            break;
        case CAM_IMAGE_CROP_RECT:
            crop.x = (static_cast<const CameraRect *>(value))->x;
            crop.y = (static_cast<const CameraRect *>(value))->y;
            crop.w = (static_cast<const CameraRect *>(value))->w;
            crop.h = (static_cast<const CameraRect *>(value))->h;
            break;
        default:
            break;
    }
}

FrameConfig::FrameConfig(int32_t type) : type_(type)
{
    crop.x = 0;
    crop.y = 0;
    crop.w = 0;
    crop.h = 0;
    SetParameter(CAM_IMAGE_CROP_RECT, crop);
    SetParameter(CAM_IMAGE_INVERT_MODE, CAM_CENTER_MIRROR);
    SetParameter(CAM_FRAME_FPS, DEFAULT_FPS);
    if (type == FRAME_CONFIG_RECORD) {
        SetParameter(CAM_IMAGE_FORMAT, CAM_FORMAT_H265);
    } else if (type == FRAME_CONFIG_CAPTURE) {
        SetParameter(CAM_IMAGE_FORMAT, CAM_FORMAT_JPEG);
    } else {
        SetParameter(CAM_IMAGE_FORMAT, CAM_FORMAT_YVU420);
    }
}
} // namespace Media
} // namespace OHOS