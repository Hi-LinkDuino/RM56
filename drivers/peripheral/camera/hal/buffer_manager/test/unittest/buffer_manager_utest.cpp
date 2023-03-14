/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "buffer_manager_utest.h"
#include <chrono>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include "buffer_adapter.h"
#include "buffer_allocator_utils.h"
#include "buffer_manager.h"
#include "buffer_tracking.h"
#include "image_buffer.h"
#include "securec.h"

using namespace OHOS;
using namespace OHOS::CameraUtest;
using namespace testing::ext;

namespace {
    const int FRAME_INTERVAL_US = 33000; // 33ms
    const int FRAME_RUN_TIME_S = 3;      // 3s
}

void BufferManagerTest::SetUpTestCase(void)
{
    std::cout << "Camera::BufferManager SetUpTestCase" << std::endl;
}

void BufferManagerTest::TearDownTestCase(void)
{
    std::cout << "Camera::BufferManager TearDownTestCase" << std::endl;
}

void BufferManagerTest::SetUp(void)
{
    std::cout << "Camera::BufferManager SetUp" << std::endl;
}

void BufferManagerTest::TearDown(void)
{
    std::cout << "Camera::BufferManager TearDown.." << std::endl;
}

HWTEST_F(BufferManagerTest, TestBufferQueueLoop, TestSize.Level0)
{
    bool running = true;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> consumer =
        std::shared_ptr<OHOS::Surface>(OHOS::Surface::CreateSurface());
    std::thread consumerThread([&consumer, &running] {
        while (running) {
            OHOS::SurfaceBuffer* buffer = consumer->AcquireBuffer();
            if (buffer != nullptr) {
                consumer->ReleaseBuffer(buffer);
                std::cout << "receive a buffer ..." << std::endl;
            }
        }
    });
    std::shared_ptr<OHOS::Surface> producer = consumer;
#else
    OHOS::sptr<OHOS::Surface> consumer = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new TestBufferConsumerListener();
    consumer->RegisterConsumerListener(listener);

    auto producer = consumer->GetProducer();
    EXPECT_EQ(false, producer == nullptr);
    std::thread consumerThread([&consumer, &running] {
        int32_t flushFence = 0;
        int64_t timestamp = 0;
        Rect damage;
        while (running) {
            sptr<SurfaceBuffer> buffer = nullptr;
            consumer->AcquireBuffer(buffer, flushFence, timestamp, damage);
            if (buffer != nullptr) {
                SurfaceError ret = consumer->ReleaseBuffer(buffer, -1);
                if (ret == SURFACE_ERROR_OK) {
                    std::cout << "receive a buffer ..." << std::endl;
                }
            }
        }
    });
#endif

    // HDI impl start from here
    auto stream = std::make_shared<BufferManagerTest::Stream>();
    stream->Init(producer);
    stream->StartStream();

    BufferManagerTest::Pipeline pipeline;
    pipeline.AddStream(stream);

    std::thread enqueueThread([&stream, &running] {
        while (running == true) {
            stream->EnqueueBufferNonBlock();
        }
    });
    pipeline.StartStream();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    running = false;
    enqueueThread.join();
    pipeline.StopStream();
    stream->StopStream();

    running = false;
    consumerThread.join();
}

HWTEST_F(BufferManagerTest, TestGetBufferPool, TestSize.Level0)
{
    Camera::BufferManager* manager = Camera::BufferManager::GetInstance();
    EXPECT_EQ(true, manager != nullptr);

    int64_t bufferPoolId = manager->GenerateBufferPoolId();
    EXPECT_EQ(true, bufferPoolId != 0);

    std::shared_ptr<IBufferPool> bufferPool1 = nullptr;
    std::thread user1([&manager, &bufferPool1, &bufferPoolId] { bufferPool1 = manager->GetBufferPool(bufferPoolId); });

    std::shared_ptr<IBufferPool> bufferPool2 = nullptr;
    std::thread user2([&manager, &bufferPool2, &bufferPoolId] { bufferPool2 = manager->GetBufferPool(bufferPoolId); });

    user1.join();
    user2.join();

    EXPECT_EQ(true, bufferPool1 != nullptr);
    EXPECT_EQ(true, bufferPool1 == bufferPool2);

    int64_t bufferPoolId2 = manager->GenerateBufferPoolId();
    EXPECT_EQ(true, bufferPoolId != 0);
    std::shared_ptr<IBufferPool> bufferPool3 = manager->GetBufferPool(bufferPoolId2);
    EXPECT_EQ(true, bufferPool1 != bufferPool3);

    std::shared_ptr<IBufferPool> nullbufferPool = manager->GetBufferPool(1);
    EXPECT_EQ(true, nullbufferPool == nullptr);
}

