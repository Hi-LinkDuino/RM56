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

#include "core/components/test/json/color_creator.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

Color ColorCreator::CreateFromJson(const JsonValue& componentJson)
{
    LOGD("CreateFromJson color");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != COLOR_NAME) {
        LOGE("Create color err: not a color json.");
        return Color();
    }
    if (componentJson.Contains(COLOR_VALUE) && componentJson.GetValue(COLOR_VALUE)->IsNumber()) {
        uint32_t value = componentJson.GetValue(COLOR_VALUE)->GetUInt();
        return Color(value);
    }
    uint8_t red = 0;
    if (componentJson.Contains(COLOR_RED) && componentJson.GetValue(COLOR_RED)->IsNumber()) {
        red = static_cast<uint8_t>(componentJson.GetValue(COLOR_RED)->GetInt());
    }
    uint8_t green = 0;
    if (componentJson.Contains(COLOR_GREEN) && componentJson.GetValue(COLOR_GREEN)->IsNumber()) {
        green = static_cast<uint8_t>(componentJson.GetValue(COLOR_GREEN)->GetInt());
    }
    uint8_t blue = 0;
    if (componentJson.Contains(COLOR_BLUE) && componentJson.GetValue(COLOR_BLUE)->IsNumber()) {
        blue = static_cast<uint8_t>(componentJson.GetValue(COLOR_BLUE)->GetInt());
    }
    if (componentJson.Contains(COLOR_ALPHA) && componentJson.GetValue(COLOR_ALPHA)->IsNumber()) {
        uint8_t alpha = static_cast<uint8_t>(componentJson.GetValue(COLOR_ALPHA)->GetInt());
        return Color::FromARGB(alpha, red, green, blue);
    }
    if (componentJson.Contains(COLOR_OPACITY) && componentJson.GetValue(COLOR_OPACITY)->IsNumber()) {
        double opacity = componentJson.GetValue(COLOR_OPACITY)->GetDouble();
        return Color::FromRGBO(red, green, blue, opacity);
    }
    return Color::FromRGB(red, green, blue);
}

} // namespace OHOS::Ace