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

#include "host_stream.h"

namespace OHOS::Camera {
class HostStreamImpl : public HostStream {
public:
    StreamIntent GetStreamType() const override;
    HostStreamInfo GetStreamInfo() const override;
    int GetStreamId() const override;
    bool GetStreamState() const override;
    void SetStreamState(bool state) override;
    BufferCb GetBufferCb() const override;
    HostStreamImpl(const HostStreamInfo& type, BufferCb c = nullptr);
    ~HostStreamImpl() override = default;
protected:
    HostStreamInfo info_;
    BufferCb   callBack_;
};

HostStreamImpl::HostStreamImpl(const HostStreamInfo& info, BufferCb c) :
    info_(info),
    callBack_(c)
{
}

StreamIntent HostStreamImpl::GetStreamType() const
{
    return info_.type_;
}

int HostStreamImpl::GetStreamId() const
{
    return info_.streamId_;
}

bool HostStreamImpl::GetStreamState() const
{
    return info_.builed_;
}

void HostStreamImpl::SetStreamState(bool state)
{
    info_.builed_ = state;
    return;
}
HostStreamInfo HostStreamImpl::GetStreamInfo() const
{
    return info_;
}

BufferCb HostStreamImpl::GetBufferCb() const
{
    return callBack_;
}

std::unique_ptr<HostStream> HostStream::Create(const HostStreamInfo& info, BufferCb c)
{
    return std::make_unique<HostStreamImpl>(info, c);
}
}