HWTEST_F(BufferManagerTest, TestHeapBuffer, TestSize.Level0)
{
    Camera::BufferManager* manager = Camera::BufferManager::GetInstance();
    EXPECT_EQ(true, manager != nullptr);

    int64_t bufferPoolId = manager->GenerateBufferPoolId();
    EXPECT_EQ(true, bufferPoolId != 0);

    std::shared_ptr<IBuffer> buffer;
    {
        std::shared_ptr<IBufferPool> bufferPool = manager->GetBufferPool(bufferPoolId);
        EXPECT_EQ(true, bufferPool != nullptr);

        RetCode rc = bufferPool->Init(2, 1, CAMERA_USAGE_SW_WRITE_OFTEN, CAMERA_FORMAT_YCBCR_422_P, 1,
                                      CAMERA_BUFFER_SOURCE_TYPE_HEAP);
        EXPECT_EQ(true, rc == RC_OK);

        buffer = bufferPool->AcquireBuffer();
        EXPECT_EQ(true, buffer != nullptr);
        EXPECT_EQ(true, buffer->GetVirAddress() != nullptr);
        EXPECT_EQ(true, buffer->GetWidth() == 2);
        EXPECT_EQ(true, buffer->GetHeight() == 1);
        EXPECT_EQ(true, buffer->GetUsage() == CAMERA_USAGE_SW_WRITE_OFTEN);
        EXPECT_EQ(true, buffer->GetFormat() == CAMERA_FORMAT_YCBCR_422_P);
        EXPECT_EQ(true, buffer->GetSize() == 4);

        char src[4] = {'t', 'e', 's', 't'};
        char* dest = reinterpret_cast<char*>(buffer->GetVirAddress());
        if (dest == nullptr) {
            std::cerr << "dest allocate failed!" << std::endl;
        }
        (void)memcpy_s(dest, sizeof(char) * 4, src, sizeof(char) * 4);
        EXPECT_EQ(true, 0 == ::memcmp(src, dest, 4));

        EXPECT_EQ(true, RC_OK == bufferPool->ReturnBuffer(buffer));
    }
    EXPECT_EQ(true, buffer->GetVirAddress() == nullptr);
}

#ifndef CAMERA_BUILT_ON_OHOS_LITE
HWTEST_F(BufferManagerTest, TestGrallocBuffer, TestSize.Level0)
{
    Camera::BufferManager* manager = Camera::BufferManager::GetInstance();
    EXPECT_EQ(true, manager != nullptr);

    uint32_t formatTest[] = {CAMERA_FORMAT_RGBA_8888, CAMERA_FORMAT_YCBCR_420_SP, CAMERA_FORMAT_YCRCB_420_SP,
                             CAMERA_FORMAT_YCBCR_420_P, CAMERA_FORMAT_YCRCB_420_P};
    for (auto f : formatTest) {
        int64_t bufferPoolId = manager->GenerateBufferPoolId();
        EXPECT_EQ(true, bufferPoolId != 0);

        std::shared_ptr<IBufferPool> bufferPool = manager->GetBufferPool(bufferPoolId);
        EXPECT_EQ(true, bufferPool != nullptr);

        RetCode rc = RC_ERROR;
        rc = bufferPool->Init(1280, 720,
                              CAMERA_USAGE_SW_WRITE_OFTEN | CAMERA_USAGE_SW_READ_OFTEN | CAMERA_USAGE_MEM_DMA, f, 3,
                              CAMERA_BUFFER_SOURCE_TYPE_HEAP);
        EXPECT_EQ(true, rc == RC_OK);
    }
}
#endif

