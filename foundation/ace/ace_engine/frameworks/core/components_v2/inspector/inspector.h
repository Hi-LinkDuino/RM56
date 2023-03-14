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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_H

#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace::V2 {

struct JsKeyEvent final {
    KeyCode code { KeyCode::KEY_UNKNOWN };
    KeyAction action { KeyAction::UNKNOWN };
    int64_t timeStamp = 0;
    int32_t metaKey = 0;
    int32_t sourceDevice = 0;
    int32_t deviceId = 0;
};

class ACE_EXPORT Inspector {
public:
    static std::string GetInspectorNodeByKey(const RefPtr<PipelineContext>& context, const std::string& key);

    static std::string GetInspectorTree(const RefPtr<PipelineContext>& context);

    static bool SendEventByKey(
        const RefPtr<PipelineContext>& context, const std::string& key, int action, const std::string& params);

    static bool SendKeyEvent(const RefPtr<PipelineContext>& context, const JsKeyEvent& event);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_H
