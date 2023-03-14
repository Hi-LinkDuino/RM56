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

#ifndef MEDIA_PIPELINE_PLAYER_EXECUTOR_H
#define MEDIA_PIPELINE_PLAYER_EXECUTOR_H

#include <memory>
#include "pipeline/core/error_code.h"
#include "plugin/common/media_source.h"

namespace OHOS {
namespace Media {
using MediaSource = Plugin::MediaSource;

struct SeekInfo {
    int64_t hstTime;
    Plugin::SeekMode mode;
};

class PlayExecutor {
public:
    virtual ~PlayExecutor()
    {
    }

    virtual bool IsSingleLoop() = 0;

    virtual ErrorCode PrepareFilters()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoSetSource(const std::shared_ptr<MediaSource>& source)
    {
        (void)source;
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoPlay()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoPause()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoResume()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoStop()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoReset()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoSeek(bool allowed, int64_t hstTime, Plugin::SeekMode mode, bool appTriggered)
    {
        (void)allowed;
        (void)hstTime;
        (void)mode;
        (void)appTriggered;
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoOnReady()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoOnComplete()
    {
        return ErrorCode::SUCCESS;
    }

    virtual ErrorCode DoOnError(ErrorCode errorCode)
    {
        (void)errorCode;
        return ErrorCode::SUCCESS;
    }
};
} // namespace Media
} // namespace OHOS

#endif
