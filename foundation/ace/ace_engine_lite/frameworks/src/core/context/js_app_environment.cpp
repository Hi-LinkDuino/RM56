/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "js_app_environment.h"
#include "ace_event_error_code.h"
#include "ace_log.h"
#include "async_task_manager.h"
#include "handler.h"
#include "js_app_context.h"
#include "js_framework_raw.h"
#include "js_fwk_common.h"
#include "js_profiler.h"
#include "module_manager.h"
#include "platform_adapter.h"
#include "presets/console_module.h"
#include "presets/feature_ability_module.h"
#include "presets/intl_module.h"
#include "presets/jstest_module.h"
#include "presets/localization_module.h"
#include "presets/profiler_module.h"
#include "presets/render_module.h"
#include "presets/require_module.h"
#include "presets/timer_module.h"
#include "presets/version_module.h"
#include "product_adapter.h"
#include "system_info.h"
#if (JS_ENGINE_STATIC_MULTI_CONTEXTS_ENABLED == 1)

extern "C" {
#include "generate-bytecode.h"
}
#endif

namespace OHOS {
namespace ACELite {
JsAppEnvironment::JsAppEnvironment()
{
    SetEngineSnapshotMode(snapshotMode_);
}

void JsAppEnvironment::LoadAceBuiltInModules() const
{
    ConsoleModule::Load();
    RenderModule::Load();
    RequireModule::Load();
    FeaAbilityModule::Load();
    JsTestModule::Load();
    TimersModule::Load();
    PerformaceProfilerModule::Load();
    AceVersionModule::Load();
    IntlControlModule::Load();
}

void JsAppEnvironment::InitJsFramework() const
{
    START_TRACING(ENGINE_INIT);
#if (JS_ENGINE_STATIC_MULTI_CONTEXTS_ENABLED == 1)
    js_task_context_init();
#endif
#if (JERRY_PORTING_DEPENDENCY == 0)
    Srand((unsigned)jerry_port_get_current_time());
#endif
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);
    STOP_TRACING();
    START_TRACING(FWK_INIT);
#if (JSFWK_TEST == 1)
    jerry_value_t globalThis = jerry_get_global_object();
    jerry_release_value(jerryx_set_property_str(globalThis, "globalThis", globalThis));
    jerry_release_value(globalThis);
#endif // JSFWK_TEST
    AsyncTaskManager::GetInstance().Init();
    LoadAceBuiltInModules();
    ProductAdapter::LoadExtraPresetModules();
    LoadFramework();
    LocalModule::Load();
    SystemInfo::GetInstance().Initialize();
    STOP_TRACING();
}

void JsAppEnvironment::LoadFramework() const
{
    size_t len = 0;
    // load framework js/snapshot file to buffer
    const char * const jsFrameworkScript = GetFrameworkRawBuffer(snapshotMode_, len);
    const jerry_char_t *jScript = reinterpret_cast<const jerry_char_t *>(jsFrameworkScript);
    // eval framework to expose
    START_TRACING(FWK_CODE_EVAL);

    jerry_value_t retValue = UNDEFINED;
    if (snapshotMode_) {
        retValue = jerry_exec_snapshot(reinterpret_cast<const uint32_t *>(jScript), len, 0, 1);
    } else {
        retValue = jerry_eval(jScript, len, JERRY_PARSE_NO_OPTS);
    }
    STOP_TRACING();
    bool hasError = jerry_value_is_error(retValue);
    if (hasError) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to load JavaScript framework.");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_INIT_FWK_FAILED);
        PrintErrorMessage(retValue);
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "Success to load JavaScript framework.");
    }
    jerry_release_value(retValue);
    Debugger::GetInstance().StartDebugger();
}

void JsAppEnvironment::Cleanup()
{
    Debugger::GetInstance().TearDownDebugger();
    FeaAbilityModule::Release();
    ProductAdapter::UnloadExtraPresetModules();

    // clean up engine, NOTE: all JS value must be released properly befor cleanup
    jerry_cleanup();
    // free the external JS context, only can be called after clean up engine
    Debugger::GetInstance().ReleaseJSContext();
#if (JS_ENGINE_STATIC_MULTI_CONTEXTS_ENABLED == 1)
    jerry_port_default_remove_current_context_record();
#endif
}

/**
 * Decide the runtime mode of jerry
 */
void JsAppEnvironment::InitRuntimeMode()
{
    // if debugger is disabled, give a chance to use JS mode manually on device
    SetEngineSnapshotModeManually(snapshotMode_);
    if (snapshotMode_) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "ACELite is running in snapshot mode");
    } else {
        HILOG_DEBUG(HILOG_MODULE_ACE, "ACELite is running in JS parser mode");
    }
}
} // namespace ACELite
} // namespace OHOS
