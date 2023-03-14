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

#define HST_LOG_TAG "PipelineCore"

#include "pipeline_core.h"
#include <queue>
#include <stack>
#include "foundation/log.h"
#include "osal/thread/scoped_lock.h"
#include "utils/steady_clock.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
std::shared_ptr<const Plugin::Meta> OHOS::Media::Pipeline::MetaBundle::GeTrackMeta(int32_t trackId)
{
    for (auto& ptr : trackMeta_) {
        uint32_t found = 0;
        if (ptr->GetUint32(Plugin::MetaID::TRACK_ID, found) && found == trackId) {
            return ptr;
        }
    }
    return nullptr;
}

void MetaBundle::UpdateGlobalMeta(const Plugin::Meta& meta)
{
    if (globalMeta_ == nullptr) {
        globalMeta_ = std::make_shared<Plugin::Meta>();
    }
    globalMeta_->Update(meta);
}

void MetaBundle::UpdateTrackMeta(const Plugin::Meta& meta)
{
    uint32_t trackId = 0;
    if (!meta.GetUint32(Plugin::MetaID::TRACK_ID, trackId)) {
        MEDIA_LOG_W("update stream meta with invalid meta, which contains no track id, will ignore this meta");
        return;
    }
    for (const auto& tmp : trackMeta_) {
        uint32_t tid = 0;
        if (tmp->GetUint32(Plugin::MetaID::TRACK_ID, tid) && trackId == tid) {
            tmp->Update(meta);
            return;
        }
    }
    auto ptr = std::make_shared<Plugin::Meta>();
    ptr->Update(meta);
    trackMeta_.emplace_back(ptr);
}

PipelineCore::PipelineCore(const std::string& name)
    : name_(name), eventReceiver_(nullptr), filterCallback_(nullptr), metaBundle_(std::make_shared<MetaBundle>())
{
}

const std::string& PipelineCore::GetName()
{
    return name_;
}

const EventReceiver* PipelineCore::GetOwnerPipeline() const
{
    return eventReceiver_;
}

void PipelineCore::Init(EventReceiver* receiver, FilterCallback* callback)
{
    eventReceiver_ = receiver;
    filterCallback_ = callback;
    state_ = FilterState::INITIALIZED;
}

ErrorCode PipelineCore::Prepare()
{
    state_ = FilterState::PREPARING;
    ErrorCode rtv = ErrorCode::SUCCESS;
    OSAL::ScopedLock lock(mutex_);
    ReorderFilters();
    for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
        auto& filterPtr = *it;
        if (filterPtr) {
            if ((rtv = filterPtr->Prepare()) != ErrorCode::SUCCESS) {
                break;
            }
        } else {
            MEDIA_LOG_E("invalid pointer in filters.");
        }
    }
    return rtv;
}

ErrorCode PipelineCore::Start()
{
    state_ = FilterState::RUNNING;
    for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
        auto rtv = (*it)->Start();
        FALSE_RETURN_V(rtv == ErrorCode::SUCCESS, rtv);
    }
    return ErrorCode::SUCCESS;
}

ErrorCode PipelineCore::Pause()
{
    if (state_ == FilterState::PAUSED) {
        return ErrorCode::SUCCESS;
    }
    if (state_ != FilterState::READY && state_ != FilterState::RUNNING) {
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    state_ = FilterState::PAUSED;
    for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
        if ((*it)->Pause() != ErrorCode::SUCCESS) {
            MEDIA_LOG_I("pause filter: " PUBLIC_LOG_S, (*it)->GetName().c_str());
        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode PipelineCore::Resume()
{
    for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
        MEDIA_LOG_I("Resume filter: " PUBLIC_LOG_S, (*it)->GetName().c_str());
        auto rtv = (*it)->Resume();
        FALSE_RETURN_V(rtv == ErrorCode::SUCCESS, rtv);
    }
    state_ = FilterState::RUNNING;
    return ErrorCode::SUCCESS;
}

ErrorCode PipelineCore::Stop()
{
    readyEventCnt_ = 0;
    state_ = FilterState::INITIALIZED;
    filtersToRemove_.clear();
    filtersToRemove_.reserve(filters_.size());
    for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
        if (*it == nullptr) {
            MEDIA_LOG_E("PipelineCore error: " PUBLIC_LOG_ZU, filters_.size());
            continue;
        }
        MEDIA_LOG_I("Stop filter: " PUBLIC_LOG_S, (*it)->GetName().c_str());
        PROFILE_BEGIN();
        auto rtv = (*it)->Stop();
        PROFILE_END("Stop finished for %s", (*it)->GetName().c_str());
        FALSE_RETURN_V(rtv == ErrorCode::SUCCESS, rtv);
    }
    for (const auto& filter : filtersToRemove_) {
        RemoveFilter(filter);
    }
    MEDIA_LOG_I("Stop finished, filter number: " PUBLIC_LOG_ZU, filters_.size());
    return ErrorCode::SUCCESS;
}

void PipelineCore::FlushStart()
{
    for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
        MEDIA_LOG_I("FlushStart for filter: " PUBLIC_LOG_S, (*it)->GetName().c_str());
        (*it)->FlushStart();
    }
}

void PipelineCore::FlushEnd()
{
    for (auto it = filters_.rbegin(); it != filters_.rend(); ++it) {
        MEDIA_LOG_I("FlushEnd for filter: " PUBLIC_LOG_S, (*it)->GetName().c_str());
        (*it)->FlushEnd();
    }
}

FilterState PipelineCore::GetState()
{
    return state_;
}

