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
#include "js_async_work.h"
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
// Initialization of static variables
QueueHandler JsAsyncWork::appQueuehandler_ = nullptr;
FatalHandleFunc JsAsyncWork::isFatalErrorHitted_ = nullptr;
FatalHandleFunc JsAsyncWork::isAppExiting_ = nullptr;
#if (defined(__LINUX__) || defined(__LITEOS_A__))
PostUITaskFunc JsAsyncWork::postUITask_ = nullptr;
#endif
bool JsAsyncWork::isEnvInitialized_ = false;

void JsAsyncWork::SetAppQueueHandler(const QueueHandler handler)
{
    appQueuehandler_ = handler;
}

void JsAsyncWork::SetEnvStatus(bool envInitialized)
{
    isEnvInitialized_ = envInitialized;
}

bool JsAsyncWork::DispatchToLoop(AbilityMsgId msgId, void *data)
{
    if (appQueuehandler_ == nullptr || !isEnvInitialized_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork app not launched yet!");
        return false;
    }

    if (msgId >= MSG_ID_MAX) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork invalid msgId[%{public}d]!", msgId);
        return false;
    }

    if (isFatalErrorHitted_ != nullptr && isFatalErrorHitted_()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork fatal error hitted, app will exit");
        return false;
    }

    if (isAppExiting_ != nullptr && isAppExiting_()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork app is exiting");
        return false;
    }

    AbilityInnerMsg innerMsg;
    innerMsg.msgId = msgId;
    innerMsg.data = data;
    int8_t ret = MessageQueueUtils::PutMessage(appQueuehandler_, static_cast<void *>(&innerMsg), 0);
    return (ret == MSGQ_OK);
}

bool JsAsyncWork::DispatchAsyncWork(AsyncWorkHandler workHandler, void *data)
{
    return DispatchAsyncWorkInner(workHandler, nullptr, data);
}

bool JsAsyncWork::DispatchAsyncWork(AsyncHandler handler, void *data)
{
    return DispatchAsyncWorkInner(nullptr, handler, data);
}

bool JsAsyncWork::DispatchAsyncWorkInner(AsyncWorkHandler workHandler, AsyncHandler handler, void *data)
{
    if ((workHandler == nullptr) && (handler == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork parameters invalid");
        return false;
    }

    if ((isFatalErrorHitted_ != nullptr) && (isFatalErrorHitted_())) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork fatal processing, app will exit");
        return false;
    }
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    if ((workHandler != nullptr) && (postUITask_ != nullptr)) {
        auto task1 = [workHandler, data]() {
            workHandler(data);
        };
        return postUITask_(task1);
    }
    if ((handler != nullptr) && (postUITask_ != nullptr)) {
        auto task2 = [handler, data]() {
            handler(data, ERR_OK);
        };
        return postUITask_(task2);
    }
    return false;
#else
    AsyncWork* asyncWork = new AsyncWork();
    if (asyncWork == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork create async work failed!");
        return false;
    }
    asyncWork->workHandler = workHandler;
    asyncWork->handler = handler;
    asyncWork->data = data;

    bool ret = DispatchToLoop(ASYNCWORK, static_cast<void *>(asyncWork));
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:DispatchAsyncWork failed!, handler[%{public}p]",
            asyncWork->workHandler);
        delete(asyncWork);
        asyncWork = nullptr;
    }
    return ret;
#endif
}

void JsAsyncWork::ExecuteAsyncWork(AsyncWork *&asyncWork, int8_t statusCode)
{
    if (asyncWork == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "JsAsyncWork:ExecuteAsyncWork parameters invalid!");
        return;
    }
    AsyncWorkHandler workHandler = asyncWork->workHandler;
    if (workHandler != nullptr) {
        workHandler(asyncWork->data);
    }
    AsyncHandler handler = asyncWork->handler;
    if (handler != nullptr) {
        handler(asyncWork->data, statusCode);
    }
    delete asyncWork;
    asyncWork = nullptr;
}

void JsAsyncWork::SetFatalHandleFunc(FatalHandleFunc isFatalErrorHitted, FatalHandleFunc isAppExiting)
{
    isFatalErrorHitted_ = isFatalErrorHitted;
    isAppExiting_ = isAppExiting;
}

#if (defined(__LINUX__) || defined(__LITEOS_A__))
void JsAsyncWork::SetPostUITaskFunc(PostUITaskFunc postUITask)
{
    postUITask_ = postUITask;
}
#endif
} // namespace ACELite
} // namespace OHOS
