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

#ifndef OHOS_INTERNAL_MESSAGE_H
#define OHOS_INTERNAL_MESSAGE_H

#include <cstring>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <any>

namespace OHOS {
namespace Wifi {
const int MAX_POOL_SIZE_INIT = 50;
class MessageBody {
public:
    /**
     * @Description : Save an Integer Data.
     *
     * @param data - Integer Data.[in]
     */
    void SaveIntData(int data);

    /**
     * @Description : Save a String Data.
     *
     * @param data - String Data.[in]
     */
    void SaveStringData(std::string data);

    /**
     * @Description : Get an Integer Data.
     *
     * @return int
     */
    int GetIntData();

    /**
     * @Description : Get a String Data.
     *
     * @return std::string
     */
    std::string GetStringData();

    /**
     * @Description : Clear all Data.
     *
     */
    void ClearAllData();

    /**
     * @Description : Copy a message body.
     *
     * @param origBody - Source Message Body.[in]
     */
    void CopyMessageBody(const MessageBody &origBody);

private:
    /* Integer data. */
    std::list<int> intArray_;
    /* String data. */
    std::list<std::string> stringArray_;
};

class InternalMessage {
public:
    /**
     * @Description : Construct a new Internal Message object.
     *
     */
    InternalMessage();

    /**
     * @Description Destroy the Internal Message object.
     *
     */
    ~InternalMessage();

    /**
     * @Description : Get message name.
     *
     * @return int
     */
    int GetMessageName() const;

    /**
     * @Description : Obtains the first parameter in the message body.
     *
     * @return int
     */
    int GetParam1() const;

    /**
     * @Description : Obtains the second parameter in the message body.
     *
     * @return int
     */
    int GetParam2() const;

    /**
     * @Description : Obtains Integer data from message.
     *
     * @return int
     */
    int GetIntFromMessage();

    /**
     * @Description : Obtains Sting data from message.
     *
     * @return std::string
     */
    std::string GetStringFromMessage();

    /**
     * @Description : Obtains message body.
     *
     * @return MessageBody&
     */
    const MessageBody &GetMessageBody() const;

    /**
     * @Description : Copy message body.
     *
     * @param origBody - Source Message Body.[in]
     */
    void CopyMessageBody(const MessageBody &origBody);

    /**
     * @Description : Get next message.
     *
     * @return InternalMessage*
     */
    InternalMessage *GetNextMsg() const;

    /**
     * @Description : Obtains time.
     *
     * @return int64_t
     */
    int64_t GetHandleTime() const;

    /**
     * @Description : Set message name.
     *
     * @param msgName - Message name.[in]
     */
    void SetMessageName(int msgName);

    /**
     * @Description : Set the first parameter in the message body.
     *
     * @param param1 - The first parameter.[in]
     */
    void SetParam1(int param1);

    /**
     * @Description : Set the second parameter in the message body.
     *
     * @param param2 - The second parameter.[in]
     */
    void SetParam2(int param2);

    /**
     * @DescriptionSet the Message Obj object - brief
     * @tparam  - T Custom type to be set
     * @param  messageObj - User-defined data to be set
     */
    template<typename T>
    void SetMessageObj(const T &messageObj)
    {
        mMessageObj = messageObj;
    }

    /**
     * @DescriptionSet the Message Obj object - brief
     * @tparam  - T Custom type to be set
     * @param  messageObj - User-defined data to be set
     */
    template<typename T>
    void SetMessageObj(T &&messageObj)
    {
        mMessageObj = T(messageObj);
    }

    void SetMessageObj(const std::any &messageObj)
    {
        mMessageObj = messageObj;
    }

    /**
     * @DescriptionGet the Message Obj object
     * @tparam  - T Custom type to be set
     * @param  messageObj - Gets data of an actual specific object.
     * @return - bool true:success   false:failed  
     */
    template<typename T>
    bool GetMessageObj(T &messageObj) const
    {
        messageObj = std::any_cast<const T &>(mMessageObj);
        return true;
    }

    const std::any &GetMessageObj(void) const
    {
        return mMessageObj;
    }

    /**
     * @Description : Release Message Object.
     *
     */
    void ReleaseMessageObj();

    /**
     * @Description : Add integer message body.
     *
     * @param data - Integer data.[in]
     */
    void AddIntMessageBody(int data);

    /**
     * @Description : Add string message body.
     *
     * @param data - String data.[in]
     */
    void AddStringMessageBody(std::string data);

    /**
     * @Description : Clear message body.
     *
     */
    void ClearMessageBody();

    /**
     * @Description : Sets next message.
     *
     * @param next - The next message.[in]
     */
    void SetNextMsg(InternalMessage *nextMsg);

    /**
     * @Description : Set the time.
     *
     * @param time - Time.[in]
     */
    void SetHandleTime(int64_t time);

private:
    /* Message Name */
    int mMsgName;
    /* Parameter 1 */
    int mParam1;
    /* Parameter 2 */
    int mParam2;
    /* any message obj. */
    std::any mMessageObj;
    /* Message bodies that cannot be directly copied */
    MessageBody mMessageBody;
    /* Next message in the resource pool or message queue */
    InternalMessage *pNextMsg;
    /* Message execution time */
    int64_t mHandleTime;
};
class MessageManage {
public:
    /**
     * @Description : Obtains a single instance.
     *
     * @return MessageManage&
     */
    static MessageManage &GetInstance();

    /**
     * @Description : Message obtaining function.
     *
     * @return InternalMessage*
     */
    InternalMessage *CreateMessage();

    /**
     * @Description : Obtain original messages.
     *
     * @param orig - Original messages.[in]
     * @return InternalMessage*
     */
    InternalMessage *CreateMessage(const InternalMessage *orig);

    /**
     * @Description : Obtains the message name.
     *
     * @param messageName - Message name.[in]
     * @return InternalMessage*
     */
    InternalMessage *CreateMessage(int messageName);

    /**
     * @Description :Obtaining Message Information.
     *
     * @param messageName - Message name.[in]
     * @param messageObj - Message pointer.[in]
     * @return InternalMessage*
     */
    InternalMessage *CreateMessage(int messageName, const std::any &messageObj);

    /**
     * @Description : Obtaining Message Information.
     *
     * @param messageName - Message name.[in]
     * @param param1 - param1.[in]
     * @param param2 - param2.[in]
     * @return InternalMessage*
     */
    InternalMessage *CreateMessage(int messageName, int param1, int param2);

    /**
     * @Description : Obtaining Message Information.
     *
     * @param messageName - Message name.[in]
     * @param param1 - param1.[in]
     * @param param2 - param2.[in]
     * @param messageObj - Message pointer.[in]
     * @return InternalMessage*
     */
    InternalMessage *CreateMessage(int messageName, int param1, int param2, const std::any &messageObj);

    /**
     * @Description :Recycle message.
     *
     * @param m - message.[in]
     */
    void ReclaimMsg(InternalMessage *m);

    /**
     * @Description : Release pool.
     *
     */

    void ReleasePool();

    /**
     * @Description : Construct a new Message Manage object.
     *
     */
    MessageManage();

    /**
     * @Description : Destroy the Message Manage object.
     *
     */
    ~MessageManage();

private:
    /* Maximum number of messages in the message resource pool */
    const int MAX_MSG_NUM_IN_POOL = 50;
    /* Message resource pool */
    InternalMessage *pMsgPool;
    /* Number of messages in the message resource pool */
    int mMsgPoolSize;
    /* Mutex for operating the message resource pool */
    std::mutex mPoolMutex;
    static std::unique_ptr<MessageManage> msgManage;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
