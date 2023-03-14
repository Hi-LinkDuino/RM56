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

#ifndef GRAPHIC_LITE_FOCUS_MANAGER_H
#define GRAPHIC_LITE_FOCUS_MANAGER_H

#include "graphic_config.h"
#if ENABLE_FOCUS_MANAGER
#include "components/ui_view_group.h"
namespace OHOS {
/**
 * @brief Enumerates focus directions.
 *
 * @since 5.0
 * @version 3.0
 */
enum : uint8_t {
    FOCUS_DIRECTION_RIGHT,
    FOCUS_DIRECTION_LEFT,
    FOCUS_DIRECTION_UP,
    FOCUS_DIRECTION_DOWN,
};

class FocusManager {
public:
    /**
     * @brief Get the FocusManager's singleton.
     *
     * @return FocusManager's singleton.
     * @since 5.0
     * @version 3.0
     */
    static FocusManager* GetInstance();

    /**
     * @brief Clear the focus.
     *
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool ClearFocus();

    /**
     * @brief Get the focused view.
     *
     * @return the focused view.
     * @since 5.0
     * @version 3.0
     */
    UIView* GetFocusedView()
    {
        return focusView_;
    }

    /**
     * @brief Request the focus.
     *
     * @param view the focus.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool RequestFocus(UIView* view);

    /**
     * @brief Request focus by direction.
     *
     * @param direction focus direction.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool RequestFocusByDirection(uint8_t direction);

private:
    FocusManager() : focusView_(nullptr), lastFocusView_(nullptr) {}
    ~FocusManager() {}

    bool GetNextFocus(UIView* focusedView, UIView*& candidate, uint8_t direction);
    bool GetNextFocus(UIView* focusedView, UIView*& candidate, UIView* view, uint8_t direction);
    bool GetNextFocus(UIView* focusedView, UIView*& candidate, UIViewGroup* viewGroup, uint8_t direction);
    bool IsAtSameCol(const Rect& rect1, const Rect& rect2);
    bool IsAtSameRow(const Rect& rect1, const Rect& rect2);
    bool CompareCandidates(UIView* focusedView, UIView*& candidate, UIView* current, uint8_t direction);
    bool CompareCandidatesByUp(UIView* focusedView, UIView*& candidate, UIView* current);
    bool CompareCandidatesByDown(UIView* focusedView, UIView*& candidate, UIView* current);
    bool CompareCandidatesByLeft(UIView* focusedView, UIView*& candidate, UIView* current);
    bool CompareCandidatesByRight(UIView* focusedView, UIView*& candidate, UIView* current);
    bool CompareCandidatesDistance(const Rect& focused, const Rect& candidate, const Rect& current);

    UIView* focusView_;
    UIView* lastFocusView_;
};
} // namespace OHOS
#endif
#endif // GRAPHIC_LITE_FOCUS_MANAGER_H
