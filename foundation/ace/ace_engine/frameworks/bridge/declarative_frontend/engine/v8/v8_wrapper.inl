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

#include <sstream>
#include <string>

#include "v8_wrapper.h"

namespace OHOS::Ace::Framework {
template<typename C>
V8Wrapper::V8Wrapper(v8::Local<v8::Object> obj, C* instance)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    obj->SetAlignedPointerInInternalField(0, instance);
    handle_.Reset(isolate, obj);
    handle_.SetWeak(
        this,
        [](const v8::WeakCallbackInfo<V8Wrapper>& info) {
            LOGD("Finalizer for %s", V8Class<C>::ThisJSClass::JSName());
            V8Wrapper* param = info.GetParameter();
            C* instance = param->GetInstance<C>();

            if (V8Class<C>::jsDestructor_) {
                V8Class<C>::jsDestructor_(instance);
            } else {
                LOGD("No destructor callback installed on %s, proceeding with simple 'delete' of instance",
                    V8Class<C>::ThisJSClass::JSName());
                delete instance;
            }

            delete param;
        },
        v8::WeakCallbackType::kFinalizer);
}

template<typename C, typename... Args>
V8Wrapper* V8Wrapper::New(v8::Local<v8::Object> obj, Args&&... args)
{
    return new V8Wrapper(obj, new C(std::forward<Args>(args)...));
}

template<typename C>
V8Wrapper* V8Wrapper::New(v8::Local<v8::Object> obj, C* instance)
{
    return new V8Wrapper(obj, instance);
}

template<typename C>
C* V8Wrapper::GetInstance() const
{
    return static_cast<C*>(handle_.Get(v8::Isolate::GetCurrent())->GetAlignedPointerFromInternalField(0));
}
} // namespace OHOS::Ace::Framework
