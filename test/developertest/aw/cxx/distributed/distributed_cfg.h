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

#ifndef AW_CXX_DISTRIBUTED_DISTRIBUTED_CFG_H_
#define AW_CXX_DISTRIBUTED_DISTRIBUTED_CFG_H_

#include <map>
#include <iostream>

namespace OHOS {
namespace DistributeSystemTest {
using namespace std;
enum AGENT_NO {
    ZERO = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9
};

class DistributedCfg {
public:
    DistributedCfg();
    ~DistributedCfg();
    bool OpenCfg(std::string fileName);
    bool GetCfgVal(std::string key, std::string &value);
    std::string GetValueInString(std::string str, size_t devNo);
    std::string GetDeviceIp(std::string fileName, size_t devNo);
    std::string GetDeviceUuid(std::string fileName, size_t devNo);
    static std::unique_ptr<DistributedCfg>& GetInstance();
    static std::unique_ptr<DistributedCfg> getCfg_;

private:
    std::map<std::string, std::string> cfgMap_;  // <cfgkey, cfgvalue>  from env.desc file
};
} // namespace DistributeSystemTest
} // namespace OHOS

#endif // AW_CXX_DISTRIBUTED_DISTRIBUTED_CFG_H_
