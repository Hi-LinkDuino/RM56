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
#ifndef HIVIEW_PLUGIN_EXAMPLES_BUNDLE_EVENT_SOURCE_EXAMPLE1
#define HIVIEW_PLUGIN_EXAMPLES_BUNDLE_EVENT_SOURCE_EXAMPLE1
#include "event_source.h"
namespace OHOS {
namespace HiviewDFX {
class BundleEventSourceExampleEvent : public PipelineEvent {
public:
    BundleEventSourceExampleEvent(const std::string& sender, PipelineEventProducer* handler):
        PipelineEvent(sender, handler),
        data_(nullptr),
        addon_("") {};

    BundleEventSourceExampleEvent(const BundleEventSourceExampleEvent& obj) : PipelineEvent(obj)
    {
        data_ = nullptr;
        addon_ = obj.addon_;
    };

    BundleEventSourceExampleEvent& operator=(const BundleEventSourceExampleEvent& obj)
    {
        if (&obj == this) {
            return *this;
        }

        PipelineEvent::operator=(obj);
        data_ = nullptr;
        addon_ = obj.addon_;
        return *this;
    };

    ~BundleEventSourceExampleEvent()
    {
        if (data_ != nullptr) {
            free(data_);
            data_ = nullptr;
        }
    }

    // example for add more metrics
    char* data_;
    std::string addon_;
};

class BundleEventSourceExample : public FileDescriptorEventCallback, public EventSource {
public:
    BundleEventSourceExample();
    ~BundleEventSourceExample();

    static std::set<std::string> count;

    void OnLoad() override;
    void OnUnload() override;
    void StartEventSource() override;

    bool OnFileDescriptorEvent(int fd, int type) override;
    int32_t GetPollFd() override;
    int32_t GetPollType() override;

    void Recycle(PipelineEvent* event) override;
    void PauseDispatch(std::weak_ptr<Plugin> plugin) override;
    const static inline int PIPELINE_EVENT_ID_AAA = 901000000;
    const static inline int PIPELINE_EVENT_ID_BBB = 901000001;
    const static inline int PIPELINE_EVENT_ID_CCC = 901000002;
    const static inline int PIPELINE_EVENT_ID_TAA = 901000010;

private:
    void CreateWatchFile(const std::string& path);
    void CreateAndPublishEvent(const std::string& file);
    const static inline std::string SYSTEM_FAULT_LOG_PATH = "/data/test/faultlog2";
    int inotifyFd_;
    std::map<std::string, int> fileMap_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_PLUGIN_EXAMPLES_EVENT_SOURCE_EXAMPLE