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
#ifndef SYNTAX_RULES_H
#define SYNTAX_RULES_H

#include <list>
#include <map>
#include <string>
#include <vector>
namespace OHOS {
namespace HiviewDFX {
const std::string L1_SEG_STATUS = "segmentStatus";
const std::string L1_RULE = "Rule";
const std::string L2_DIR_OR_FILE = "DirOrFile";
const std::string L2_SKIP = "skip";
const std::string L2_SUBCATELOG = "Subcatalog";
const std::string L2_FEATUREID = "featureId";
const std::string L2_RULES = "rules";
const std::string L2_SEGMENT_RULE = "segmentRule";
const std::string L2_SEGMENT_TYPE = "segmentType";
const std::string L2_SEGMENT_START = "startSeg";
const std::string L2_SEGMENT_STACK = "segStack";

const std::string L3_NAMESPACE = "namespace";
const std::string L3_MATCH_KEY = "matchKey";
const std::string L3_NUM = "num";
const std::string L3_PARAM = "v_";
const std::string L3_DEPEND = "depend";
const std::string L3_DESCRIPTOR_LEFT = "@{";
const std::string L3_DESCRIPTOR_RIGHT = "}@";
const std::string L3_VARIABLE_TRACE_BLOCK = "@T@";
const std::string L3_SEEK_LAST = "@L@";
const std::string L3_OR_DESCRIPTOR = "@|@";
const std::string L3_AND_DESCRIPTOR = "@&@";
const std::string L3_REGULAR_DESCRIPTOR = "@R@";

struct FeatureRule {
    int num {0};
    std::string name;
    std::string source;
    std::string depend;
    std::string cmdType;
    std::map<std::string, std::string> param;
};

struct LineFeature {
    int lineCursor{0};
    std::string value;
};

struct FeatureSet {
    std::string fullPath;
    std::string segmentType;
    int skipStep;
    std::list<FeatureRule> rules;
    std::vector<std::string> startSegVec;
    std::vector<std::string> segStackVec;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SYNTAX_RULES_H */