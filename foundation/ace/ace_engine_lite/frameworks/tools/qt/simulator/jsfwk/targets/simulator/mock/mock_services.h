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

#ifndef OHOS_ACELITE_MOCK_SERVICES_H
#define OHOS_ACELITE_MOCK_SERVICES_H

#include "ams_thread.h"
#include "timer_thread.h"
#include "vsync_thread.h"

namespace OHOS {
namespace ACELite {
class MockServices final {
public:
    MockServices() = default;
    virtual ~MockServices() = default;
    void StartAll();
    void QuitAll();

private:
    AMSThread amsThread_;
    VsyncThread vsyncThread_;
    TimerThread timerThread_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_MOCK_SERVICES_H
