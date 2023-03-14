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
#ifndef UTILS_UNICODE_EX_H
#define UTILS_UNICODE_EX_H
#include <string>
namespace OHOS {
bool String8ToString16(const std::string& str8, std::u16string& str16);
bool String16ToString8(const std::u16string& str16, std::string& str8);
}
#endif  // UTILS_BASE_LOG_H
