/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef I_PIPELINE_CORE_H
#define I_PIPELINE_CORE_H

#include <stdint.h>
#include <cstdio>
#include "istream_pipeline_core.h"
#include "no_copyable.h"
#include "idevice_manager.h"
namespace OHOS::Camera {
class IPipelineCore : public NoCopyable {
public:
    static std::shared_ptr<IPipelineCore> Create();
    virtual RetCode Init() = 0;
    virtual std::shared_ptr<HostStreamMgr> GetHostStreamMgr() const = 0;
    virtual std::shared_ptr<IStreamPipelineCore> GetStreamPipelineCore() const = 0;
    virtual void UpdateMetadata(std::shared_ptr<CameraStandard::CameraMetadata> meta)  = 0;
    virtual ~IPipelineCore() = default;
};
}
#endif
