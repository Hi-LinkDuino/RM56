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

#ifndef OHOS_ACELITE_LAZY_LOAD_WATCHER_H
#define OHOS_ACELITE_LAZY_LOAD_WATCHER_H

#include "jerryscript-core.h"
#include "js_fwk_common.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class LazyLoadWatcher final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LazyLoadWatcher);

    LazyLoadWatcher(jerry_value_t nativeElement, jerry_value_t attrName, jerry_value_t getter, uint16_t keyId);

    ~LazyLoadWatcher();

    jerry_value_t GetNativeElement() const
    {
        return nativeElement_;
    }

    jerry_value_t GetAttrName() const
    {
        return attrName_;
    }

    jerry_value_t GetAttrGetter() const
    {
        return getter_;
    }

    uint16_t GetKeyId() const
    {
        return keyId_;
    }
private:
    jerry_value_t nativeElement_;
    jerry_value_t attrName_;
    jerry_value_t getter_;
    uint16_t keyId_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_LAZY_LOAD_WATCHER_H
