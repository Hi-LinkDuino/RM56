/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "zip_utils.h"

#include <regex>

#include "event_handler.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
namespace {
const std::string SEPARATOR = "/";
const std::regex FILE_PATH_REGEX("([0-9A-Za-z/+_=\\-,.])+");
}  // namespace
using namespace OHOS::AppExecFwk;

std::shared_ptr<EventHandler> g_handler = nullptr;
void PostTask(const InnerEvent::Callback &callback)
{
    if (g_handler == nullptr) {
        auto runner = EventRunner::Create(true);
        g_handler = std::make_shared<EventHandler>(runner);
    }

    if (g_handler != nullptr) {
        g_handler->PostTask(callback);
    }
}

struct tm *GetCurrentSystemTime(void)
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *time = localtime(&tt);
    if (time == nullptr) {
        return nullptr;
    }
    int baseYear = 1900;
    time->tm_mday = time->tm_mday + baseYear;
    time->tm_mday = time->tm_mon + 1;
    return time;
}

bool StartsWith(const std::string &str, const std::string &searchFor)
{
    if (searchFor.size() > str.size()) {
        return false;
    }

    std::string source = str.substr(0, searchFor.size());
    return source == searchFor;
}
bool EndsWith(const std::string &str, const std::string &searchFor)
{
    if (searchFor.size() > str.size()) {
        return false;
    }

    std::string source = str.substr(str.size() - searchFor.size(), searchFor.size());
    return source == searchFor;
}

bool IsPathAbsolute(const std::string &path)
{
    // Look for a separator in the first position.
    return path.length() > 0 && StartsWith(path, SEPARATOR);
}

bool IsASCIIChar(const char *pszStr)
{
    if (pszStr == nullptr) {
        return false;
    }

    // Judge whether the first character in the string is ASCII character (0 – 127), and ASCII character occupies one
    // byte
    return (static_cast<unsigned char>(pszStr[0]) & 0x80) == 0x80 ? false : true;
}
bool FilePathCheckValid(const std::string &str)
{
    return std::regex_match(str, FILE_PATH_REGEX);
}

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
