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

#define HST_LOG_TAG "CodecMode"

#include "codec_mode.h"
#include "common/plugin_utils.h"
#include "foundation/log.h"
#include "foundation/cpp_ext/memory_ext.h"
#include "utils/steady_clock.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
bool CodecMode::Init(std::shared_ptr<Plugin::Codec>& plugin, std::vector<POutPort>& outPorts)
{
    if (plugin == nullptr) {
        MEDIA_LOG_I("plugin is null");
        return false;
    }
    plugin_ = plugin;

    if (outPorts.empty()) {
        MEDIA_LOG_I("outPorts is empty");
        return false;
    }
    outPorts_ = outPorts;
    return true;
}

ErrorCode CodecMode::Configure()
{
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Prepare()), "Prepare plugin fail");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Start()), "Start plugin fail");
    return ErrorCode::SUCCESS;
}

ErrorCode CodecMode::Prepare()
{
    return ErrorCode::SUCCESS;
}

ErrorCode CodecMode::Release()
{
    return ErrorCode::SUCCESS;
}

void CodecMode::SetBufferPoolSize(uint32_t inBufPoolSize, uint32_t outBufPoolSize)
{
    inBufPoolSize_ = inBufPoolSize;
    outBufPoolSize_ = outBufPoolSize;
}

uint32_t CodecMode::GetInBufferPoolSize() const
{
    return inBufPoolSize_;
}

uint32_t CodecMode::GetOutBufferPoolSize() const
{
    return outBufPoolSize_;
}

void CodecMode::CreateOutBufferPool(std::shared_ptr<Allocator>& outAllocator,
                                    uint32_t bufferCnt, uint32_t bufferSize, Plugin::BufferMetaType bufferMetaType)
{
    // 每次重新创建bufferPool
    outBufPool_ = std::make_shared<BufferPool<AVBuffer>>(bufferCnt);
    if (outAllocator == nullptr) {
        MEDIA_LOG_I("plugin doest not support out allocator, using framework allocator");
        outBufPool_->Init(bufferSize, bufferMetaType);
    } else {
        MEDIA_LOG_I("using plugin output allocator");
        for (size_t cnt = 0; cnt < bufferCnt; cnt++) {
            auto buf = CppExt::make_unique<AVBuffer>(bufferMetaType);
            if (buf == nullptr || buf->AllocMemory(outAllocator, bufferSize) == nullptr) {
                MEDIA_LOG_I("alloc buffer " PUBLIC_LOG_U32 " fail", static_cast<uint32_t>(cnt));
                continue;
            }
            outBufPool_->Append(std::move(buf));
        }
    }
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS