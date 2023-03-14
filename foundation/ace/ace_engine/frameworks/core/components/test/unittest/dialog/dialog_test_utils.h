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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DIALOG_DIALOG_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DIALOG_DIALOG_TEST_UTILS_H

#include "core/components/bubble/render_bubble.h"
#include "core/components/dialog_tween/render_dialog_tween.h"
#include "core/components/popup/popup_component.h"

namespace OHOS::Ace {

class MockRenderDialogTween final : public RenderDialogTween {
    DECLARE_ACE_TYPE(MockRenderDialogTween, RenderDialogTween);

public:
    MockRenderDialogTween() = default;
    ~MockRenderDialogTween() override = default;

    bool GetAutoCancel() const
    {
        return autoCancel_;
    }
};

class MockRenderBubble final : public RenderBubble {
    DECLARE_ACE_TYPE(MockRenderBubble, RenderBubble);

public:
    MockRenderBubble() = default;
    ~MockRenderBubble() override = default;

    Placement GetPlacement() const
    {
        return placement_;
    }

    void SetPlacement(Placement placement)
    {
        placement_ = placement;
    }

    Color GetMaskColor() const
    {
        return maskColor_;
    }

    Offset GetOffset() const
    {
        return targetOffset_;
    }

    void SetTargetOffset(const Offset& targetOffset)
    {
        targetOffset_ = targetOffset;
    }

    Size GetSize() const
    {
        return targetSize_;
    }

    void SetTargetSize(const Size& targetSize)
    {
        targetSize_ = targetSize;
    }
};

class DialogTestUtils {
public:
    static RefPtr<MockRenderBubble> CreateRenderBubble(const Placement placement,
        const Offset& offset, const Size& size);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DIALOG_DIALOG_TEST_UTILS_H
