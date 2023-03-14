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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_SHARE_DATA_JS_SHARE_DATA_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_SHARE_DATA_JS_SHARE_DATA_H

#include "base/memory/ace_type.h"
#include "bridge/codec/standard_codec_buffer_operator.h"
#include "bridge/declarative_frontend/engine/bindings.h"
#include "bridge/declarative_frontend/engine/js_ref_ptr.h"

namespace OHOS::Ace::Framework {

class JSShareData : public Referenced {
public:
    explicit JSShareData(int32_t bufferId);
    ~JSShareData();

    static void JSBind(BindingTarget globalObj);

    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSShareData* instance);

private:
    void ReadInt32(const JSCallbackInfo& info);
    void ReadInt64(const JSCallbackInfo& info);
    void ReadDouble(const JSCallbackInfo& info);
    void ReadString(const JSCallbackInfo& info);
    void ReadBoolean(const JSCallbackInfo& info);
    void ReadObject(const JSCallbackInfo& info);
    void ReleaseShareBufferById(const JSCallbackInfo& info);

private:
    int32_t id_;
    StandardCodecBufferReader* byteBufferPtr_ = nullptr;
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_SHARE_DATA_JS_SHARE_DATA_H