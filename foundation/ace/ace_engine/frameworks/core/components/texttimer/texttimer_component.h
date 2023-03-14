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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_TEXTTIMER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_TEXTTIMER_COMPONENT_H

#include "core/components_v2/common/common_def.h"
#include "core/components/declaration/texttimer/texttimer_declaration.h"
#include "core/components/texttimer/texttimer_controller.h"
#include "core/pipeline/base/render_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {
class ACE_EXPORT TextTimerComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(TextTimerComponent, SoleChildComponent);

public:
    TextTimerComponent();
    ~TextTimerComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    double GetInputCount() const;
    void SetInputCount(double value);

    bool GetIsCountDown() const;
    void SetIsCountDown(bool isCountDown);

    const std::string& GetFormat() const;
    void SetFormat(const std::string& value);

    const TextStyle& GetTextStyle() const;
    void SetTextStyle(const TextStyle& style);

    RefPtr<TextTimerController> GetTextTimerController() const;

    ACE_DEFINE_COMPONENT_EVENT(OnTimer, void(uint64_t, uint64_t));

private:
    RefPtr<TextTimerController> textTimerController_;
    RefPtr<TextTimerDeclaration> declaration_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXTTIMER_TEXTTIMER_COMPONENT_H