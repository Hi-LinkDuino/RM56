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

#ifndef STREAM_OPERATOR_ISTREAM_H
#define STREAM_OPERATOR_ISTREAM_H

#include "camera.h"
#include "capture_message.h"
#include "ibuffer.h"
#include "ipipeline_core.h"
#include "object_factory.h"
#include "stream_tunnel.h"
#include <memory>
#include <string>
#include <map>

#define STREAM_INTENT_TO_STRING(m) #m

namespace OHOS::Camera {
class CaptureRequest;
class OfflineStream;

static std::map<StreamIntent, std::string> g_availableStreamType;

class IStream {
public:
    virtual ~IStream() = default;
    virtual RetCode ConfigStream(StreamConfiguration& config) = 0;
    virtual RetCode CommitStream() = 0;
    virtual RetCode StartStream() = 0;
    virtual RetCode StopStream() = 0;
    virtual RetCode AddRequest(std::shared_ptr<CaptureRequest>& request) = 0;
    virtual RetCode CancelRequest(const std::shared_ptr<CaptureRequest>& request) = 0;
    virtual RetCode AttachStreamTunnel(std::shared_ptr<StreamTunnel>& tunnel) = 0;
    virtual RetCode DetachStreamTunnel() = 0;
    virtual RetCode ChangeToOfflineStream(std::shared_ptr<OfflineStream> offlineStream) = 0;
    virtual bool GetTunnelMode() const = 0;
    virtual StreamConfiguration GetStreamAttribute() const = 0;
    virtual int32_t GetStreamId() const = 0;
    virtual RetCode Capture(const std::shared_ptr<CaptureRequest>& request) = 0;
    virtual RetCode OnFrame(const std::shared_ptr<CaptureRequest>& request) = 0;
    virtual bool IsRunning() const = 0;

public:
    static std::map<StreamIntent, std::string> g_availableStreamType;
};

using StreamFactory = RegisterFactoty<IStream,
                                      const int32_t,
                                      const StreamIntent,
                                      std::shared_ptr<IPipelineCore>&,
                                      std::shared_ptr<CaptureMessageOperator>&>;
#define REGISTERSTREAM(cls, ...)                                                                           \
    namespace {                                                                                            \
    static std::string g_##cls = StreamFactory::Instance().DoRegister<cls>(                                \
        __VA_ARGS__,                                                                                       \
        [](const int32_t id,                                                                               \
           const StreamIntent type,                                                                        \
           std::shared_ptr<IPipelineCore>& p,                                                              \
           std::shared_ptr<CaptureMessageOperator>& m) { return std::make_shared<cls>(id, type, p, m); }); \
    }
} // namespace OHOS::Camera
#endif // STREAM_OPERATOR_ISTREAM_H
