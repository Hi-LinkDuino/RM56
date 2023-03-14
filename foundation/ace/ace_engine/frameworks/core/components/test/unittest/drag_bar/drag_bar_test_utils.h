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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DRAG_BAR_DRAG_BAR_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DRAG_BAR_DRAG_BAR_TEST_UTILS_H

#include "core/components/drag_bar/render_drag_bar.h"

namespace OHOS::Ace {
namespace {

class MockRenderDragBar final : public RenderDragBar {
    DECLARE_ACE_TYPE(MockRenderDragBar, RenderDragBar);

public:
    MockRenderDragBar() = default;
    ~MockRenderDragBar() override = default;

    bool CheckPanelMode(PanelMode mode)
    {
        if (mode == PanelMode::HALF) {
            // Three point in the same horizontal line when half mode.
            if (!NearEqual(barLeftPoint_.GetY(), barCenterPoint_.GetY()) ||
                !NearEqual(barCenterPoint_.GetY(), barRightPoint_.GetY())) {
                return false;
            }
        } else {
            // Three point in the different horizontal line when half mode.
            if (NearEqual(barLeftPoint_.GetY(), barCenterPoint_.GetY()) ||
                NearEqual(barCenterPoint_.GetY(), barRightPoint_.GetY())) {
                return false;
            }
        }
        return showMode_ == mode;
    }

    bool CheckHasDragBar(bool hasDragBar)
    {
        return hasDragBar_ == hasDragBar;
    }

    bool CheckScale()
    {
        return NearEqual(scaleX_, 1.0) && NearEqual(scaleY_, 1.0);
    }

    bool CheckIconOffset(const Offset& offset)
    {
        return iconOffset_ == offset;
    }

    bool CheckDragOffset(bool offsetX, bool offsetY)
    {
        bool checkX = false;
        bool checkY = false;
        if (offsetX) {
            checkX = dragOffset_.GetX() > 0.0;
        } else {
            checkX = dragOffset_.GetX() < 0.0;
        }
        if (offsetY) {
            checkY = dragOffset_.GetY() > 0.0;
        } else {
            checkY = dragOffset_.GetY() < 0.0;
        }
        return checkX && checkY;
    }
};

} // namespace

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DRAG_BAR_DRAG_BAR_TEST_UTILS_H
