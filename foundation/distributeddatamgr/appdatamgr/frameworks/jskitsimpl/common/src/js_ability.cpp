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

#include "js_ability.h"

#include "js_logger.h"

namespace OHOS {
namespace AppDataMgrJsKit {
Context::Context(std::shared_ptr<AbilityRuntime::Context> stageContext)
{
    databaseDir_ = stageContext->GetDatabaseDir();
    LOG_DEBUG("Stage: DatabaseDir %{public}s", databaseDir_.c_str());
    preferencesDir_ = stageContext->GetPreferencesDir();
    LOG_DEBUG("Stage: PreferencesDir %{public}s", preferencesDir_.c_str());
    bundleName_ = stageContext->GetBundleName();
    LOG_DEBUG("Stage: BundleName %{public}s", bundleName_.c_str());
}

Context::Context(std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext)
{
    databaseDir_ = abilityContext->GetDatabaseDir();
    LOG_DEBUG("FA: DatabaseDir %{public}s", databaseDir_.c_str());
    preferencesDir_ = abilityContext->GetPreferencesDir();
    LOG_DEBUG("FA: PreferencesDir %{public}s", preferencesDir_.c_str());
    bundleName_ = abilityContext->GetBundleName();
    LOG_DEBUG("FA: BundleName %{public}s", bundleName_.c_str());
}

std::string Context::GetDatabaseDir()
{
    return databaseDir_;
}

std::string Context::GetPreferencesDir()
{
    return preferencesDir_;
}

std::string Context::GetBundleName()
{
    return bundleName_;
}

bool JSAbility::CheckContext(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = { 0 };
    bool mode = false;
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    napi_status status = AbilityRuntime::IsStageContext(env, args[0], mode);
    LOG_DEBUG("Check context as stage mode, mode is %{public}d, status is %{public}d", mode, status == napi_ok);
    return status == napi_ok;
}

std::shared_ptr<Context> JSAbility::GetContext(napi_env env, napi_value value)
{
    bool mode = false;
    AbilityRuntime::IsStageContext(env, value, mode);
    if (mode) {
        LOG_DEBUG("Get context as stage mode.");
        auto stageContext = AbilityRuntime::GetStageModeContext(env, value);
        if (stageContext == nullptr) {
            LOG_ERROR("GetStageModeContext failed.");
            return nullptr;
        }
        return std::make_shared<Context>(stageContext);
    }

    LOG_DEBUG("Get context as feature ability mode.");
    auto ability = AbilityRuntime::GetCurrentAbility(env);
    if (ability == nullptr) {
        LOG_ERROR("GetCurrentAbility failed.");
        return nullptr;
    }
    auto abilityContext = ability->GetAbilityContext();
    if (abilityContext == nullptr) {
        LOG_ERROR("GetAbilityContext failed.");
        return nullptr;
    }
    return std::make_shared<Context>(abilityContext);
}
} // namespace AppDataMgrJsKit
} // namespace OHOS
