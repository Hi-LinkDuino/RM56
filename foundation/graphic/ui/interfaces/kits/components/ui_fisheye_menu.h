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


#ifndef UI_FISHEYE_MENU_H
#define UI_FISHEYE_MNEU_H

#include "components/ui_list.h"

namespace OHOS{

class UIFisheyeMenu : public UIList
{
public:
    UIFisheyeMenu();
    ~UIFisheyeMenu(){}

    void SetHeight(int16_t height) override;
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

private:
    static constexpr float MIN_SCALE = 0.1f;
    static constexpr float MAX_SCALE = 1.0f;
    static constexpr float FISRT_SCALE_RANGE = 0.25f;
    static constexpr float SENCOD_SCALE_RANGE = 0.45f;
    static constexpr uint8_t MIN_OPACITY = 20;
    static constexpr uint8_t MAX_OPACITY = 255;
    void CalculateScaleAndOpacity();
    int16_t centerPosition_;
    uint16_t childHeight_ = 0;
};
} // namespace OHOS
#endif //UI_FISHEYE_MENU_H