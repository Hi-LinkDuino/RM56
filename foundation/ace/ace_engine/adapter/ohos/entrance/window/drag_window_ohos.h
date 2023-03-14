/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_OSAL_DRAG_WINDOW_OHOS_H
#define FOUNDATION_ACE_ADAPTER_OHOS_OSAL_DRAG_WINDOW_OHOS_H

#include "wm/window.h"
#include "wm/window_option.h"
#include "wm/wm_common.h"

#ifdef ENABLE_ROSEN_BACKEND
#include "render_service_client/core/ui/rs_ui_director.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_root_node.h"
#endif

#include "base/window/drag_window.h"

namespace OHOS::Ace {
class DragWindowOhos : public DragWindow {
public:
    explicit DragWindowOhos(OHOS::sptr<OHOS::Rosen::Window> dragWindow) : dragWindow_(dragWindow) {}
    ~DragWindowOhos() = default;

    void MoveTo(int32_t x, int32_t y) const override;
    void Destory() const override;
    void DrawPixelMap(const RefPtr<PixelMap>& pixelMap) override;

private:
    OHOS::sptr<Rosen::Window> dragWindow_;
    std::shared_ptr<Rosen::RSNode> rootNode_;
    std::shared_ptr<Rosen::RSUIDirector> rsUiDirector_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_OSAL_DRAG_WINDOW_OHOS_H