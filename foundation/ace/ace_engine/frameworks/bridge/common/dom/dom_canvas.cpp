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

#include "frameworks/bridge/common/dom/dom_canvas.h"

#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/declaration/canvas/canvas_declaration.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Dimension DEFAULT_WIDTH = 300.0_px;
constexpr Dimension DEFAULT_HEIGHT = 150.0_px;

} // namespace

DOMCanvas::DOMCanvas(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    paintChild_ = AceType::MakeRefPtr<CustomPaintComponent>();
    ResetInitializedStyle();
}

void DOMCanvas::PrepareSpecializedComponent()
{
    paintChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    // This Func will be triggered when style/attr updated.
    auto declaration = AceType::DynamicCast<CanvasDeclaration>(declaration_);
    if (!declaration) {
        LOGE("Get Canvas Declaration failed.");
        return;
    }
}

void DOMCanvas::ResetInitializedStyle()
{
    SetWidth(DEFAULT_WIDTH);
    SetHeight(DEFAULT_HEIGHT);
    boxComponent_->SetWidth(DEFAULT_WIDTH.Value(), DEFAULT_WIDTH.Unit());
    boxComponent_->SetHeight(DEFAULT_HEIGHT.Value(), DEFAULT_HEIGHT.Unit());
}

} // namespace OHOS::Ace::Framework
