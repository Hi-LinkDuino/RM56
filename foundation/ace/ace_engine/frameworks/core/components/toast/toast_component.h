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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOAST_TOAST_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOAST_TOAST_COMPONENT_H

#include <string>

#include "base/memory/ace_type.h"
#include "base/utils/singleton.h"
#include "core/components/box/box_component.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/text/text_component.h"
#include "core/components/toast/toast_theme.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class ToastComponent final : public Singleton<ToastComponent> {
    DECLARE_SINGLETON(ToastComponent);

public:
    void Show(const RefPtr<PipelineContext>& context, const std::string& message, int32_t duration,
        const std::string& bottom, bool isRightToLeft);

private:
    static int32_t GenerateNextToastId();
    void BuildToastContent(const RefPtr<TextComponent>& text, const RefPtr<ToastTheme>& toastTheme);
    void BuildPackageBox(const RefPtr<PipelineContext>& context, const RefPtr<BoxComponent>& box,
        const RefPtr<TextComponent>& text, const RefPtr<ToastTheme>& toastTheme);
    void InitToastAnimation();

    TweenOption tweenOption_;
    float toastDurationTime_ = 0.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOAST_TOAST_COMPONENT_H
