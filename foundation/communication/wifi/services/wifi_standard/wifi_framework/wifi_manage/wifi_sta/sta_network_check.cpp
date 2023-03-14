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
#include "sta_network_check.h"
#include "if_config.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("StaNetworkCheck");

namespace OHOS {
namespace Wifi {
constexpr int NET_ERR_OK = 200;
constexpr int NET_ERR_CREATED = 201;
constexpr int NET_ERR_NO_CONTENT = 204;
constexpr int NET_ERR_BAD_REQUEST = 400;

constexpr int NET_ERR_REDIRECT_CLASS_MAX = 399;
constexpr int NET_ERR_REQUEST_ERROR_CLASS_MAX = 499;

StaNetworkCheck::StaNetworkCheck(NetStateHandler handle)
{
    pDealNetCheckThread = nullptr;
    netStateHandler = handle;
    lastNetState = NETWORK_STATE_UNKNOWN;
    isStopNetCheck = true;
    isExitNetCheckThread = false;
}

StaNetworkCheck::~StaNetworkCheck()
{
    WIFI_LOGI("StaNetworkCheck::~StaNetworkCheck enter\n");
    ExitNetCheckThread();
    WIFI_LOGI("StaNetworkCheck::~StaNetworkCheck complete\n");
}

bool StaNetworkCheck::HttpDetection()
{
    WIFI_LOGI("Enter httpDetection");
    /* Detect portal hotspot and send message to InterfaceSeervice if result is yes. */
    HttpRequest httpRequest;
    std::string httpReturn;
    std::string httpMsg(DEFAULT_PORTAL_HTTPS_URL);
    const std::string genStr("generate_204");
    const std::string contStr("Content-Length:");

    if (httpRequest.HttpGet(httpMsg, httpReturn) == 0) {
        size_t retCode = httpReturn.find("HTTP/");
        int codeNum = 0;
        if (retCode >= 0) {
            constexpr int NET_ERROR_POS = 8;
            constexpr int NET_ERROR_LEN = 5;
            codeNum = std::atoi(httpReturn.substr(retCode + NET_ERROR_POS, NET_ERROR_LEN).c_str());
        }
        
        size_t contLenStr = httpReturn.find(contStr);
        int contLenNum = 0;
        if (contLenStr > 0) {
            constexpr int NET_CONTENT_LENGTH = 6;
            contLenNum = std::atoi(httpReturn.substr(contLenStr + contStr.length(), NET_CONTENT_LENGTH).c_str());
        }

        constexpr int PORTAL_CONTENT_LENGTH_MIN = 4;
        if (codeNum == NET_ERR_NO_CONTENT) {
            WIFI_LOGE("This network is normal!");
            if ((lastNetState != NETWORK_STATE_WORKING) && (isExitNetCheckThread == false) &&
                (isStopNetCheck == false)) {
                netStateHandler(StaNetState::NETWORK_STATE_WORKING, "");
            }
            lastNetState = NETWORK_STATE_WORKING;
            return true;
        } else if (codeNum != NET_ERR_NO_CONTENT &&
            (codeNum >= NET_ERR_CREATED && codeNum <= NET_ERR_REDIRECT_CLASS_MAX)) {
            /* Callback result to InterfaceService. */
            WIFI_LOGI("This network is portal AP, need certification!");
            std::string urlTmp;
            const std::string locStr("Location: ");
            size_t startStr = httpReturn.find(locStr);
            if (startStr > 0) {
                startStr += locStr.length();
                size_t endstr = httpReturn.find(genStr, startStr);
                if (endstr > 0) {
                    endstr += genStr.length();
                    urlTmp = httpReturn.substr(startStr, endstr-startStr);
                }
                netStateHandler(StaNetState::NETWORK_CHECK_PORTAL, urlTmp);
            }
            return false;
        } else if ((codeNum == NET_ERR_OK ||
            (codeNum >= NET_ERR_BAD_REQUEST && codeNum <= NET_ERR_REQUEST_ERROR_CLASS_MAX)) &&
            contLenNum > PORTAL_CONTENT_LENGTH_MIN) {
            WIFI_LOGI("This network is portal AP, need certification!");
            std::string urlTmp;
            const std::string locStr("http");
            size_t startStr = httpReturn.find(locStr);
            if (startStr > 0) {
                size_t endstr = httpReturn.find(genStr, startStr);
                if (endstr > 0) {
                    endstr += genStr.length();
                    urlTmp = httpReturn.substr(startStr, endstr-startStr);
                }
                netStateHandler(StaNetState::NETWORK_CHECK_PORTAL, urlTmp);
            }
            return false;
        } else {
            WIFI_LOGE("http msg error[%s]!", httpReturn.c_str());
            netStateHandler(StaNetState::NETWORK_STATE_NOWORKING, "");
            lastNetState = NETWORK_STATE_NOWORKING;
            return true;
        }
    }
    WIFI_LOGE("This network can't online!");
    if ((lastNetState != NETWORK_STATE_NOWORKING) && (isExitNetCheckThread == false) && (isStopNetCheck == false)) {
        netStateHandler(StaNetState::NETWORK_STATE_NOWORKING, "");
    }
    lastNetState = NETWORK_STATE_NOWORKING;
    return true;
}

void StaNetworkCheck::RunNetCheckThreadFunc()
{
    WIFI_LOGI("enter RunNetCheckThreadFunc!\n");
    int timeoutMs = 3000;
    for (;;) {
        while (isStopNetCheck && !isExitNetCheckThread) {
            LOGI("waiting for signal.\n");
            std::unique_lock<std::mutex> lck(mMutex);
            mCondition.wait(lck);
        }

        if (isExitNetCheckThread) {
            WIFI_LOGI("break the loop\n");
            break;
        }

        if (!HttpDetection()) {
            isStopNetCheck = true;
        }

        if (!isExitNetCheckThread) {
            std::unique_lock<std::mutex> lck(mMutex);
            if (mCondition_timeout.wait_for(lck, std::chrono::milliseconds(timeoutMs)) == std::cv_status::timeout) {
                LOGD("mCondition_timeout timeout.\n");
            } else {
                LOGD("Wake up, break the loop.\n");
                break;
            }
        }
    }
}

ErrCode StaNetworkCheck::InitNetCheckThread()
{
    pDealNetCheckThread = new (std::nothrow) std::thread(&StaNetworkCheck::RunNetCheckThreadFunc, this);
    if (pDealNetCheckThread == nullptr) {
        WIFI_LOGE("In StaNetworkCheck start NetCheck thread failed!\n");
        return ErrCode::WIFI_OPT_FAILED;
    }
    return ErrCode::WIFI_OPT_SUCCESS;
}

void StaNetworkCheck::StopNetCheckThread()
{
    WIFI_LOGI("enter StopNetCheckThread!\n");
    std::unique_lock<std::mutex> lck(mMutex);
    isStopNetCheck = true;
}

void StaNetworkCheck::SignalNetCheckThread()
{
    WIFI_LOGI("enter SignalNetCheckThread!\n");
    std::unique_lock<std::mutex> lck(mMutex);
    lastNetState = NETWORK_STATE_UNKNOWN;
    isStopNetCheck = false;
    mCondition.notify_one();
}

void StaNetworkCheck::ExitNetCheckThread()
{
    {
        std::unique_lock<std::mutex> lck(mMutex);
        isStopNetCheck = false;
        isExitNetCheckThread = true;
        mCondition.notify_one();
        mCondition_timeout.notify_one();
    }

    if (pDealNetCheckThread != nullptr) {
        LOGD("pDealNetCheckThread->join();");
        pDealNetCheckThread->join();
        delete pDealNetCheckThread;
        pDealNetCheckThread = nullptr;
        LOGD("pDealNetCheckThread = nullptr; done");
    }
}
}  // namespace Wifi
}  // namespace OHOS
