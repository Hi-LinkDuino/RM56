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

#include "sys_event_callback_ohos_test.h"

#include "string_util.h"

namespace OHOS {
namespace HiviewDFX {
static std::string U16String2String(std::u16string source)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convertor;
    return convertor.to_bytes(source);
}

void SysEventCallbackOhosTest::Handle(const std::u16string& domain, const std::u16string& eventName, uint32_t eventType,
    const std::u16string& eventDetail)
{
    printf("domain %s.\n", U16String2String(domain).c_str());
    printf("eventName %s.\n", U16String2String(eventName).c_str());
    printf("eventType %d.\n", eventType);
    printf("eventDetail %s.\n", U16String2String(eventDetail).c_str());
}
} // namespace HiviewDFX
} // namespace OHOS