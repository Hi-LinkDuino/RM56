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

#ifndef AW_CXX_HWEXT_PERF_H_
#define AW_CXX_HWEXT_PERF_H_

#include <string>
#include <list>
#include <map>
#include <ctime>
#include "securec.h"

#include <gtest/gtest.h>

namespace OHOS {
namespace TestAW {

#define PERF_BASELINE_CONFIG_PATH    "/data/test/baseline.xml"

// Common Function
struct BaseLineItem {
    std::string testcasename;
    double baseline;
    double lastvalue;
    double floatrange;
    BaseLineItem()
    {
        testcasename = "";
        baseline = 0;
        lastvalue = 0;
        floatrange = 0;
    }
};

struct BaselineConfig {
    std::string date;
    std::string url;
    std::list<std::map<std::string, std::string>> items;
};

// BaseLineManager
class BaseLineManager {
public:
    BaseLineManager();
    explicit BaseLineManager(const std::string path);
    ~BaseLineManager();

    bool LoadConfig(const std::string path);
    bool IsNoBaseline();
    bool GetExtraValueDouble(const std::string testcaseName, const std::string extra, double &value);

private:
    bool ReadXmlFile(std::string path);
    double StrtoDouble(const std::string &str);

private:
    BaselineConfig m_bastCfg;
    bool m_bNoBaseline;
};

// should be defined in GTEST testcase function.
class GtestPerfTestCase {
private:
    GtestPerfTestCase() = delete;
public:
    GtestPerfTestCase(BaseLineManager* pManager, testing::Test *tester,
        int caseVersion,
        std::string testClassName = "",
        std::string testInterfaceName = "");

    ~GtestPerfTestCase() {}

    // expect result is larger than or equal baseline*(1.0-float_range).
    bool ExpectLarger(double testValue);

    // expect result is smaller than or equal baseline*(1.0-float_range).
    bool ExpectSmaller(double testValue);

    // specific baseline name (test case name).
    bool SetBaseLine(std::string testcaseName);

    // ==================  extra APIs for advanced usage   ==================
    // get test case name.
    std::string GetCaseName()
    {
        return m_strCaseName;
    };

    // get baseline value.
    bool HasBaseLine()
    {
        return m_bHasBaseLine;
    };
    double GetBaseLine()
    {
        return m_dbBaseLine;
    };

    // get last test result from remote database
    bool HasLastValue()
    {
        return m_bHasLastValue;
    };
    double GetLastValue()
    {
        return m_dbLastValue;
    };

    // get floating range: 0.0 ~ 1.0
    bool HasFloatRange()
    {
        return m_bHasFloatRange;
    };
    double GetFloatRange()
    {
        return m_dbFloatRange;
    };

    // get test result, valid after ExpectXXX() is called.
    bool GetTestResult(double *out = nullptr)
    {
        if (out != nullptr) {
            *out = m_dbTestResult;
        }
        return m_bTestResult;
    };

    // output result to stdout & xml.
    bool SaveResult(double testValue);

private:
    bool Initialize();
    void ResetValues();
    bool UpdateBaseLineName(std::string testcaseName);
    bool ExpectValue(double testValue, bool isLargerBetter);

private:
    BaseLineManager *m_pManager;
    testing::Test   *m_pTester;
    std::string      m_strCaseName;
    std::string      m_strTestClassName;
    std::string      m_strTestInterfaceName;
    int     m_dCaseVersion;

    bool    m_bHasBaseLine;
    double  m_dbBaseLine;
    bool    m_bHasLastValue;
    double  m_dbLastValue;
    bool    m_bHasFloatRange;
    double  m_dbFloatRange; // 0.0~1.0

    bool    m_bTestResult;
    double  m_dbTestResult;
};
} // namespace TestAW
} // namespace OHOS

#endif // AW_CXX_HWEXT_PERF_H_
