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

#include "host_stream_mgr.h"
#include "host_stream.h"

namespace OHOS::Camera {
class HostStreamMgrImpl : public HostStreamMgr {
public:
    RetCode CreateHostStream(const HostStreamInfo& info, BufferCb c) override;
    RetCode DestroyHostStream(const std::vector<int>& types) override;
    void GetStreamTypes(std::vector<int32_t>& s) const override;
    HostStreamInfo GetStreamInfo(const int32_t& id) const override;
    BufferCb GetBufferCb(const int32_t& type) const override;
    void GetStreamIds(std::vector<int32_t>& s) const;
    int32_t DesignateStreamIdForType(const int32_t streamType);
    HostStreamMgrImpl() = default;
    ~HostStreamMgrImpl() override = default;
protected:
    std::vector<std::unique_ptr<HostStream>> streams_;
};

RetCode HostStreamMgrImpl::CreateHostStream(const HostStreamInfo& info, BufferCb c)
{
    auto it = std::find_if(streams_.begin(), streams_.end(), [info](const std::unique_ptr<HostStream>& s) {
                return s->GetStreamId() == info.streamId_;
                });
    if (it != streams_.end()) {
        CAMERA_LOGE("host stream %{public}d exists.", info.streamId_);
        return RC_OK;
    }
    CAMERA_LOGI("bufferpool id = %{public}llu , stream id = %{public}d,stream type = %{public}d, encode = %{public}d",
        info.bufferPoolId_, info.streamId_, info.type_, info.encodeType_);
    streams_.push_back(HostStream::Create(info, c));
    for (auto& stream : streams_) {
        stream->SetStreamState(false);
    }
    return RC_OK;
}

RetCode HostStreamMgrImpl::DestroyHostStream(const std::vector<int>& streamIds)
{
    if (streamIds.empty()) {
        return RC_OK;
    }

    for (auto& streamId : streamIds) {
        auto it = std::find_if(streams_.begin(), streams_.end(), [streamId](const std::unique_ptr<HostStream>& s) {
            return s->GetStreamId() == streamId;
            });
        if (it != streams_.end()) {
            streams_.erase(it);
        } else {
            CAMERA_LOGE("stream id not found. [stream id = %{public}d]", streamId);
        }
    }
    return RC_OK;
}

void HostStreamMgrImpl::GetStreamTypes(std::vector<int32_t>& s) const
{
    for (const auto& it : streams_) {
        s.push_back(static_cast<std::underlying_type<StreamIntent>::type>(it->GetStreamType()));
    }
    std::sort(s.begin(), s.end(), [](const int32_t& f, const int32_t& n) {
                    return f < n;
                });
}

void HostStreamMgrImpl::GetStreamIds(std::vector<int32_t>& s) const
{
    for (auto& it : streams_) {
        s.emplace_back(it->GetStreamId());
    }
}

HostStreamInfo HostStreamMgrImpl::GetStreamInfo(const int32_t& id) const
{
    for (auto& it : streams_) {
        if (it->GetStreamId() == id) {
            return it->GetStreamInfo();
        }
    }
    return {};
}

BufferCb HostStreamMgrImpl::GetBufferCb(const int& streamId) const
{
    auto it = std::find_if(streams_.begin(), streams_.end(), [streamId](const std::unique_ptr<HostStream>& s) {
                return s->GetStreamId() == streamId;
                });
    if (it != streams_.end()) {
        return (*it)->GetBufferCb();
    }
    return nullptr;
}

int32_t HostStreamMgrImpl::DesignateStreamIdForType(const int32_t streamType)
{
    for (auto& it : streams_) {
        if (streamType == it->GetStreamType()) {
            if (it->GetStreamState() == false) {
                it->SetStreamState(true);
                return it->GetStreamId();
            }
        }
    }
    return -1;
}

std::shared_ptr<HostStreamMgr> HostStreamMgr::Create()
{
    return std::make_shared<HostStreamMgrImpl>();
}
}
