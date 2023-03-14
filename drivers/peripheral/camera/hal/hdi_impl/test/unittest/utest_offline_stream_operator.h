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

#ifndef UTEST_CAMERA_HOST_IMPL_TEST_H
#define UTEST_CAMERA_HOST_IMPL_TEST_H

#include "utest_camera_hdi_base.h"

class OffileStreamOperatorImplTest : public CameraHdiBaseTest {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);

    class TestBufferConsumerListener: public IBufferConsumerListener {
        public:
            TestBufferConsumerListener()
            {
            }
             void OnBufferAvailable()
            {
            }
            ~TestBufferConsumerListener()
            {
            }
    };

    class StreamConsumer {
    public:
        OHOS::sptr<OHOS::IBufferProducer> CreateProducer(std::function<void(void*, uint32_t)> callback)
        {
            consumer_ = OHOS::Surface::CreateSurfaceAsConsumer();
            if (consumer_ == nullptr) {
                return nullptr;
            }
            sptr<IBufferConsumerListener> listener = new TestBufferConsumerListener();
            consumer_->RegisterConsumerListener(listener);

            auto producer = consumer_->GetProducer();
            CAMERA_LOGI("%{public}s, to create a buffer queue producer %{public}p", __FUNCTION__, producer.GetRefPtr());

            if (producer == nullptr) {
                return nullptr;
            }
            callback_ = callback;

            consumerThread_ = new std::thread([this] {
                OHOS::Rect damage;
                int32_t flushFence = 0;
                int64_t timestamp = 0;
                while (running_ == true) {
                    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
                    consumer_->AcquireBuffer(buffer, flushFence, timestamp, damage);
                    if (buffer != nullptr) {
                        void* address = buffer->GetVirAddr();
                        uint32_t size = buffer->GetSize();
                        if (callback_ != nullptr) {
                            callback_(address, size);
                        }
                        consumer_->ReleaseBuffer(buffer, -1);
                        shotCount_--;
                        if (shotCount_ == 0) {
                            std::unique_lock<std::mutex> l(l_);
                            cv_.notify_one();
                        }
                    }
                }
            });

            return producer;
        }

        void TakeSnapshot()
        {
            shotCount_++;
        }

        void WaitSnapshotEnd()
        {
            std::unique_lock<std::mutex> l(l_);
            cv_.wait(l, [this]() { return shotCount_ == 0; });
        }

        ~StreamConsumer()
        {
            running_ = false;
            if (consumerThread_ != nullptr) {
                consumerThread_->join();
                delete consumerThread_;
            }
        }

    public:
        std::atomic<uint64_t> shotCount_ = 0;
        std::mutex l_;
        bool running_ = true;
        std::condition_variable cv_;
        OHOS::sptr<OHOS::Surface> consumer_ = nullptr;
        std::function<void(void*, uint32_t)> callback_ = nullptr;
        std::thread* consumerThread_ = nullptr;
    };
};
#endif /* UTEST_CAMERA_HOST_IMPL_TEST_H */
