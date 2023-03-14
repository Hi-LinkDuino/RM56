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

#include "core/components/test/json/transform_creator.h"

#include "core/components/test/json/align_creator.h"
#include "core/components/test/json/offset_creator.h"
#include "core/components/transform/transform_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> TransformCreator::CreateFromJson(const JsonValue& json, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson transform");
    if (!json.Contains(CLASS_NAME)) {
        LOGE("Create TransformComponent error: not a transform class.");
        return nullptr;
    }
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != TRANSFORM_NAME) {
        LOGE("Create TransformComponent error: not a transform json.");
        return nullptr;
    }

    auto transformComponent = AceType::MakeRefPtr<TransformComponent>();
    if (json.Contains(CHILD) && json.GetValue(CHILD)->IsObject()) {
        auto childJson = json.GetValue(CHILD);
        auto child = factory.CreateComponent(*childJson);
        transformComponent->SetChild(child);
    }

    if (json.Contains(ORIGIN) && json.GetValue(ORIGIN)->IsObject()) {
        auto offset = OffsetCreator::CreateFromJson(*(json.GetValue(ORIGIN)));
        transformComponent->SetOriginDimension(offset);
    }

    if (json.Contains(TRANSFORM) && json.GetValue(TRANSFORM)->IsString()) {
        std::string transform = json.GetValue(TRANSFORM)->GetString();
        if (transform == TRANSLATE || transform == SCALE || transform == ROTATE) {
            performCreateFromJson(transform, json, transformComponent);
        }
    }
    return transformComponent;
}

void TransformCreator::performCreateFromJson(
    const std::string& key, const JsonValue& json, RefPtr<TransformComponent>& component)
{
    Matrix4 result;
    if (json.GetValue(PARAM)->IsObject()) {
        auto paramJson = json.GetValue(PARAM);
        if (paramJson->Contains(PARAM_X) && paramJson->GetValue(PARAM_X)->IsNumber() && paramJson->Contains(PARAM_Y) &&
            paramJson->GetValue(PARAM_Y)->IsNumber() && paramJson->Contains(PARAM_Z) &&
            paramJson->GetValue(PARAM_Z)->IsNumber()) {
            double px = json.GetValue(PARAM)->GetValue(PARAM_X)->GetDouble();
            double py = json.GetValue(PARAM)->GetValue(PARAM_Y)->GetDouble();
            double pz = json.GetValue(PARAM)->GetValue(PARAM_Z)->GetDouble();
            if (key == TRANSLATE) {
                result = Matrix4::CreateTranslate(px, py, 0.0f);
            } else if (key == SCALE) {
                result = Matrix4::CreateScale(px, py, 1.0f);
            } else if (key == ROTATE && json.GetValue(ANGLE_VALUE)->IsNumber()) {
                float angle = json.GetValue(ANGLE_VALUE)->GetDouble();
                result = Matrix4::CreateIdentity();
                result.SetEntry(INDEX_THREE, INDEX_TWO, PERSPECTIVE);
                result.Rotate(angle, px, py, pz);
            }
        }
    } else if (key == SCALE && json.GetValue(SCALE_VALUE)->IsNumber()) {
        float scale = json.GetValue(SCALE_VALUE)->GetDouble();
        result = Matrix4::CreateScale(scale, scale, 1.0f);
    } else if (key == ROTATE && json.GetValue(ANGLE_VALUE)->IsNumber()) {
        float angle = json.GetValue(ANGLE_VALUE)->GetDouble();
        result = Matrix4::CreateRotate(angle, 0.0f, 0.0f, 1.0f);
    }

    component->SetTransform(result);
}

} // namespace OHOS::Ace