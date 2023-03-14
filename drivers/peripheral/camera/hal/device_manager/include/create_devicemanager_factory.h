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

#ifndef HOS_CAMERA_CREATE_DEVICEMANAGER_FACTORY_H
#define HOS_CAMERA_CREATE_DEVICEMANAGER_FACTORY_H

#include "idevice_manager.h"
#include "device_manager_adapter.h"
#include <map>

namespace OHOS {
namespace Camera {
typedef std::shared_ptr<IDeviceManager> (*createClass)(void);
class CreateDeviceManagerFactory {
public:
    virtual ~CreateDeviceManagerFactory(){};
    std::shared_ptr<IDeviceManager> GetDeviceManagerByName(std::string className)
    {
        std::map<std::string, createClass>::const_iterator iter;

        iter = deviceManagerMap_.find(className);
        if (iter == deviceManagerMap_.end()) {
            return NULL;
        } else {
            return iter->second();
        }
    };
    void RegistClass(std::string name, createClass method)
    {
        deviceManagerMap_.insert(std::pair<std::string, createClass>(name, method));
    };
    static CreateDeviceManagerFactory& SharedDeviceManagerFactory()
    {
        static CreateDeviceManagerFactory deviceManagerFactory_;
        return deviceManagerFactory_;
    };

private:
    CreateDeviceManagerFactory(){};
    std::map<std::string, createClass> deviceManagerMap_;
};

class DeviceManagerDynamicClass {
public:
    DeviceManagerDynamicClass(std::string name, createClass method)
    {
        CreateDeviceManagerFactory::SharedDeviceManagerFactory().RegistClass(name, method);
    };
    ~DeviceManagerDynamicClass() {}
};

#define DECLARE_DEVICEMANAGER(className)                                                \
private:                                                                                \
    static DeviceManagerDynamicClass* className##_;                                     \
                                                                                        \
public:                                                                                 \
    static std::shared_ptr<IDeviceManager> createInstance()                             \
    {                                                                                   \
        return std::static_pointer_cast<IDeviceManager>(std::make_shared<className>()); \
    }

#define IMPLEMENT_DEVICEMANAGER(className) \
    DeviceManagerDynamicClass* className::className##_ = \
        new DeviceManagerDynamicClass(#className, &className::createInstance);

#define GetDeviceManagerFactory CreateDeviceManagerFactory::SharedDeviceManagerFactory()
} // namespace Camera
} // namespace OHOS
#endif
