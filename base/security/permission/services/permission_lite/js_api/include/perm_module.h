/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_PERM_MODULE_H
#define OHOS_ACELITE_PERM_MODULE_H

#include "jsi.h"

namespace OHOS {
namespace ACELite {
class PermModule {
public:
    PermModule() {}
    ~PermModule() {}
    static JSIValue CheckSelfPerm(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
};

void InitPermModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "check", PermModule::CheckSelfPerm);
}
} // namespace ACELite
} // namespace OHOS

#endif /* OHOS_ACELITE_PERM_MODULE_H */
