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

#ifndef HOS_BUFFER_TRACKING_H
#define HOS_BUFFER_TRACKING_H

#include "buffer_trace.h"

#define PIPELINE_REPORT_BUFFER_LOCATION(I, F, N) TRACKING_REPORT_BUFFER_LOCATION(I, F, N, false);

#define POOL_REPORT_BUFFER_LOCATION(I, F) TRACKING_REPORT_BUFFER_LOCATION(I, F, "", true);

#define TRACKING_REPORT_BUFFER_LOCATION(I, F, N, R)         \
    do {                                                    \
        auto m = std::make_shared<BufferTrackingMessage>(); \
        m->trackingId = I;                                  \
        m->frameNumber = F;                                 \
        m->nodeName = N;                                    \
        m->isReturnBack = R;                                \
        BufferTracking::ReportBufferLocation(m);            \
    } while (0);

namespace OHOS::Camera {
class BufferTracking {
public:
    // begin to track a stream, call AddTrackingNode to add node after this function.
    // trackingId can be steam id, poolId is id of buffer pool which in SinkNode.
    static void AddTrackingStreamBegin(const int32_t trackingId, const int64_t poolId);

    // add node complete, call this before StartTracking, after AddTrackingNode.
    static void AddTrackingStreamEnd(const int32_t trackingId);

    // disable tracking a stream
    static void DeleteTrackingStream(const int32_t trackingId);

    // append node to tracking list, call this before AddTrackingStreamEnd, after AddTrackingStreamBegin.
    static void AddTrackingNode(const int32_t trackingId, const std::string node);

    /* report location of a buffer, after this buffer has been delivered to a node or pool.
     *            report          report    report    report    report      report
     *               |               |         |         |        |            |
     * BUFFERPOOL---->SOURCENODE----->NODE----->NODE----->NODE---->SINKNODE---->BUFFERPOOL
     */
    static void ReportBufferLocation(const std::shared_ptr<BufferTrackingMessage>& message);

    // start a thread to tracking buffers of a stream.
    static void StartTracking();

    // stop stacking
    static void StopTracking();

    // check if there are buffers in a specific node.
    static int32_t IsNodeEmpty(const int32_t id, const std::string node);

    // check if there are buffers from beginNode to endNode.
    static int32_t IsNodeEmpty(const int32_t id, const std::string beginNode, const std::string endNode);

    static void DumpBufferTrace(const int32_t id, BufferTraceGraph& graph);
};
} // namespace OHOS::Camera
#endif
