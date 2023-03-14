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

#ifndef GRAPHIC_LITE_UI_DRAWER_LAYOUT_H
#define GRAPHIC_LITE_UI_DRAWER_LAYOUT_H

#include "components/ui_abstract_scroll.h"

namespace OHOS{
class UIDrawerLayout : public UIAbstractScroll
{

public:
    UIDrawerLayout();
    ~UIDrawerLayout();

    void Add(UIView* view) override;

    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;

protected:

    constexpr static uint16_t ANIMATOR_TIME = 12;
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;
    

private:
    static constexpr uint8_t STATUS_NORMAL = 1;
    static constexpr uint8_t STATUS_SHOW_UP = 2;
    static constexpr uint8_t STATUS_SHOW_BOTTOM = 3;

    static constexpr uint8_t DRAG_NONE = 3;
    static constexpr uint8_t DRAG_EDGE_UP = 1;
    static constexpr uint8_t DRAG_EDGE_BOTTOM = 2;

    static constexpr uint16_t EDGE_SLIDE_SIZE = 80;

    UIView* topLayout;
    UIView* centerLayout;
    UIView* bottomLayout;
    uint16_t edgeSlideDistance_;
    uint8_t status = STATUS_NORMAL;
    uint8_t dragStatus = DRAG_NONE;
    int16_t tickTime_;
    
};
} //namespace OHOS
#endif