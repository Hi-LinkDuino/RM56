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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_CLOCK_RENDER_TEXT_CLOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_CLOCK_RENDER_TEXT_CLOCK_H

#include "base/i18n/localization.h"
#include "base/utils/time_util.h"
#include "core/components/image/render_image.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/components/text_clock/text_clock_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
using TimeCallback = std::function<void()>;

class RenderTextClock : public RenderNode {
    DECLARE_ACE_TYPE(RenderTextClock, RenderNode);

public:
    ~RenderTextClock() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    std::string GetFormatDateTime(const std::string& format) const;
    DateTime GetDateTime() const;
    void UpdateTimeText();
    std::function<void(uint64_t)> onDateChange;
    void SetOnDateChange(const std::function<void(uint64_t)>& value)
    {
        onDateChange = value;
    }
    RefPtr<TextClockComponent> GetTextClockComponent()
    {
        return textClockComponent_;
    }
    void UpdateTimeTextCallBack();

    double GetHoursWest() const
    {
        return hoursWest_;
    }

    const std::string& GeFormat() const
    {
        return format_;
    }

    const RefPtr<RenderText>& GetRenderText() const
    {
        return renderTimeText_;
    }

protected:
    RenderTextClock();

    std::string format_;
    double hoursWest_ = DBL_MAX;
    TextStyle textStyle_;
    std::string currentTimeText_;
    bool isStart_ = true;
    RefPtr<TextClockComponent> textClockComponent_;
    RefPtr<TextComponent> timeTextComponent_;
    RefPtr<RenderText> renderTimeText_;
    TimeCallback timeCallback_;
    uint64_t currentMillisecond_ = 0;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_CLOCK_RENDER_TEXT_CLOCK_H