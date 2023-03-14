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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENT_FORM_FORM_WINDOW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENT_FORM_FORM_WINDOW_H

#include <memory>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "core/common/ace_page.h"
#include "core/common/window.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class ACE_EXPORT FormWindow : public Window {
public:
    explicit FormWindow(WeakPtr<PipelineContext> context) : Window(nullptr), outSidePipelineContext_(context) {}
    ~FormWindow() = default;

    void RequestFrame() override;

    void Destroy() override
    {}

    void SetRootRenderNode(const RefPtr<RenderNode>& root) override
    {}

    void SetVsyncCallback(AceVsyncCallback&& callback) override;

private:
    WeakPtr<PipelineContext> outSidePipelineContext_;

    ACE_DISALLOW_COPY_AND_MOVE(FormWindow);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENT_FORM_FORM_WINDOW_H
