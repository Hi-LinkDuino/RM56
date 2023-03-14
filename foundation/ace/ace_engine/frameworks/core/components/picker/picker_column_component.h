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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_COLUMN_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_COLUMN_COMPONENT_H

#include "core/accessibility/accessibility_manager.h"
#include "core/components/picker/picker_theme.h"
#include "core/components/picker/toss_animation_controller.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

using ColumnChangeCallback = std::function<void(const std::string&, bool, uint32_t, bool)>;
using ColumnFinishCallback = std::function<void(bool)>;

class PickerColumnComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(PickerColumnComponent, ComponentGroup);

public:
    PickerColumnComponent() = default;
    ~PickerColumnComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void Initialize();

    const RefPtr<PickerTheme>& GetTheme() const
    {
        return theme_;
    }
    void SetTheme(const RefPtr<PickerTheme>& value)
    {
        theme_ = value;
    }

    void ClearOption()
    {
        options_.clear();
    }

    void AppendOption(const std::string& value)
    {
        options_.emplace_back(value);
    }

    uint32_t GetOptionCount() const
    {
        return options_.size();
    }

    std::string GetOption(uint32_t index) const
    {
        if (index >= GetOptionCount()) {
            LOGE("index out of range.");
            return "";
        }
        return options_[index];
    }

    uint32_t GetCurrentIndex() const
    {
        return currentIndex_;
    }
    void SetCurrentIndex(uint32_t value)
    {
        currentIndex_ = value;
    }

    bool GetInDialog() const
    {
        return inDialog_;
    }
    void SetInDialog(bool value)
    {
        inDialog_ = value;
    }

    bool GetHasSplitter() const
    {
        return hasSplitter_;
    }
    void SetHasSplitter(bool value)
    {
        hasSplitter_ = value;
    }

    std::string GetCurrentText() const
    {
        return GetOption(GetCurrentIndex());
    }

    const ColumnChangeCallback& GetChangeCallback() const
    {
        return changeCallback_;
    }
    void SetChangeCallback(const ColumnChangeCallback& value)
    {
        changeCallback_ = value;
    }

    const ColumnFinishCallback& GetFinishCallback() const
    {
        return finishCallback_;
    }
    void SetFinishCallback(const ColumnFinishCallback& value)
    {
        finishCallback_ = value;
    }

    const std::string& GetColumnTag() const
    {
        return columnTag_;
    }
    void SetColumnTag(const std::string& value)
    {
        columnTag_ = value;
    }

    const std::string& GetPrefix() const
    {
        return prefix_;
    }
    void SetPrefix(const std::string& value)
    {
        prefix_ = value;
    }

    const std::string& GetSuffix() const
    {
        return suffix_;
    }
    void SetSuffix(const std::string& value)
    {
        suffix_ = value;
    }

    uint32_t GetWidthRatio() const
    {
        return widthRatio_;
    }
    void SetWidthRatio(uint32_t value)
    {
        widthRatio_ = value;
    }

    int32_t GetNodeId() const
    {
        return nodeId_;
    }
    void SetNodeId(int32_t value)
    {
        nodeId_ = value;
    }

    const Dimension& GetFixHeight() const
    {
        return fixHeight_;
    }
    void SetFixHeight(const Dimension& value)
    {
        fixHeight_ = value;
    }

    void HandleChangeCallback(bool isAdd, bool needNotify)
    {
        if (changeCallback_) {
            changeCallback_(GetColumnTag(), isAdd, GetCurrentIndex(), needNotify);
        } else {
            LOGE("change callback is null.");
        }
    }

    void HandleFinishCallback(bool success)
    {
        if (finishCallback_) {
            finishCallback_(success);
        } else {
            LOGE("finish callback is null.");
        }
    }

    const std::function<void()>& GetRequestFocusCallback() const
    {
        return requestFocusCallback_;
    }
    void SetRequestFocusCallback(const std::function<void()>& value)
    {
        requestFocusCallback_ = value;
    }

    void HandleRequestFocus()
    {
        if (!requestFocusCallback_) {
            LOGE("function of request focus is null.");
            return;
        }

        requestFocusCallback_();
    }

    const RefPtr<TossAnimationController>& GetToss() const
    {
        return tossAnimationController_;
    }

    void SetNeedVibrate(bool needVibrate)
    {
        needVibrate_ = needVibrate;
    }

    bool GetNeedVibrate()
    {
        return needVibrate_;
    }

private:
    uint32_t currentIndex_ = 0;

    bool inDialog_ = true;

    bool hasSplitter_ = false;

    int32_t nodeId_ = -1; // use -1 that is invalidate for default.

    RefPtr<PickerTheme> theme_;

    std::vector<std::string> options_;

    ColumnChangeCallback changeCallback_;

    ColumnFinishCallback finishCallback_;

    std::string columnTag_;

    std::string prefix_;
    std::string suffix_;

    Dimension fixHeight_;

    uint32_t widthRatio_ = 1; // default column ratio is 1:1

    std::function<void()> requestFocusCallback_;

    RefPtr<TossAnimationController> tossAnimationController_ = AceType::MakeRefPtr<TossAnimationController>();

    bool needVibrate_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_COLUMN_COMPONENT_H
