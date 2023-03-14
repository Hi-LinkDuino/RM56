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

#include "mock_services.h"

namespace OHOS {
namespace ACELite {
void MockServices::StartAll()
{
    if (!amsThread_.isRunning()) {
        amsThread_.start();
    }
    if (!vsyncThread_.isRunning()) {
        vsyncThread_.start();
    }
    if (!timerThread_.isRunning()) {
        timerThread_.start();
    }
}

void MockServices::QuitAll()
{
    if (vsyncThread_.isRunning()) {
        vsyncThread_.quit();
    }
    if (amsThread_.isRunning()) {
        amsThread_.quit();
    }
    if (timerThread_.isRunning()) {
        timerThread_.quit();
    }
}
} // namespace ACELite
} // namespace OHOS
