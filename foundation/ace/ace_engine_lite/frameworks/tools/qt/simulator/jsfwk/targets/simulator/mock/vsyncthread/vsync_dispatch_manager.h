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

#ifndef OHOS_ACELITE_MOCK_TE_DISPATCH_MANAGER_H
#define OHOS_ACELITE_MOCK_TE_DISPATCH_MANAGER_H

#include <QList>
#include <QMutex>

#include "js_thread.h"

namespace OHOS {
namespace ACELite {
/**
 * @brief The VsyncDispatchManager class is one sington instance for easy vsync message(tick event) dispatching.
 *
 * Please note, TE(tick event) is same thing with vsync message.
 */
class VsyncDispatchManager final {
public:
    VsyncDispatchManager() = default;
    virtual ~VsyncDispatchManager() = default;
    // NOTE: this is one sington
    static VsyncDispatchManager &GetInstance();
    void RegisterVsyncReceiver(JSThread *app);
    void UnregisterVsyncReceiver(JSThread *app);
    TEDispatchingResult DispatchVsyncEvent();

private:
    QList<JSThread *> appList_;
    QMutex listLock_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_MOCK_TE_DISPATCH_MANAGER_H
