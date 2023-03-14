/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef CAPABILITY_MODULE_H
#define CAPABILITY_MODULE_H

#include "jsi.h"

namespace OHOS {
namespace ACELite {
class CapabilityModule {
public:
    /**
     * judge if the system has the capability
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSIValue HasCapability(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
};

void InitCapabilityModule(JSIValue exports);
} // namespace ACELite
} // namespace OHOS
#endif // CAPABILITY_MODULE_H