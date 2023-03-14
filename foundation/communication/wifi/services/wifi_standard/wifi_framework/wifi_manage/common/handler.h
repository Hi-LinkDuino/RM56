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

#ifndef OHOS_HANDLER_H
#define OHOS_HANDLER_H

#include <pthread.h>
#include "internal_message.h"
#include "message_queue.h"

namespace OHOS {
namespace Wifi {
const int USEC_1000 = 1000;

class Handler {
public:
    /**
     * @Description : Construct a new Handler:: Handler object.
     *
     */
    Handler();

    /**
     * @Description : Destroy the Handler:: Handler object.
     *
     */
    virtual ~Handler();

    /**
     * @Description : Initialize Handler
     *
     * @return true : Initialize Handler success, false: Initialize Handler failed.
     */
    bool InitialHandler();

    /**
     * @Description : Thread processing function
     *
     * @param pInstance - Handler Instance pointer.[in]
     */
    static void *RunHandleThreadFunc(void *pInstance);

    /**
     * @Description :Stop the thread for obtaining messages.
     *
     */
    void StopHandlerThread();

    /**
     * @Description : Send a message and place the message in the message queue.
     *
     * @param msg - Message to be sent.[in]
     */
    void SendMessage(InternalMessage *msg);

    /**
     * @Description : Send a message, place the message in the message queue, and
                     process the message after delayTimeMs is delayed.
     *
     * @param msg - Message to be sent.[in]
     * @param delayTimeMs - Delay Time.[in]
     */
    void MessageExecutedLater(InternalMessage *msg, int64_t delayTimeMs);

    /**
     * @Description : Send a message, place the message in the message queue, and
                     process the message at the execTime time point.
     *
     * @param msg - Message to be sent.[in]
     * @param execTime - Time when a message is processed.[in]
     */
    void MessageExecutedAtTime(InternalMessage *msg, int64_t execTime);

    /**
     * @Description : Send a message and place the message at the top of the message queue.
     *
     * @param msg - Message to be sent.[in]
     */
    void PlaceMessageTopOfQueue(InternalMessage *msg);

    /**
     * @Description : Delete messages from the queue.
     *
     * @param messageName - Name of the message to be deleted.[in]
     */
    void DeleteMessageFromQueue(int messageName);

    /**
     * @Description : Distributing Messages.
     *
     * @param msg - Messages to be processed.[in]
     */
    void DistributeMessage(InternalMessage *msg);

    /**
     * @Description : Invoke the ExecuteStateMsg interface of the current state
                     to process messages sent to the state machine. The entry/exit
                    of the state machine is also called, and the delayed message
                    is put back into queue when transitioning to a new state.
    *
    * @param msg - Messages.[in]
    */
    virtual void ExecuteMessage(InternalMessage *msg) = 0;

    /**
     * @Description : Obtains messages from the message queue, distributes the
                         messages, and recycles the messages.
     *
     */
    void GetAndDistributeMessage();

private:
    /* message queue. */
    std::unique_ptr<MessageQueue> pMyQueue;
    /* Thread handle. */
    pthread_t handleThread;

    /* Running flag. */
    bool isRunning;
};
}  // namespace Wifi
}  // namespace OHOS
#endif