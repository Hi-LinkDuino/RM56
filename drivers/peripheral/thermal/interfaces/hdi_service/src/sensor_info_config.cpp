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
#include "sensor_info_config.h"

#define HDF_LOG_TAG SensorInfoConfig

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
void SensorInfoConfig::SetXMLThermalZoneInfo(std::vector<XMLThermalZoneInfo> &vXmlTz)
{
    xmlTzInfoList_ = vXmlTz;
}

void SensorInfoConfig::SetXMLThermalNodeInfo(std::vector<XMLThermalNodeInfo> &vXmlTn)
{
    xmlTnInfoList_ = vXmlTn;
}

std::vector<XMLThermalZoneInfo> SensorInfoConfig::GetXMLThermalZoneInfo()
{
    return xmlTzInfoList_;
}

std::vector<XMLThermalNodeInfo> SensorInfoConfig::GetXMLThermalNodeInfo()
{
    return xmlTnInfoList_;
}

void SensorInfoConfig::SetGroupName(const std::string &groupName)
{
    groupName_ = groupName;
}

void SensorInfoConfig::SetGroupInterval(int32_t interval)
{
    interval_ = interval;
}

int32_t SensorInfoConfig::GetInterval()
{
    return interval_;
}

std::string SensorInfoConfig::GetGroupName()
{
    return groupName_;
}
} // V1_0
} // Thermal
} // HDI
} // OHOS