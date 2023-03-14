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

#ifndef CAMERA_DEMO_TEST_H
#define CAMERA_DEMO_TEST_H

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include "display_type.h"
#include "ibuffer.h"
#include "ibuffer_pool.h"
#include "surface.h"
#include "ibuffer_consumer_listener.h"

namespace OHOS::CameraUtest {
using namespace OHOS::Camera;

class BufferManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);

public:
    class Stream {
    public:
        Stream() = default;
        ~Stream() = default;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        bool Init(std::shared_ptr<OHOS::Surface>& producer);
#else
        bool Init(OHOS::sptr<OHOS::IBufferProducer>& producer);
#endif
        void StartStream();
        void StopStream();
        void EnqueueBufferNonBlock();
        void DequeueBuffer(std::shared_ptr<IBuffer>& buffer);
        std::shared_ptr<IBufferPool> GetBufferPool() const;
        int64_t GetPoolId() const
        {
            return bufferPoolId_;
        }

    private:
        void StartInnerStream() const;
        void StartExternalStream();

    private:
        std::mutex lock_;
#ifdef CAMERA_BUILT_ON_OHOS_LITE
        std::shared_ptr<OHOS::Surface> producer_ = nullptr;
#else
        OHOS::sptr<OHOS::Surface> producer_ = nullptr;
#endif

        uint32_t width_ = 1920;
        uint32_t height_ = 1080;
        uint32_t queueSize_ = 7;
        uint64_t usage_ = CAMERA_USAGE_SW_WRITE_OFTEN | CAMERA_USAGE_SW_READ_OFTEN | CAMERA_USAGE_MEM_DMA;
        uint32_t format_ = CAMERA_FORMAT_YCBCR_420_SP;
        int64_t bufferPoolId_ = -1;

        std::shared_ptr<IBufferPool> bufferPool_ = nullptr;

#ifdef CAMERA_BUILT_ON_OHOS_LITE
        std::vector<std::pair<OHOS::SurfaceBuffer*, std::shared_ptr<IBuffer>>> bufferVec_ = {};
#else
        std::vector<std::pair<OHOS::sptr<OHOS::SurfaceBuffer>, std::shared_ptr<IBuffer>>> bufferVec_ = {};
        int32_t releaseFence_ = 0;
        OHOS::BufferRequestConfig requestConfig_ = {};
        OHOS::BufferFlushConfig flushConfig_ = {};
#endif
    };

#ifndef CAMERA_BUILT_ON_OHOS_LITE
    class TestBufferConsumerListener: public IBufferConsumerListener {
        public:
            TestBufferConsumerListener()
            {
            }

            ~TestBufferConsumerListener()
            {
            }

            void OnBufferAvailable()
            {
            }
    };
#endif

    class Node {
    public:
        explicit Node(const std::string name)
        {
            name_ = name;
        }
        virtual ~Node() = default;

        virtual void Connect(std::shared_ptr<Node>& nextNode);
        virtual void Deliver(std::shared_ptr<IBuffer>& buffer);
        virtual void Receive(std::shared_ptr<IBuffer>& buffer);
        virtual void Process(std::shared_ptr<IBuffer>& buffer);
        virtual std::string GetName() const;

    private:
        std::string name_ = "";
        std::shared_ptr<Node> nextNode_ = nullptr;

    private:
        Node() = default;
    };

    class SinkNode : public Node {
    public:
        SinkNode(const std::string name) : Node(name) {}
        ~SinkNode() {}
        void Deliver(std::shared_ptr<IBuffer>& buffer) override;
        void BindCallback(std::function<void(std::shared_ptr<IBuffer>&)> callback);

    private:
        std::function<void(std::shared_ptr<IBuffer>&)> callback_ = nullptr;
    };

    class SourceNode : public Node {
    public:
        SourceNode(const std::string name) : Node(name) {}
        void Process(std::shared_ptr<IBuffer>& buffer) override;
        ~SourceNode() {}
    private:
        int cacheSize_ = 3;
        std::list<std::shared_ptr<IBuffer>> cache_ = {};
    };

    class Pipeline {
    public:
        Pipeline() = default;
        ~Pipeline() = default;

        bool AddStream(std::shared_ptr<Stream>& stream);
        void StartStream();
        void StopStream();

    private:
        void CollectBuffers();
        void DeliverBuffer();
        void DeliverBuffer(std::shared_ptr<IBuffer>& buffer);
        bool BuildPipeline();

    private:
        bool running = true;
        std::mutex streamLock_;
        std::thread* collectThread_ = nullptr;
        std::shared_ptr<Node> sourceNode_ = nullptr;
        uint64_t frameNumber = 0;

        struct LocalStream {
            std::mutex deviceLock_;
            std::shared_ptr<Stream> stream_ = nullptr;
            std::thread* deliverThread_ = nullptr;
            std::list<std::shared_ptr<IBuffer>> deviceBufferList_ = {};
        };
        std::shared_ptr<LocalStream> localStream_ = nullptr;
    };
};
} // namespace OHOS::CameraUtest
#endif // CAMERA_DEMO_TEST_H
