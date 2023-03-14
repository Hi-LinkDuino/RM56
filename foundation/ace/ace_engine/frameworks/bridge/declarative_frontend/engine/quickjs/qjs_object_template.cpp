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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_object_template.h"

namespace OHOS::Ace::Framework {

QJSObjectTemplate::QJSObjectTemplate()
{
    JSContext* ctx = QJSContext::Current();
    proto_ = JS_NewObject(ctx);
}

QJSObjectTemplate::~QJSObjectTemplate() {}

JSValue QJSObjectTemplate::operator*() const
{
    return proto_;
}

JSValue QJSObjectTemplate::NewInstance() const
{
    JSContext* ctx = QJSContext::Current();
    return JS_NewObjectProto(ctx, proto_);
}

} // namespace OHOS::Ace::Framework
