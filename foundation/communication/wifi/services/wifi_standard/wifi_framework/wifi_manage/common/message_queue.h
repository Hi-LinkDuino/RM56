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

#ifndef OHOS_MESSAGE_QUEUE_H
#define OHOS_MESSAGE_QUEUE_H

#include <chrono>
#include <condition_variable>
#include <mutex>
#include "internal_message.h"

namespace OHOS {
namespace Wifi {
#define TIME_USEC_1000 1000
#define TIME_INTERVAL 30000
class MessageQueue {
public:
    /**
     * @Description : Construct a new Message Queue object.
     *
     */
    MessageQueue();

    /**
     * @Description : Destroy the Message Queue object.
     *
     */
    ~MessageQueue();

    /**
     * @Description : Inserting Messages into Queues.
     *
     * @param message - Message to be inserted.[in]
     * @param handleTime - Message execution time.[in]
     * @return true : success, false : failed.
     */
    bool AddMessageToQueue(InternalMessage *message, int64_t handleTime);

    /**
     * @Description : Delete messages from the queue.
     *
     * @param messageName - Name of the message to be deleted.[in]
     * @return true : success, false : failed.
     */
    bool DeleteMessageFromQueue(int messageName);

    /**
     * @Description : Obtain messages from the queue for processing.
     * If no message is found, the system blocks the messages.
     *
     */
    InternalMessage *GetNextMessage();

    /**
     * @Description : Obtain messages from the queue for processing.
     * If no message is found, the system blocks the messages.
     */
    void StopQueueLoop();

private:
    /* Message Queuing */
    InternalMessage *pMessageQueue;
    /* No messages to be executed, blocking */
    bool mIsBlocked;
    /* Exit Loop */
    bool mNeedQuit;
    /* Thread lock of operation queue */
    std::mutex mMtxQueue;
    /* Blocked thread lock */
    std::mutex mMtxBlock;
    /* blocking condition variable */
    std::condition_variable mCvQueue;
};
}  // namespace Wifi
}  // namespace OHOS
#endif