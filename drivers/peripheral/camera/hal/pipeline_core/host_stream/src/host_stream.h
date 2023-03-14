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

#ifndef HOST_STREAM_H
#define HOST_STREAM_H
#include <memory>
#include "host_stream_data_structure.h"
#include "no_copyable.h"

namespace OHOS::Camera {
class HostStream : public NoCopyable {
public:
    static std::unique_ptr<HostStream> Create(const HostStreamInfo& info, BufferCb c = nullptr);
    virtual StreamIntent GetStreamType() const = 0;
    virtual int GetStreamId() const = 0;
    virtual bool GetStreamState() const = 0;
    virtual void SetStreamState(bool state) = 0;
    virtual HostStreamInfo GetStreamInfo() const = 0;
    virtual BufferCb GetBufferCb() const = 0;
    virtual ~HostStream() = default;
};
}
#endif
