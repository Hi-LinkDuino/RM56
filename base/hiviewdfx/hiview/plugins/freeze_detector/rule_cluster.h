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

#ifndef FREEZE_RULE_CLUSTER_H
#define FREEZE_RULE_CLUSTER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "singleton.h"
#include "watch_point.h"

namespace OHOS {
namespace HiviewDFX {
class FreezeRelevance {
public:
    FreezeRelevance() : domain_(""), stringId_("") {};
    FreezeRelevance(const std::string& domain, const std::string& stringId) : domain_(domain), stringId_(stringId) {};
    ~FreezeRelevance() {};
    std::string GetDomain() const { return domain_; };
    void SetDomain(const std::string& domain) { domain_ = domain; };
    std::string GetStringId() const { return stringId_; };
    void SetStringId(const std::string& stringId) { stringId_ = stringId; };

private:
    std::string domain_;
    std::string stringId_;
};

class FreezeResult {
public:
    FreezeResult() : code_(0), scope_(""), domain_(""), stringId_(""), relevances_({}) {};
    FreezeResult(const std::string& domain, const std::string& stringId)
        : code_(0), scope_(""), domain_(domain), stringId_(stringId), relevances_({}) {};
    FreezeResult(unsigned long code, const std::string& scope)
        : code_(code), scope_(scope), domain_(""), stringId_(""), relevances_({}) {};
    ~FreezeResult()
    {
        relevances_.clear();
    };
    unsigned long GetId() const { return code_; };
    void SetId(unsigned long code) { code_ = code; };
    std::string GetScope() const { return scope_; };
    void SetScope(const std::string& scope) { scope_ = scope; };
    void AddRelevance(const std::string& domain, const std::string& stringId, const FreezeRelevance& relevance);
    std::vector<std::string> GetRelevanceStringIds() const;

private:
    unsigned long code_;
    std::string scope_;
    std::string domain_;
    std::string stringId_;
    std::map<std::string, FreezeRelevance> relevances_;
};

class FreezeRule {
public:
    FreezeRule() : window_(0), domain_(""), stringId_(""), results_({}) {};
    FreezeRule(unsigned long window, const std::string& domain, const std::string& stringId)
        : window_(window), domain_(domain), stringId_(stringId), results_({}) {};
    ~FreezeRule()
    {
        results_.clear();
    };
    std::string GetDomain() const { return domain_; };
    void SetDomain(const std::string& domain) { domain_ = domain; };
    std::string GetStringId() const { return stringId_; };
    void SetStringId(const std::string& stringId) { stringId_ = stringId; };
    unsigned long GetWindow() const { return window_; };
    void AddResult(const std::string& domain, const std::string& stringId, const FreezeResult& result);
    bool GetResult(const std::string& domain, const std::string& stringId, FreezeResult& result);

private:
    unsigned long window_;
    std::string domain_;
    std::string stringId_;
    std::map<std::string, FreezeResult> results_;
};

class FreezeRuleCluster : public Singleton<FreezeRuleCluster> {
    DECLARE_SINGLETON(FreezeRuleCluster);

public:
    bool Init();
    bool CheckFileSize(const std::string& path);
    bool ParseRuleFile(const std::string& file);
    void ParseTagFreeze(xmlNode* tag);
    void ParseTagRules(xmlNode* tag);
    void ParseTagRule(xmlNode* tag);
    void ParseTagLinks(xmlNode* tag, FreezeRule& rule);
    void ParseTagEvent(xmlNode* tag, FreezeResult& result);
    void ParseTagResult(xmlNode* tag, FreezeResult& result);
    void ParseTagRelevance(xmlNode* tag, FreezeResult& result);
    unsigned long GetAttributeUnsignedLongValue(xmlNode* node, const std::string& name);
    std::string GetAttributeStringValue(xmlNode* node, const std::string& name);
    bool GetTimeWindow(const WatchPoint& watchPoint, unsigned long& window);
    bool GetResult(const WatchPoint& watchPoint, WatchPoint& matchedWatchPoint,
        const std::list<WatchPoint>& list, FreezeResult& result);

private:
    static const inline std::string DEFAULT_RULE_FILE = "/system/etc/hiview/freeze_rules.xml";
    static const inline std::string TAG_FREEZE = "freeze";
    static const inline std::string TAG_RULES = "rules";
    static const inline std::string TAG_RULE = "rule";
    static const inline std::string TAG_LINKS = "links";
    static const inline std::string TAG_EVENT = "event";
    static const inline std::string TAG_RESULT = "result";
    static const inline std::string TAG_RELEVANCE = "relevance";
    static const inline std::string ATTRIBUTE_ID = "id";
    static const inline std::string ATTRIBUTE_WINDOW = "window";
    static const inline std::string ATTRIBUTE_DOMAIN = "domain";
    static const inline std::string ATTRIBUTE_STRINGID = "stringid";
    static const inline std::string ATTRIBUTE_TYPE = "type";
    static const inline std::string ATTRIBUTE_USER = "user";
    static const inline std::string ATTRIBUTE_WATCHPOINT = "watchpoint";
    static const inline std::string ATTRIBUTE_CODE = "code";
    static const inline std::string ATTRIBUTE_SCOPE = "scope";
    static const inline std::string ATTRIBUTE_APPLICATION = "application";
    static const inline std::string ATTRIBUTE_SYSTEM = "system";
    static const int MAX_FILE_SIZE = 512 * 1024;

    std::map<std::string, FreezeRule> rules_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
