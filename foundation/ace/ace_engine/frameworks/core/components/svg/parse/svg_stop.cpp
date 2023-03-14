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

#include "frameworks/core/components/svg/parse/svg_stop.h"

namespace OHOS::Ace {

SvgStop::SvgStop() : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgStopComponent>("svg_stop_nodeId", "stop");
}

RefPtr<SvgNode> SvgStop::Create()
{
    return AceType::MakeRefPtr<SvgStop>();
}

void SvgStop::SetAttr(const std::string& name, const std::string& value)
{
    component_->GetDeclaration()->SetSpecializedAttr(std::make_pair(name, value));
}

} // namespace OHOS::Ace
