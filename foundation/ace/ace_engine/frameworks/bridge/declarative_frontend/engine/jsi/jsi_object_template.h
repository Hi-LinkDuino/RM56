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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_OBJECT_TEMPLATE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_OBJECT_TEMPLATE_H

#include "ecmascript/napi/include/jsnapi.h"

#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_value_conversions.h"

namespace OHOS::Ace::Framework {

class JsiObjectTemplate {
public:
    JsiObjectTemplate();
    ~JsiObjectTemplate();

protected:
    panda::Local<panda::ObjectRef> operator*() const;

    template<typename T>
    void Constant(const char* name, T val);

    panda::Local<panda::ObjectRef> NewInstance() const;

private:
    panda::Global<panda::ObjectRef> proto_;
};

} // namespace OHOS::Ace::Framework

#include "jsi_object_template.inl"

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_OBJECT_TEMPLATE_H
