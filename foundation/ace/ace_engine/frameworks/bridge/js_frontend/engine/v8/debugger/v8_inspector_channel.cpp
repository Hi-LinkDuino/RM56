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

#include "frameworks/bridge/js_frontend/engine/v8/debugger/v8_inspector_channel.h"

#include <functional>
#include <unicode/unistr.h>

namespace V8Debugger {

static std::string ConvertToString(v8_inspector::StringView view)
{
    if (view.length() == 0) {
        return "";
    }
    if (view.is8Bit()) {
        return std::string(reinterpret_cast<const char*>(view.characters8()), view.length());
    }
    const uint16_t* src = view.characters16();
    const UChar* unicodeSrc = reinterpret_cast<const UChar*>(src);
    if (sizeof(*src) != sizeof(*unicodeSrc)) {
        return "";
    }
    size_t strLen = view.length() * sizeof(*src);
    std::string str(strLen, '\0');
    icu::UnicodeString utf16(unicodeSrc, view.length());
    bool flag = false;
    while (!flag) {
        icu::CheckedArrayByteSink sink(&str[0], strLen);
        utf16.toUTF8(sink);
        strLen = sink.NumberOfBytesAppended();
        str.resize(strLen);
        flag = !sink.Overflowed();
    }
    return str;
}

void V8InspectorChannel::sendResponse(int callId, std::unique_ptr<v8_inspector::StringBuffer> message)
{
    const std::string response = ConvertToString(message->string());
    v8OnResponse(response);
}

void V8InspectorChannel::sendNotification(std::unique_ptr<v8_inspector::StringBuffer> message)
{
    const std::string notification = ConvertToString(message->string());
    v8OnResponse(notification);
}

void V8InspectorChannel::flushProtocolNotifications() {}

} // namespace V8Debugger