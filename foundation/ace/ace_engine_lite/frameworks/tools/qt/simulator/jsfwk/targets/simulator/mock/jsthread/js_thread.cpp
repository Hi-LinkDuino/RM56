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

#include "js_thread.h"
#include "js_async_work.h"
#include "q_auto_lock_guard.h"
#include "string_util.h"
#include "vsync_dispatch_manager.h"

namespace OHOS {
namespace ACELite {
JSThread::~JSThread()
{
    ResetAbilityInfo();
}

void JSThread::run()
{
    HandleEventLoop();
}

void JSThread::ResetAbilityInfo()
{
    if (abilityPath_ != nullptr) {
        ace_free(abilityPath_);
        abilityPath_ = nullptr;
    }
    if (abilityBundleName_ != nullptr) {
        ace_free(abilityBundleName_);
        abilityBundleName_ = nullptr;
    }
}

void JSThread::ConfigAbilityInfo(const char *path, const char *bundleName, uint16_t token)
{
    ResetAbilityInfo();
    abilityPath_ = StringUtil::Copy(path);
    abilityBundleName_ = StringUtil::Copy(bundleName);
    abilityToken_ = token;
}

void JSThread::ProcessOneRenderTick()
{
    jsAbility_.HandleRenderTick();
}

void JSThread::PostCommand(AbilityMsgId command)
{
    AbilityInnerMsg *msg = new AbilityInnerMsg();
    if (msg == nullptr) {
        return;
    }
    msg->msgId = command;
    PostMessage(msg);
}

int JSThread::PostMessage(const AbilityInnerMsg *msg)
{
    if (msg == nullptr) {
        return -1;
    }

    QAutoLockGuard lockGuard(mutexlock_);
    msgQueue_.enqueue(msg);
    // notify there is new message
    queueCondition_.wakeAll();
    return 0;
}

const AbilityInnerMsg *JSThread::GetMessage()
{
    QAutoLockGuard lockGuard(mutexlock_);
    if (msgQueue_.isEmpty()) {
        // wait until any new message is putted into
        queueCondition_.wait(&mutexlock_);
    }
    const AbilityInnerMsg *msg = msgQueue_.dequeue();
    return msg;
}

void JSThread::HandleEventLoop()
{
    while (true) {
        const AbilityInnerMsg *innerMsg = GetMessage();
        if (innerMsg == nullptr) {
            // error case
            return;
        }
        // the innerMsg will be freed out of the scop
        std::unique_ptr<const AbilityInnerMsg> msgGuard(innerMsg);
        switch ((uint32_t)innerMsg->msgId) {
            case AbilityMsgId::START: {
                actived_ = false;
                JsAsyncWork::SetAppQueueHandler(this);
                VsyncDispatchManager::GetInstance().RegisterVsyncReceiver(this);
                jsAbility_.Launch(abilityPath_, abilityBundleName_, abilityToken_);
                break;
            }
            case AbilityMsgId::ACTIVE: {
                jsAbility_.Show();
                actived_ = true;
                break;
            }
            case AbilityMsgId::BACKGROUND: {
                jsAbility_.Hide();
                actived_ = false;
                break;
            }
            case AbilityMsgId::DESTORY: {
                VsyncDispatchManager::GetInstance().UnregisterVsyncReceiver(this);
                // cleanup the message queue id to present any new async message
                JsAsyncWork::SetAppQueueHandler(nullptr);
                jsAbility_.TransferToDestroy();
                return; // here exit the loop, and abort all messages afterwards
            }
            case AbilityMsgId::ASYNCWORK: {
                AsyncWork *work = reinterpret_cast<AsyncWork *>(innerMsg->data);
                JsAsyncWork::ExecuteAsyncWork(work);
                break;
            }
            case AbilityMsgId::TE_EVENT: {
                // vsync arrived, process
                ProcessOneRenderTick();
                jsHeapStatsDumper_.Dump();
                break;
            }
            default:
                break;
        }
    }
}

TEDispatchingResult JSThread::TryToDispatchTEEvent()
{
    if (actived_) {
        PostCommand(AbilityMsgId::TE_EVENT);
        return TEDispatchingResult::ACCEPTED;
    }
    return TEDispatchingResult::REFUSED;
}
} // namespace ACELite
} // namespace OHOS
