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
#include "segment.h"

#include "string_util.h"
using namespace std;
namespace OHOS {
namespace HiviewDFX {
int Segment::GetId(void) const
{
    return StringUtil::StrToInt(GetValue(SEGMENT_ID));
}

int Segment::GetSysTid(void) const
{
    return StringUtil::StrToInt(GetValue(SEGMENT_SYSTID));
}

const string Segment::GetName(void) const
{
    return GetValue(SEGMENT_NAME);
}

void Segment::SetValue(const string& name, const string& value)
{
    if (!name.empty() && !value.empty()) {
        bundle_[name] = value;
    }
}

const string Segment::GetValue(const string& name) const
{
    auto it = bundle_.find(name);
    return (it != bundle_.end()) ? it->second : string("");
}

// example: proc_name:thread_name(sysTid=1234)
const string Segment::GetFullName(void) const
{
    string fullName;
    if (!GetValue(SEGMENT_PARENT_NAME).empty()) {
        fullName += GetValue(SEGMENT_PARENT_NAME) + ":";
    }

    if (!GetName().empty()) {
        fullName += GetName();
    } else if (GetId() > 0) {
        fullName += to_string(GetId());
    } else {
        fullName += "unknown";
    }

    int sysTid = GetSysTid();
    if (sysTid > 0) {
        fullName += "(sysTid=" + to_string(sysTid) + ")";
    }
    return fullName;
}

// example: proc_name:thread_name(sysTid=1234) waiting on lock held by
const string Segment::GetDesc(const map<string, vector<string>>& segStatusCfg) const
{
    bool found = false;
    string desc = GetFullName();
    for (const auto& cfg : segStatusCfg) {
        if (!GetValue(cfg.first).empty() && !cfg.second[1].empty()) {
            desc += cfg.second[1];
            found = true;
            break;
        }
    }
    if (!found) {
        desc += " blocked";
    }
    return desc;
}
} // namespace HiviewDFX
} // namespace OHOS