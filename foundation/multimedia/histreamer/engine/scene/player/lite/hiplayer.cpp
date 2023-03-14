/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "HiPlayer"

#include "scene/lite/hiplayer.h"

#include "hiplayer_impl.h"
#include "foundation/log.h"

namespace OHOS {
namespace Media {
std::shared_ptr<Media::PlayerInterface> CreateHiPlayer()
{
    MEDIA_LOG_W("Histreamer compile time: " PUBLIC_LOG_S " " PUBLIC_LOG_S, __DATE__, __TIME__);
    return HiPlayerImpl::CreateHiPlayerImpl();
}
} // namespace Media
} // namespace OHOS
