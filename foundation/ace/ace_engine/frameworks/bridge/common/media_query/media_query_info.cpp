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

#include "frameworks/bridge/common/media_query/media_query_info.h"

#include "base/log/log.h"
#include "core/common/container.h"
#include "core/common/thread_checker.h"

namespace OHOS::Ace::Framework {

std::string MediaQueryInfo::GetDeviceType()
{
    if (SystemProperties::GetParamDeviceType() == "tablet") {
        return "tablet";
    }
    switch (SystemProperties::GetDeviceType()) {
        case DeviceType::TV:
            return "tv";
        case DeviceType::CAR:
            return "car";
        case DeviceType::WATCH:
            return "wearable";
        case DeviceType::TABLET:
            return "tablet";
        default:
            return "phone";
    }
}

std::string MediaQueryInfo::GetOrientation()
{
    switch (SystemProperties::GetDevcieOrientation()) {
        case DeviceOrientation::PORTRAIT:
            return "portrait";
        case DeviceOrientation::LANDSCAPE:
            return "landscape";
        default:
            LOGE("unsupported orientation type");
    }
    return "";
}

/* 1.0 info */
std::string MediaQueryInfo::GetMediaQueryInfo() const
{
    auto json = GetMediaQueryJsonInfo();
    json->Put("isInit", false);
    return json->ToString();
}

/* 2.0 info */
std::unique_ptr<JsonValue> MediaQueryInfo::GetMediaQueryJsonInfo()
{
    CHECK_RUN_ON(JS);
    auto json = JsonUtil::Create(true);
    auto container = Container::Current();
    int32_t width = container ? container->GetViewWidth() : 0;
    int32_t height = container ? container->GetViewHeight() : 0;
    double aspectRatio = (height != 0) ? (static_cast<double>(width) / height) : 1.0;
    json->Put("width", width);
    json->Put("height", height);
    json->Put("aspect-ratio", aspectRatio);
    json->Put("round-screen", SystemProperties::GetIsScreenRound());
    json->Put("device-width", SystemProperties::GetDeviceWidth());
    json->Put("device-height", SystemProperties::GetDeviceHeight());
    json->Put("resolution", SystemProperties::GetResolution());
    json->Put("orientation", GetOrientation().c_str());
    json->Put("device-type", GetDeviceType().c_str());
    json->Put("dark-mode", SystemProperties::GetColorMode() == ColorMode::DARK);
    json->Put("api-version", StringUtils::StringToInt(SystemProperties::GetApiVersion()));
    return json;
}

} // namespace OHOS::Ace::Framework