HWTEST_F(BufferManagerTest, TestInternalBufferLoop, TestSize.Level0)
{
    Camera::BufferManager* manager = Camera::BufferManager::GetInstance();
    EXPECT_EQ(true, manager != nullptr);
    int64_t bufferPoolId = manager->GenerateBufferPoolId();
    EXPECT_EQ(true, bufferPoolId != 0);
    std::shared_ptr<IBufferPool> bufferPool = manager->GetBufferPool(bufferPoolId);
    EXPECT_EQ(true, bufferPool != nullptr);
    int count = 5;
    RetCode rc = bufferPool->Init(1280, 720, CAMERA_USAGE_SW_WRITE_OFTEN, CAMERA_FORMAT_YCBCR_422_P, count,
                                  CAMERA_BUFFER_SOURCE_TYPE_HEAP);
    EXPECT_EQ(true, rc == RC_OK);
    std::vector<std::shared_ptr<IBuffer>> bufferVector;
    for (int i = 0; i < count; i++) {
        auto buffer = bufferPool->AcquireBuffer();
        if (buffer != nullptr) {
            bufferVector.emplace_back(buffer);
        }
    }
    EXPECT_EQ(true, 5 == bufferVector.size());
    EXPECT_EQ(true, bufferPool->AcquireBuffer() == nullptr);
    auto begin = std::chrono::system_clock::now();
    auto nullBuffer = bufferPool->AcquireBuffer(1);
    auto end = std::chrono::system_clock::now();
    auto timeElapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "timeElapsed = " << timeElapsed.count() << std::endl;
    EXPECT_EQ(true,
              timeElapsed >= std::chrono::microseconds(1000000) && timeElapsed < std::chrono::microseconds(1100000));
    EXPECT_EQ(true, nullBuffer == nullptr);
    int32_t id = bufferVector[1]->GetIndex();
    EXPECT_EQ(true, bufferPool->ReturnBuffer(bufferVector[1]) == RC_OK);
    begin = std::chrono::system_clock::now();
    nullBuffer = bufferPool->AcquireBuffer(1);
    end = std::chrono::system_clock::now();
    timeElapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    EXPECT_EQ(true, timeElapsed < std::chrono::microseconds(1000000));
    EXPECT_EQ(true, id == nullBuffer->GetIndex());
    id = bufferVector[3]->GetIndex();
    std::thread task([&bufferPool, &bufferVector] {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        bufferPool->ReturnBuffer(bufferVector[3]);
    });
    begin = std::chrono::system_clock::now();
    auto lastBuffer = bufferPool->AcquireBuffer(-1);
    end = std::chrono::system_clock::now();
    task.join();
    timeElapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    EXPECT_EQ(true, timeElapsed <= std::chrono::microseconds(5000000 + 10000) &&
                        timeElapsed >= std::chrono::microseconds(5000000 - 10000));
    EXPECT_EQ(true, lastBuffer != nullptr);
    EXPECT_EQ(true, id == lastBuffer->GetIndex());
}

