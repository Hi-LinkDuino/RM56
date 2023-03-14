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

#include "rule_cluster.h"

#include <sys/stat.h>
#include <unistd.h>

#include "logger.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("FreezeDetector");

FreezeRuleCluster::FreezeRuleCluster()
{
    rules_.clear();
}

FreezeRuleCluster::~FreezeRuleCluster()
{
    rules_.clear();
}

bool FreezeRuleCluster::Init()
{
    if (access(DEFAULT_RULE_FILE.c_str(), R_OK) != 0) {
        HIVIEW_LOGE("cannot access rule file.");
        return false;
    }

    if (CheckFileSize(DEFAULT_RULE_FILE) == false) {
        HIVIEW_LOGE("bad rule file size.");
        return false;
    }

    if (ParseRuleFile(DEFAULT_RULE_FILE) == false) {
        HIVIEW_LOGE("failed to parse rule file.");
        return false;
    }

    if (rules_.size() == 0) {
        HIVIEW_LOGE("no rule in rule file.");
        return false;
    }

    return true;
}

bool FreezeRuleCluster::CheckFileSize(const std::string& path)
{
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    if (st.st_size > MAX_FILE_SIZE) {
        return false;
    }
    return true;
}

bool FreezeRuleCluster::ParseRuleFile(const std::string& file)
{
    xmlDoc* doc = xmlReadFile(file.c_str(), nullptr, 0);
    if (doc == nullptr) {
        HIVIEW_LOGE("failed to read rule file.");
        return false;
    }

    xmlNode* root = xmlDocGetRootElement(doc);
    if (root == nullptr) {
        HIVIEW_LOGE("failed to get root element in rule file.");
        xmlFreeDoc(doc);
        doc = nullptr;
        return false;
    }

    for (xmlNode* node = root; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (strcmp((char*)(node->name), TAG_FREEZE.c_str()) == 0) {
            ParseTagFreeze(node);
            break;
        }
    }

    xmlFreeDoc(doc);
    doc = nullptr;
    return true;
}

void FreezeRuleCluster::ParseTagFreeze(xmlNode* tag)
{
    for (xmlNode* node = tag->children; node; node = node->next) {
        if (strcmp((char*)(node->name), TAG_RULES.c_str()) == 0) {
            ParseTagRules(node);
        }
    }
}

void FreezeRuleCluster::ParseTagRules(xmlNode* tag)
{
    for (xmlNode* node = tag->children; node; node = node->next) {
        if (strcmp((char*)(node->name), TAG_RULE.c_str()) == 0) {
            ParseTagRule(node);
        }
    }
}

void FreezeRuleCluster::ParseTagRule(xmlNode* tag)
{
    unsigned long window = GetAttributeUnsignedLongValue(tag, ATTRIBUTE_WINDOW);
    std::string domain = GetAttributeStringValue(tag, ATTRIBUTE_DOMAIN);
    if (domain == "") {
        HIVIEW_LOGE("null rule attribute:domain.");
        return;
    }
    std::string stringId = GetAttributeStringValue(tag, ATTRIBUTE_STRINGID);
    if (stringId == "") {
        HIVIEW_LOGE("null rule attribute:stringid.");
        return;
    }

    FreezeRule rule = FreezeRule(window, domain, stringId);

    for (xmlNode* node = tag->children; node; node = node->next) {
        if (strcmp((char*)(node->name), TAG_LINKS.c_str()) == 0) {
            ParseTagLinks(node, rule);
        }
    }

    if (rules_.find(domain + stringId) != rules_.end()) {
        HIVIEW_LOGE("skip duplicated rule, stringid:%{public}s.", stringId.c_str());
        return;
    }

    rules_[domain + stringId] = rule;
}

void FreezeRuleCluster::ParseTagLinks(xmlNode* tag, FreezeRule& rule)
{
    for (xmlNode* node = tag->children; node; node = node->next) {
        if (strcmp((char*)(node->name), TAG_EVENT.c_str()) == 0) {
            std::string domain = GetAttributeStringValue(node, ATTRIBUTE_DOMAIN);
            if (domain == "") {
                HIVIEW_LOGE("null event attribute:domain.");
                return;
            }
            std::string stringId = GetAttributeStringValue(node, ATTRIBUTE_STRINGID);
            if (stringId == "") {
                HIVIEW_LOGE("null event attribute:stringid.");
                return;
            }

            FreezeResult result = FreezeResult(domain, stringId);
            ParseTagEvent(node, result);
            rule.AddResult(domain, stringId, result);
        }
    }
}

void FreezeRuleCluster::ParseTagEvent(xmlNode* tag, FreezeResult& result)
{
    for (xmlNode* node = tag->children; node; node = node->next) {
        if (strcmp((char*)(node->name), TAG_RESULT.c_str()) == 0) {
            ParseTagResult(node, result);
            break;
        }
    }
}

