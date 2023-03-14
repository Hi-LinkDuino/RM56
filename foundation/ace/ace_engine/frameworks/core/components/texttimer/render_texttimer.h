/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_RENDER_TEXTTIMER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_RENDER_TEXTTIMER_H

#include <functional>

#include "base/utils/system_properties.h"
#include "core/components/text/render_text.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
class RenderTextTimer : public RenderNode {
    DECLARE_ACE_TYPE(RenderTextTimer, RenderNode)

public:
    RenderTextTimer();
    ~RenderTextTimer() override;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void Tick(uint64_t duration);
    void UpdateValue(uint32_t elapsedTime);
    bool GetIsCountDown() const
    {
        return isCountDown_;
    }
    double GetCount() const
    {
        return inputCount_;
    }
    std::string GetFormat() const
    {
        return format_;
    }
    TextStyle GetTextStyle() const
    {
        if (textComponent_) {
            return textComponent_->GetTextStyle();
        }
        return TextStyle();
    }

protected:
    uint64_t elapsedTime_ = 0; // millisecond.
    double inputCount_ = 0.0;
    bool isCountDown_ = false;
    std::string format_;
    std::function<void(uint64_t, uint64_t)> onTimer_;

private:
    void HandleStart();
    void HandlePause();
    void HandleReset();

    Size realSize_;
    RefPtr<TextComponent> textComponent_;
    RefPtr<RenderText> renderText_;
    RefPtr<Scheduler> scheduler_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_RENDER_TEXT_OVERLAY_H
