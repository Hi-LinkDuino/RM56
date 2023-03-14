/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef HIVIEW_BASE_PIPELINE_H
#define HIVIEW_BASE_PIPELINE_H
#include <memory>

#include "event.h"
#include "event_loop.h"
#include "plugin.h"

namespace OHOS {
namespace HiviewDFX {
class Pipeline;

class PipelineEventProducer;

class PipelineEvent : public Event {
public:
    PipelineEvent(const std::string& sender, PipelineEventProducer* handler)
        : Event(sender), handler_(handler), startDeliver_(false), pipelineName_("")
    {
        isPipeline_ = true;
    };

    PipelineEvent(const Event& event) : Event(event), handler_(nullptr), startDeliver_(false), pipelineName_("")
    {
        isPipeline_ = true;
    };

    PipelineEvent(const PipelineEvent& obj)
        : Event(obj),
          inputLogfileParamList_(std::move(obj.inputLogfileParamList_)),
          deleteLogfileParamList_(std::move(obj.deleteLogfileParamList_)),
          handler_(obj.handler_),
          startDeliver_(obj.startDeliver_),
          pipelineName_(obj.pipelineName_),
          processors_(std::move(obj.processors_)){};

    PipelineEvent& operator=(const PipelineEvent& obj)
    {
        if (&obj == this) {
            return *this;
        }

        Event::operator=(obj);
        inputLogfileParamList_ = std::move(obj.inputLogfileParamList_);
        deleteLogfileParamList_ = std::move(obj.deleteLogfileParamList_);
        handler_ = obj.handler_;
        startDeliver_ = obj.startDeliver_;
        pipelineName_ = obj.pipelineName_;
        processors_ = std::move(obj.processors_);
        return *this;
    };

    virtual ~PipelineEvent(){};

    void OnRepack() override;
    void OnPending() override;
    bool OnContinue() override;
    bool OnFinish() override;
    uint32_t GetPendingProcessorSize() override;
    void SetPipelineInfo(const std::string& pipelineName, std::list<std::weak_ptr<Plugin>>& processors);
    std::string GetPipelineInfo();
    std::list<std::string> inputLogfileParamList_;
    std::list<std::string> deleteLogfileParamList_;

    // transform an event into pipeline event
    // fill with pipeline info of given name
    // the new event share no relationship with the old one
    // EventType should be the correct type of the event object
    // PipelineEventType should be the type of the target repack type
    // copy construct will be invoked in the repack process
    template <typename EventType, typename PipelineEventType>
    static std::shared_ptr<PipelineEventType> RepackPipelineEvent(std::shared_ptr<Plugin> caller,
                                                                  const std::string& pipelineName,
                                                                  std::shared_ptr<Event>& event,
                                                                  bool deliverFromCurrent)
    {
        auto base = std::static_pointer_cast<EventType>(event);
        auto derived = std::make_shared<PipelineEventType>(*(base.get()));
        auto pipe = std::static_pointer_cast<PipelineEvent>(derived);
        pipe->OnRepack();
        FillPipelineInfo(caller, pipelineName, pipe, deliverFromCurrent);
        return derived;
    };

    static void FillPipelineInfo(std::shared_ptr<Plugin> caller, const std::string& pipelineName,
                                 std::shared_ptr<PipelineEvent> event, bool deliverFromCurrent);

private:
    PipelineEventProducer* handler_;
    bool startDeliver_;
    std::string pipelineName_;
    std::list<std::weak_ptr<Plugin>> processors_;
};

class PipelineEventProducer {
public:
    virtual ~PipelineEventProducer(){};
    // Notify event producer that an event has finish it's delivery
    virtual void Recycle(PipelineEvent* event) = 0;
    // Pause dispatch and schedule resume dispatch
    virtual void PauseDispatch(std::weak_ptr<Plugin> plugin) = 0;
};

class Pipeline : public std::enable_shared_from_this<Pipeline> {
public:
    Pipeline(const std::string& name, std::list<std::weak_ptr<Plugin>>& processors)
        : name_(name), processors_(std::move(processors)){};
    bool CanProcessEvent(std::shared_ptr<PipelineEvent> event);
    void ProcessEvent(std::shared_ptr<PipelineEvent> event);
    ~Pipeline() {};

    const std::string& GetName() const
    {
        return name_;
    };

    std::list<std::weak_ptr<Plugin>> GetProcessSequence()
    {
        return processors_;
    };
    void AppendProcessor(std::weak_ptr<Plugin> plugin);
    void RemoveProcessor(std::weak_ptr<Plugin> plugin);
private:
    std::string name_;
    std::list<std::weak_ptr<Plugin>> processors_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_PIPELINE_H