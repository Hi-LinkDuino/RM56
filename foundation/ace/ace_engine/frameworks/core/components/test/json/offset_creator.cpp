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

#include "core/components/test/json/offset_creator.h"

#include "base/log/log.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

Offset OffsetCreator::CreateFromJson(const JsonValue& componentJson)
{
    LOGD("CreateFromJson Offset");
    double dx = 0.0;
    double dy = 0.0;
    if (componentJson.Contains(HORIZONTAL_OFFSET) && componentJson.GetValue(HORIZONTAL_OFFSET)->IsNumber()) {
        dx = componentJson.GetValue(HORIZONTAL_OFFSET)->GetDouble();
    }
    if (componentJson.Contains(VERTICAL_OFFSET) && componentJson.GetValue(VERTICAL_OFFSET)->IsNumber()) {
        dy = componentJson.GetValue(VERTICAL_OFFSET)->GetDouble();
    }
    Offset offsetComponent = Offset(dx, dy);
    return offsetComponent;
}

} // namespace OHOS::Ace
