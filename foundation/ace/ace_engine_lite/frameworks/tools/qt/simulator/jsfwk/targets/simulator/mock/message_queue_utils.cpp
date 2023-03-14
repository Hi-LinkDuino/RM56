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

#include "message_queue_utils.h"

#include "ace_log.h"
#include "acelite_config.h"
#include "js_thread.h"
#include "securec.h"

// This is the implementation base on QThread handling on Simulator for mocking the message queue preocess.
namespace OHOS {
namespace ACELite {
QueueHandler MessageQueueUtils::CreateMessageQueue(uint32_t capacity, uint32_t msgSize)
{
    (void)(capacity);
    (void)(msgSize);
    HILOG_WARN(HILOG_MODULE_ACE, "is not supported!");
    return nullptr;
}

int8_t MessageQueueUtils::DeleteMessageQueue(QueueHandler handler)
{
    (void)(handler);
    HILOG_WARN(HILOG_MODULE_ACE, "is not supported!");
    return MSGQ_FAIL;
}

int8_t MessageQueueUtils::PutMessage(QueueHandler handler, const void *msgPtr, uint32_t timeOut)
{
    (void)(timeOut);
    if ((handler == nullptr) || (msgPtr == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:PutMessage parameters invalid!");
        return MSGQ_FAIL;
    }

    AbilityInnerMsg *message = new AbilityInnerMsg();
    if (memcpy_s(message, sizeof(AbilityInnerMsg), msgPtr, sizeof(AbilityInnerMsg)) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:PutMessage copy message body failed!");
        delete message;
        message = nullptr;
        return MSGQ_FAIL;
    }
    JSThread *eventLoopOwner = static_cast<JSThread *>(handler);
    eventLoopOwner->PostMessage(message);
    return MSGQ_OK;
}

int8_t MessageQueueUtils::GetMessage(QueueHandler handler, void *msgPtr, uint32_t timeOut)
{
    (void)(handler);
    (void)(msgPtr);
    (void)(timeOut);
    HILOG_WARN(HILOG_MODULE_ACE, "is not supported!");
    return MSGQ_FAIL;
}
} // namespace ACELite
} // namespace OHOS
