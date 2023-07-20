/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ace_mem_base.h"
#include "ctype.h"
#include "js_config.h"
#include "securec.h"
#include "string_util.h"

namespace OHOS {
namespace ACELite {
char *StringUtil::Copy(const char *sequence)
{
    if (sequence == nullptr) {
        return nullptr;
    }
    size_t size = strlen(sequence);
    if (size >= UINT16_MAX) {
        return nullptr;
    }
    char *buffer = StringUtil::Malloc(size);
    if (buffer == nullptr) {
        return nullptr;
    }
    if (strncpy_s(buffer, size + 1, sequence, size) == 0) {
        return buffer;
    }
    ace_free(buffer);
    return nullptr;
}
char *StringUtil::Malloc(const uint32_t size)
{
    char *buffer = static_cast<char *>(ace_malloc(size + 1));
    if (buffer == nullptr) {
        return nullptr;
    }
    buffer[size] = '\0';
    return buffer;
}
char *StringUtil::Slice(const char *sequence, const int32_t start)
{
    return StringUtil::Slice(sequence, start, strlen(sequence));
}
char *StringUtil::Slice(const char *sequence, const int32_t start, const int32_t end)
{
    if (sequence == nullptr) {
        return nullptr;
    }
    uint32_t size = strlen(sequence);
    if (size == 0 || size >= UINT16_MAX) {
        return nullptr;
    }
    int32_t startIdx = (start < 0) ? (start + size) : start;
    startIdx = (startIdx < 0) ? 0 : startIdx;
    int32_t endIdx = (end < 0) ? (end + size) : end;
    endIdx = (endIdx > static_cast<int32_t>(size)) ? size : endIdx;
    if (startIdx > endIdx || endIdx < 0) {
        return nullptr;
    }
    int32_t diffSize = endIdx - startIdx;
    if (diffSize < 0) {
        return nullptr;
    }
    char *buffer = StringUtil::Malloc(diffSize);
    if (buffer == nullptr) {
        return nullptr;
    }
    if (strncpy_s(buffer, diffSize + 1, sequence + startIdx, diffSize) == 0) {
        return buffer;
    }
    ace_free(buffer);
    return nullptr;
}
bool StringUtil::StartsWith(const char *sequence, const char *subsequence)
{
    if (sequence == nullptr) {
        return subsequence == nullptr;
    }
    if (subsequence == nullptr) {
        return true;
    }
    return strncmp(sequence, subsequence, strlen(subsequence)) == 0;
}

char *StringUtil::Trim(char *sequence)
{
    if (sequence == nullptr) {
        return nullptr;
    }
    if (strlen(sequence) == 0) {
        return sequence;
    }
    char *leftP = sequence;
    char *rightP = sequence;
    char *endP = sequence;
    // find the first no-space position
    while (*leftP != '\0' && isspace(*leftP)) {
        leftP++;
    }
    // copy all charaters one by one from the first no-space position to the start of the buffer
    while (*leftP != '\0') {
        *rightP = *leftP;
        if (!isspace(*rightP)) {
            // record the next position of the last no-space character
            endP = rightP + 1;
        }
        leftP++;
        rightP++;
    }
    *endP = '\0';
    return sequence;
}

void StringUtil::Str2map(const std::string& item, std::map<std::string, float>& kv) {
    int startIndex = item.find_first_of('(');
    int endIndex = item.find_first_of(')');
    std::string key = item.substr(0, startIndex);
    std::string value = item.substr(startIndex + 1, endIndex - startIndex - 1);
    if (key == "rotate" || key == "translateX" || key == "translateY") {
        kv[key] = atoi(value.c_str());
    }
    else if (key == "scale") {
        kv[key] = atof(value.c_str());
    }
    else {
        std::cout << "transform params error" << std::endl;
    }
}

std::string StringUtil::TrimStr(const std::string& str, char cTrim)
{
    auto firstPos = str.find_first_not_of(cTrim);
    if (firstPos == std::string::npos) {
        return str;
    }
    auto endPos = str.find_last_not_of(cTrim);
    return str.substr(firstPos, endPos - firstPos + 1);
}

void StringUtil::SplitStr(const std::string& str, const std::string& sep, std::map<std::string, float>& out, bool needTrim)
{
    out.erase(out.begin(), out.end());
    if (str.empty() || sep.empty()) {
        return;
    }
    std::string strPart;
    std::string::size_type startPos = 0;
    std::string::size_type pos = str.find_first_of(sep, startPos);
    while (pos != std::string::npos) {
        if (pos > startPos) {
            strPart = needTrim ? TrimStr(str.substr(startPos, pos - startPos)) : str.substr(startPos, pos - startPos);
            Str2map(std::move(strPart), out);
        }
        startPos = pos + sep.size();
        pos = str.find_first_of(sep, startPos);
    }
    if (startPos < str.size()) {
        strPart = needTrim ? TrimStr(str.substr(startPos)) : str.substr(startPos);
        Str2map(std::move(strPart), out);
    }
}

bool StringUtil::ComparePath(const char* path, const char* otherPath)
{
    if (path == nullptr || otherPath == nullptr){
        return false;
    }
    
    int pathLen = strlen(path);
    int otherPathLen = strlen(otherPath); 

    //suffix .bc or .js
    if(pathLen <= 3 || otherPathLen <= 3){
        return false;
    }

    int pathIndex = path[0] == '/' ? 1 : 0;
    int otherPathIndex = otherPath[0] == '/' ? 1:0;

    while(pathIndex < pathLen && otherPathIndex < otherPathLen - 3){
        if(path[pathIndex] != otherPath[otherPathIndex]){
            return false;
        }
        pathIndex++;
        otherPathIndex++;
    }

    return true;
}

} // namespace ACELite
} // namespace OHOS
