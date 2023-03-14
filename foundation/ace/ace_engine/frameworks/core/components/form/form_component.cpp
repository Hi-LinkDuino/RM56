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

#include "core/components/form/form_component.h"

#include "core/components/form/form_element.h"
#include "core/components/form/render_form.h"
#include "core/components/stage/stage_component.h"

namespace OHOS::Ace {


RefPtr<Element> FormComponent::CreateElement()
{
    return AceType::MakeRefPtr<FormElement>();
}

RefPtr<RenderNode> FormComponent::CreateRenderNode()
{
    return RenderForm::Create();
}

} // namespace OHOS::Ace