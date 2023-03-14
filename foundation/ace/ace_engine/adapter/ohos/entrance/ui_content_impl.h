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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_ACE_UI_CONTENT_IMPL_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_ACE_UI_CONTENT_IMPL_H

#include "ability_info.h"
#include "interfaces/innerkits/ace/ui_content.h"
#include "interfaces/innerkits/ace/viewport_config.h"
#include "key_event.h"
#include "native_engine/native_value.h"
#include "native_engine/native_engine.h"

#include "base/utils/macros.h"
#include "wm/window.h"

namespace OHOS {

class Window;

} // namespace OHOS

namespace OHOS::Ace {

class ACE_FORCE_EXPORT UIContentImpl : public UIContent {
public:
    UIContentImpl(OHOS::AbilityRuntime::Context* context, void* runtime);
    ~UIContentImpl() = default;

    // UI content lifecycles
    void Initialize(OHOS::Rosen::Window* window, const std::string& url, NativeValue* storage) override;
    void Foreground() override;
    void Background() override;
    void Focus() override;
    void UnFocus() override;
    void Destroy() override;

    // distribute
    void Restore(OHOS::Rosen::Window* window, const std::string& contentInfo, NativeValue* storage) override;
    std::string GetContentInfo() const override;

    // UI content event process
    bool ProcessBackPressed() override;
    bool ProcessPointerEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) override;
    bool ProcessKeyEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) override;
    bool ProcessAxisEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) override;
    bool ProcessVsyncEvent(uint64_t timeStampNanos) override;
    void UpdateConfiguration(const std::shared_ptr<OHOS::AppExecFwk::Configuration>& config) override;
    void UpdateViewportConfig(const ViewportConfig& config, OHOS::Rosen::WindowSizeChangeReason reason) override;
    void UpdateWindowMode(OHOS::Rosen::WindowMode mode) override;

    void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) override;

private:
    void CommonInitialize(OHOS::Rosen::Window* window, const std::string& contentInfo, NativeValue* storage);
    void InitWindowCallback(const std::shared_ptr<OHOS::AppExecFwk::AbilityInfo>& info);
    void InitializeSubWindow(OHOS::Rosen::Window* window);
    std::weak_ptr<OHOS::AbilityRuntime::Context> context_;
    void* runtime_ = nullptr;
    OHOS::Rosen::Window* window_ = nullptr;
    std::string startUrl_;
    int32_t instanceId_ = -1;
    OHOS::sptr<OHOS::Rosen::IWindowDragListener> dragWindowListener_ = nullptr;
    OHOS::sptr<OHOS::Rosen::IOccupiedAreaChangeListener> occupiedAreaChangeListener_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_ACE_UI_CONTENT_IMPL_H
