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

#include "hdi_composition_check.h"
#include "display_test.h"
#include "display_test_utils.h"
namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
void HdiCompositionCheck::GetCheckPoints(Point center, std::vector<Point> &points)
{
    constexpr uint32_t step = 3;
    points.push_back(center);
    points.push_back({ center.x + step, center.y });
    points.push_back({ center.x + step, center.y + step });
    points.push_back({ center.x + step, center.y - step });
    points.push_back({ center.x, center.y + step });
    points.push_back({ center.x - step, center.y });
    points.push_back({ center.x - step, center.y - step });
    points.push_back({ center.x - step, center.y + step });
    points.push_back({ center.x, center.y - step });
}
// simple hande the alpha it may not compatible with all scenarios
void HdiCompositionCheck::SimpleHandleAlpha(const LayerSettings &layers, uint32_t &color)
{
    const float inv = 1.0f / 255.0f;
    if (layers.alpha != -1) {
        switch (layers.blendType) {
            case BLEND_SRC:
                color = (color & 0xffffff00) | (layers.alpha & 0xff); // get the alpha
                break;
            case BLEND_SRCOVER:
                color = color * (layers.alpha * inv);
                color = (color & 0xffffff00) | (layers.alpha & 0xff); // get the alpha
                break;
            default:
                break;
        }
    }
}

std::vector<uint32_t> HdiCompositionCheck::GetCheckColors(std::vector<LayerSettings> &layers,
    std::vector<Point> &points)
{
    std::vector<uint32_t> colors;
    for (auto point : points) {
        uint32_t color = 0;
        for (uint32_t i = layers.size(); i > 0; i--) {
            auto layer = layers[i - 1];
            IRect &rect = layer.displayRect;
            // check wether the point is inside the rect
            if ((point.x >= rect.x) && (point.x < (rect.x + rect.w)) && (point.y >= rect.y) &&
                (point.y < (rect.y + rect.h))) {
                if (layer.compositionType != COMPOSITION_VIDEO) {
                    color = layer.color;
                    SimpleHandleAlpha(layer, color);
                }
                break;
            }
        }
        colors.push_back(color);
    }
    return colors;
}

int32_t HdiCompositionCheck::Check(std::vector<LayerSettings> &layers, BufferHandle &clientBuffer, uint32_t checkType)
{
    int ret = DISPLAY_SUCCESS;
    const int midPos = 2;
    // get the all check point
    std::vector<Point> points;
    for (auto layer : layers) {
        IRect &rect = layer.displayRect;
        if (checkType == CHECK_VERTEX) {
            GetCheckPoints({ rect.x, rect.y }, points);
            GetCheckPoints({ rect.x, rect.y + rect.h }, points);
            GetCheckPoints({ rect.x + rect.w, rect.y }, points);
            GetCheckPoints({ rect.x + rect.w, rect.y + rect.h }, points);
        } else {
            GetCheckPoints({ rect.x + rect.w / midPos, rect.y + rect.h / midPos }, points); // center point
        }
    }

    // get all the check color
    std::vector<uint32_t> colors = GetCheckColors(layers, points);
    DISPLAY_TEST_CHK_RETURN((colors.size() != points.size()), DISPLAY_FAILURE,
        DISPLAY_TEST_LOGE("Points and colors don't match"));
    for (uint32_t i = 0; i < points.size(); i++) {
        if ((points[i].x >= clientBuffer.width) || (points[i].x < 0) || (points[i].y < 0) ||
            (points[i].y >= clientBuffer.height)) {
            continue;
        }
        ret = CheckPixel(clientBuffer, points[i].x, points[i].y, colors[i]);
        if (ret != DISPLAY_SUCCESS) {
            DISPLAY_TEST_LOGE("check failed");
            break;
        }
    }
    return ret;
}
} // OHOS
} // HDI
} // DISPLAY
} // TEST