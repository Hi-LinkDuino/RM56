/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "FilterPort"

#include "port.h"
#include <algorithm>
#include "filter.h"
#include "foundation/log.h"
#include "foundation/pre_defines.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
std::shared_ptr<InPort> EmptyInPort::port = std::make_shared<EmptyInPort>();
std::shared_ptr<OutPort> EmptyOutPort::port = std::make_shared<EmptyOutPort>();

const std::string& Port::GetName()
{
    return name;
}

const InfoTransfer* Port::GetOwnerFilter() const
{
    return filter;
}

std::shared_ptr<Port> Port::GetPeerPort()
{
    return nullptr;
}

ErrorCode InPort::Connect(const std::shared_ptr<Port>& port)
{
    prevPort = port;
    return ErrorCode::SUCCESS;
}

ErrorCode InPort::Disconnect()
{
    prevPort.reset();
    return ErrorCode::SUCCESS;
}

ErrorCode InPort::Activate(const std::vector<WorkMode>& modes, WorkMode& outMode)
{
    if (auto ptr = prevPort.lock()) {
        FAIL_RETURN(ptr->Activate(modes, workMode));
        outMode = workMode;
        return ErrorCode::SUCCESS;
    }
    MEDIA_LOG_E("[Filter " PUBLIC_LOG_S "] InPort " PUBLIC_LOG_S " Activate error: prevPort destructed",
                filter->GetName().c_str(), name.c_str());
    return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
}

std::shared_ptr<Port> InPort::GetPeerPort()
{
    return prevPort.lock();
}

bool InPort::Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                       Plugin::Capability& negotiatedCap,
                       const Plugin::TagMap& upstreamParams,
                       Plugin::TagMap& downstreamParams)
{
    return filter && filter->Negotiate(name, upstreamCap, negotiatedCap, upstreamParams, downstreamParams);
}

bool InPort::Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    return filter && filter->Configure(name, upstreamMeta);
}


void InPort::PushData(const AVBufferPtr& buffer, int64_t offset)
{
    if (filter) {
        filter->PushData(name, buffer, offset);
    } else {
        MEDIA_LOG_E("filter destructed");
    }
}

ErrorCode InPort::PullData(uint64_t offset, size_t size, AVBufferPtr& data)
{
    if (auto ptr = prevPort.lock()) {
        return ptr->PullData(offset, size, data);
    }
    MEDIA_LOG_E("prevPort destructed");
    return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
}

ErrorCode OutPort::Connect(const std::shared_ptr<Port>& port)
{
    if (InSamePipeline(port)) {
        nextPort = port;
        return ErrorCode::SUCCESS;
    }
    MEDIA_LOG_E("Connect filters that are not in the same pipeline.");
    return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
}

ErrorCode OutPort::Disconnect()
{
    nextPort.reset();
    return ErrorCode::SUCCESS;
}

bool OutPort::InSamePipeline(const std::shared_ptr<Port>& port) const
{
    auto filter1 = GetOwnerFilter();
    FALSE_RETURN_V(filter1 != nullptr, false);
    auto filter2 = port->GetOwnerFilter();
    FALSE_RETURN_V(filter2 != nullptr, false);
    auto pipeline1 = filter1->GetOwnerPipeline();
    FALSE_RETURN_V(pipeline1 != nullptr, false);
    auto pipeline2 = filter2->GetOwnerPipeline();
    FALSE_RETURN_V(pipeline2 != nullptr, false);
    return pipeline1 == pipeline2;
}

ErrorCode OutPort::Activate(const std::vector<WorkMode>& modes, WorkMode& outMode)
{
    if (filter) {
        auto supportedModes = filter->GetWorkModes();
        for (auto mode : modes) {
            auto found = std::find(supportedModes.cbegin(), supportedModes.cend(), mode);
            if (found != supportedModes.cend()) {
                outMode = mode;
                workMode = mode;
                return ErrorCode::SUCCESS; // 最先找到的兼容的mode，作为最后结果
            }
        }
    } else {
        MEDIA_LOG_E("no valid filter");
    }
    MEDIA_LOG_E("negotiate failed");
    return ErrorCode::ERROR_UNKNOWN;
}

std::shared_ptr<Port> OutPort::GetPeerPort()
{
    return nextPort;
}

bool OutPort::Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                        Plugin::Capability& negotiatedCap,
                        const Plugin::TagMap& upstreamParams,
                        Plugin::TagMap& downstreamParams)
{
    return nextPort->Negotiate(upstreamCap, negotiatedCap, upstreamParams, downstreamParams);
}

