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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_MEDIA_QUERYER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_MEDIA_QUERYER_H

#include <string>
#include <unordered_map>

#include "base/json/json_util.h"
#include "base/utils/resource_configuration.h"

namespace OHOS::Ace::Framework {

using MediaFeature = std::unique_ptr<JsonValue>;

class ACE_FORCE_EXPORT_WITH_PREVIEW MediaQueryer {
public:
    bool MatchCondition(const std::string& condition, const MediaFeature& mediaFeature);
    std::unique_ptr<JsonValue> GetMediaFeature() const;
    void SetColorMode(ColorMode colorMode)
    {
        colorMode_ = colorMode;
    }

    void SetSurfaceSize(int32_t width, int32_t height)
    {
        width_ = width;
        height_ = height;
    }

private:
    struct QueryHistory {
        std::string mediaFeatureValue;
        bool result = false;
    };
    std::unordered_map<std::string, QueryHistory> queryHistories;
    int32_t width_ = 0;
    int32_t height_ = 0;
    ColorMode colorMode_ = ColorMode::LIGHT;

};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CARD_FRONTEND_MEDIA_QUERYER_H
