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

#ifndef OHOS_HDI_SENSOR_V1_0_SENSORCALLBACKIMPL_H
#define OHOS_HDI_SENSOR_V1_0_SENSORCALLBACKIMPL_H

#include <hdf_base.h>
#include "v1_0/sensor_callback_stub.h"

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V1_0 {
class SensorCallbackImpl : public SensorCallbackStub {
public:
    virtual ~SensorCallbackImpl() {}

    int32_t OnDataEvent(const HdfSensorEvents& event) override;
    static uint32_t sensorDataFlag;
};
} // V1_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V1_0_SENSORCALLBACKIMPL_H
