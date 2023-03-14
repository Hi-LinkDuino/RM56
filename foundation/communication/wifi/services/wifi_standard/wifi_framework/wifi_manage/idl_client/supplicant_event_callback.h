/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_SUPPLICANT_EVENT_CALLBACK_H
#define OHOS_SUPPLICANT_EVENT_CALLBACK_H

#include <functional>

namespace OHOS {
namespace Wifi {
struct SupplicantEventCallback {
    std::function<void(int)> onScanNotify;
};
}  // namespace Wifi
}  // namespace OHOS
#endif