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

#include "bridge/declarative_frontend/jsview/js_loading_progress.h"

#include "core/components/common/properties/color.h"
#include "core/components/progress/loading_progress_component.h"
#include "core/components/progress/progress_component.h"
#include "core/components/track/track_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSLoadingProgress::Create()
{
    RefPtr<LoadingProgressComponent> loadingProgressComponent =
        AceType::MakeRefPtr<OHOS::Ace::LoadingProgressComponent>();
    ViewStackProcessor::GetInstance()->Push(loadingProgressComponent);
}

void JSLoadingProgress::JSBind(BindingTarget globalObj)
{
    JSClass<JSLoadingProgress>::Declare("LoadingProgress");
    MethodOptions opt = MethodOptions::NONE;

    JSClass<JSLoadingProgress>::StaticMethod("create", &JSLoadingProgress::Create, opt);
    JSClass<JSLoadingProgress>::StaticMethod("color", &JSLoadingProgress::SetColor, opt);
    JSClass<JSLoadingProgress>::Inherit<JSViewAbstract>();
    JSClass<JSLoadingProgress>::Bind(globalObj);
}

void JSLoadingProgress::SetColor(const JSCallbackInfo& info)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto loadingProgress = AceType::DynamicCast<LoadingProgressComponent>(component);
    Color progressColor;
    if (!ParseJsColor(info[0], progressColor)) {
        return;
    }
    loadingProgress->SetProgressColor(progressColor);
}

}; // namespace OHOS::Ace::Framework
