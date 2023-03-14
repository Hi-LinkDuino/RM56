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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_RENDER_PIECE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_RENDER_PIECE_H

#include "core/components/common/properties/border.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/edge.h"
#include "core/components/piece/piece_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderPiece : public RenderNode {
    DECLARE_ACE_TYPE(RenderPiece, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnStatusChanged(RenderStatus renderStatus) override;
    void OnPaintFinish() override;
protected:
    bool MouseHoverTest(const Point& parentLocalPoint) override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    RefPtr<PieceComponent> pieceComponent_;

private:
    RenderStatus renderStatus_ = RenderStatus::DEFAULT;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_RENDER_PIECE_H
