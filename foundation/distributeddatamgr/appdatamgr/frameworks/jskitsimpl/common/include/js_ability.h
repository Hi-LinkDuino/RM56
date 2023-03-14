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

#ifndef DISTRIBUTEDDATAMGR_APPDATAMGR_JSABILITY_H
#define DISTRIBUTEDDATAMGR_APPDATAMGR_JSABILITY_H
#include <iostream>
#include <string>

#include "ability.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_base_context.h"

namespace OHOS {
namespace AppDataMgrJsKit {
class Context {
public:
    explicit Context(std::shared_ptr<AbilityRuntime::Context> stageContext);
    explicit Context(std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext);

    std::string GetDatabaseDir();
    std::string GetPreferencesDir();
    std::string GetBundleName();

private:
    std::string databaseDir_;
    std::string preferencesDir_;
    std::string bundleName_;
};

class JSAbility final {
public:
    static bool CheckContext(napi_env env, napi_callback_info info);
    static std::shared_ptr<Context> GetContext(napi_env env, napi_value object);
};
} // namespace AppDataMgrJsKit
} // namespace OHOS

#endif // DISTRIBUTEDDATAMGR_APPDATAMGR_JSABILITY_H
