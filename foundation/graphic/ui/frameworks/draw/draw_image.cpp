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

#include "draw/draw_image.h"

#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"

namespace OHOS {
void DrawImage::DrawCommon(BufferInfo& gfxDstBuffer, const Rect& coords, const Rect& mask,
    const ImageInfo* img, const Style& style, uint8_t opaScale)
{
    if (img == nullptr) {
        return;
    }
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.imageOpa_);
    uint8_t pxBitSize = DrawUtils::GetPxSizeByColorMode(img->header.colorMode);
    DrawUtils::GetInstance()->DrawImage(gfxDstBuffer, coords, mask, img->data, opa, pxBitSize,
                                        img->header, static_cast<uint8_t*>(img->userData));
}

void DrawImage::DrawCommon(BufferInfo& gfxDstBuffer, const Rect& coords, const Rect& mask,
    const char* path, const Style& style, uint8_t opaScale)
{
    if (path == nullptr) {
        return;
    }
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.imageOpa_);

    CacheEntry entry;
    if (CacheManager::GetInstance().Open(path, style, entry) != RetCode::OK) {
        return;
    }

    uint8_t pxBitSize = DrawUtils::GetPxSizeByColorMode(entry.GetImageInfo().header.colorMode);
    if (entry.InCache()) {
        DrawUtils::GetInstance()->DrawImage(gfxDstBuffer, coords, mask, entry.GetImgData(), opa, pxBitSize,
                                             entry.GetImageInfo().header, static_cast<uint8_t*>(entry.GetImageInfo().userData));
    } else {
        Rect valid = coords;
        if (!valid.Intersect(valid, mask)) {
            return;
        }

        int16_t width = valid.GetWidth();
        if (width <= 0) {
            return;
        }
        uint8_t* buf = static_cast<uint8_t*>(UIMalloc(static_cast<uint32_t>(width) * ((COLOR_DEPTH >> SHIFT_3) + 1)));
        if (buf == nullptr) {
            return;
        }

        Rect line = valid;
        line.SetHeight(1);
        Point start;
        start.x = valid.GetLeft() - coords.GetLeft();
        start.y = valid.GetTop() - coords.GetTop();
        for (int16_t row = valid.GetTop(); row <= valid.GetBottom(); row++) {
            if (entry.ReadLine(start, width, buf) != RetCode::OK) {
                CacheManager::GetInstance().Close(path);
                UIFree(buf);
                return;
            }
            DrawUtils::GetInstance()->DrawImage(gfxDstBuffer, line, mask, buf, opa, pxBitSize,
                                                entry.GetImageInfo().header, static_cast<uint8_t*>(entry.GetImageInfo().userData));
            line.SetTop(line.GetTop() + 1);
            line.SetBottom(line.GetBottom() + 1);
            start.y++;
        }
        UIFree(buf);
    }
}
} // namespace OHOS