HWTEST_F(BufferManagerTest, TestExternalBufferLoop, TestSize.Level0)
{
    Camera::BufferManager* manager = Camera::BufferManager::GetInstance();
    EXPECT_EQ(true, manager != nullptr);
    int64_t bufferPoolId = manager->GenerateBufferPoolId();
    EXPECT_EQ(true, bufferPoolId != 0);
    std::shared_ptr<IBufferPool> bufferPool = manager->GetBufferPool(bufferPoolId);
    EXPECT_EQ(true, bufferPool != nullptr);
    int count = 5;
    RetCode rc = bufferPool->Init(1280, 720, CAMERA_USAGE_SW_WRITE_OFTEN, CAMERA_FORMAT_YCBCR_422_P, count,
                                  CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL);
    EXPECT_EQ(true, rc == RC_OK);
    std::list<std::shared_ptr<IBuffer>> idleList;
    std::list<std::shared_ptr<IBuffer>> busyList;
    for (int i = 0; i < count; i++) {
        std::shared_ptr<IBuffer> buffer = std::make_shared<ImageBuffer>();
        buffer->SetIndex(i);
        busyList.emplace_back(buffer);
        bufferPool->AddBuffer(buffer);
        std::cout << "Add buffer : " << i << std::endl;
    }
    bool running = true;
    std::mutex lock;
    int realFrameCount = 0;
    std::thread dispatchBufferTask([&running, &idleList, &busyList, &bufferPool, &lock, &realFrameCount] {
        while (running) {
            std::lock_guard<std::mutex> l(lock);
            if (!idleList.empty()) {
                auto it = idleList.begin();
                if (RC_OK == bufferPool->AddBuffer(*it)) {
                    busyList.splice(busyList.begin(), idleList, it);
                    std::cout << "Enq buffer : " << (*it)->GetIndex() << std::endl;
                    realFrameCount++;
                }
            }
        }
    });
    std::list<std::shared_ptr<IBuffer>> inuseBufferList;
    std::thread acquireBufferTask([&running, &bufferPool, &lock, &inuseBufferList] {
        while (running) {
            auto buffer = bufferPool->AcquireBuffer(2);
            if (buffer != nullptr) {
                std::lock_guard<std::mutex> l(lock);
                inuseBufferList.emplace_back(buffer);
            }
        }
    });
    std::thread returnBufferTask([&running, &lock, &inuseBufferList, &bufferPool, &busyList, &idleList] {
        while (running) {
            std::this_thread::sleep_for(std::chrono::microseconds(FRAME_INTERVAL_US));
            {
                std::lock_guard<std::mutex> l(lock);
                if (!inuseBufferList.empty()) {
                    auto it = inuseBufferList.begin();
                    bufferPool->ReturnBuffer(*it);
                    auto retBufferIt = std::find(busyList.begin(), busyList.end(), *it);
                    if (retBufferIt != busyList.end()) {
                        idleList.splice(idleList.end(), busyList, retBufferIt);
                        std::cout << "Deq buffer : " << (*retBufferIt)->GetIndex() << std::endl;
                    }
                    inuseBufferList.erase(it);
                }
            }
        }
    });
    std::this_thread::sleep_for(std::chrono::seconds(3));
    running = false;
    dispatchBufferTask.join();
    acquireBufferTask.join();
    returnBufferTask.join();
    int expectFrameCount = FRAME_RUN_TIME_S * 1000 * 1000 / FRAME_INTERVAL_US;
    std::cout << "expectFrameCount = " << expectFrameCount << ", realFrameCount = " << realFrameCount << std::endl;
    EXPECT_EQ(true, realFrameCount >= expectFrameCount / 2 && realFrameCount <= expectFrameCount + 5);
}

HWTEST_F(BufferManagerTest, TestTrackingBufferLoop, TestSize.Level0)
{
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<OHOS::Surface> producer = nullptr;
#else
    sptr<OHOS::IBufferProducer> producer = nullptr;
#endif
    // HDI impl start from here
    bool running = true;
    auto stream = std::make_shared<Stream>();
    stream->Init(producer);
    stream->StartStream();

    Pipeline pipeline;
    pipeline.AddStream(stream);

    std::thread enqueueThread([&stream, &running] {
        while (running == true) {
            stream->EnqueueBufferNonBlock();
        }
    });
    pipeline.StartStream();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    int32_t emptyCheck1 = BufferTracking::IsNodeEmpty(0, "SourceNode");
    int32_t emptyCheck2 = BufferTracking::IsNodeEmpty(0, "SourceNode", "SinkNode");
    BufferTraceGraph graph{};
    BufferTracking::DumpBufferTrace(0, graph);
    running = false;
    enqueueThread.join();
    pipeline.StopStream();
    stream->StopStream();
    EXPECT_EQ(true, emptyCheck1 == NODE_IS_NOT_EMPTY);
    EXPECT_EQ(true, emptyCheck2 == NODE_IS_NOT_EMPTY);
    EXPECT_EQ(true, graph.size() == 10);
    EXPECT_EQ(true, graph.front().first == "SourceNode");
    EXPECT_EQ(true, graph.front().second.size() == 3);
    EXPECT_EQ(true, graph.back().first == "SinkNode");
    EXPECT_EQ(true, graph.back().second.size() < 2);
    for (auto it = graph.begin(); it != graph.end(); it++) {
        std::cout << "node [" << it->first << "] has buffer {";
        for (auto& b : it->second) {
            std::cout << static_cast<unsigned long>(b.GetFrameNumber()) << ", ";
        }
        std::cout << "}" << std::endl;
    }
}

