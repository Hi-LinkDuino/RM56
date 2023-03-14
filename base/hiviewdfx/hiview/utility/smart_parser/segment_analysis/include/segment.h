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
#ifndef SMART_PARSER_SEGMENT_H
#define SMART_PARSER_SEGMENT_H
#include <map>
#include <string>
#include <vector>
namespace OHOS {
namespace HiviewDFX {
namespace {
static const std::string SEGMENT_ID = "v_id";
static const std::string SEGMENT_NAME = "v_name";
static const std::string SEGMENT_SYSTID = "v_sysTid";
static const std::string SEGMENT_PARENT_NAME = "parentName";
}
// describe a segment in log file
class Segment {
public:
    Segment() {}
    ~Segment() {}
    Segment(const Segment&) = delete;
    Segment& operator=(const Segment&) = delete;
    Segment(Segment&&) = delete;
    Segment& operator=(Segment&&) = delete;
    int GetId(void) const;
    int GetSysTid(void) const;
    const std::string GetName(void) const;
    void SetValue(const std::string& name, const std::string& value);
    const std::string GetValue(const std::string& name) const;
    const std::string GetFullName(void) const;
    const std::string GetDesc(const std::map<std::string, std::vector<std::string>>& segStatusCfg) const;

private:
    std::map<std::string, std::string> bundle_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SMART_PARSER_SEGMENT_H */
