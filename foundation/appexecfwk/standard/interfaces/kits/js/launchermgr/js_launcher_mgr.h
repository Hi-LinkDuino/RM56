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

#ifndef APPEXECFWK_JS_LAUNCHER_MGR_H
#define APPEXECFWK_JS_LAUNCHER_MGR_H

#include <mutex>
#include <string>
#include <vector>

#include "launcher_service.h"
#include "nocopyable.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
class JSLauncherMgr {
    DECLARE_DELAYED_SINGLETON(JSLauncherMgr);

public:
    static OHOS::sptr<LauncherService> GetLauncherService();

private:
    OHOS::sptr<LauncherService> launcherService_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // APPEXECFWK_JS_LAUNCHER_MGR_H
