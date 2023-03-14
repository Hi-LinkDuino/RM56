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

#include "sys_event_service.h"

#include <cstdio>
#include <memory>

#include "event.h"
#include "hiview_global.h"
#include "logger.h"
#include "plugin_factory.h"
#include "sys_event.h"
#include "sys_event_db_mgr.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(SysEventService);
DEFINE_LOG_TAG("HiView-SysEventService");
SysEventService::SysEventService() : hasLoaded_(false)
{
    sysEventDbMgr_ = std::make_unique<SysEventDbMgr>();
    sysEventStat_ = std::make_unique<SysEventStat>();
}

SysEventService::~SysEventService() {}

void SysEventService::OnLoad()
{
    HIVIEW_LOGI("sys event service load");
    auto notifyFunc = [&] (std::shared_ptr<Event> event) -> void {
        SysEventService& servicePlugin = *this;
        servicePlugin.SendEvent(event);
    };
    SysEventServiceAdapter::StartService(this, notifyFunc);
    sysEventDbMgr_->StartCheckStoreTask(this->workLoop_);
    std::string dbFile =
        HiviewGlobal::GetInstance()->GetHiViewDirectory(HiviewContext::DirectoryType::CONFIG_DIRECTORY);
    dbFile = (dbFile[dbFile.size() - 1] != '/') ? (dbFile + "/hisysevent.def") : (dbFile + "hisysevent.def");
    HIVIEW_LOGE("dbFile is %{public}s", dbFile.c_str());
    sysEventParser_ = std::make_unique<EventJsonParser>(dbFile);
    auto getTagFunc = std::bind(&EventJsonParser::GetDefinedTagByDomainEventName, *(sysEventParser_.get()),
        std::placeholders::_1, std::placeholders::_2);
    SysEventServiceAdapter::BindGetTagFunc(getTagFunc);
    hasLoaded_ = true;
}

void SysEventService::SendEvent(std::shared_ptr<Event>& event)
{
    GetHiviewContext()->PostUnorderedEvent(shared_from_this(), event);
}

void SysEventService::OnUnload()
{
    HIVIEW_LOGI("sys event service unload");
}

std::shared_ptr<SysEvent> SysEventService::Convert2SysEvent(std::shared_ptr<Event>& event)
{
    if (event == nullptr) {
        HIVIEW_LOGE("event is null");
        return nullptr;
    }
    if (event->messageType_ != Event::MessageType::SYS_EVENT) {
        HIVIEW_LOGE("receive out of sys event type");
        return nullptr;
    }
    HIVIEW_LOGI("domain is %{public}s, eventName is %{public}s.", event->domain_.c_str(), event->eventName_.c_str());
    std::shared_ptr<SysEvent> sysEvent = Event::DownCastTo<SysEvent>(event);
    if (sysEvent == nullptr) {
        HIVIEW_LOGE("sysevent is null");
    }
    return sysEvent;
}

bool SysEventService::OnEvent(std::shared_ptr<Event>& event)
{
    if (!hasLoaded_) {
        HIVIEW_LOGE("SysEventService not ready");
        return false;
    }
    std::shared_ptr<SysEvent> sysEvent = Convert2SysEvent(event);
    if (sysEvent == nullptr) {
        sysEventStat_->AccumulateEvent(false);
        return false;
    }

    if (!sysEventParser_->HandleEventJson(sysEvent)) {
        HIVIEW_LOGI("HandleEventJson fail");
        sysEventStat_->AccumulateEvent(sysEvent->domain_, sysEvent->eventName_, false);
        return false;
    } else {
        sysEventStat_->AccumulateEvent(sysEvent->domain_, sysEvent->eventName_);
    }
    SysEventServiceAdapter::OnSysEvent(sysEvent);
    sysEventDbMgr_->SaveToStore(sysEvent);
    return true;
}

static void ShowUsage(int fd, const std::vector<std::string>& cmds)
{
    dprintf(fd, "invalid cmd:");
    for (auto it = cmds.begin(); it != cmds.end(); it++) {
        dprintf(fd, "%s ", it->c_str());
    }
    dprintf(fd, "\n");
    dprintf(fd, "usage: SysEventService [sum|detail|invalid|clear]\n");
}

void SysEventService::Dump(int fd, const std::vector<std::string>& cmds)
{
    if (cmds.size() >= 2) { // args from the second item
        std::string arg1 = cmds[1];
        if (arg1 == "sum") {
            sysEventStat_->StatSummary(fd);
        } else if (arg1 == "detail") {
            sysEventStat_->StatDetail(fd);
        } else if (arg1 == "invalid") {
            sysEventStat_->StatInvaliDetail(fd);
        } else if (arg1 == "clear") {
            sysEventStat_->Clear(fd);
        } else {
            ShowUsage(fd, cmds);
        }
    } else {
        sysEventStat_->StatSummary(fd);
    }
}
} // namespace HiviewDFX
} // namespace OHOS
