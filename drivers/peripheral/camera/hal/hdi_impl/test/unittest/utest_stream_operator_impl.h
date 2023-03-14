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

class StreamOperatorImplTest : public CameraHdiBaseTest {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);
    class TestBufferConsumerListener: public IBufferConsumerListener {
    public:
        void OnBufferAvailable()
        {
        }

        TestBufferConsumerListener()
        {
        }

        ~TestBufferConsumerListener()
        {
        }
    };
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    class StreamConsumer {
    public:
        std::shared_ptr<OHOS::Surface> CreateProducer(std::function<void(OHOS::SurfaceBuffer*)> callback)
        {
            consumer_ = std::shared_ptr<OHOS::Surface>(OHOS::Surface::CreateSurface());
            if (consumer_ == nullptr) {
                return nullptr;
            }

            callback_ = callback;

            consumerThread_ = new std::thread([this] {
                while (running_ == true) {
                    OHOS::SurfaceBuffer* buffer = consumer_->AcquireBuffer();
                    if (buffer != nullptr) {
                        if (callback_ != nullptr) {
                            callback_(buffer);
                        }
                        consumer_->ReleaseBuffer(buffer);
                    }
                }
            });

            return consumer_;
        }

        ~StreamConsumer()
        {
            running_ = false;
            if (consumerThread_ != nullptr) {
                consumerThread_->join();
                delete consumerThread_;
            }
        }

    private:
        bool running_ = true;
        std::shared_ptr<OHOS::Surface> consumer_ = nullptr;
        std::thread* consumerThread_ = nullptr;
        std::function<void(OHOS::SurfaceBuffer*)> callback_ = nullptr;
    };

#else
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
            CAMERA_LOGI("%{public}s, create a buffer queue producer %{public}p", __FUNCTION__, producer.GetRefPtr());

            if (producer == nullptr) {
                return nullptr;
            }
            callback_ = callback;

            consumerThread_ = new std::thread([this] {
                int32_t flushFence = 0;
                int64_t timestamp = 0;
                OHOS::Rect damage;
                while (running_ == true) {
                    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
                    consumer_->AcquireBuffer(buffer, flushFence, timestamp, damage);
                    if (buffer != nullptr) {
                        void* addr = buffer->GetVirAddr();
                        uint32_t size = buffer->GetSize();
                        if (callback_ != nullptr) {
                            callback_(addr, size);
                        }
                        consumer_->ReleaseBuffer(buffer, -1);
                    }
                }
            });

            return producer;
        }
        ~StreamConsumer()
        {
            running_ = false;
            if (consumerThread_ != nullptr) {
                consumerThread_->join();
                delete consumerThread_;
            }
        }

    private:
        bool running_ = true;
        OHOS::sptr<OHOS::Surface> consumer_ = nullptr;
        std::thread* consumerThread_ = nullptr;
        std::function<void(void*, uint32_t)> callback_ = nullptr;
    };
#endif

private:
    void Init();
    void OnError(Camera::ErrorType type, int32_t errorMsg);
    void OnResult(uint64_t timestamp, const std::shared_ptr<CameraStandard::CameraMetadata>& result);
    void ForkProcess();
    void StopProcess();
};

#endif /* UTEST_CAMERA_HOST_IMPL_TEST_H */
