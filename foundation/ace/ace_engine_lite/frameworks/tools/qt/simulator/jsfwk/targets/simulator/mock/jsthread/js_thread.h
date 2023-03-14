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

#ifndef OHOS_ACELITE_MOCK_JS_THREAD_H
#define OHOS_ACELITE_MOCK_JS_THREAD_H

#include <QList>
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>

#include "js_ability.h"
#include "js_heap_stats_dumper.h"
#include "message_queue_utils.h"
#include "product_adapter.h"

namespace OHOS {
namespace ACELite {
class JSThread : public QThread {
public:
    JSThread() : abilityPath_(nullptr), abilityBundleName_(nullptr), abilityToken_(0), actived_(false) {}
    ~JSThread();
    void run() override;
    void ConfigAbilityInfo(const char *path, const char *bundleName, uint16_t token);
    void PostCommand(AbilityMsgId command);
    int PostMessage(const AbilityInnerMsg *msg);
    TEDispatchingResult TryToDispatchTEEvent();

private:
    void HandleEventLoop();
    const AbilityInnerMsg *GetMessage();
    void ProcessOneRenderTick();
    void ResetAbilityInfo();
    ACELite::JSAbility jsAbility_;
    char *abilityPath_ = nullptr;
    char *abilityBundleName_ = nullptr;
    uint16_t abilityToken_ = 0;
    QQueue<const AbilityInnerMsg *> msgQueue_;
    QWaitCondition queueCondition_;
    QMutex mutexlock_;
    bool actived_;
    JSHeapStatsDumper jsHeapStatsDumper_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_MOCK_JS_THREAD_H
