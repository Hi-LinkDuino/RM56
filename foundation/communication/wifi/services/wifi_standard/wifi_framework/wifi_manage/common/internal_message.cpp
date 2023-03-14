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
#include "internal_message.h"
#include "securec.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_INTERNAL_MESSAGE"

namespace OHOS {
namespace Wifi {
void MessageBody::SaveIntData(int data)
{
    intArray_.push_back(data);
    return;
}

void MessageBody::SaveStringData(std::string data)
{
    stringArray_.push_back(data);
    return;
}

int MessageBody::GetIntData()
{
    if (intArray_.empty()) {
        LOGE("intArray is null.");
        return 0;
    }

    int tmp = intArray_.front();
    intArray_.pop_front();
    return tmp;
}

std::string MessageBody::GetStringData()
{
    std::string tmp;
    if (stringArray_.empty()) {
        LOGE("stringArray is null.");
        return tmp;
    }

    tmp = stringArray_.front();
    stringArray_.pop_front();
    return tmp;
}

void MessageBody::ClearAllData()
{
    intArray_.clear();
    stringArray_.clear();
    return;
}

void MessageBody::CopyMessageBody(const MessageBody &origBody)
{
    intArray_.assign(origBody.intArray_.begin(), origBody.intArray_.end());
    stringArray_.assign(origBody.stringArray_.begin(), origBody.stringArray_.end());

    return;
}

InternalMessage::InternalMessage()
    : mMsgName(0),
      mParam1(0),
      mParam2(0),
      pNextMsg(nullptr),
      mHandleTime(0)
{}

InternalMessage::~InternalMessage()
{
}

int InternalMessage::GetMessageName() const
{
    return mMsgName;
}

int InternalMessage::GetParam1() const
{
    return mParam1;
}

int InternalMessage::GetParam2() const
{
    return mParam2;
}

int InternalMessage::GetIntFromMessage()
{
    return mMessageBody.GetIntData();
}

std::string InternalMessage::GetStringFromMessage()
{
    return mMessageBody.GetStringData();
}

const MessageBody &InternalMessage::GetMessageBody() const
{
    return mMessageBody;
}

void InternalMessage::CopyMessageBody(const MessageBody &origBody)
{
    mMessageBody.CopyMessageBody(origBody);
    return;
}

InternalMessage *InternalMessage::GetNextMsg() const
{
    return pNextMsg;
}

int64_t InternalMessage::GetHandleTime() const
{
    return mHandleTime;
}

void InternalMessage::SetMessageName(int msgName)
{
    mMsgName = msgName;
    return;
}

void InternalMessage::SetParam1(int param1)
{
    mParam1 = param1;
    return;
}

void InternalMessage::SetParam2(int param2)
{
    mParam2 = param2;
    return;
}

void InternalMessage::ReleaseMessageObj()
{
    mMessageObj.reset();
    return;
}

void InternalMessage::AddIntMessageBody(int data)
{
    mMessageBody.SaveIntData(data);
    return;
}

void InternalMessage::AddStringMessageBody(std::string data)
{
    mMessageBody.SaveStringData(data);
    return;
}

void InternalMessage::ClearMessageBody()
{
    mMessageBody.ClearAllData();
    return;
}

void InternalMessage::SetNextMsg(InternalMessage *nextMsg)
{
    pNextMsg = nextMsg;
    return;
}

void InternalMessage::SetHandleTime(int64_t time)
{
    mHandleTime = time;
    return;
}

std::unique_ptr<MessageManage> MessageManage::msgManage;

MessageManage &MessageManage::GetInstance()
{
    if (msgManage.get() == nullptr) {
        msgManage = std::make_unique<MessageManage>();
    }
    return *msgManage;
}

MessageManage::MessageManage() : pMsgPool(nullptr), mMsgPoolSize(0)
{}

MessageManage::~MessageManage()
{
    ReleasePool();
    return;
}

InternalMessage *MessageManage::CreateMessage()
{
    {
        std::unique_lock<std::mutex> lock(mPoolMutex);
        if (pMsgPool != nullptr) {
            InternalMessage *m = pMsgPool;
            pMsgPool = m->GetNextMsg();
            m->SetNextMsg(nullptr);
            mMsgPoolSize--;
            return m;
        }
    }

    auto pMessage = new (std::nothrow) InternalMessage();
    return pMessage;
}

InternalMessage *MessageManage::CreateMessage(const InternalMessage *orig)
{
    InternalMessage *m = CreateMessage();
    if (m == nullptr) {
        return nullptr;
    }

    m->SetMessageName(orig->GetMessageName());
    m->SetParam1(orig->GetParam1());
    m->SetParam2(orig->GetParam2());
    m->SetMessageObj(orig->GetMessageObj());
    m->CopyMessageBody(orig->GetMessageBody());

    return m;
}

InternalMessage *MessageManage::CreateMessage(int messageName)
{
    InternalMessage *m = CreateMessage();
    if (m == nullptr) {
        return nullptr;
    }

    m->SetMessageName(messageName);
    return m;
}

InternalMessage *MessageManage::CreateMessage(int messageName, const std::any &messageObj)
{
    InternalMessage *m = CreateMessage();
    if (m == nullptr) {
        return nullptr;
    }

    m->SetMessageName(messageName);

    m->SetMessageObj(messageObj);
    return m;
}

InternalMessage *MessageManage::CreateMessage(int messageName, int param1, int param2)
{
    InternalMessage *m = CreateMessage();
    if (m == nullptr) {
        return nullptr;
    }

    m->SetMessageName(messageName);
    m->SetParam1(param1);
    m->SetParam2(param2);
    return m;
}

InternalMessage *MessageManage::CreateMessage(int messageName, int param1, int param2, const std::any &messageObj)
{
    InternalMessage *m = CreateMessage();
    if (m == nullptr) {
        return nullptr;
    }

    m->SetMessageName(messageName);
    m->SetParam1(param1);
    m->SetParam2(param2);
    m->SetMessageObj(messageObj);
    return m;
}

void MessageManage::ReclaimMsg(InternalMessage *m)
{
    if (m == nullptr) {
        return;
    }

    m->SetMessageName(0);
    m->SetParam1(0);
    m->SetParam2(0);
    m->ReleaseMessageObj();
    m->ClearMessageBody();

    {
        std::unique_lock<std::mutex> lock(mPoolMutex);
        if (mMsgPoolSize < MAX_MSG_NUM_IN_POOL) {
            m->SetNextMsg(pMsgPool);
            pMsgPool = m;
            mMsgPoolSize++;
            return;
        }
    }

    delete m;
    m = nullptr;
    return;
}

void MessageManage::ReleasePool()
{
    std::unique_lock<std::mutex> lock(mPoolMutex);
    InternalMessage *current = pMsgPool;
    InternalMessage *next = nullptr;
    while (current != nullptr) {
        next = current->GetNextMsg();
        delete current;
        current = next;
    }

    return;
}
}  // namespace Wifi
}  // namespace OHOS