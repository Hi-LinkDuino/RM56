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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_base_utils.h"

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"

namespace OHOS::Ace::Framework {
int32_t GetLineOffset(const AceType *data)
{
    if (AceType::InstanceOf<JsiEngineInstance>(data)) {
        const int32_t jsiOffset = 14;
        return jsiOffset;
    }
    if (AceType::InstanceOf<JsiDeclarativeEngineInstance>(data)) {
        const int32_t jsiDeclarativeOffset = 0;
        return jsiDeclarativeOffset;
    }
    return 0;
}

RefPtr<JsAcePage> GetRunningPage(const AceType *data)
{
    if (data == nullptr) {
        return nullptr;
    }
    if (AceType::InstanceOf<JsiEngineInstance>(data)) {
        auto instance = static_cast<const JsiEngineInstance *>(data);
        return instance->GetRunningPage();
    }
    if (AceType::InstanceOf<JsiDeclarativeEngineInstance>(data)) {
        auto instance = static_cast<const JsiDeclarativeEngineInstance *>(data);
        return instance->GetRunningPage();
    }
    return nullptr;
}
} // namespace OHOS::Ace::Framework
