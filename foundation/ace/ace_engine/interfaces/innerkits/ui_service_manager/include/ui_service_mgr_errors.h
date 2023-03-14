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

#ifndef OHOS_AAFWK_UI_SERVICE_MANAGER_ERRORS_H
#define OHOS_AAFWK_UI_SERVICE_MANAGER_ERRORS_H

#include "errors.h"

namespace OHOS {
namespace Ace {
enum {
    /**
     *  Module type: ui Manager Service side
     */
    UI_MODULE_TYPE_SERVICE = 2,
};

// offset of aafwk error, only be used in this file.
constexpr ErrCode AAFWK_UI_SERVICE_ERR_OFFSET = ErrCodeOffset(SUBSYS_AAFWK, UI_MODULE_TYPE_SERVICE);

enum {
    UI_SERVICE_NOT_CONNECTED = AAFWK_UI_SERVICE_ERR_OFFSET,
    GET_UI_SERVICE_FAILED,
    UI_SERVICE_PROXY_INNER_ERR,
    UI_SERVICE_IS_NULL,
    UI_SERVICE_HANDLER_IS_NULL,
    UI_SERVICE_POST_TASK_FAILED,
    NO_CALLBACK_FOR_KEY,
    UI_SERVICE_INVALID_PARAMETER,
    UI_SERVICE_GET_PROXY_FAILED,
    UI_SERVICE_CREATE_WINDOW_FAILED,
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_AAFWK_UI_SERVICE_MANAGER_ERRORS_H
