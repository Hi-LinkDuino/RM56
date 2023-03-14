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

#ifndef FOUNDATION_ACE_ADAPTER_PREVIEW_ACE_ABILITY_H
#define FOUNDATION_ACE_ADAPTER_PREVIEW_ACE_ABILITY_H

#include <atomic>

#include "flutter/shell/platform/glfw/public/flutter_glfw.h"

#include "adapter/preview/entrance/ace_run_args.h"
#include "core/event/touch_event.h"

#ifndef ACE_PREVIEW_EXPORT
#ifdef _WIN32
#define ACE_PREVIEW_EXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define ACE_PREVIEW_EXPORT __attribute__((visibility("default")))
#endif
#endif // ACE_PREVIEW_EXPORT

namespace OHOS::Ace::Platform {

struct ConfigChanges {
    bool watchLocale = false;
    bool watchLayout = false;
    bool watchFontSize = false;
    bool watchOrientation = false;
    bool watchDensity = false;
};

struct SystemParams {
    int32_t deviceWidth { 0 };
    int32_t deviceHeight { 0 };
    bool isRound = false;
    double density { 1.0 };
    std::string language = "zh";
    std::string region = "CN";
    std::string script = "";
    OHOS::Ace::DeviceType deviceType { DeviceType::PHONE };
    OHOS::Ace::ColorMode colorMode { ColorMode::LIGHT };
    OHOS::Ace::DeviceOrientation orientation { DeviceOrientation::PORTRAIT };
};

class ACE_PREVIEW_EXPORT AceAbility {
public:
    explicit AceAbility(const AceRunArgs& runArgs);
    ~AceAbility();

    // Be called in Previewer frontend thread, which is not ACE platform thread.    
    static std::unique_ptr<AceAbility> CreateInstance(AceRunArgs& runArgs);    
    void InitEnv();    
    void Start();
    static void Stop();

    static bool DispatchTouchEvent(const TouchEvent& event);
    static bool DispatchBackPressedEvent();

    void OnConfigurationChanged(const DeviceConfig& newConfig);
    void SurfaceChanged(
        const DeviceOrientation& orientation, const double& resolution, int32_t& width, int32_t& height);
    void ReplacePage(const std::string& url, const std::string& params);
    void LoadDocument(const std::string& url, const std::string& componentName, SystemParams& systemParams);

    std::string GetJSONTree();
    std::string GetDefaultJSONTree();
    bool OperateComponent(const std::string& attrsJson);

private:
    void RunEventLoop();

    void SetConfigChanges(const std::string& configChanges);

    void SetGlfwWindowController(const FlutterDesktopWindowControllerRef& controller)
    {
        controller_ = controller;
    }

    // flag indicating if the glfw message loop should be running.
    static std::atomic<bool> loopRunning_;

    AceRunArgs runArgs_;
    ConfigChanges configChanges_;
    FlutterDesktopWindowControllerRef controller_ = nullptr;
};

} // namespace OHOS::Ace::Platform

#endif // FOUNDATION_ACE_ADAPTER_PREVIEW_ACE_ABILITY_H
