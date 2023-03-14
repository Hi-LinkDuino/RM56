/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components/declaration/texttimer/texttimer_declaration.h"

#include "bridge/common/utils/utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "core/components/texttimer/texttimer_controller.h"

namespace OHOS::Ace {
using namespace Framework;

TextTimerDeclaration::TextTimerDeclaration()
{
    textTimerController_ = AceType::MakeRefPtr<TextTimerController>();
}

void TextTimerDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_TEXTTIMER_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_TEXTTIMER_STYLE);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_TEXTTIMER_EVENT);
    AddSpecializedMethod(DeclarationConstants::DEFAULT_TEXTTIMER_METHOD);
}
} // namespace OHOS::Ace
