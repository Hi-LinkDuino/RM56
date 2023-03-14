/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "handler.h"
#include <iostream>
#include <sys/time.h>
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_HANDLER"

namespace OHOS {
namespace Wifi {
Handler::Handler() : pMyQueue(nullptr), handleThread(0), isRunning(true)
{}

Handler::~Handler()
{
    LOGI("Handler::~Handler");
    StopHandlerThread();
    return;
}

bool Handler::InitialHandler()
{
    if (handleThread != 0) {
        return true;
    }
    if (pMyQueue == nullptr) {
        pMyQueue = std::make_unique<MessageQueue>();
        if (pMyQueue == nullptr) {
            LOGE("pMyQueue alloc failed.\n");
            return false;
        }
    }

    int ret = pthread_create(&handleThread, nullptr, RunHandleThreadFunc, this);
    if (ret < 0) {
        LOGE("pthread_create failed.\n");
        return false;
    }

    return true;
}

void Handler::StopHandlerThread()
{
    LOGI("Handler::StopHandlerThread");
    if (isRunning) {
        isRunning = false;
        if (pMyQueue != nullptr) {
            pMyQueue->StopQueueLoop();
        }

        if (handleThread != 0) {
            pthread_join(handleThread, nullptr);
        }
    }

    return;
}

void *Handler::RunHandleThreadFunc(void *pInstance)
{
    if (pInstance == nullptr) {
        LOGE("pInstance is null.\n");
        return nullptr;
    }

    Handler *pHandler = (Handler *)pInstance;
    pHandler->GetAndDistributeMessage();

    return nullptr;
}

void Handler::GetAndDistributeMessage()
{
    if (pMyQueue == nullptr) {
        LOGE("pMyQueue is null.\n");
        return;
    }

    while (isRunning) {
        InternalMessage *msg = pMyQueue->GetNextMessage();
        if (msg == nullptr) {
            LOGE("GetNextMessage failed.\n");
            continue;
        }

        DistributeMessage(msg);
        MessageManage::GetInstance().ReclaimMsg(msg);
    }

    return;
}

void Handler::SendMessage(InternalMessage *msg)
{
    if (msg == nullptr) {
        LOGE("Handler::SendMessage: msg is null.");
        return;
    }

    LOGD("Handler::SendMessage msg:%{public}d", msg->GetMessageName());
    MessageExecutedLater(msg, 0);
    return;
}

void Handler::MessageExecutedLater(InternalMessage *msg, int64_t delayTimeMs)
{
    if (msg == nullptr) {
        LOGE("Handler::MessageExecutedLater: msg is null.");
        return;
    }

    LOGD("Handler::MessageExecutedLater msg:%{public}d", msg->GetMessageName());
    int64_t delayTime = delayTimeMs;
    if (delayTime < 0) {
        delayTime = 0;
    }

    /* Obtains the current time, accurate to milliseconds. */
    struct timeval curTime = {0, 0};
    if (gettimeofday(&curTime, nullptr) != 0) {
        LOGE("gettimeofday failed.\n");
        MessageManage::GetInstance().ReclaimMsg(msg);
        return;
    }
    int64_t nowTime = static_cast<int64_t>(curTime.tv_sec) * USEC_1000 + curTime.tv_usec / USEC_1000;

    MessageExecutedAtTime(msg, nowTime + delayTime);
    return;
}

void Handler::MessageExecutedAtTime(InternalMessage *msg, int64_t execTime)
{
    if (msg == nullptr) {
        LOGE("Handler::MessageExecutedAtTime: msg is null.");
        return;
    }

    LOGD("Handler::MessageExecutedAtTime msg: %{public}d", msg->GetMessageName());
    if (pMyQueue == nullptr) {
        LOGE("pMyQueue is null.\n");
        MessageManage::GetInstance().ReclaimMsg(msg);
        return;
    }

    if (pMyQueue->AddMessageToQueue(msg, execTime) != true) {
        LOGE("AddMessageToQueue failed.\n");
        return;
    }

    return;
}

void Handler::PlaceMessageTopOfQueue(InternalMessage *msg)
{
    if (msg == nullptr) {
        LOGE("Handler::PlaceMessageTopOfQueue: msg is null.");
        return;
    }

    LOGD("Handler::PlaceMessageTopOfQueue msg: %{public}d", msg->GetMessageName());
    if (pMyQueue == nullptr) {
        LOGE("pMyQueue is null.\n");
        MessageManage::GetInstance().ReclaimMsg(msg);
        return;
    }

    if (!pMyQueue->AddMessageToQueue(msg, 0)) {
        LOGE("AddMessageToQueue failed.\n");
        return;
    }

    return;
}

void Handler::DeleteMessageFromQueue(int messageName)
{
    LOGD("Handler::DeleteMessageFromQueue msg is: %{public}d", messageName);
    if (pMyQueue == nullptr) {
        LOGE("pMyQueue is null.\n");
        return;
    }

    if (!pMyQueue->DeleteMessageFromQueue(messageName)) {
        LOGE("DeleteMessageFromQueue failed.\n");
        return;
    }

    return;
}

void Handler::DistributeMessage(InternalMessage *msg)
{
    if (msg == nullptr) {
        return;
    }
    ExecuteMessage(msg);
    return;
}
}  // namespace Wifi
}  // namespace OHOS