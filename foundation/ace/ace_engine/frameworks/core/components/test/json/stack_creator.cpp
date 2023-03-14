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

#include "core/components/test/json/stack_creator.h"

#include "base/log/log.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/test/json/align_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const StackFit STACK_FITS[] = { StackFit::KEEP, StackFit::STRETCH, StackFit::INHERIT };
const Overflow OVERFLOWS[] = { Overflow::CLIP, Overflow::OBSERVABLE };
const MainStackSize MAIN_STACK_SIZES[] = { MainStackSize::MAX, MainStackSize::MIN, MainStackSize::NORMAL };

} // namespace

RefPtr<Component> StackCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Stack");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != STACK_NAME) {
        LOGE("Create stack err: not an stack json.");
        return nullptr;
    }
    auto children = factory.CreateChildComponent(componentJson);

    Alignment align = Alignment::TOP_LEFT;
    if (componentJson.Contains(STACK_ALIGNMENT) && componentJson.GetValue(STACK_ALIGNMENT)->IsObject()) {
        JsonValue alignmentJson = *(componentJson.GetValue(STACK_ALIGNMENT));
        align = AlignCreator::GetAlignmentFromJson(alignmentJson);
    }
    StackFit stackFit = StackFit::KEEP;
    if (componentJson.Contains(STACK_FIT) && componentJson.GetValue(STACK_FIT)->IsNumber()) {
        stackFit = ConvertIntToEnum(componentJson.GetValue(STACK_FIT)->GetInt(), STACK_FITS, StackFit::KEEP);
    }
    Overflow overflow = Overflow::CLIP;
    if (componentJson.Contains(STACK_OVERFLOW) && componentJson.GetValue(STACK_OVERFLOW)->IsNumber()) {
        overflow = ConvertIntToEnum(componentJson.GetValue(STACK_OVERFLOW)->GetInt(), OVERFLOWS, Overflow::CLIP);
    }
    MainStackSize mainStackSize = MainStackSize::MIN;
    if (componentJson.Contains(STACK_MAIN_STACK_SIZE) && componentJson.GetValue(STACK_MAIN_STACK_SIZE)->IsNumber()) {
        mainStackSize = ConvertIntToEnum(
            componentJson.GetValue(STACK_MAIN_STACK_SIZE)->GetInt(), MAIN_STACK_SIZES, MainStackSize::NORMAL);
    }
    auto stack = AceType::MakeRefPtr<StackComponent>(align, stackFit, overflow, children);
    stack->SetMainStackSize(mainStackSize);
    return stack;
}

} // namespace OHOS::Ace