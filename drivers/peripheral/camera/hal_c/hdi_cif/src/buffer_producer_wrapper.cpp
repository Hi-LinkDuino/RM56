/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "buffer_producer_wrapper.h"
#include "camera_types_c_if.h"
#include "surface.h"
#include <map>

static std::map<int, OHOS::sptr<OHOS::Surface>> g_producerMap;
static std::map<GraphicBufferCIF*, OHOS::sptr<OHOS::SurfaceBuffer>> g_surfaceBufferMap;

void BindBufferProducer(int id, OHOS::sptr<OHOS::IBufferProducer>& producer)
{
    g_producerMap[id] = OHOS::Surface::CreateSurfaceAsProducer(producer);
    return;
}

int BufferProducerRequestBuffer(GraphicBufferCIF* buffer)
{
    if (buffer == nullptr) {
        return -1;
    }

    OHOS::sptr<OHOS::SurfaceBuffer> sb = nullptr;
    int fence = 0;
    OHOS::BufferRequestConfig requestConfig = {};
    requestConfig.width = buffer->width;
    requestConfig.height = buffer->height;
    requestConfig.strideAlignment = 8;
    requestConfig.format = buffer->format;
    requestConfig.usage = buffer->usage;
    requestConfig.timeout = 0;

    OHOS::sptr<OHOS::Surface> producer = g_producerMap[buffer->streamId];
    if (producer == nullptr) {
        return -1;
    }
    OHOS::SurfaceError ret = producer->RequestBuffer(sb, fence, requestConfig);
    BufferHandle* handle = sb->GetBufferHandle();
    if (handle == nullptr) {
        return -1;
    }

    buffer->fd = handle->fd;
    buffer->width = handle->width;
    buffer->stride = handle->stride;
    buffer->height = handle->height;
    buffer->size = handle->size;
    buffer->format = handle->format;
    buffer->usage = handle->usage;
    buffer->virAddr = handle->virAddr;
    buffer->phyAddr = handle->phyAddr;
    buffer->fence = fence;

    g_surfaceBufferMap[buffer] = sb;
    return static_cast<int>(ret);
}

int BufferProducerCancelBuffer(GraphicBufferCIF* buffer)
{
    if (buffer == nullptr) {
        return -1;
    }
    OHOS::sptr<OHOS::Surface> producer = g_producerMap[buffer->streamId];
    if (producer == nullptr) {
        return -1;
    }
    OHOS::sptr<OHOS::SurfaceBuffer> sb = g_surfaceBufferMap[buffer];
    if (sb == nullptr) {
        return -1;
    }

    OHOS::SurfaceError ret = producer->CancelBuffer(sb);
    g_surfaceBufferMap.erase(buffer);
    return static_cast<int>(ret);
}

int BufferProducerFlushBuffer(GraphicBufferCIF* buffer)
{
    if (buffer == nullptr) {
        return -1;
    }

    OHOS::BufferFlushConfig flushConfig = {};
    flushConfig.damage.x = 0;
    flushConfig.damage.y = 0;
    flushConfig.damage.w = buffer->width;
    flushConfig.damage.h = buffer->height;
    flushConfig.timestamp = 0;

    OHOS::sptr<OHOS::Surface> producer = g_producerMap[buffer->streamId];
    if (producer == nullptr) {
        return -1;
    }
    OHOS::sptr<OHOS::SurfaceBuffer> sb = g_surfaceBufferMap[buffer];
    if (sb == nullptr) {
        return -1;
    }

    OHOS::SurfaceError ret = producer->FlushBuffer(sb, -1, flushConfig);
    g_surfaceBufferMap.erase(buffer);
    return static_cast<int>(ret);
}

int BufferProducerGetQueueSize(int streamId)
{
    OHOS::sptr<OHOS::Surface> producer = g_producerMap[streamId];
    if (producer == nullptr) {
        return -1;
    }

    return producer->GetQueueSize();
}

unsigned int BufferProducerSetQueueSize(int streamId, int size)
{
    OHOS::sptr<OHOS::Surface> producer = g_producerMap[streamId];
    if (producer == nullptr) {
        return -1;
    }

    return producer->SetQueueSize(size);
}

void BufferProducerDetachBufferProducer(int id)
{
    OHOS::sptr<OHOS::Surface> producer = g_producerMap[id];
    if (producer == nullptr) {
        return;
    }

    g_producerMap.erase(id);

}
