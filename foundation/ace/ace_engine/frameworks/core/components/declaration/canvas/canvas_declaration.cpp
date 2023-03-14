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

#include "frameworks/core/components/declaration/canvas/canvas_declaration.h"

namespace OHOS::Ace {

void CanvasDeclaration::InitSpecialized()
{
    // No specialized info for canvas.
}

bool CanvasDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // No specialized attr for canvas.
    return false;
}

bool CanvasDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // No specialized style for canvas.
    return false;
}

bool CanvasDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    // No specialized event for canvas.
    return false;
}

} // namespace OHOS::Ace
