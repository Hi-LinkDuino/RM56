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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_CLOCK_TEXT_CLOCK_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_CLOCK_TEXT_CLOCK_COMPONENT_H

#include <float.h>
#include "base/resource/internal_resource.h"
#include "base/utils/utils.h"
#include "core/components_v2/common/common_def.h"
#include "core/components/text/text_component_v2.h"
#include "core/components/text_clock/text_clock_controller.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {
class ACE_EXPORT TextClockComponent : public TextComponentV2 {
    DECLARE_ACE_TYPE(TextClockComponent, TextComponentV2);

public:
    explicit TextClockComponent(const std::string& data) : TextComponentV2(data) {}
    ~TextClockComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetHoursWest(double hoursWest);
    double GetHoursWest() const;

    void SetFormat(const std::string& format);
    std::string GetFormat() const;

    void SetTextClockController(RefPtr<TextClockController> textClockController);

    RefPtr<TextClockController> GetTextClockController() const;
    ACE_DEFINE_COMPONENT_EVENT(OnDateChange, void(uint64_t));

private:
    std::string format_;
    double hoursWest_ = DBL_MAX;
    RefPtr<TextClockController> textClockController_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_CLOCK_TEXT_CLOCK_COMPONENT_H
