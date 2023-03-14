/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "js_app_host.h"

#include "ability_manager_inner.h"
#include "ability_message_id.h"
#include "ability_state.h"
#include "adapter.h"

using namespace OHOS::ACELite;

namespace OHOS {
JsAppHost::JsAppHost()
{
}

JsAppHost::~JsAppHost()
{
    if (jsAbility_ != nullptr) {
        delete jsAbility_;
    }
}

void JsAppHost::JsAppTaskHandler(uint32_t uwArg)
{
    auto jsappHost = reinterpret_cast<JsAppHost *>((uintptr_t)uwArg);
    if (jsappHost == nullptr) {
        return;
    }

    osMessageQueueId_t appQueueId = jsappHost->GetMessageQueueId();
    if (appQueueId == nullptr) {
        return;
    }

    for (;;) {
        AbilityInnerMsg innerMsg;
        uint8_t prio = 0;
        osStatus_t ret = osMessageQueueGet(appQueueId, &innerMsg, &prio, osWaitForever);
        if (ret != osOK) {
            return;
        }
        LP_TaskBegin();
        switch ((uint32_t)innerMsg.msgId) {
            case ACTIVE:
                jsappHost->OnActive(innerMsg.token, innerMsg.bundleName, innerMsg.path);
                break;
            case BACKGROUND:
                jsappHost->OnBackground(innerMsg.token);
                break;
            case DESTORY:
                // cleanup the message queue id to present any new async message
                JsAsyncWork::SetAppQueueHandler(nullptr);
                jsappHost->OnDestroy(innerMsg.token);
                LP_TaskEnd();
                return; // here exit the loop, and abort all messages afterwards
            case BACKPRESSED:
                jsappHost->BackPressed();
                break;
            case ASYNCWORK: {
                AsyncWork* work = reinterpret_cast<AsyncWork *>(innerMsg.data);
                JsAsyncWork::ExecuteAsyncWork(work);
                break;
            }
            case TE_EVENT:
                jsappHost->HandleTickEvent();
                break;
            default:
                break;
        }
        LP_TaskEnd();
    }
}

void JsAppHost::OnActive(uint16_t token, const char *bundleName, const char *path)
{
    if (jsAbility_ == nullptr) {
        jsAbility_ = new ACELite::JSAbility();
        if (jsAbility_ == nullptr) {
            return;
        }
        jsAbility_->Launch(const_cast<char *>(path), bundleName, token);
    }
    jsAbility_->Show();
    isBackground_ = false;
    SendMsgToAbilityService(token, STATE_ACTIVE);
}

void JsAppHost::OnBackground(uint16_t token)
{
    if (jsAbility_ != nullptr) {
        isBackground_ = true;
        jsAbility_->Hide();
        SendMsgToAbilityService(token, STATE_BACKGROUND);
    }
}

void JsAppHost::OnDestroy(uint16_t token)
{
    if (jsAbility_ != nullptr) {
        // the TE owner will be JS application after JS application start up except for it's lying in background,
        // call render once to make sure the last TE message is processed properly
        if (!isBackground_) {
            jsAbility_->HandleRenderTick();
        }
        jsAbility_->TransferToDestroy();
    }
    SendMsgToAbilityService(token, STATE_UNINITIALIZED);
}

void JsAppHost::BackPressed()
{
    if (jsAbility_ != nullptr) {
        jsAbility_->BackPressed();
    }
}

void JsAppHost::ForceDestroy() const
{
    if (jsAbility_ != nullptr) {
        jsAbility_->ForceDestroy();
    }
}

void JsAppHost::HandleTickEvent()
{
    if (jsAbility_ != nullptr) {
        jsAbility_->HandleRenderTick();
    }
}

void JsAppHost::SendMsgToAbilityService(uint16_t token, int32_t id)
{
    SchedulerLifecycleDone(token, id);
}
} // namespace OHOS
