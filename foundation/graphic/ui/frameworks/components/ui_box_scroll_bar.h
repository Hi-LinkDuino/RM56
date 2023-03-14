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

#ifndef GRAPHIC_LITE_UI_BOX_SCROLL_BAR_H
#define GRAPHIC_LITE_UI_BOX_SCROLL_BAR_H

#include "components/ui_abstract_scroll_bar.h"

namespace OHOS {
class UIBoxScrollBar : public UIAbstractScrollBar {
public:
    UIBoxScrollBar() = default;

    virtual ~UIBoxScrollBar() = default;

    void SetPosition(int16_t x, int16_t y, int16_t width, int16_t height) override;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa) override;

protected:
    Rect backgroundRect_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_BOX_SCROLL_BAR_H
