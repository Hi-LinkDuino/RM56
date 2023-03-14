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

#ifndef THERMAL_SIMULATION_NODE_H
#define THERMAL_SIMULATION_NODE_H

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include "thermal_types.h"

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
class ThermalSimulationNode {
public:
    ThermalSimulationNode() = default;
    ~ThermalSimulationNode() = default;

    int32_t NodeInit();
private:
    int32_t CreateNodeDir(std::string dir);
    int32_t CreateNodeFile(std::string filePath);
    int32_t AddSensorTypeTemp();
    int32_t AddMitigationDevice();
    int32_t WriteFile(std::string path, std::string buf, size_t size);
    std::mutex mutex_;
};
} // V1_0
} // Thermal
} // HDI
} // OHOS
#endif // THERMAL_SIMULATION_NODE_H