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
#ifndef TEST_CONFIG_PARSER_H
#define TEST_CONFIG_PARSER_H

#include <iostream>
#include <fstream>
#include <string>

#include "nlohmann/json.hpp"

namespace OHOS {
const std::string STRESS_TEST_CONFIG_FILE_PATH {"/data/testconfig/stressconfig.json"};

const std::string STRESS_TEST_BMS_KEY {"BMS"};

struct StressTestLevel {
    int32_t BMSLevel {1};
};

class TestConfigParser {
public:
    void ParseFromFile4StressTest(const std::string &path, StressTestLevel &stlevel)
    {
        std::ifstream jf(path);
        if (!jf.is_open()) {
            std::cout << "json file can not open!" << std::endl;
            return;
        }

        nlohmann::json jsonObj;
        jf >> jsonObj;

        const auto &jsonObjEnd = jsonObj.end();
        if (jsonObj.find(STRESS_TEST_BMS_KEY) != jsonObjEnd) {
            jsonObj.at(STRESS_TEST_BMS_KEY).get_to(stlevel.BMSLevel);
            if (stlevel.BMSLevel == 0) {
                stlevel.BMSLevel = 1;
            }
        }
    }
};
}  // namespace OHOS

#endif  // TEST_CONFIG_PARSER_H