ErrorCode PipelineCore::AddFilters(std::initializer_list<Filter*> filtersIn)
{
    std::vector<Filter*> filtersToAdd;
    for (auto& filterIn : filtersIn) {
        bool matched = false;
        for (auto& filter : filters_) {
            if (filterIn == filter) {
                matched = true;
                break;
            }
        }
        if (!matched) {
            filtersToAdd.push_back(filterIn);
        }
    }
    if (filtersToAdd.empty()) {
        MEDIA_LOG_I("filters already exists");
        return ErrorCode::SUCCESS;
    }
    {
        OSAL::ScopedLock lock(mutex_);
        this->filters_.insert(this->filters_.end(), filtersToAdd.begin(), filtersToAdd.end());
    }
    InitFilters(filtersToAdd);
    return ErrorCode::SUCCESS;
}

ErrorCode PipelineCore::RemoveFilter(Filter* filter)
{
    auto it = std::find_if(filters_.begin(), filters_.end(),
                           [&filter](const Filter* filterPtr) { return filterPtr == filter; });
    if (it != filters_.end()) {
        MEDIA_LOG_I("RemoveFilter " PUBLIC_LOG_S, (*it)->GetName().c_str());
        filters_.erase(it);
    }
    return ErrorCode::SUCCESS;
}

ErrorCode PipelineCore::RemoveFilterChain(Filter* firstFilter)
{
    if (!firstFilter) {
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    std::queue<Filter*> levelFilters;
    levelFilters.push(firstFilter);
    while (!levelFilters.empty()) {
        auto filter = levelFilters.front();
        levelFilters.pop();
        filter->UnlinkPrevFilters();
        filtersToRemove_.push_back(filter);
        for (auto&& nextFilter : filter->GetNextFilters()) {
            levelFilters.push(nextFilter);
        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode PipelineCore::LinkFilters(std::initializer_list<Filter*> filters)
{
    std::vector<Filter*> filtersToLink;
    std::vector<Filter*>(filters).swap(filtersToLink);
    int count = std::max((int)(filtersToLink.size()) - 1, 0);
    for (int i = 0; i < count; i++) {
        filtersToLink[i]->GetOutPort(PORT_NAME_DEFAULT)->Connect(filtersToLink[i + 1]->GetInPort(PORT_NAME_DEFAULT));
        filtersToLink[i + 1]->GetInPort(PORT_NAME_DEFAULT)->Connect(filtersToLink[i]->GetOutPort(PORT_NAME_DEFAULT));
    }
    return ErrorCode::SUCCESS;
}

ErrorCode PipelineCore::LinkPorts(std::shared_ptr<OutPort> port1, std::shared_ptr<InPort> port2)
{
    FAIL_RETURN(port1->Connect(port2));
    FAIL_RETURN(port2->Connect(port1));
    return ErrorCode::SUCCESS;
}
void PipelineCore::NotifyEvent(const Event& event)
{
    if (eventReceiver_) {
        eventReceiver_->OnEvent(event);
    } else {
        MEDIA_LOG_I("no event receiver when receive type " PUBLIC_LOG_D32, event.type);
    }
}
void PipelineCore::OnEvent(const Event& event)
{
    if (event.type != EventType::EVENT_READY) {
        NotifyEvent(event);
        return;
    }

    readyEventCnt_++;
    MEDIA_LOG_I("OnEvent readyCnt: " PUBLIC_LOG_ZU " / " PUBLIC_LOG_ZU, readyEventCnt_, filters_.size());
    if (readyEventCnt_ == filters_.size()) {
        state_ = FilterState::READY;
        NotifyEvent(event);
        readyEventCnt_ = 0;
    }
}

void PipelineCore::InitFilters(const std::vector<Filter*>& filters)
{
    for (auto& filter : filters) {
        filter->Init(this, filterCallback_);
    }
}

namespace {
struct FilterNode {
    size_t inDegree {0};
    Filter* filter {nullptr};
    std::vector<size_t> nexts {}; // store filter index
    FilterNode(size_t degree, Filter* f, std::vector<size_t> next) : inDegree(degree), filter(f), nexts(std::move(next))
    {
    }
};

void ConstructGraph(const std::vector<Filter*> &filters, std::vector<FilterNode> &graph, std::stack<size_t> &stack)
{
    std::map<Filter*, size_t> mapInfo; // filter to index map, index is the node index in graph
    size_t index = 0;
    for (const auto& f : filters) {
        graph.emplace_back(FilterNode(f->GetPreFilters().size(), f, {}));
        if (f->GetPreFilters().empty()) {
            stack.push(index);
        }
        mapInfo[f] = index++;
    }
    for (const auto& f : filters) {
        auto& tmp = graph[mapInfo[f]].nexts;
        for (const auto& next : f->GetNextFilters()) {
            tmp.emplace_back(mapInfo[next]);
        }
    }
}
}

void PipelineCore::ReorderFilters()
{
    std::vector<FilterNode> graph;
    std::stack<size_t> stack;
    ConstructGraph(filters_, graph, stack);
    std::vector<Filter*> result;
    while (!stack.empty()) {
        auto cur = stack.top();
        stack.pop();
        for (const auto& idx : graph[cur].nexts) {
            graph[idx].inDegree--;
            if (graph[idx].inDegree == 0) {
                stack.push(idx);
            }
        }
        result.emplace_back(graph[cur].filter);
    }
    if (result.size() == filters_.size()) {
        filters_.clear();
        filters_.assign(result.begin(), result.end());
    }
}

void PipelineCore::SetSyncCenter(std::weak_ptr<IMediaSyncCenter> syncCenter)
{
    syncCenter_ = syncCenter;
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
