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

#ifndef OHOS_ACELITE_SCOPE_JS_VALUE_H
#define OHOS_ACELITE_SCOPE_JS_VALUE_H

#include "jerryscript.h"
#include "non_copyable.h"

#include "stdlib.h"

namespace OHOS {
namespace ACELite {
// Local scope JS value, which can be released (decrease the ref count) automatically
// when leaving its scope, do not transfer one ScopeJSValue out of its outtest scope.
class ScopeJSValue final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ScopeJSValue);
    ScopeJSValue(jerry_value_t value) : value_(value) {}
    ~ScopeJSValue()
    {
        if (value_ == 0) {
            return;
        }
        jerry_release_value(value_);
        value_ = 0;
    }

    jerry_value_t Obain() const
    {
        return value_;
    }

private:
    // disallow create the object on the heap, only can use on the stack
    void *operator new(size_t size)
    {
        (void)(size);
        // return buffer to dismiss the compolier warning,
        // actually no-fact as this function never be used
        const size_t bufferLen = 4;
        return malloc(bufferLen);
    }
    void operator delete(void *ptr)
    {
        (void)(ptr);
    }

    jerry_value_t value_ = 0;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_SCOPE_JS_VALUE_H
