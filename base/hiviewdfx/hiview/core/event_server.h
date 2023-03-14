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

#ifndef COER_EVENT_SERVER_H
#define COER_EVENT_SERVER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "device_node.h"

struct epoll_event;
namespace OHOS {
namespace HiviewDFX {
class SocketDevice : public DeviceNode {
public:
    SocketDevice() {};
    ~SocketDevice() {};
    int Close() override;
    int Open() override;
    uint32_t GetEvents() override;
    std::string GetName() override;
    int ReceiveMsg(std::vector<std::shared_ptr<EventReceiver>> &receivers) override;
private:
    void InitSocket(int &socketId);
    int socketId_ = -1;
};

class BBoxDevice : public DeviceNode {
public:
    BBoxDevice() {};
    ~BBoxDevice() {};
    int Close() override;
    int Open() override;
    uint32_t GetEvents() override;
    std::string GetName() override;
    int ReceiveMsg(std::vector<std::shared_ptr<EventReceiver>> &receivers) override;
private:
    int fd_ = -1;
};

class EventServer {
public:
    EventServer(): isStart_(false) {};
    ~EventServer() {}
    void Start();
    void Stop();
    void AddReceiver(std::shared_ptr<EventReceiver> receiver);
private:
    void AddDev(std::shared_ptr<DeviceNode> dev);
    int OpenDevs();
    void CloseDevs();
    int AddToMonitor(int pollFd, struct epoll_event pollEvents[]);
    std::map<int, std::shared_ptr<DeviceNode>> devs_;
    std::vector<std::shared_ptr<EventReceiver>> receivers_;
    bool isStart_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // COER_EVENT_SERVER_H