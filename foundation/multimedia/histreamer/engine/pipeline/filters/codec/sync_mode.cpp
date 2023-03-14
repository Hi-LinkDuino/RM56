/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "SyncMode"

#include "sync_mode.h"
#include "common/plugin_utils.h"
#include "filters/common/dump_buffer.h"
#include "foundation/log.h"
#include "utils/steady_clock.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
SyncMode::SyncMode(std::string name) : CodecMode(std::move(name))
{
    MEDIA_LOG_I(PUBLIC_LOG_S " ThreadMode: SYNC", codecName_.c_str());
}

SyncMode::~SyncMode()
{
    MEDIA_LOG_D("sync mode dtor called");
}

ErrorCode SyncMode::Configure()
{
    return CodecMode::Configure();
}

ErrorCode SyncMode::PushData(const std::string &inPort, const AVBufferPtr& buffer, int64_t offset)
{
    const static int8_t maxRetryCnt = 3; // max retry times of handling one frame
    ErrorCode handleFrameRes;
    int8_t retryCnt = 0;
    do {
        handleFrameRes = HandleFrame(buffer);
        while (FinishFrame() == ErrorCode::SUCCESS) {
            MEDIA_LOG_DD("finish frame");
        }
        retryCnt++;
        if (retryCnt >= maxRetryCnt) { // if retry cnt exceeds we will drop this frame
            break;
        }
        // if timed out or returns again we should try again
    } while (handleFrameRes == ErrorCode::ERROR_TIMED_OUT || handleFrameRes == ErrorCode::ERROR_AGAIN);
    return ErrorCode::SUCCESS;
}

ErrorCode SyncMode::Stop()
{
    MEDIA_LOG_D("SyncMode stop");
    return ErrorCode::SUCCESS;
}

void SyncMode::FlushStart()
{
    MEDIA_LOG_D("SyncMode FlushStart.");
}

void SyncMode::FlushEnd()
{
    MEDIA_LOG_D("SyncMode FlushEnd.");
}

ErrorCode SyncMode::HandleFrame(const std::shared_ptr<AVBuffer>& buffer)
{
    MEDIA_LOG_DD("SyncMode HandleFrame called");
    auto ret = TranslatePluginStatus(plugin_->QueueInputBuffer(buffer, 0));
    if (ret != ErrorCode::SUCCESS && ret != ErrorCode::ERROR_TIMED_OUT) {
        MEDIA_LOG_E("Queue input buffer to plugin fail: " PUBLIC_LOG_D32, CppExt::to_underlying(ret));
    }
    return ret;
}

ErrorCode SyncMode::FinishFrame()
{
    MEDIA_LOG_DD("SyncMode begin finish frame");
    auto outBuffer = outBufPool_->AllocateAppendBufferNonBlocking();
    if (outBuffer == nullptr) {
        MEDIA_LOG_E("Get out buffer from buffer pool fail");
        return ErrorCode::ERROR_NO_MEMORY;
    }
    outBuffer->Reset();
    auto status = plugin_->QueueOutputBuffer(outBuffer, 0);
    if (status != Plugin::Status::OK && status != Plugin::Status::END_OF_STREAM) {
        if (status != Plugin::Status::ERROR_NOT_ENOUGH_DATA) {
            MEDIA_LOG_E("Queue output buffer to plugin fail: " PUBLIC_LOG_D32, static_cast<int32_t>((status)));
        }
    }
    MEDIA_LOG_DD("SyncMode end finish frame");
    return TranslatePluginStatus(status);
}

void SyncMode::OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& buffer)
{
    MEDIA_LOG_DD("begin");
    FALSE_RETURN(buffer != nullptr);

    // push to port
    auto oPort = outPorts_[0];
    if (oPort->GetWorkMode() == WorkMode::PUSH) {
        DUMP_BUFFER2FILE("decoder_output.data", buffer);
        oPort->PushData(buffer, -1);
    } else {
        MEDIA_LOG_W("decoder out port works in pull mode");
    }
    // 释放buffer 如果没有被缓存使其回到buffer pool 如果被sink缓存 则从buffer pool拿其他的buffer
    std::const_pointer_cast<Plugin::Buffer>(buffer).reset();
    MEDIA_LOG_DD("end");
}

ErrorCode SyncMode::Prepare()
{
    return CodecMode::Prepare();
}

ErrorCode SyncMode::Release()
{
    return CodecMode::Release();
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS