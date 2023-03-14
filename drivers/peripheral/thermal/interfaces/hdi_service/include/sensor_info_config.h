/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SENSOR_INFO_CONFIG_H
#define SENSOR_INFO_CONFIG_H

#include <string>
#include <list>
#include <vector>

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
struct XMLThermalZoneInfo {
    std::string type;
    std::string replace;
    bool isReplace = false;
    bool operator == (const std::string &sensorType)
    {
        return (this->type == sensorType);
    }
};

struct XMLThermalNodeInfo {
    std::string type;
    std::string path;
    bool operator == (const std::string &sensorType)
    {
        return (this->type == sensorType);
    }
};

struct ReportedThermalData {
    std::string type;
    std::string tempPath;
};

class SensorInfoConfig {
public:
    SensorInfoConfig() = default;
    ~SensorInfoConfig() = default;
    void SetXMLThermalZoneInfo(std::vector<XMLThermalZoneInfo> &vXmlTz);
    void SetXMLThermalNodeInfo(std::vector<XMLThermalNodeInfo> &vXmlTn);
    void SetGroupName(const std::string &groupName);
    void SetGroupInterval(int32_t interval);
    std::vector<XMLThermalZoneInfo> GetXMLThermalZoneInfo();
    std::vector<XMLThermalNodeInfo> GetXMLThermalNodeInfo();
    int32_t GetInterval();
    std::string GetGroupName();
    int32_t multiple_;
    std::list<ReportedThermalData> thermalDataList_;
private:
    std::string groupName_;
    std::int32_t interval_;
    std::vector<XMLThermalZoneInfo> xmlTzInfoList_;
    std::vector<XMLThermalNodeInfo> xmlTnInfoList_;
};
} // V1_0
} // Thermal
} // HDI
} // OHOS
#endif