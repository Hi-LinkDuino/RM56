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

#include "core/components/test/json/progress_creator.h"

#include "base/log/log.h"
#include "core/components/progress/progress_component.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> ProgressCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson progress");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != PROGRESS_NAME) {
        LOGE("Create progress error: not progress json");
        return nullptr;
    }

    if (!componentJson.Contains(PROGRESS_VALUE) || !componentJson.GetValue(PROGRESS_VALUE)->IsNumber()) {
        LOGE("Create progress error: missing initial value");
        return nullptr;
    }
    if (!componentJson.Contains(PROGRESS_MIN) || !componentJson.GetValue(PROGRESS_MIN)->IsNumber()) {
        LOGE("Create progress error: missing min");
        return nullptr;
    }
    if (!componentJson.Contains(PROGRESS_MAX) || !componentJson.GetValue(PROGRESS_MAX)->IsNumber()) {
        LOGE("Create progress error: missing max");
        return nullptr;
    }
    if (!componentJson.Contains(PROGRESS_CACHED_VALUE) || !componentJson.GetValue(PROGRESS_CACHED_VALUE)->IsNumber()) {
        LOGE("Create progress error: missing cached value");
        return nullptr;
    }
    double value = componentJson.GetValue(PROGRESS_VALUE)->GetDouble();
    double min = componentJson.GetValue(PROGRESS_MIN)->GetDouble();
    double max = componentJson.GetValue(PROGRESS_MAX)->GetDouble();
    double cachedValue = componentJson.GetValue(PROGRESS_CACHED_VALUE)->GetDouble();
    if (max <= min || min > value || max < value || min > cachedValue || max < cachedValue) {
        LOGE("Create progress error: max is smaller or equals than min");
        return nullptr;
    }

    auto progressTypeJson = componentJson.GetValue(PROGRESS_TYPE);
    ProgressType type = ProgressType::LINEAR;
    if (progressTypeJson && progressTypeJson->IsNumber()) {
        type = static_cast<ProgressType>(progressTypeJson->GetInt());
    }
    auto progressComponent = AceType::MakeRefPtr<ProgressComponent>(min, value, cachedValue, max, type);
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto theme = themeManager->GetTheme<ProgressTheme>();
    if (theme) {
        progressComponent->InitStyle(theme);
    }
    CreateProgressTrack(progressComponent, componentJson);
    return progressComponent;
}

void ProgressCreator::CreateProgressTrack(RefPtr<ProgressComponent>& progress, const JsonValue& componentJson)
{
    if (!progress) {
        LOGE("progress is null");
        return;
    }
    auto progressTrack = progress->GetTrack();
    auto progressTrackColorJson = componentJson.GetValue(PROGRESS_BACKGROUND_COLOR);
    if (progressTrackColorJson && progressTrackColorJson->IsObject()) {
        auto progressTrackColor = ColorCreator::CreateFromJson(*progressTrackColorJson);
        progressTrack->SetBackgroundColor(progressTrackColor);
    }

    auto selectColorJson = componentJson.GetValue(PROGRESS_SELECT_COLOR);
    if (selectColorJson && selectColorJson->IsObject()) {
        auto selectColor = ColorCreator::CreateFromJson(*selectColorJson);
        progressTrack->SetSelectColor(selectColor);
    }

    auto cachedColorJson = componentJson.GetValue(PROGRESS_CACHED_COLOR);
    if (cachedColorJson && cachedColorJson->IsObject()) {
        auto cachedColor = ColorCreator::CreateFromJson(*cachedColorJson);
        progressTrack->SetCachedColor(cachedColor);
    }

    auto trackThicknessJson = componentJson.GetValue(PROGRESS_THICKNESS);
    if (trackThicknessJson && trackThicknessJson->IsNumber()) {
        progressTrack->SetTrackThickness(Dimension(trackThicknessJson->GetDouble(), DimensionUnit::PX));
    }
}

} // namespace OHOS::Ace