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

#include "core/components/test/unittest/stack/stack_test_utils.h"

#include "core/components/positioned/positioned_component.h"
#include "core/components/stack/stack_component.h"

namespace OHOS::Ace {

RefPtr<RenderStack> StackTestUtils::CreateRenderStack(const Alignment& alignment, Overflow overflow, StackFit stackFit)
{
    RefPtr<RenderStack> stack = AceType::MakeRefPtr<MockRenderStack>();
    std::list<RefPtr<Component>> children;
    RefPtr<StackComponent> stackComponent =
        AceType::MakeRefPtr<StackComponent>(alignment, stackFit, overflow, children);
    stack->Update(stackComponent);
    return stack;
}

RefPtr<RenderPositioned> StackTestUtils::CreateRenderPositioned(RefPtr<PositionedComponent> positioned)
{
    RefPtr<RenderPositioned> renderPositioned = AceType::MakeRefPtr<RenderPositioned>();
    renderPositioned->Update(positioned);
    return renderPositioned;
}

} // namespace OHOS::Ace
