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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_INTERNAL_RESOURCE_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_INTERNAL_RESOURCE_H

#include <functional>

#include "base/utils/singleton.h"

namespace OHOS::Ace {

class InternalResource final : public Singleton<InternalResource> {
    DECLARE_SINGLETON(InternalResource);

public:
    enum class ResourceId : uint32_t {
        NO_ID = 0,
        SELECT_ARROW_NORMAL_PNG,
        SELECT_ARROW_DISABLED_PNG,
        INDEXER_ARROW_PNG,
        FA_CLOCK_WIDGET_HOUR,
        FA_CLOCK_WIDGET_MINUTE,
        FA_CLOCK_WIDGET_SECOND,
        FA_BLACK_CLOCK_WIDGET_HOUR,
        FA_BLACK_CLOCK_WIDGET_MINUTE,
        FA_BLACK_CLOCK_WIDGET_SECOND,

        // 10000 - 20000 is reserved for SVG resource.
        SVG_START = 10000,
        CORRECT_SVG,
        WRONG_SVG,
        SEARCH_SVG,
        CLOSE_SVG,
        PLAY_SVG,
        PAUSE_SVG,
        FULLSCREEN_SVG,
        QUIT_FULLSCREEN_SVG,
        UP_ARROW_SVG,
        WATCH_UP_ARROW_SVG,
        DOWN_ARROW_SVG,
        WATCH_DOWN_ARROW_SVG,
        SHOW_PASSWORD_SVG,
        HIDE_PASSWORD_SVG,
        SEMI_MODAL_BAR_MIDDLE_SVG,
        SEMI_MODAL_BAR_DOWN_SVG,
        LEFT_ARROW_SVG,
        RIGHT_ARROW_SVG,
        STEPPER_BACK_ARROW,
        STEPPER_NEXT_ARROW,
        PIECE_DELETE_SVG,
        RATE_STAR_BIG_OFF_SVG,
        RATE_STAR_BIG_ON_SVG,
        RATE_STAR_SMALL_OFF_SVG,
        RATE_STAR_SMALL_ON_SVG,
        TV_RATE_STAR_BIG_OFF_SVG,
        TV_RATE_STAR_BIG_ON_SVG,
        TV_RATE_STAR_SMALL_OFF_SVG,
        TV_RATE_STAR_SMALL_ON_SVG,
        IC_BACK,
        IC_MORE,
        IC_MORE_DARK,
        IC_MORE_TRANSPARENT,
        TRANSLATE_C2E_SVG,
        SHARE_SVG,
        SIDE_BAR,
        CONTAINER_MODAL_WINDOW_CLOSE,
        CONTAINER_MODAL_WINDOW_MAXIMIZE,
        CONTAINER_MODAL_WINDOW_MINIMIZE,
        CONTAINER_MODAL_WINDOW_RECOVER,
        CONTAINER_MODAL_WINDOW_SPLIT_LEFT,
        CONTAINER_MODAL_WINDOW_DEFOCUS_CLOSE,
        CONTAINER_MODAL_WINDOW_DEFOCUS_MAXIMIZE,
        CONTAINER_MODAL_WINDOW_DEFOCUS_MINIMIZE,
        CONTAINER_MODAL_WINDOW_DEFOCUS_RECOVER,
        CONTAINER_MODAL_WINDOW_DEFOCUS_SPLIT_LEFT,
        SVG_END = 20000,

        // 20001 - 30000 is reserved for i18n resource.
        I18N_START = 20001,
        INDEXLETTER_BAR_JSON,
        ERRORINFO_JSON,
        ENTRY_JSON,
        I18N_END = 30000,
    };

    const uint8_t* GetResource(const ResourceId id, size_t& size) const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_INTERNAL_RESOURCE_H
