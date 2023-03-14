/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_CORE_CLOCK_MANAGER_H
#define HISTREAMER_PIPELINE_CORE_CLOCK_MANAGER_H

#include <memory>
#include <queue>

#include "clock_provider.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class ClockManager {
public:
    ClockManager(const ClockManager&) = delete;
    ClockManager operator=(const ClockManager&) = delete;
    ~ClockManager() = default;
    static ClockManager& Instance()
    {
        static ClockManager manager;
        return manager;
    }

    // todo how to decide the priority of providers
    void RegisterProvider(const std::shared_ptr<ClockProvider>& provider);
    void UnRegisterProvider(const std::shared_ptr<ClockProvider>& provider);
    void ClearProviders();

    ClockProvider& GetProvider()
    {
        return providerProxy_;
    }
private:
    ClockManager() = default;
    class SyncClockProviderProxy : public ClockProvider {
    public:
        ~SyncClockProviderProxy() override = default;

        void SetStub(std::shared_ptr<ClockProvider>& stub)
        {
            stub_ = stub;
        }

        ErrorCode CheckPts(int64_t pts, int64_t &delta) override
        {
            if (stub_ == nullptr) {
                return ErrorCode::ERROR_INVALID_OPERATION;
            }
            return stub_->CheckPts(pts, delta);
        }

        ErrorCode GetCurrentPosition(int64_t &position) override
        {
            if (stub_ == nullptr) {
                return ErrorCode::ERROR_INVALID_OPERATION;
            }
            return stub_->GetCurrentPosition(position);
        }

        ErrorCode GetCurrentTimeNano(int64_t& nowNano) override
        {
            if (stub_ == nullptr) {
                return ErrorCode::ERROR_INVALID_OPERATION;
            }
            return stub_->GetCurrentTimeNano(nowNano);
        }

    private:
        std::shared_ptr<ClockProvider> stub_;
    };

    SyncClockProviderProxy providerProxy_;

    std::priority_queue<std::shared_ptr<ClockProvider>> providers_;
};
} // Pipeline
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_CORE_CLOCK_MANAGER_H
