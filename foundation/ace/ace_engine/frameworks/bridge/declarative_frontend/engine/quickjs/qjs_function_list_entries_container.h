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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_FUNCTION_LIST_ENTRIES_CONTAINER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_QUICKJS_QJS_FUNCTION_LIST_ENTRIES_CONTAINER_H

#include <vector>

#include "base/utils/singleton.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "third_party/quickjs/cutils.h"
#include "third_party/quickjs/quickjs-libc.h"
#ifdef __cplusplus
}
#endif

namespace OHOS::Ace::Framework {

class QJSFunctionListEntriesContainer : public Singleton<QJSFunctionListEntriesContainer> {
    DECLARE_SINGLETON(QJSFunctionListEntriesContainer)
public:
    template<typename... Args>
    JSCFunctionListEntry* New(Args&&... args)
    {
        JSCFunctionListEntry* entry = new JSCFunctionListEntry { std::forward<Args>(args)... };
        entries_.push_back(entry);
        return entry;
    }

private:
    std::vector<JSCFunctionListEntry*> entries_;
};
} // namespace OHOS::Ace::Framework

#endif
