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

#include "core/components/split_container/row_split_component.h"

#include "core/components/split_container/render_row_split.h"
#include "core/components/split_container/row_split_element.h"

namespace OHOS::Ace {

RefPtr<Element> RowSplitComponent::CreateElement()
{
    return AceType::MakeRefPtr<RowSplitElement>();
}

RefPtr<RenderNode> RowSplitComponent::CreateRenderNode()
{
    return RenderRowSplit::Create();
}

} // namespace OHOS::Ace