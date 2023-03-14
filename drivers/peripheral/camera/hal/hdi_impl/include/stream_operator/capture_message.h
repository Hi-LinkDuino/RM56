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

#ifndef STREAM_OPERATOR_CAPTURE_MESSAGE_H
#define STREAM_OPERATOR_CAPTURE_MESSAGE_H

#include "camera.h"
#include "types.h"
#include <condition_variable>
#include <list>
#include <thread>
#include <unordered_map>
#include <vector>

namespace OHOS::Camera {
enum CaptureMessageType {
    CAPTURE_MESSAGE_TYPE_INVALID = 0,
    CAPTURE_MESSAGE_TYPE_ON_STARTED,
    CAPTURE_MESSAGE_TYPE_ON_ERROR,
    CAPTURE_MESSAGE_TYPE_ON_ENDED,
    CAPTURE_MESSAGE_TYPE_ON_SHUTTER,
    CAPTURE_MESSAGE_TYPE_MAX,
};

class ICaptureMessage {
public:
    ICaptureMessage(int32_t streamId, int32_t captureId, uint64_t time, uint32_t count);
    virtual ~ICaptureMessage() = default;
    virtual uint32_t GetPeerMessageCount() const;
    virtual uint64_t GetTimestamp() const;
    virtual int32_t GetStreamId() const;
    virtual CaptureMessageType GetMessageType() const;
    virtual int32_t GetCaptureId() const;

    ICaptureMessage(const ICaptureMessage& other) = delete;
    ICaptureMessage(ICaptureMessage&& other) = delete;
    ICaptureMessage& operator=(const ICaptureMessage& other) = delete;
    ICaptureMessage& operator=(ICaptureMessage&& other) = delete;

protected:
    uint32_t peerMessageCount_ = 1;
    uint64_t timestamp_ = 0;
    int32_t streamId_ = -1;
    int32_t captureId_ = -1;
};

class CaptureStartedMessage final : public ICaptureMessage {
public:
    virtual ~CaptureStartedMessage() = default;
    CaptureStartedMessage(int32_t streamId, int32_t captureId, uint64_t time, uint32_t count)
        : ICaptureMessage(streamId, captureId, time, count)
    {
    }
    virtual CaptureMessageType GetMessageType() const override
    {
        return CAPTURE_MESSAGE_TYPE_ON_STARTED;
    }
};

class CaptureEndedMessage final : public ICaptureMessage {
public:
    virtual ~CaptureEndedMessage() = default;
    CaptureEndedMessage(int32_t streamId, int32_t captureId, uint64_t time, uint32_t peerCount, uint32_t frameCount)
        : ICaptureMessage(streamId, captureId, time, peerCount)
    {
        frameCount_ = frameCount;
    }
    uint32_t GetFrameCount() const
    {
        return frameCount_;
    }
    virtual CaptureMessageType GetMessageType() const override
    {
        return CAPTURE_MESSAGE_TYPE_ON_ENDED;
    }

private:
    uint32_t frameCount_ = 0;
};

class CaptureErrorMessage final : public ICaptureMessage {
public:
    virtual ~CaptureErrorMessage() = default;
    CaptureErrorMessage(int32_t streamId, int32_t captureId, uint64_t time, uint32_t peerCount, StreamError errorCode)
        : ICaptureMessage(streamId, captureId, time, peerCount)
    {
        errorCode_ = errorCode;
    }

    StreamError GetStreamError() const
    {
        return errorCode_;
    }

    virtual CaptureMessageType GetMessageType() const override
    {
        return CAPTURE_MESSAGE_TYPE_ON_ERROR;
    }

private:
    StreamError errorCode_;
};

class FrameShutterMessage final : public ICaptureMessage {
public:
    virtual ~FrameShutterMessage() = default;
    FrameShutterMessage(int32_t streamId, int32_t captureId, uint64_t time, uint32_t count)
        : ICaptureMessage(streamId, captureId, time, count)
    {
    }
    virtual CaptureMessageType GetMessageType() const override
    {
        return CAPTURE_MESSAGE_TYPE_ON_SHUTTER;
    }
};

using MessageGroup = std::vector<std::shared_ptr<ICaptureMessage>>;
using MessageOperatorFunc = std::function<void(MessageGroup&)>;

class CaptureMessageOperator {
public:
    CaptureMessageOperator() = default;
    CaptureMessageOperator(MessageOperatorFunc f);
    virtual ~CaptureMessageOperator();
    CaptureMessageOperator(const CaptureMessageOperator& other) = delete;
    CaptureMessageOperator(CaptureMessageOperator&& other) = delete;
    CaptureMessageOperator& operator=(const CaptureMessageOperator& other) = delete;
    CaptureMessageOperator& operator=(CaptureMessageOperator&& other) = delete;

    void SendMessage(std::shared_ptr<ICaptureMessage>& message);
    RetCode StartProcess();

private:
    void HandleMessage();

private:
    MessageOperatorFunc messageOperator_ = nullptr;
    std::atomic<bool> running_ = false;
    std::atomic<bool> wakeup_ = false;
    std::unique_ptr<std::thread> messageHandler_ = nullptr;
    std::mutex lock_ = {};
    std::condition_variable cv_ = {};
    std::unordered_map<uint32_t, std::list<MessageGroup>> messageBox_ = {};
};
} // namespace OHOS::Camera
#endif
