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

#include "bridge/declarative_frontend/sharedata/js_share_data.h"

#include <functional>
#include <string>

#include "bridge/declarative_frontend/engine/bindings.h"
#include "bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "core/common/sharedata/share_data.h"

namespace OHOS::Ace::Framework {

JSShareData::JSShareData(int32_t bufferId)
{
    auto bufferPtr = ShareData::GetShareBufferById(bufferId);
    this->id_ = bufferId;
    if (bufferPtr) {
        byteBufferPtr_ = new StandardCodecBufferReader(*bufferPtr);
    }
}

JSShareData::~JSShareData()
{
    if (byteBufferPtr_) {
        delete byteBufferPtr_;
        byteBufferPtr_ = nullptr;
    }
}

void JSShareData::JSBind(BindingTarget globalObj)
{
    JSClass<JSShareData>::Declare("JSShareData");
    JSClass<JSShareData>::CustomMethod("readInt32", &JSShareData::ReadInt32);
    JSClass<JSShareData>::CustomMethod("readInt64", &JSShareData::ReadInt64);
    JSClass<JSShareData>::CustomMethod("readDouble", &JSShareData::ReadDouble);
    JSClass<JSShareData>::CustomMethod("readString", &JSShareData::ReadString);
    JSClass<JSShareData>::CustomMethod("readBoolean", &JSShareData::ReadBoolean);
    JSClass<JSShareData>::CustomMethod("readObject", &JSShareData::ReadObject);
    JSClass<JSShareData>::CustomMethod("release", &JSShareData::ReleaseShareBufferById);

    JSClass<JSShareData>::Bind(
        globalObj, &JSShareData::Constructor, &JSShareData::Destructor);
}

void JSShareData::Constructor(const JSCallbackInfo& info)
{
    int argc = info.Length();
    if (argc >= 1 && info[0]->IsNumber()) {
        int32_t bufferId = info[0]->ToNumber<int32_t>();
        auto instance = AceType::MakeRefPtr<JSShareData>(bufferId);
        instance->IncRefCount();
        info.SetReturnValue(AceType::RawPtr(instance));
    }
}

void JSShareData::Destructor(JSShareData* instance)
{
    instance->DecRefCount();
}

void JSShareData::ReadInt32(const JSCallbackInfo& info)
{
    if (byteBufferPtr_ != nullptr) {
        int32_t value = 0;
        CodecData data(value);
        byteBufferPtr_->ReadData(data);
        auto returnValue = JSVal(ToJSValue(data.GetIntValue()));
        auto returnPtr = JSRef<JSVal>::Make(returnValue);
        info.SetReturnValue(returnPtr);
    }
}

void JSShareData::ReadInt64(const JSCallbackInfo& info)
{
    if (byteBufferPtr_ != nullptr) {
        int64_t value = 0;
        CodecData data(value);
        byteBufferPtr_->ReadData(data);
        auto returnValue = JSVal(ToJSValue(data.GetLongValue()));
        auto returnPtr = JSRef<JSVal>::Make(returnValue);
        info.SetReturnValue(returnPtr);
    }
}

void JSShareData::ReadDouble(const JSCallbackInfo& info)
{
    if (byteBufferPtr_ != nullptr) {
        double value = 0.0;
        CodecData data(value);
        byteBufferPtr_->ReadData(data);
        auto returnValue = JSVal(ToJSValue(data.GetDoubleValue()));
        auto returnPtr = JSRef<JSVal>::Make(returnValue);
        info.SetReturnValue(returnPtr);
    }
}

void JSShareData::ReadString(const JSCallbackInfo& info)
{
    if (byteBufferPtr_) {
        std::string str;
        CodecData data(str);
        byteBufferPtr_->ReadData(data);
        auto returnValue = JSVal(ToJSValue(data.GetStringValue()));
        auto returnPtr = JSRef<JSVal>::Make(returnValue);
        info.SetReturnValue(returnPtr);
    }
}

void JSShareData::ReadBoolean(const JSCallbackInfo& info)
{
    if (byteBufferPtr_) {
        CodecData data(true);
        byteBufferPtr_->ReadData(data);
        auto returnValue = JSVal(ToJSValue(data.GetBoolValue()));
        auto returnPtr = JSRef<JSVal>::Make(returnValue);
        info.SetReturnValue(returnPtr);
    }
}

void JSShareData::ReadObject(const JSCallbackInfo& info)
{
    if (byteBufferPtr_) {
        int32_t size = 0;
        if (byteBufferPtr_->ReadMapSize(size)) {
            JSRef<JSObject> obj = JSRef<JSObject>::New();
            for (int i = 0; i < size; i++) {
                std::string str;
                CodecData key(str);
                byteBufferPtr_->ReadData(key);
                str = key.GetStringValue();
                CodecData value;
                byteBufferPtr_->ReadData(value);
                switch (value.GetType()) {
                    case BufferDataType::TYPE_STRING:
                        obj->SetProperty<std::string>(str.c_str(), value.GetStringValue());
                        break;
                    case BufferDataType::TYPE_INT:
                        obj->SetProperty<int32_t>(str.c_str(), value.GetIntValue());
                        break;
                    case BufferDataType::TYPE_DOUBLE:
                        obj->SetProperty<double>(str.c_str(), value.GetDoubleValue());
                        break;
                    case BufferDataType::TYPE_LONG:
                        obj->SetProperty<int64_t>(str.c_str(), value.GetLongValue());
                        break;
                    default:
                        break;
                }
            }
            info.SetReturnValue(obj);
        }
    }
}

void JSShareData::ReleaseShareBufferById(const JSCallbackInfo& info)
{
    ShareData::ReleaseShareBufferById(this->id_);
    if (byteBufferPtr_) {
        delete byteBufferPtr_;
        byteBufferPtr_ = nullptr;
    }
}

} // namespace OHOS::Ace::Framework
