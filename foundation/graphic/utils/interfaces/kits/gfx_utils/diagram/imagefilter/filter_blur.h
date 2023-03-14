/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef GRAPHIC_LITE_FILTER_BLUR_H
#define GRAPHIC_LITE_FILTER_BLUR_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/graphic_math.h"
#include "graphic_config.h"
#include "securec.h"

namespace OHOS {
class Filterblur {
#if GRAPHIC_ENABLE_BLUR_EFFECT_FLAG

public:
    Filterblur()
    {
        integral_ = nullptr;
        imageWidth_ = 0;
        imageHeight_ = 0;
    }
    ~Filterblur()
    {
        if (integral_ != nullptr) {
            free(integral_);
        }
    }

    template <class Img>
    void BoxBlur(Img& img, uint16_t radius, int32_t channel, int32_t stride)
    {
        if (radius < 1) {
            return;
        }
        int32_t width = img.GetWidth();
        int32_t height = img.GetHeight();
        bool isGetRGBAIntegral = false;
        if (integral_ == nullptr || ((imageWidth_ * imageHeight_) != (width * height))) {
            if (integral_ != nullptr) {
                free(integral_);
            }
            integral_ = (int32_t*)malloc((width + 1) * (height + 1) * channel * sizeof(int32_t));
            isGetRGBAIntegral = true;
        }
        if (channel == FOUR_TIMES) {
            if (isGetRGBAIntegral) {
                GetRGBAIntegralImage((uint8_t*)img.PixValuePtr(0, 0), width, height, stride);
            }
#pragma omp parallel for
            for (int32_t y = 0; y < height; y++) {
                int32_t y1 = MATH_MAX(y - radius, 0);
                int32_t y2 = MATH_MIN(y + radius + 1, height);
                uint8_t* lineImageHeader = (uint8_t*)img.PixValuePtr(0, 0) + y * stride;
                uint8_t* linePD = lineImageHeader;
                int32_t* lineP1 = integral_ + y1 * ((width + 1) << 2);
                int32_t* lineP2 = integral_ + y2 * ((width + 1) << 2);

                for (int32_t x = 0; x < width; x++) {
                    int32_t x1 = MATH_MAX(x - radius, 0);
                    int32_t x2 = MATH_MIN(x + radius + 1, width);
                    int32_t index1 = x1 << 2;
                    int32_t index2 = x2 << 2;
                    int32_t sumB = lineP2[index2 + 0] - lineP1[index2 + 0] -
                               lineP2[index1 + 0] + lineP1[index1 + 0];
                    int32_t sumG = lineP2[index2 + 1] - lineP1[index2 + 1] -
                               lineP2[index1 + 1] + lineP1[index1 + 1];
                    int32_t sumR = lineP2[index2 + 2] - lineP1[index2 + 2] -
                               lineP2[index1 + 2] + lineP1[index1 + 2];

                    const int32_t pixelCount = (x2 - x1) * (y2 - y1);
                    linePD[0] = (sumB + (pixelCount >> 1)) / pixelCount;
                    linePD[1] = (sumG + (pixelCount >> 1)) / pixelCount;
                    linePD[2] = (sumR + (pixelCount >> 1)) / pixelCount;
                    uint8_t* alpha = lineImageHeader + (x << 2);
                    linePD[3] = alpha[3];
                    linePD += 4;
                }
            }
        }
    }
private:
    void GetRGBAIntegralImage(uint8_t* src, uint16_t width, uint16_t height, uint16_t stride)
    {
        int32_t channel = FOUR_TIMES;
        if (integral_ != nullptr && ((imageWidth_ * imageHeight_) != (width * height))) {
            int32_t integralSize = (width + 1) * channel * sizeof(int32_t);
            memset_s(integral_, integralSize, 0, integralSize);
        }
        for (int y = 0; y < height; y++) {
            uint8_t* linePS = src + y * stride;
            //	last position
            int32_t* linePL = integral_ + y * (width + 1) * channel + channel;
            //	curretn position£¬waring the first column of every line row is zero
            int32_t* linePD = integral_ + (y + 1) * (width + 1) * channel + channel;
            //	the first column is 0
            linePD[-4] = 0;
            linePD[-3] = 0;
            linePD[-2] = 0;
            linePD[-1] = 0;
            for (int x = 0, sumB = 0, sumG = 0, sumR = 0, sumA = 0; x < width; x++) {
                sumB += linePS[0];
                sumG += linePS[1];
                sumR += linePS[2];
                sumA += linePS[3];
                linePD[0] = linePL[0] + sumB;
                linePD[1] = linePL[1] + sumG;
                linePD[2] = linePL[2] + sumR;
                linePD[3] = linePL[3] + sumA;
                linePS += channel;
                linePL += channel;
                linePD += channel;
            }
        }
    }

    int32_t* integral_;
    int32_t imageWidth_;
    int32_t imageHeight_;
#endif
};
} // namespace OHOS
#endif
