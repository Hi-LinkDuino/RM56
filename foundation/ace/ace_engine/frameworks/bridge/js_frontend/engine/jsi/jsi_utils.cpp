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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_base_utils.h"

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"

namespace OHOS::Ace::Framework {
int32_t GetLineOffset(const AceType *data)
{
    const int32_t offset = 14;
    return offset;
}

RefPtr<JsAcePage> GetRunningPage(const AceType *data)
{
    if (data == nullptr || !AceType::InstanceOf<JsiEngineInstance>(data)) {
        return nullptr;
    }
    auto instance = static_cast<const JsiEngineInstance *>(data);

    return instance->GetRunningPage();
}
} // namespace OHOS::Ace::Framework
