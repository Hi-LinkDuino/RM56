/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_HDI_POWER_V1_0_POWERTYPES_H
#define OHOS_HDI_POWER_V1_0_POWERTYPES_H

#include <string>
#include <message_parcel.h>

namespace OHOS {
namespace HDI {
namespace Power {
namespace V1_0 {
using namespace OHOS;

enum PowerHdfCmd {
    CMD_REGISTER_CALLBCK = 0,
    CMD_START_SUSPEND,
    CMD_STOP_SUSPEND,
    CMD_FORCE_SUSPEND,
    CMD_SUSPEND_BLOCK,
    CMD_SUSPEND_UNBLOCK,
    CMD_DUMP,
};

enum PowerHdfCallbackCmd {
    CMD_ON_SUSPEND = 0,
    CMD_ON_WAKEUP,
};

enum PowerHdfState {
    AWAKE = 0,
    INACTIVE,
    SLEEP,
};
} // V1_0
} // Power
} // HDI
} // OHOS

#endif // OHOS_HDI_POWER_V1_0_POWERTYPES_H
