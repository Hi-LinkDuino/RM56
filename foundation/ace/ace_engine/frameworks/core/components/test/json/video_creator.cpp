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

#include "core/components/test/json/video_creator.h"

#include "base/log/log.h"
#include "core/components/video/video_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> VideoCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson video");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != VIDEO_NAME) {
        LOGE("Create video err: not a video json.");
        return nullptr;
    }

    if (!componentJson.Contains(VIDEO_SRC) || !componentJson.GetValue(VIDEO_SRC)->IsString()) {
        LOGE("Create video err: no src param.");
        return nullptr;
    }
    auto video = AceType::MakeRefPtr<VideoComponent>();
    video->SetSrc(componentJson.GetValue(VIDEO_SRC)->GetString());

    if (componentJson.Contains(VIDEO_AUTOPLAY) && componentJson.GetValue(VIDEO_AUTOPLAY)->IsBool()) {
        video->SetAutoPlay(componentJson.GetValue(VIDEO_AUTOPLAY)->GetBool());
    }

    if (componentJson.Contains(VIDEO_CONTROLS) && componentJson.GetValue(VIDEO_CONTROLS)->IsBool()) {
        video->SetNeedControls(componentJson.GetValue(VIDEO_CONTROLS)->GetBool());
    }

    if (componentJson.Contains(VIDEO_POSTER) && componentJson.GetValue(VIDEO_POSTER)->IsString()) {
        video->SetPoster(componentJson.GetValue(VIDEO_POSTER)->GetString());
    }

    if (componentJson.Contains(VIDEO_MUTED) && componentJson.GetValue(VIDEO_MUTED)->IsBool()) {
        video->SetMute(componentJson.GetValue(VIDEO_MUTED)->GetBool());
    }

    if (componentJson.Contains(VIDEO_FIT) && componentJson.GetValue(VIDEO_FIT)->IsString()) {
        auto imageFit = componentJson.GetValue(VIDEO_FIT)->GetString();
        video->SetFit(ConvertStrToFit(imageFit));
    }
    BuildEventFromJson(componentJson, video);

    return video;
}
void VideoCreator::BuildEventFromJson(const JsonValue& componentJson, const RefPtr<VideoComponent>& video)
{
    if (componentJson.Contains(VIDEO_PREPARED_EVENT_ID) &&
        componentJson.GetValue(VIDEO_PREPARED_EVENT_ID)->IsString()) {
        video->SetPreparedEventId(EventMarker(componentJson.GetValue(VIDEO_PREPARED_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_START_EVENT_ID) && componentJson.GetValue(VIDEO_START_EVENT_ID)->IsString()) {
        video->SetStartEventId(EventMarker(componentJson.GetValue(VIDEO_START_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_PAUSE_EVENT_ID) && componentJson.GetValue(VIDEO_PAUSE_EVENT_ID)->IsString()) {
        video->SetPauseEventId(EventMarker(componentJson.GetValue(VIDEO_PAUSE_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_FINISH_EVENT_ID) && componentJson.GetValue(VIDEO_FINISH_EVENT_ID)->IsString()) {
        video->SetFinishEventId(EventMarker(componentJson.GetValue(VIDEO_FINISH_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_ERROR_EVENT_ID) && componentJson.GetValue(VIDEO_ERROR_EVENT_ID)->IsString()) {
        video->SetErrorEventId(EventMarker(componentJson.GetValue(VIDEO_ERROR_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_SEEKING_EVENT_ID) && componentJson.GetValue(VIDEO_SEEKING_EVENT_ID)->IsString()) {
        video->SetSeekingEventId(EventMarker(componentJson.GetValue(VIDEO_SEEKING_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_SEEKED_EVENT_ID) && componentJson.GetValue(VIDEO_SEEKED_EVENT_ID)->IsString()) {
        video->SetSeekedEventId(EventMarker(componentJson.GetValue(VIDEO_SEEKED_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_TIMEUPDATE_EVENT_ID) &&
        componentJson.GetValue(VIDEO_TIMEUPDATE_EVENT_ID)->IsString()) {
        video->SetTimeUpdateEventId(EventMarker(componentJson.GetValue(VIDEO_TIMEUPDATE_EVENT_ID)->GetString()));
    }

    if (componentJson.Contains(VIDEO_FULLSCREENCHANGE_EVENT_ID) &&
        componentJson.GetValue(VIDEO_FULLSCREENCHANGE_EVENT_ID)->IsString()) {
        video->SetFullscreenChangeEventId(
            EventMarker(componentJson.GetValue(VIDEO_FULLSCREENCHANGE_EVENT_ID)->GetString()));
    }
}

ImageFit VideoCreator::ConvertStrToFit(const std::string& fit)
{
    ImageFit imageFit;
    if (fit == "fill") {
        imageFit = ImageFit::FILL;
    } else if (fit == "contain") {
        imageFit = ImageFit::CONTAIN;
    } else if (fit == "cover") {
        imageFit = ImageFit::COVER;
    } else if (fit == "scaledown") {
        imageFit = ImageFit::SCALEDOWN;
    } else {
        imageFit = ImageFit::NONE;
    }
    return imageFit;
}

} // namespace OHOS::Ace
