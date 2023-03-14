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

#ifndef HOS_CAMERA_CREATE_SENSOR_FACTORY_H
#define HOS_CAMERA_CREATE_SENSOR_FACTORY_H

#include "isensor.h"
#include "device_manager_adapter.h"
#include <map>

namespace OHOS {
namespace Camera {
typedef std::shared_ptr<ISensor> (*createClass) (void);
class CreateSensorFactory {
public:
    virtual ~CreateSensorFactory(){};
    std::shared_ptr<ISensor> GetSensorByName(std::string className)
    {
        std::map<std::string, createClass>::const_iterator iter;

        iter = senorMap_.find(className);
        if (iter == senorMap_.end()) {
            return NULL;
        } else {
            return iter->second();
        }
    };
    void RegistClass(std::string name, createClass method)
    {
        senorMap_.insert(std::pair<std::string, createClass>(name, method));
    };
    static CreateSensorFactory& SharedSensorFactory()
    {
        static CreateSensorFactory sensorFactory_;
        return sensorFactory_;
    };

private:
    CreateSensorFactory(){};
    std::map<std::string, createClass> senorMap_;
};

class SensorDynamicClass {
public:
    SensorDynamicClass(std::string name, createClass method)
    {
        CreateSensorFactory::SharedSensorFactory().RegistClass(name, method);
    };
    ~SensorDynamicClass() {}
};

#define DECLARE_SENSOR(className)                                                \
private:                                                                         \
    static SensorDynamicClass* className##_;                                     \
                                                                                 \
public:                                                                          \
    static std::shared_ptr<ISensor> createInstance()                             \
    {                                                                            \
        return std::static_pointer_cast<ISensor>(std::make_shared<className>()); \
    }

#define IMPLEMENT_SENSOR(className) \
    SensorDynamicClass* className::className##_ = new SensorDynamicClass(#className, &className::createInstance);

#define GetSensorFactory CreateSensorFactory::SharedSensorFactory()
} // namespace Camera
} // namespace OHOS
#endif
