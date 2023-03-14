/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/common/utils/source_map.h"

#include <iostream>
#include <vector>

namespace OHOS::Ace::Framework {

const char SOURCES[] = "sources";
const char NAMES[] = "names";
const char MAPPINGS[] = "mappings";
const char FILE[] = "file";
const char SOURCE_CONTENT[] = "sourceContent";
const char SOURCE_ROOT[] = "sourceRoot";
const char DELIMITER_COMMA = ',';
const char DELIMITER_SEMICOLON = ';';
const char DOUBLE_SLASH = '\\';
const char WEBPACK[] = "webpack:///";

MappingInfo RevSourceMap::Find(int32_t row, int32_t col)
{
    if (row < 1 || col < 1) {
        LOGE("the input pos is wrong");
        return MappingInfo {};
    }
    row--;
    col--;
    // binary search
    int32_t left = 0;
    int32_t right = static_cast<int32_t>(afterPos_.size()) - 1;
    int32_t res = 0;
    if (row > afterPos_[afterPos_.size() - 1].afterRow) {
        return MappingInfo { row + 1, col + 1, files_[0] };
    }
    while (right - left >= 0) {
        int32_t mid = (right + left) / 2;
        if ((afterPos_[mid].afterRow == row && afterPos_[mid].afterColumn > col) || afterPos_[mid].afterRow > row) {
            right = mid - 1;
        } else {
            res = mid;
            left = mid + 1;
        }
    }
    std::string sources = sources_[afterPos_[res].sourcesVal];
    auto pos = sources.find(WEBPACK);
    if (pos != std::string::npos) {
        sources.replace(pos, sizeof(WEBPACK) - 1, "");
    }

    return MappingInfo {
        .row = afterPos_[res].beforeRow + 1,
        .col = afterPos_[res].beforeColumn + 1,
        .sources = sources,
    };
}

void RevSourceMap::ExtractKeyInfo(const std::string& sourceMap, std::vector<std::string>& sourceKeyInfo)
{
    uint32_t cnt = 0;
    std::string tempStr;
    for (uint32_t i = 0; i < sourceMap.size(); i++) {
        // reslove json file
        if (sourceMap[i] == DOUBLE_SLASH) {
            i++;
            tempStr += sourceMap[i];
            continue;
        }
        // cnt is used to represent a pair of double quotation marks: ""
        if (sourceMap[i] == '"') {
            cnt++;
        }
        if (cnt == 2) {
            sourceKeyInfo.push_back(tempStr);
            tempStr = "";
            cnt = 0;
        } else if (cnt == 1) {
            if (sourceMap[i] != '"') {
                tempStr += sourceMap[i];
            }
        }
    }
}

void RevSourceMap::Init(const std::string& sourceMap)
{
    std::vector<std::string> sourceKeyInfo;
    std::string mark = "";

    ExtractKeyInfo(sourceMap, sourceKeyInfo);

    // first: find the key info and record the temp key info
    // second: add the detail into the keyinfo
    for (auto keyInfo : sourceKeyInfo) {
        if (keyInfo == SOURCES || keyInfo == NAMES || keyInfo == MAPPINGS || keyInfo == FILE ||
            keyInfo == SOURCE_CONTENT ||  keyInfo == SOURCE_ROOT) {
            // record the temp key info
            mark = keyInfo;
        } else if (mark == SOURCES) {
            sources_.push_back(keyInfo);
        } else if (mark == NAMES) {
            names_.push_back(keyInfo);
        } else if (mark == MAPPINGS) {
            mappings_.push_back(keyInfo);
        } else if (mark == FILE) {
            files_.push_back(keyInfo);
        } else {
            continue;
        }
    }

    // transform to vector for mapping easily
    mappings_ = HandleMappings(mappings_[0]);

    // the first bit: the column after transferring.
    // the second bit: the source file.
    // the third bit: the row before transferring.
    // the fourth bit: the column before transferring.
    // the fifth bit: the variable name.
    for (const auto& mapping : mappings_) {
        if (mapping == ";") {
            // plus a line for each semicolon
            nowPos_.afterRow++,
            nowPos_.afterColumn = 0;
            continue;
        }
        // decode each mapping ";QAABC"
        std::vector<int32_t> ans;
        if (!VlqRevCode(mapping, ans)) {
            LOGE("decode code fail");
            return;
        }
        if (ans.size() == 1) {
            nowPos_.afterColumn += ans[0];
            continue;
        }
        // after decode, assgin each value to the position
        nowPos_.afterColumn += ans[0];
        nowPos_.sourcesVal += ans[1];
        nowPos_.beforeRow += ans[2];
        nowPos_.beforeColumn += ans[3];
        if (ans.size() == 5) {
            nowPos_.namesVal += ans[4];
        }
        afterPos_.push_back({
            nowPos_.beforeRow,
            nowPos_.beforeColumn,
            nowPos_.afterRow,
            nowPos_.afterColumn,
            nowPos_.sourcesVal,
            nowPos_.namesVal
        });
    }
    mappings_.clear();
    mappings_.shrink_to_fit();
    sourceKeyInfo.clear();
    sourceKeyInfo.shrink_to_fit();
};

std::vector<std::string> RevSourceMap::HandleMappings(const std::string& mapping)
{
    std::vector<std::string> keyInfo;
    std::string tempStr;
    for (uint32_t i = 0; i < mapping.size(); i++) {
        if (mapping[i] == DELIMITER_COMMA) {
            keyInfo.push_back(tempStr);
            tempStr = "";
        } else if (mapping[i] == DELIMITER_SEMICOLON) {
            if (tempStr != "") {
                keyInfo.push_back(tempStr);
            }
            tempStr = "";
            keyInfo.push_back(";");
        } else {
            tempStr += mapping[i];
        }
    }
    if (tempStr != "") {
        keyInfo.push_back(tempStr);
    }
    return keyInfo;
};

uint32_t RevSourceMap::Base64CharToInt(char charCode)
{
    if ('A' <= charCode && charCode <= 'Z') {
        // 0 - 25: ABCDEFGHIJKLMNOPQRSTUVWXYZ
        return charCode - 'A';
    } else if ('a' <= charCode && charCode <= 'z') {
        // 26 - 51: abcdefghijklmnopqrstuvwxyz
        return charCode - 'a' + 26;
    } else if ('0' <= charCode && charCode <= '9') {
        // 52 - 61: 0123456789
        return charCode - '0' + 52;
    } else if (charCode == '+') {
        // 62: +
        return 62;
    } else if (charCode == '/') {
        // 63: /
        return 63;
    }
    return 64;
};

bool RevSourceMap::VlqRevCode(const std::string& vStr, std::vector<int32_t>& ans)
{
    const int32_t VLQ_BASE_SHIFT = 5;
    // binary: 100000
    uint32_t VLQ_BASE = 1 << VLQ_BASE_SHIFT;
    // binary: 011111
    uint32_t VLQ_BASE_MASK = VLQ_BASE - 1;
    // binary: 100000
    uint32_t VLQ_CONTINUATION_BIT = VLQ_BASE;
    uint32_t result = 0;
    uint32_t shift = 0;
    bool continuation = 0;
    for (uint32_t i = 0; i < vStr.size(); i++) {
        uint32_t digit = Base64CharToInt(vStr[i]);
        if (digit == 64) {
            LOGE("the arg is error");
            return false;
        }
        continuation = digit & VLQ_CONTINUATION_BIT;
        digit &= VLQ_BASE_MASK;
        result += digit << shift;
        if (continuation) {
            shift += VLQ_BASE_SHIFT;
        } else {
            bool isNegate = result & 1;
            result >>= 1;
            ans.push_back(isNegate ? -result : result);
            result = 0;
            shift = 0;
        }
    }
    if (continuation) {
        LOGE("the arg is error");
        return false;
    }
    return true;
};

} // namespace OHOS::Ace::Framework
