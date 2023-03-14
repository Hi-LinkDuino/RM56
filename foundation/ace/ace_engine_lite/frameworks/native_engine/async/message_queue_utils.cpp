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

#include "message_queue_utils.h"
#include "ace_log.h"
#include "acelite_config.h"
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
#include "cmsis_os2.h"
#endif

namespace OHOS {
namespace ACELite {
QueueHandler MessageQueueUtils::CreateMessageQueue(uint32_t capacity, uint32_t msgSize)
{
    if (capacity == 0 || msgSize == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:CreateMessageQueue parameters invalid!");
        return nullptr;
    }
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    HILOG_WARN(HILOG_MODULE_ACE, "call linux createMessageQueue interface here!");
    return nullptr;
#else
    osMessageQueueId_t queueId = osMessageQueueNew(capacity, msgSize, nullptr);
    return static_cast<QueueHandler>(queueId);
#endif
}

int8_t MessageQueueUtils::DeleteMessageQueue(QueueHandler handler)
{
    if (handler == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:DeleteMessageQueue parameters invalid!");
        return MSGQ_FAIL;
    }
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    HILOG_WARN(HILOG_MODULE_ACE, "call linux deleteMessageQueue interface here!");
    return MSGQ_FAIL;
#else
    osMessageQueueId_t queueId = static_cast<osMessageQueueId_t>(handler);
    if (osMessageQueueDelete(queueId) != osOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:DeleteMessageQueue failed!");
        return MSGQ_FAIL;
    }
    return MSGQ_OK;
#endif
}

int8_t MessageQueueUtils::PutMessage(QueueHandler handler, const void* msgPtr, uint32_t timeOut)
{
    if ((handler == nullptr) || (msgPtr == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:PutMessage parameters invalid!");
        return MSGQ_FAIL;
    }
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    HILOG_WARN(HILOG_MODULE_ACE, "call linux putmsg interface here!");
    return MSGQ_FAIL;
#else
    osMessageQueueId_t queueId = static_cast<osMessageQueueId_t>(handler);
    if (osMessageQueuePut(queueId, msgPtr, 0, timeOut) != osOK) {
        uint32_t msgCount = osMessageQueueGetCount(queueId);
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:PutMessage failed! msg count[%{public}u]", msgCount);
        return MSGQ_FAIL;
    }
    return MSGQ_OK;
#endif
}

int8_t MessageQueueUtils::GetMessage(QueueHandler handler, void* msgPtr, uint32_t timeOut)
{
    if ((handler == nullptr) || (msgPtr == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:GetMessage parameters invalid!");
        return MSGQ_FAIL;
    }
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    HILOG_WARN(HILOG_MODULE_ACE, "call linux getmsg interface here!");
    return MSGQ_FAIL;
#else
    osMessageQueueId_t queueId = static_cast<osMessageQueueId_t>(handler);
    if (osMessageQueueGet(queueId, msgPtr, 0, timeOut) != osOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MessageQueueUtils:GetMessage failed!");
        return MSGQ_FAIL;
    }
    return MSGQ_OK;
#endif
}
} // namespace ACELite
} // namespace OHOS
