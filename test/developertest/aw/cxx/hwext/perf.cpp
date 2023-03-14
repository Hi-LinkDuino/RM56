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

#include "perf.h"

#include <cstring>
#include <sstream>
#include <cstdio>
#include <cstdlib>

#include <libxml/parser.h>
using namespace std;

namespace OHOS {
namespace TestAW {

#define ERR_MSG(...)    fprintf(stderr, __VA_ARGS__)
#define INF_MSG(...)    fprintf(stdout, __VA_ARGS__)
#define DBG_MSG(...)    fprintf(stdout, __VA_ARGS__)

#define ID_LARGER_IS_BETTER    true
#define ID_SMALLER_IS_BETTER   false

namespace {
    const auto XML_TAG_ROOT       = "configuration";
    const auto XML_TAG_DATETIME   = "datetime";
    const auto XML_TAG_URL        = "url";
    const auto XML_TAG_CASENAME   = "name";
    const int ID_PROPERTY_LENGTH = 64;
}

// BaseLineManager
BaseLineManager::BaseLineManager()
    : m_bNoBaseline(false)
{
}

BaseLineManager::BaseLineManager(const string path)
    : m_bNoBaseline(false)
{
    LoadConfig(path);
}

BaseLineManager::~BaseLineManager()
{
    if (m_bNoBaseline) {
        ERR_MSG("[ WARNING  ] no baseline loaded, please manual check output value is OK.\n");
    }
}

// parser configuration file in json format.
bool BaseLineManager::LoadConfig(const string path)
{
    m_bNoBaseline = false;
    if (!ReadXmlFile(path)) {
        ERR_MSG("[ WARNING  ] failed to load config from %s\n", path.c_str());
        m_bNoBaseline = true;
        return false;
    }

    INF_MSG("[ PERF     ] Load BaseLines from: %s\n", path.c_str());
    return true;
}

void ParseProperties(const xmlNode currNode, map<string, string>& properties)
{
    for (auto attrs = currNode.properties; attrs != nullptr; attrs = attrs->next) {
        auto name = attrs->name;
        if (name == nullptr) {
            continue;
        }
        auto value = xmlGetProp(&currNode, name);
        if (value == nullptr) {
            continue;
        }
        string propName(reinterpret_cast<const char*>(name));
        string propValue(reinterpret_cast<char*>(value));
        properties[propName] = std::move(propValue);
        xmlFree(value);
    }
}

bool BaseLineManager::ReadXmlFile(string baselinePath)
{
    xmlDocPtr ptrXmlDoc = xmlReadFile(baselinePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    if (ptrXmlDoc == nullptr) {
        return false;
    }

    xmlNodePtr ptrRootNode = xmlDocGetRootElement(ptrXmlDoc);
    if (ptrRootNode == nullptr || ptrRootNode->name == nullptr ||
        xmlStrcmp(ptrRootNode->name, reinterpret_cast<const xmlChar*>(XML_TAG_ROOT))) {
        xmlFreeDoc(ptrXmlDoc);
        return false;
    }

    map<string, string> properties;
    ParseProperties(*ptrRootNode, properties);
    if (properties.count(XML_TAG_DATETIME) == 1) {
        m_bastCfg.date = properties[XML_TAG_DATETIME];
    }
    if (properties.count(XML_TAG_URL) == 1) {
        m_bastCfg.date = properties[XML_TAG_URL];
    }

    xmlNodePtr currNodePtr = ptrRootNode->xmlChildrenNode;
    for (; currNodePtr != nullptr; currNodePtr = currNodePtr->next) {
        if (currNodePtr->name == nullptr || currNodePtr->type == XML_COMMENT_NODE) {
            xmlFreeDoc(ptrXmlDoc);
            return false;
        }

        map<string, string> properties_temp;
        ParseProperties(*currNodePtr, properties_temp);
        m_bastCfg.items.push_back(properties_temp);
    }

    xmlFreeDoc(ptrXmlDoc);
    return true;
}

bool BaseLineManager::IsNoBaseline()
{
    return m_bNoBaseline;
}

double BaseLineManager::StrtoDouble(const string& str)
{
    istringstream iss(str);
    double num;
    iss >> num;
    return num;
}

bool BaseLineManager::GetExtraValueDouble(const string testcaseName, const string extra, double &value)
{
    if (testcaseName == "" || extra == "") {
        DBG_MSG("[ ERROR    ] invalid arguments: testcaseName=%s, extra=%s\n", testcaseName.c_str(), extra.c_str());
        return false;
    }

    for (auto iter = m_bastCfg.items.begin(); iter != m_bastCfg.items.end(); iter++) {
        map<string, string> properties = *iter;
        if (properties.count(XML_TAG_CASENAME) == 1 && properties[XML_TAG_CASENAME] == testcaseName) {
            if (properties.count(extra) == 1) {
                value = StrtoDouble(properties[extra]);
            }
            break;
        }
    }

    return true;
}

// GtestPerfTestCase
GtestPerfTestCase::GtestPerfTestCase(BaseLineManager* pManager,
    testing::Test *tester,
    int caseVersion,
    std::string testClassName,
    std::string testInterfaceName)
{
    m_pManager = pManager;
    m_pTester = tester;
    m_dCaseVersion = caseVersion;
    m_strCaseName = "";
    m_strTestClassName = testClassName;
    m_strTestInterfaceName = testInterfaceName;

    // get test case name from GTEST API.
    // should be use tester->XXX() instead of this.
    if (tester != nullptr && ::testing::UnitTest::GetInstance() != nullptr) {
        m_strCaseName = string(::testing::UnitTest::GetInstance()->current_test_info()->name());
    }

    // start initialize.
    Initialize();
}

bool GtestPerfTestCase::SetBaseLine(string testcaseName)
{
    if (testcaseName == "") {
        return false;
    }

    m_strCaseName = testcaseName;

    return Initialize();
}

void GtestPerfTestCase::ResetValues()
{
    m_bHasBaseLine = false;
    m_dbBaseLine = -1.0;
    m_bHasLastValue = false;
    m_dbLastValue = -1.0;
    m_bHasFloatRange = false;
    m_dbFloatRange = -1.0;

    m_bTestResult = false;
    m_dbTestResult = -1.0;
}

bool GtestPerfTestCase::Initialize()
{
    // clear all values.
    ResetValues();
    if (m_strCaseName == "" || m_pManager == nullptr) {
        return false;
    }

    // get baseline value
    m_bHasBaseLine = m_pManager->GetExtraValueDouble(m_strCaseName, "baseline", m_dbBaseLine);
    if (!m_bHasBaseLine) {
        return false;
    }

    // get last test value from config.
    m_bHasLastValue = m_pManager->GetExtraValueDouble(m_strCaseName, "lastvalue", m_dbLastValue);

    // get float range value from config.
    m_bHasFloatRange = m_pManager->GetExtraValueDouble(m_strCaseName, "floatrange", m_dbFloatRange);
    // check values is valid, and update them.
    if (m_bHasFloatRange && (m_dbFloatRange < 0 || m_dbFloatRange >= 1)) {
        DBG_MSG("[ ERROR    ] %s has invalid float range: %f.\n", m_strCaseName.c_str(), m_dbFloatRange);
        m_bHasFloatRange = false;
    }

    if (!m_bHasFloatRange) {
        m_dbFloatRange = 0.0;
    }
    
    if (!m_bHasLastValue) {
        m_dbLastValue = m_dbBaseLine;
    }

    return true;
}

// return true if testValue >= baseline value
bool GtestPerfTestCase::ExpectLarger(double testValue)
{
    return ExpectValue(testValue, ID_LARGER_IS_BETTER);
}

// return true if testValue <= baseline value
bool GtestPerfTestCase::ExpectSmaller(double testValue)
{
    return ExpectValue(testValue, ID_SMALLER_IS_BETTER);
}

bool GtestPerfTestCase::ExpectValue(double testValue, bool isLargerBetter)
{
    if (m_strCaseName == "") {
        ERR_MSG("[ ERROR    ] failed to get testcase name.\n");
        return false;
    }

    m_bTestResult = false;
    m_dbTestResult = testValue;

    // check pass or failed.
    if (m_pManager != nullptr && m_pManager->IsNoBaseline()) {
        // no baseline.json is loaded at startup.
        // set result to TRUE, please check testValue manually.
        m_bTestResult = true;
        EXPECT_TRUE(m_bTestResult);
    } else if (!m_bHasBaseLine) {
        ERR_MSG("[ ERROR    ] %s has NO baseline.\n", m_strCaseName.c_str());
        EXPECT_TRUE(m_bHasBaseLine);
    } else {
        double baseValue = -1;
        if (isLargerBetter) {
            baseValue = (m_dbLastValue >= m_dbBaseLine) ? m_dbLastValue : m_dbBaseLine;
            EXPECT_GE(testValue, (baseValue * (1.0 - m_dbFloatRange)));
            m_bTestResult = (testValue >= (baseValue * (1.0 - m_dbFloatRange))) ? true : false;
        } else {
            baseValue = (m_dbLastValue <= m_dbBaseLine) ? m_dbLastValue : m_dbBaseLine;
            EXPECT_LE(testValue, (baseValue * (1.0 + m_dbFloatRange)));
            m_bTestResult = (testValue <= (baseValue * (1.0 + m_dbFloatRange))) ? true : false;
        }
    }

    // save result.
    SaveResult(testValue);

    return m_bTestResult;
}

bool GtestPerfTestCase::SaveResult(double testValue)
{
    char buffer[ID_PROPERTY_LENGTH] = {0};

    if (m_pTester == nullptr) {
        ERR_MSG("[ ERROR    ] m_pTester is nullptr.\n");
        return false;
    }

    INF_MSG("[ PERF     ] %s: baseline:%f, test_result: %f\n", m_strCaseName.c_str(), m_dbBaseLine, testValue);

    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    if (snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%g", m_dbBaseLine) > 0) {
        m_pTester->RecordProperty("baseline", buffer);
    }

    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    if (snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%d", m_dCaseVersion) > 0) {
        m_pTester->RecordProperty("tc_version", buffer);
    }

    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    if (snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%g", m_dbLastValue) > 0) {
        m_pTester->RecordProperty("lastvalue", m_bHasLastValue ? buffer : "");
    }

    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    if (snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%g", testValue) > 0) {
        m_pTester->RecordProperty("value", buffer);
    }

    m_pTester->RecordProperty("category", "performance");
    m_pTester->RecordProperty("test_class", m_strTestClassName.c_str());
    m_pTester->RecordProperty("test_interface", m_strTestInterfaceName.c_str());

    return true;
}
} // namespace TestAW
} // namespace OHOS