bool OutPort::Configure(const std::shared_ptr<const Plugin::Meta> &upstreamMeta)
{
    return nextPort->Configure(upstreamMeta);
}

void OutPort::PushData(const AVBufferPtr& buffer, int64_t offset)
{
    nextPort->PushData(buffer, offset);
}

ErrorCode OutPort::PullData(uint64_t offset, size_t size, AVBufferPtr& data)
{
    if (filter) {
        return filter->PullData(name, offset, size, data);
    }
    MEDIA_LOG_E("filter destructed");
    return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
}

ErrorCode EmptyInPort::Connect(const std::shared_ptr<Port>& another)
{
    UNUSED_VARIABLE(another);
    MEDIA_LOG_E("Connect in EmptyInPort");
    return ErrorCode::SUCCESS;
}

ErrorCode EmptyInPort::Activate(const std::vector<WorkMode>& modes, WorkMode& outMode)
{
    UNUSED_VARIABLE(modes);
    UNUSED_VARIABLE(outMode);
    MEDIA_LOG_E("Activate in EmptyInPort");
    return ErrorCode::SUCCESS;
}

bool EmptyInPort::Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                            Plugin::Capability& negotiatedCap,
                            const Plugin::TagMap& upstreamParams,
                            Plugin::TagMap& downstreamParams)
{
    UNUSED_VARIABLE(upstreamCap);
    UNUSED_VARIABLE(negotiatedCap);
    UNUSED_VARIABLE(upstreamParams);
    UNUSED_VARIABLE(downstreamParams);
    MEDIA_LOG_E("Negotiate in EmptyInPort");
    return false;
}

bool EmptyInPort::Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    UNUSED_VARIABLE(upstreamMeta);
    MEDIA_LOG_E("Configure in EmptyInPort");
    return false;
}

void EmptyInPort::PushData(const AVBufferPtr& buffer, int64_t offset)
{
    UNUSED_VARIABLE(buffer);
    UNUSED_VARIABLE(offset);
    MEDIA_LOG_E("PushData in EmptyInPort");
}
ErrorCode EmptyInPort::PullData(uint64_t offset, size_t size, AVBufferPtr& data)
{
    UNUSED_VARIABLE(offset);
    UNUSED_VARIABLE(size);
    UNUSED_VARIABLE(data);
    MEDIA_LOG_E("PullData in EmptyInPort");
    return ErrorCode::ERROR_UNIMPLEMENTED;
}

ErrorCode EmptyOutPort::Connect(const std::shared_ptr<Port>& another)
{
    UNUSED_VARIABLE(another);
    MEDIA_LOG_E("Connect in EmptyOutPort");
    return ErrorCode::SUCCESS;
}
ErrorCode EmptyOutPort::Activate(const std::vector<WorkMode>& modes, WorkMode& outMode)
{
    UNUSED_VARIABLE(modes);
    UNUSED_VARIABLE(outMode);
    MEDIA_LOG_E("Activate in EmptyOutPort");
    return ErrorCode::SUCCESS;
}
bool EmptyOutPort::Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                             Plugin::Capability& negotiatedCap,
                             const Plugin::TagMap& upstreamParams,
                             Plugin::TagMap& downstreamParams)
{
    UNUSED_VARIABLE(upstreamCap);
    UNUSED_VARIABLE(negotiatedCap);
    UNUSED_VARIABLE(upstreamParams);
    UNUSED_VARIABLE(downstreamParams);
    MEDIA_LOG_E("Negotiate in EmptyOutPort");
    return false;
}

bool EmptyOutPort::Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    UNUSED_VARIABLE(upstreamMeta);
    MEDIA_LOG_E("Configure in EmptyOutPort");
    return false;
}

void EmptyOutPort::PushData(const AVBufferPtr& buffer, int64_t offset)
{
    UNUSED_VARIABLE(buffer);
    UNUSED_VARIABLE(offset);
    MEDIA_LOG_E("PushData in EmptyOutPort");
}
ErrorCode EmptyOutPort::PullData(uint64_t offset, size_t size, AVBufferPtr& data)
{
    UNUSED_VARIABLE(offset);
    UNUSED_VARIABLE(size);
    UNUSED_VARIABLE(data);
    MEDIA_LOG_E("PullData in EmptyOutPort");
    return ErrorCode::ERROR_UNIMPLEMENTED;
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