namespace OHOS::CameraUtest {
#ifdef CAMERA_BUILT_ON_OHOS_LITE
bool BufferManagerTest::Stream::Init(std::shared_ptr<OHOS::Surface>& producer)
{
    Camera::BufferManager* manager = Camera::BufferManager::GetInstance();
    if (manager == nullptr) {
        return false;
    }
    bufferPoolId_ = manager->GenerateBufferPoolId();
    if (bufferPoolId_ == 0) {
        return false;
    }
    bufferPool_ = manager->GetBufferPool(bufferPoolId_);
    if (bufferPool_ == nullptr) {
        return false;
    }
    RetCode rc = RC_ERROR;
    if (producer == nullptr) {
        rc = bufferPool_->Init(width_, height_, usage_, format_, queueSize_, CAMERA_BUFFER_SOURCE_TYPE_HEAP);
        std::cout << "init inner buffer loop" << std::endl;
    }
    if (producer_ != nullptr) {
        producer_->SetQueueSize(queueSize_);
        if (producer_->GetQueueSize() != queueSize_) {
            return false;
        }
        producer_->SetWidthAndHeight(width_, height_);
        producer_->SetFormat(BufferAdapter::CameraFormatToPixelFormat(format_));
        producer_->SetStrideAlignment(8); // 8:value of strideAlignment
        rc = bufferPool_->Init(width_, height_, usage_, format_, queueSize_, CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL);
        std::cout << "init external buffer loop" << std::endl;
    }
    if (rc != RC_OK) {
        return false;
    }

    return true;
}
#else
bool BufferManagerTest::Stream::Init(sptr<IBufferProducer>& producer)
{
    Camera::BufferManager* manager = Camera::BufferManager::GetInstance();
    if (manager == nullptr) {
        return false;
    }
    bufferPoolId_ = manager->GenerateBufferPoolId();
    if (bufferPoolId_ == 0) {
        return false;
    }
    bufferPool_ = manager->GetBufferPool(bufferPoolId_);
    if (bufferPool_ == nullptr) {
        return false;
    }
    RetCode rc = RC_ERROR;
    if (producer == nullptr) {
        rc = bufferPool_->Init(width_, height_, usage_, format_, queueSize_, CAMERA_BUFFER_SOURCE_TYPE_HEAP);
        std::cout << "init inner buffer loop" << std::endl;
    }
    if (producer != nullptr) {
        producer_ = Surface::CreateSurfaceAsProducer(producer);
        producer_->SetQueueSize(queueSize_);
        if (producer_->GetQueueSize() != queueSize_) {
            return false;
        }

        requestConfig_.width = static_cast<int32_t>(width_);
        requestConfig_.height = static_cast<int32_t>(height_);
        requestConfig_.strideAlignment = 8; // 8:value of strideAlignment
        requestConfig_.format = static_cast<int32_t>(BufferAdapter::CameraFormatToPixelFormat(format_));
        requestConfig_.usage = static_cast<int32_t>(BufferAdapter::CameraUsageToGrallocUsage(usage_));
        requestConfig_.timeout = 0;
        flushConfig_.damage.x = 0;
        flushConfig_.damage.y = 0;
        flushConfig_.damage.w = width_;
        flushConfig_.damage.h = height_;
        flushConfig_.timestamp = 0;
        rc = bufferPool_->Init(width_, height_, usage_, format_, queueSize_, CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL);
        std::cout << "init external buffer loop" << std::endl;
    }
    if (rc != RC_OK) {
        return false;
    }

    return true;
}
#endif

void BufferManagerTest::Stream::StartInnerStream() const
{
    if (producer_ != nullptr) {
        return;
    }
    std::cout << "start inner buffer loop" << std::endl;
    return;
}

void BufferManagerTest::Stream::StartExternalStream()
{
    if (producer_ == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < queueSize_; i++) {
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    OHOS::SurfaceBuffer *sb = nullptr;
    sb = producer_->RequestBuffer();
#else
        sptr<SurfaceBuffer> sb = nullptr;
        SurfaceError ret = producer_->RequestBuffer(sb, releaseFence_, requestConfig_);
        if (ret != SURFACE_ERROR_OK) {
            continue;
        }
#endif
        if (sb == nullptr) {
            continue;
        }
        std::cout << "request a buffer ..." << std::endl;

        std::shared_ptr<IBuffer> cameraBuffer = std::make_shared<ImageBuffer>(CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL);
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        RetCode rc = BufferAdapter::SurfaceBufferToCameraBuffer(sb, producer_, cameraBuffer);
#else
        RetCode rc = BufferAdapter::SurfaceBufferToCameraBuffer(sb, cameraBuffer);
#endif
        if (rc != RC_OK) {
            continue;
        }

        {
            std::lock_guard<std::mutex> l(lock_);
            bufferVec_.emplace_back(std::make_pair(sb, cameraBuffer));
        }

        if (bufferPool_ == nullptr) {
            continue;
        }
        bufferPool_->AddBuffer(cameraBuffer);
        std::cout << "Add buffer ..." << std::endl;
    }

    std::cout << "start external buffer loop" << std::endl;

    return;
}

void BufferManagerTest::Stream::StartStream()
{
    if (producer_ == nullptr) {
        StartInnerStream();
        return;
    }

    if (producer_ != nullptr) {
        StartExternalStream();
        return;
    }

    return;
}

void BufferManagerTest::Stream::EnqueueBufferNonBlock()
{
    if (producer_ == nullptr) {
        return;
    }

#ifdef CAMERA_BUILT_ON_OHOS_LITE
    OHOS::SurfaceBuffer *sb = nullptr;
    sb = producer_->RequestBuffer();
#else
    sptr<SurfaceBuffer> sb = nullptr;
    SurfaceError ret = producer_->RequestBuffer(sb, releaseFence_, requestConfig_);
    if (ret != SURFACE_ERROR_OK) {
        return;
    }
#endif
    if (sb == nullptr) {
        return;
    }

    std::cout << "find buffer begin ..." << std::endl;
    std::shared_ptr<IBuffer> buffer = nullptr;
    {
        std::lock_guard<std::mutex> l(lock_);
        for (auto& it : bufferVec_) {
            if (it.first == sb) {
                buffer = it.second;
                break;
            }
        }
    }
    if (buffer == nullptr) {
        return;
    }
    std::cout << "find buffer end ..." << std::endl;
    bufferPool_->AddBuffer(buffer);
    std::cout << "enqueue buffer ..." << std::endl;

    return;
}

void BufferManagerTest::Stream::DequeueBuffer(std::shared_ptr<IBuffer>& buffer)
{
    if (buffer == nullptr) {
        return;
    }

    if (bufferPool_ == nullptr) {
        return;
    }

    std::cout << "ready to dequeue buffer ..." << std::endl;
    bufferPool_->ReturnBuffer(buffer);

    if (producer_ != nullptr) {
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        SurfaceBuffer* surfaceBuffer = nullptr;
#else
        sptr<SurfaceBuffer> surfaceBuffer = nullptr;
#endif
        {
            std::lock_guard<std::mutex> l(lock_);
            for (auto& it : bufferVec_) {
                if (it.second == buffer) {
                    surfaceBuffer = it.first;
                    break;
                }
            }
        }

        if (surfaceBuffer == nullptr) {
            return;
        }

#ifdef CAMERA_BUILT_ON_OHOS_LITE
        int32_t ret = producer_->FlushBuffer(surfaceBuffer);
        if (ret != 0) {
            std::cout << "flush buffer failed ..." << std::endl;
        }
#else
        SurfaceError ret = producer_->FlushBuffer(surfaceBuffer, -1, flushConfig_);
        if (ret != SURFACE_ERROR_OK) {
            std::cout << "flush buffer failed ..." << std::endl;
        }
#endif
    }

    std::cout << "dequeue buffer ..." << std::endl;

    return;
}

void BufferManagerTest::Stream::StopStream()
{
    return;
}

std::shared_ptr<IBufferPool> BufferManagerTest::Stream::GetBufferPool() const
{
    return bufferPool_;
}

bool BufferManagerTest::Pipeline::AddStream(std::shared_ptr<Stream>& stream)
{
    std::lock_guard<std::mutex> l(streamLock_);
    localStream_ = std::make_shared<LocalStream>();
    if (localStream_ == nullptr) {
        return false;
    }
    localStream_->stream_ = stream;
    return true;
}

void BufferManagerTest::Pipeline::StartStream()
{
    running = true;
    CollectBuffers();
    BuildPipeline();
    DeliverBuffer();

    BufferTracking::StartTracking();
    return;
}

bool BufferManagerTest::Pipeline::BuildPipeline()
{
    BufferTracking::AddTrackingStreamBegin(0, localStream_->stream_->GetPoolId());
    sourceNode_ = std::make_shared<SourceNode>("SourceNode");
    if (sourceNode_ == nullptr) {
        return false;
    }
    BufferTracking::AddTrackingNode(0, sourceNode_->GetName());
    std::shared_ptr<Node> tmpNode = sourceNode_;
    for (int i = 1; i < 9; i++) { // 9: numbers of node
        std::string name = "node";
        name += std::to_string(i);
        auto n = std::make_shared<Node>(name);
        tmpNode->Connect(n);
        tmpNode = n;
        BufferTracking::AddTrackingNode(0, tmpNode->GetName());
    }

    auto sinkNode = std::make_shared<SinkNode>(std::string("SinkNode"));
    if (sinkNode == nullptr) {
        return false;
    }
    sinkNode->BindCallback([this](std::shared_ptr<IBuffer>& buffer) { localStream_->stream_->DequeueBuffer(buffer); });
    std::shared_ptr<Node> node = sinkNode;
    tmpNode->Connect(node);
    BufferTracking::AddTrackingNode(0, sinkNode->GetName());
    BufferTracking::AddTrackingStreamEnd(0);

    return true;
}

void BufferManagerTest::Pipeline::CollectBuffers()
{
    collectThread_ = new std::thread([this] {
        while (running == true) {
            auto bufferPool = localStream_->stream_->GetBufferPool();
            if (bufferPool == nullptr) {
                continue;
            }
            auto buffer = bufferPool->AcquireBuffer(3); // 3:Minimum number of buffer rotation
            if (buffer == nullptr) {
                continue;
            }

            std::lock_guard<std::mutex> deviceL(localStream_->deviceLock_);
            localStream_->deviceBufferList_.emplace_back(buffer);
        }
    });

    return;
}

void BufferManagerTest::Pipeline::DeliverBuffer(std::shared_ptr<IBuffer>& buffer)
{
    buffer->SetFrameNumber(frameNumber++);
    std::cout << "deliver buffer : " << static_cast<unsigned long>(frameNumber) << std::endl;
    sourceNode_->Receive(buffer);
    return;
}

void BufferManagerTest::Pipeline::DeliverBuffer()
{
    localStream_->deliverThread_ = new std::thread([this] {
        while (running == true) {
            std::this_thread::sleep_for(std::chrono::microseconds(FRAME_INTERVAL_US));
            std::shared_ptr<IBuffer> buffer = nullptr;
            {
                std::cout << "load device buffer ..." << std::endl;
                std::lock_guard<std::mutex> l(localStream_->deviceLock_);
                if (localStream_->deviceBufferList_.empty()) {
                    continue;
                }
                buffer = localStream_->deviceBufferList_.front();
                localStream_->deviceBufferList_.pop_front();
            }
            DeliverBuffer(buffer);
        }
    });

    return;
}

void BufferManagerTest::Pipeline::StopStream()
{
    running = false;
    collectThread_->join();

    localStream_->deliverThread_->join();

    BufferTracking::DeleteTrackingStream(0);
    BufferTracking::StopTracking();
    return;
}

void BufferManagerTest::Node::Connect(std::shared_ptr<Node>& nextNode)
{
    nextNode_ = nextNode;
    return;
}

void BufferManagerTest::Node::Deliver(std::shared_ptr<IBuffer>& buffer)
{
    if (nextNode_ == nullptr) {
        return;
    }
    nextNode_->Receive(buffer);
    return;
}

void BufferManagerTest::Node::Receive(std::shared_ptr<IBuffer>& buffer)
{
    std::cout << "report buffer location to node: " << name_ << std::endl;
    PIPELINE_REPORT_BUFFER_LOCATION(0, buffer->GetFrameNumber(), name_);
    Process(buffer);
    return;
}

std::string BufferManagerTest::Node::GetName() const
{
    return name_;
}

void BufferManagerTest::Node::Process(std::shared_ptr<IBuffer>& buffer)
{
    std::this_thread::sleep_for(std::chrono::microseconds(5000)); // 5000:microsecond
    Deliver(buffer);
    return;
}

void BufferManagerTest::SinkNode::BindCallback(std::function<void(std::shared_ptr<IBuffer>&)> func)
{
    callback_ = func;
    return;
}

void BufferManagerTest::SinkNode::Deliver(std::shared_ptr<IBuffer>& buffer)
{
    if (callback_ == nullptr) {
        return;
    }
    callback_(buffer);
    return;
}

void BufferManagerTest::SourceNode::Process(std::shared_ptr<IBuffer>& buffer)
{
    if (cacheSize_ > 0) {
        cache_.emplace_back(buffer);
        cacheSize_--;
        return;
    }

    if (cache_.empty()) {
        Deliver(buffer);
        return;
    }

    auto b = cache_.front();
    cache_.pop_front();
    cache_.emplace_back(buffer);
    Deliver(b);

    return;
}
} // namespace OHOS::CameraUtest

