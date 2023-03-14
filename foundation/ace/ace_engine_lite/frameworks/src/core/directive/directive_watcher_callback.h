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

#ifndef OHOS_ACELITE_DIRECTIVE_WATCHER_CALLBACK_H
#define OHOS_ACELITE_DIRECTIVE_WATCHER_CALLBACK_H

#include "directive/descriptor_utils.h"
#include "non_copyable.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
class DirectiveWatcherCallback final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DirectiveWatcherCallback);

    /**
     * @brief Destructor.
     */
    ~DirectiveWatcherCallback() {}

    /**
     * @brief The watcher callback function which will be trigger when the directive condition was changed.
     * @param func The watcher callback function.
     * @param context The execute context of watcher callback.
     * @param args The arguments of watcher callback function.
     * @param argsSize the arguments size of watcher callback function.
     * @return The return value when the watcher callback was executed.
     */
    static JSValue Handler(const JSValue func, const JSValue context, const JSValue args[], const JSSize argsSize);

private:
    /**
     * @brief The arguments size of watcher callback function.
     */
    static const int CALLBACK_ARGS_LENGTH;

    /**
     * @brief The index of options argument.
     */
    static const int ARG_IDX_OPTIONS;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_DIRECTIVE_WATCHER_CALLBACK_H
