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

#include "core/components/scoring/scoring_component.h"

#include "core/components/scoring/render_scoring.h"
#include "core/components/scoring/scoring_element.h"

namespace OHOS::Ace {

RefPtr<RenderNode> ScoringComponent::CreateRenderNode()
{
    return RenderScoring::Create();
}

RefPtr<Element> ScoringComponent::CreateElement()
{
    return AceType::MakeRefPtr<ScoringElement>();
}

} // namespace OHOS::Ace
