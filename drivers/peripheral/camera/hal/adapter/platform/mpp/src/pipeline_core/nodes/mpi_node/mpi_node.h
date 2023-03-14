/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MPI_NODE_H
#define MPI_NODE_H

#include <vector>
#include "node_base.h"
#include "device_manager_adapter.h"
#include "mpi_device_manager.h"

namespace OHOS::Camera {
class MpiNode : virtual public NodeBase {
public:
    MpiNode(const std::string& name, const std::string& type);
    ~MpiNode() override;
    RetCode ConnectMpi(const int32_t streamId);
    RetCode DisConnectMpi(const int32_t streamId);
    RetCode GetMpiDeviceManager();
    void SendCallBack() {}

protected:
    std::shared_ptr<IDeviceManager>     deviceManager_ = nullptr;
};
} // namespace OHOS::Camera
#endif
