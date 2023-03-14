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

#ifndef HDI_COMPOSITION_CHECK_H
#define HDI_COMPOSITION_CHECK_H
#include "display_type.h"
#include "hdi_device_test.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
struct Point {
    int32_t x = 0;
    int32_t y = 0;
};

class HdiCompositionCheck {
public:
    static HdiCompositionCheck &GetInstance()
    {
        static HdiCompositionCheck instace = HdiCompositionCheck();
        return instace;
    }
    void Init(uint32_t w, uint32_t h)
    {
        mDispW = w;
        mDispH = h;
    }
    enum {
        CHECK_CENTER,
        CHECK_VERTEX
    };
    int32_t Check(std::vector<LayerSettings> &layers, BufferHandle &clientBuffer, uint32_t type = CHECK_VERTEX);

private:
    void GetCheckPoints(Point center, std::vector<Point> &points);
    void SimpleHandleAlpha(const LayerSettings &layers, uint32_t &color);
    std::vector<uint32_t> GetCheckColors(std::vector<LayerSettings> &layers, std::vector<Point> &points);
    HdiCompositionCheck() {}
    ~HdiCompositionCheck() {}
    uint32_t mDispW = 0;
    uint32_t mDispH = 0;
};
} // OHOS
} // HDI
} // DISPLAY
} // TEST

#endif // HDI_COMPOSITION_CHECK_H