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

#include "buffer_tracking.h"
#include "buffer_loop_tracking.h"

namespace OHOS::Camera {
void BufferTracking::AddTrackingStreamBegin(const int32_t trackingId, const int64_t poolId)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.AddTrackingStreamBegin(trackingId, poolId);

    return;
}

void BufferTracking::AddTrackingStreamEnd(const int32_t trackingId)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.AddTrackingStreamEnd(trackingId);

    return;
}

void BufferTracking::DeleteTrackingStream(const int32_t trackingId)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.DeleteTrackingStream(trackingId);
    return;
}

void BufferTracking::AddTrackingNode(const int32_t trackingId, const std::string node)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.AddTrackingNode(trackingId, node);
    return;
}

void BufferTracking::ReportBufferLocation(const std::shared_ptr<BufferTrackingMessage>& message)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.SendBufferMovementMessage(message);
    return;
}

void BufferTracking::StartTracking()
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.StartTracking();
    return;
}

void BufferTracking::StopTracking()
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.StopTracking();
    return;
}

int32_t BufferTracking::IsNodeEmpty(const int32_t id, const std::string node)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    return stalker.IsEmpty(id, node);
}

int32_t BufferTracking::IsNodeEmpty(const int32_t id, const std::string beginNode, const std::string endNode)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    return stalker.IsEmpty(id, beginNode, endNode);
}

void BufferTracking::DumpBufferTrace(const int32_t id, BufferTraceGraph& graph)
{
    BufferLoopTracking& stalker = BufferLoopTracking::GetInstance();
    stalker.DumpTrace(id, graph);
    return;
}
} // namespace OHOS::Camera
