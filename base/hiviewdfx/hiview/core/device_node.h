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

#ifndef CORE_DEVICE_NODE_H
#define CORE_DEVICE_NODE_H
#include <memory>
#include <string>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
class EventReceiver {
public:
    EventReceiver() {};
    virtual ~EventReceiver() {};
    virtual void HandlerEvent(const std::string& rawMsg) = 0;
};

class DeviceNode {
public:
    DeviceNode() {};
    virtual ~DeviceNode() {};
    virtual int Close() = 0;
    virtual int Open() = 0;
    virtual uint32_t GetEvents() = 0;
    virtual std::string GetName() = 0;
    virtual int ReceiveMsg(std::vector<std::shared_ptr<EventReceiver>> &receivers) = 0;
}; // DeviceNode
} // namespace HiviewDFX
} // namespace OHOS
#endif // CORE_DEVICE_NODE_H