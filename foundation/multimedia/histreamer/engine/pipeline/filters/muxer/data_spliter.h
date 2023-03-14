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

#ifndef HISTREAMER_PIPELINE_DATA_SPLITER_H
#define HISTREAMER_PIPELINE_DATA_SPLITER_H

#include <memory>
#include <utility>
#include "pipeline/core/error_code.h"
#include "pipeline/core/type_define.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class DataSpliter {
public:
    DataSpliter() = default;
    void SetNextFilter(std::shared_ptr<Filter> output)
    {
        nextFilter_ = std::move(output);
    }

    void SetMuxerPlugin(std::shared_ptr<Plugin::Muxer> muxer)
    {
        muxer_ = std::move(muxer);
    }

    ErrorCode FinishWithoutDrain()
    {
        return ErrorCode::SUCCESS;
    }

    void SetMaxOutputSize(size_t size)
    {
        maxOutputSize_ = size;
    }
    void SetMaxDurationUs(size_t duration)
    {
        maxDurationUs_ = duration;
    }
    virtual ErrorCode PushData(int32_t trackId, std::shared_ptr<AVBuffer> buffer) = 0;

    virtual ErrorCode Start()
    {
        return ErrorCode::SUCCESS;
    }
    virtual ErrorCode Stop()
    {
        return ErrorCode::SUCCESS;
    }

protected:
    static const size_t DEFAULT_MAX_DURATION_US = 60 * 1000 * 1000;
    static const size_t DEFAULT_MAX_OUTPUT_SIZE = 0;
    std::shared_ptr<Filter> nextFilter_ {};
    std::shared_ptr<Plugin::Muxer> muxer_ {};
    size_t maxOutputSize_{DEFAULT_MAX_OUTPUT_SIZE};
    size_t maxDurationUs_{DEFAULT_MAX_DURATION_US};
};
} // Pipeline
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_DATA_SPLITER_H
