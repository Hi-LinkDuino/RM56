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

#ifndef HOST_STREAM_MANAGER_H
#define HOST_STREAM_MANAGER_H
#include "host_stream_data_structure.h"
#include "no_copyable.h"
#include <memory>
#include <vector>

namespace OHOS::Camera {
class HostStreamMgr : public NoCopyable {
public:
    static std::shared_ptr<HostStreamMgr> Create();
    virtual RetCode CreateHostStream(const HostStreamInfo& info, BufferCb c) = 0;
    virtual RetCode DestroyHostStream(const std::vector<int>& types) = 0;
    virtual void GetStreamTypes(std::vector<int32_t>& s) const = 0;
    virtual HostStreamInfo GetStreamInfo(const int32_t& id) const = 0;
    virtual BufferCb GetBufferCb(const int32_t& type) const = 0;
    virtual void GetStreamIds(std::vector<int32_t>& s) const = 0;
    virtual int32_t DesignateStreamIdForType(const int32_t streamType) = 0;
    virtual ~HostStreamMgr() = default;
};
}
#endif
