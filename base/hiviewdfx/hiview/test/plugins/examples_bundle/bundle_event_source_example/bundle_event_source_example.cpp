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
#include "bundle_event_source_example.h"

#include <fstream>
#include <iostream>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/inotify.h>

#include "event_loop.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(BundleEventSourceExample);
std::set<std::string> BundleEventSourceExample::count = std::set<std::string>();
BundleEventSourceExample::BundleEventSourceExample() : inotifyFd_(0)
{
    printf("BundleEventSourceExample::BundleEventSourceExample()\n");
    count.insert("BundleEventSourceExample");
}

BundleEventSourceExample::~BundleEventSourceExample()
{
    printf("BundleEventSourceExample::~BundleEventSourceExample()\n");
    count.erase("BundleEventSourceExample");
}

void BundleEventSourceExample::OnLoad()
{
    printf("BundleEventSourceExample::OnLoad.\n");

    int isCreate = ::mkdir(SYSTEM_FAULT_LOG_PATH.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if (!isCreate) {
        printf("create path:%s \n", SYSTEM_FAULT_LOG_PATH.c_str());
    }
    CreateWatchFile(SYSTEM_FAULT_LOG_PATH + "/testbb");
    CreateWatchFile(SYSTEM_FAULT_LOG_PATH + "/testcc");
}

void BundleEventSourceExample::CreateWatchFile(const std::string& path)
{
    std::ofstream file(path);
    if (!file.good()) {
        printf("Fail to create watch file:%s.\n", path.c_str());
        return;
    }
    file << "" << std::endl;
    file.close();
}

void BundleEventSourceExample::OnUnload()
{
    printf("BundleEventSourceExample::OnUnload.\n");
}

void BundleEventSourceExample::StartEventSource()
{
    printf("BundleEventSourceExample::StartEventSource.\n");
    GetWorkLoop()->AddFileDescriptorEventCallback("BundleEventFd",
        std::static_pointer_cast<BundleEventSourceExample>(shared_from_this()));
}

bool BundleEventSourceExample::OnFileDescriptorEvent(int fd, int type)
{
    printf("BundleEventSourceExample::OnEvent fd:%d, type:%d, inotifyFd_:%d.\n", fd, type, inotifyFd_);
    const int bufSize = 2048;
    char buffer[bufSize] = {0};
    char *offset = nullptr;
    struct inotify_event *event = nullptr;
    if (inotifyFd_ < 0) {
        printf("BundleEventSourceExample Invalid inotify fd:%d", inotifyFd_);
        return false;
    }

    int len = read(inotifyFd_, buffer, bufSize);
    if (len < 0) {
        printf("BundleEventSourceExample failed to read event");
        return false;
    }

    offset = buffer;
    event = (struct inotify_event *)buffer;
    while ((reinterpret_cast<char *>(event) - buffer) < len) {
        for (const auto &it : fileMap_) {
            if (it.second != event->wd) {
                continue;
            }

            if (event->name[event->len - 1] != '\0') {
                event->name[event->len - 1] = '\0';
            }
            std::string filePath = it.first + "/" + std::string(event->name);
            std::ifstream fileS(filePath);
            if (!fileS) {
                continue;
            }
            std::string fileStr;
            fileS >> fileStr;
            if (fileStr.empty()) {
                printf("fileStr.empty()\n");
                continue;
            }
            fileS.close();
            printf("handle file event in %s \n", filePath.c_str());
            CreateAndPublishEvent(filePath);
        }
        int tmpLen = sizeof(struct inotify_event) + event->len;
        event = (struct inotify_event *)(offset + tmpLen);
        offset += tmpLen;
    }
    return true;
}

int32_t BundleEventSourceExample::GetPollFd()
{
    printf("BundleEventSourceExample::GetPollFd.\n");
    if (inotifyFd_ > 0) {
        return inotifyFd_;
    }

    inotifyFd_ = inotify_init();
    if (inotifyFd_ == -1) {
        printf("failed to init inotify: %s.\n", strerror(errno));
        return -1;
    }

    int wd = inotify_add_watch(inotifyFd_, SYSTEM_FAULT_LOG_PATH.c_str(), IN_CLOSE_WRITE | IN_MOVED_TO);
    if (wd < 0) {
        printf("failed to add watch entry : %s(%s).\n", strerror(errno), SYSTEM_FAULT_LOG_PATH.c_str());
        close(inotifyFd_);
        inotifyFd_ = -1;
        return -1;
    }

    printf("GetPollFd %d \n", inotifyFd_);
    fileMap_[SYSTEM_FAULT_LOG_PATH] = wd;
    return inotifyFd_;
}

int32_t BundleEventSourceExample::GetPollType()
{
    printf("BundleEventSourceExample::GetPollType.\n");
    return EPOLLIN;
}

void BundleEventSourceExample::CreateAndPublishEvent(const std::string &file)
{
    // create a pipeline event
    auto event = std::make_shared<BundleEventSourceExampleEvent>(file, static_cast<PipelineEventProducer *>(this));

    event->isPipeline_ = true;
    // add special information
    event->messageType_ = Event::MessageType::FAULT_EVENT;
    if (file == (SYSTEM_FAULT_LOG_PATH + "/testbb"))  {
        event->eventName_ = "testbbbb";
    }  else if (file == (SYSTEM_FAULT_LOG_PATH + "/testcc"))  {
        event->eventName_ = "testcccc";
    } else {
        return;
    }
    PublishPipelineEvent(std::dynamic_pointer_cast<PipelineEvent>(event));
}

void BundleEventSourceExample::Recycle(PipelineEvent *event)
{
    printf("BundleEventSourceExample::Recycle.\n");
    auto eventPtr = static_cast<BundleEventSourceExampleEvent *>(event);
    if (eventPtr == nullptr || eventPtr->data_ == nullptr) {
        return;
    }
    free(eventPtr->data_);
    eventPtr->data_ = nullptr;
    printf("Recycle event:%s.\n", eventPtr->addon_.c_str());
}

void BundleEventSourceExample::PauseDispatch(std::weak_ptr<Plugin> plugin)
{
    auto requester = plugin.lock();
    if (requester != nullptr) {
        printf("process pause dispatch event from plugin:%s.\n", requester->GetName().c_str());
    }
}
} // namespace HiviewDFX
} // namespace OHOS