void FreezeRuleCluster::ParseTagResult(xmlNode* tag, FreezeResult& result)
{
    unsigned long code = GetAttributeUnsignedLongValue(tag, ATTRIBUTE_CODE);
    std::string scope = GetAttributeStringValue(tag, ATTRIBUTE_SCOPE);

    for (xmlNode* node = tag->children; node; node = node->next) {
        if (strcmp((char*)(node->name), TAG_RELEVANCE.c_str()) == 0) {
            std::string domain = GetAttributeStringValue(node, ATTRIBUTE_DOMAIN);
            if (domain == "") {
                HIVIEW_LOGE("null event attribute:domain.");
                continue;
            }
            std::string stringId = GetAttributeStringValue(node, ATTRIBUTE_STRINGID);
            if (stringId == "") {
                HIVIEW_LOGE("null event attribute:stringid.");
                continue;
            }

            FreezeRelevance relevance = FreezeRelevance(domain, stringId);
            result.AddRelevance(domain, stringId, relevance);
        }
    }

    result.SetId(code);
    result.SetScope(scope);
}

unsigned long FreezeRuleCluster::GetAttributeUnsignedLongValue(xmlNode* node, const std::string& name)
{
    xmlChar* prop = xmlGetProp(node, (xmlChar*)(name.c_str()));
    if (prop == nullptr) {
        HIVIEW_LOGE("failed to get long attribute.");
        return -1;
    }
    unsigned long value = strtoul((char*)(prop), nullptr, 0);
    xmlFree(prop);
    return value;
}

std::string FreezeRuleCluster::GetAttributeStringValue(xmlNode* node, const std::string& name)
{
    xmlChar* prop = xmlGetProp(node, (xmlChar*)(name.c_str()));
    if (prop == nullptr) {
        HIVIEW_LOGE("failed to get string attribute.");
        return "";
    }
    std::string value = std::string((char*)prop);
    xmlFree(prop);
    return value;
}

bool FreezeRuleCluster::GetTimeWindow(const WatchPoint& watchPoint, unsigned long& window)
{
    std::string domain = watchPoint.GetDomain();
    std::string stringId = watchPoint.GetStringId();
    if (rules_.find(domain + stringId) == rules_.end()) {
        HIVIEW_LOGE("failed to find rule time window, domain:%{public}s stringid:%{public}s.",
            domain.c_str(), stringId.c_str());
        return false;
    }

    window = rules_[domain + stringId].GetWindow();
    return true;
}

bool FreezeRuleCluster::GetResult(const WatchPoint& watchPoint, WatchPoint& matchedWatchPoint,
    const std::list<WatchPoint>& list, FreezeResult& result)
{
    std::string domain = watchPoint.GetDomain();
    std::string stringId = watchPoint.GetStringId();
    if (rules_.find(domain + stringId) == rules_.end()) {
        HIVIEW_LOGE("failed to find rule, domain:%{public}s stringid:%{public}s.",
            domain.c_str(), stringId.c_str());
        return false;
    }

    for (auto const &item : list) {
        if (rules_[domain + stringId].GetResult(item.GetDomain(), item.GetStringId(), result)) {
            matchedWatchPoint = item; // take watchpoint back
            return true;
        }
    }

    HIVIEW_LOGE("failed to match rule, domain:%{public}s stringid:%{public}s.", domain.c_str(), stringId.c_str());
    return false;
}

void FreezeRule::AddResult(const std::string& domain, const std::string& stringId, const FreezeResult& result)
{
    if (results_.find(domain + stringId) != results_.end()) {
        HIVIEW_LOGE("skip duplicated event tag, stringid:%{public}s.", stringId.c_str());
        return;
    }

    results_[domain + stringId] = result;
}

bool FreezeRule::GetResult(const std::string& domain, const std::string& stringId, FreezeResult& result)
{
    if (results_.find(domain + stringId) == results_.end()) {
        HIVIEW_LOGE("failed to find rule result, domain:%{public}s stringid:%{public}s.",
            domain.c_str(), stringId.c_str());
        return false;
    }

    result = results_[domain + stringId]; // take result back
    return true;
}

void FreezeResult::AddRelevance(const std::string& domain, const std::string& stringId, const FreezeRelevance& relevance)
{
    if (relevances_.find(domain + stringId) != relevances_.end()) {
        HIVIEW_LOGE("skip duplicated event tag, stringid:%{public}s.", stringId.c_str());
        return;
    }

    relevances_[domain + stringId] = relevance;
}

std::vector<std::string> FreezeResult::GetRelevanceStringIds() const
{
    std::vector<std::string> stringIds;
    for (auto const &relevance : relevances_) {
        stringIds.push_back(relevance.second.GetStringId());
    }
    return stringIds;
}
} // namespace HiviewDFX
} // namespace OHOS
