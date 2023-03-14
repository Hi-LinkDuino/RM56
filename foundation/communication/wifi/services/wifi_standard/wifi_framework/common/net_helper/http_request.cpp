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

#include "http_request.h"
#include <thread>
#include <unordered_set>
#include <sys/time.h>
#include "securec.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_UTILS_HTTP_REQ"

namespace OHOS {
namespace Wifi {
HttpRequest::HttpRequest()
    : mISocketFd(INVALID_SOCKET), iPort(0), strHost(""), strIp(""), strRes(""), strParam(""), httpHead("")
{}

HttpRequest::~HttpRequest()
{
    if (mISocketFd > 0) {
        close(mISocketFd);
    }
    mISocketFd = INVALID_SOCKET;
}

int HttpRequest::HttpGet(const std::string &strUrl, std::string &strResponse)
{
    return HttpRequestExec("GET", strUrl, "", strResponse);
}

int HttpRequest::HttpPost(const std::string &strUrl, const std::string &strData, std::string &strResponse)
{
    return HttpRequestExec("POST", strUrl, strData, strResponse);
}

int HttpRequest::HttpRequestExec(
    const std::string &strMethod, const std::string &strUrl, const std::string &strData, std::string &strResponse)
{
    int iRet = 0;
    /* Check whether the URL is valid. */
    if (strUrl.empty()) {
        LOGE("HttpRequest::HttpRequestExec URL is null\n");
        return -1;
    }

    /* Limit the URL length. */
    if (URLSIZE < strUrl.length()) {
        LOGE("HttpRequest::HttpRequestExec URL length > %{public}d, error\n", URLSIZE);
        return -1;
    }

    iRet = GetHostAddrFromUrl(strUrl);
    if (iRet != 0) {
        LOGE("HttpRequest::HttpRequestExec get HostAddr failed.\n");
        return -1;
    }

    /* Create an HTTP protocol header. */
    HttpHeadCreate(strMethod, strData);

    /* Create socket */
    mISocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mISocketFd < 0) {
        LOGE("HttpRequest::HttpRequestExec socket error! Error code: %{public}d.", errno);
        return -1;
    }

    /* Bind address and port */
    GetPortFromUrl();
    if (iPort < 0) {
        LOGE("HttpRequest::HttpRequestExec get port failed from URL!\n");
        close(mISocketFd);
        mISocketFd = INVALID_SOCKET;
        return -1;
    }

    iRet = GetIPFromUrl();
    if (iRet != 0) {
        LOGE("HttpRequest::HttpRequestExec get ip failed from URL!\n");
        close(mISocketFd);
        mISocketFd = INVALID_SOCKET;
        return -1;
    }

    LOGI("iPort= [%{public}d],strIP= [%s]\n", iPort, strIp.c_str());

    iRet = HttpConnect(strResponse);
    if (iRet != 0) {
        LOGE("HttpRequest::HttpConnect failed!\n");
        close(mISocketFd);
        mISocketFd = INVALID_SOCKET;
        return -1;
    }
    close(mISocketFd);
    mISocketFd = INVALID_SOCKET;
    LOGD("HttpRequest::HttpConnect success!\n");
    return 0;
}

int HttpRequest::HttpConnect(std::string &strResponse)
{
    int flags = 0;
    struct sockaddr_in servaddr = {0};
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(iPort);
    if (inet_pton(AF_INET, strIp.c_str(), &servaddr.sin_addr) <= 0) {
        LOGE("HttpConnect inet_pton error! errno: %{public}d", errno);
        return -1;
    }

    /* Set non-blocking */
    flags = fcntl(mISocketFd, F_GETFL, 0);
    if (fcntl(mISocketFd, F_SETFL, flags | O_NONBLOCK) == -1) {
        LOGE("HttpConnect fcntl error! Error code: %{public}d", errno);
        return -1;
    }

    /* Non-blocking connection */
    int iRet = connect(mISocketFd, reinterpret_cast<struct sockaddr *>(&servaddr), sizeof(servaddr));
    if (iRet == 0) {
        iRet = HttpDataTransmit(mISocketFd);
        if (iRet != 0) {
            LOGE("HttpConnect HttpDataTransmit error\n");
            return -1;
        }
        strResponse = strRes;
        return 0;
    } else if (iRet < 0) {
        if (errno != EINPROGRESS) {
            LOGE("HttpDataTransmit connect error! Error code: %{public}d", errno);
            return -1;
        }
    }

    iRet = SocketFdCheck(mISocketFd);
    if (iRet > 0) {
        iRet = HttpDataTransmit(mISocketFd);
        if (iRet != 0) {
            LOGE("HttpRequest::HttpDataTransmit HttpDataTransmit error!");
            return -1;
        }
        strResponse = strRes;
        return 0;
    } else {
        LOGE("HttpRequest::HttpDataTransmit SocketFdCheck error!");
        return -1;
    }
    return 0;
}

void HttpRequest::HttpHeadCreate(const std::string &strMethod, const std::string &strData)
{
    std::string strHttpHead;
    strHttpHead += strMethod;
    strHttpHead += " /";
    strHttpHead += strParam;
    strHttpHead += " HTTP/1.1\r\n";
    strHttpHead += "Accept: */*\r\n";
    strHttpHead += "Accept-Language: cn\r\n";
    strHttpHead += "User-Agent: Mozilla/4.0\r\n";
    strHttpHead += "Host: ";
    strHttpHead += strHost;
    strHttpHead += "\r\n";
    strHttpHead += "Cache-Control: no-cache\r\n";
    strHttpHead += "Connection: Keep-Alive\r\n";

    if (strMethod == "POST") {
        std::size_t uLen = strData.length();
        strHttpHead += "Content-Type: application/x-www-form-urlencoded\r\n";
        strHttpHead += "Content-Length: ";
        strHttpHead += std::to_string(uLen);
        strHttpHead += "\r\n\r\n";
        strHttpHead += strData;
    }
    strHttpHead += "\r\n\r\n";
    httpHead = strHttpHead;
}

int HttpRequest::HttpDataTransmit(const int &iSockFd)
{
    int ret = send(iSockFd, httpHead.c_str(), httpHead.length() + 1, 0);
    if (ret < 0) {
        LOGE("HttpRequest::HttpDataTransmit send error! Error code: %{public}d", errno);
        return -1;
    }
    char *buf = new char[BUFSIZE]();
    if (buf == nullptr) {
        return -1;
    }
    constexpr int timeoutMs = 500;
    constexpr int timeRate = 1000;
    struct timeval tv;
    struct timeval tvEnd;
    gettimeofday(&tv, nullptr);
    long long tvTime = tv.tv_sec * timeRate + tv.tv_usec / timeRate;
    long long tvEndTime;
    bool bDataRec = false;
    while (1) {
        gettimeofday(&tvEnd, nullptr);
        tvEndTime = tvEnd.tv_sec * timeRate + tvEnd.tv_usec / timeRate;
        if (tvEndTime - tvTime > timeoutMs) {
            LOGE("HttpRequest::HttpDataTransmit recv timeout\n");
            delete[] buf;
            buf = nullptr;
            return -1;
        }
        (void)memset_s(buf, BUFSIZE, 0, BUFSIZE);
        ret = recv(iSockFd, buf, BUFSIZE, 0);
        if (ret == 0) {
            /* The connection is closed. */
            if (!bDataRec) {
                LOGE("HttpRequest::HttpDataTransmit recv error! Error code: %{public}d", errno);
                delete[] buf;
                buf = nullptr;
                return -1;
            } else {
                LOGD("HttpRequest::HttpDataTransmit recv success\n");
                delete[] buf;
                return 0;
            }
        } else if (ret > 0) {
            bDataRec = true;
            strRes += buf;
        } else if (ret < 0) {
            /* Error */
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                LOGE("HttpRequest::HttpDataTransmit recv not finish!\n");
                continue;
            } else {
                LOGE("HttpRequest::HttpDataTransmit recv error! Error code: %{public}d", errno);
                delete[] buf;
                buf = nullptr;
                return -1;
            }
        }
    }
}

int HttpRequest::GetHostAddrFromUrl(const std::string &strUrl)
{
    std::string urlTmp(strUrl);
    std::string urlTmp2;
    if (urlTmp.find("https://") != std::string::npos) {
        urlTmp2 = urlTmp.substr(HTTPS_HEADER_LENGTH);
    } else if (urlTmp.find("http://") != std::string::npos) {
        urlTmp2 = urlTmp.substr(HTTP_HEADER_LENGTH);
    } else {
        urlTmp2 = urlTmp;
    }
    LOGI("GetHostAddrFromUrl urlTmp2 = [%s].\n", urlTmp2.c_str());

    if (!urlTmp2.empty()) {
        strHost = urlTmp2.substr(0, urlTmp2.find("/"));
        strParam = urlTmp2.substr(urlTmp2.find("/") + 1);
    } else {
        LOGE("GetHostAddrFromUrl url is wrong\n");
        return -1;
    }

    return 0;
}

void HttpRequest::GetPortFromUrl()
{
    if (strHost.find(":") != std::string::npos) {
        std::string strPort = strHost.substr(strHost.find(":") + 1);
        iPort = std::stoi(strPort);
    } else {
        iPort = DEFAULT_PORT;
    }
}

std::mutex gMutex;
std::unordered_set<HostData*> gHostDataSet;
void GetHostThread(HostData* pThreadData)
{
    std::string ipOrDomain;
    {
        std::unique_lock<std::mutex> lck(gMutex);
        if (gHostDataSet.find(pThreadData) == gHostDataSet.end()) {
            LOGE("GetHostThread Error.");
            return;
        }
        ipOrDomain = pThreadData->strIpOrDomain;
    }
    hostent *he = gethostbyname(ipOrDomain.c_str());
    if (he == nullptr) {
        {
            std::unique_lock<std::mutex> lck(gMutex);
            if (gHostDataSet.find(pThreadData) != gHostDataSet.end()) {
                LOGD("GetHostThread delete.");
                gHostDataSet.erase(pThreadData);
                delete pThreadData;
                pThreadData = nullptr;
            }
        }
        LOGE("GetIPFromUrl Url is wrong. error message:[%s]\n", hstrerror(h_errno));
    } else {
        std::unique_lock<std::mutex> lck(gMutex);
        in_addr **addr_list = (in_addr **)he->h_addr_list;
        for (int i = 0; addr_list[i] != nullptr; i++) {
            pThreadData->strIp = inet_ntoa(*addr_list[i]);
        }
        pThreadData->bIp = true;
        pThreadData->mWait_timeout.notify_one();
    }
    return;
}

int HttpRequest::GetIPFromUrl()
{
    std::string strIpOrDomain;
    if (strHost.find(":") != std::string::npos) {
        strIpOrDomain = strHost.substr(0, strHost.find(":"));
    } else {
        strIpOrDomain = strHost;
    }
    if (strIpOrDomain.empty()) {
        LOGE("GetIPFromUrl strIpOrDomain is null.\n");
        return -1;
    }

    if (inet_addr(strIpOrDomain.c_str()) == INADDR_NONE) {
        LOGI("GetIPFromUrl Url maybe contain Domain.");
        HostData* pData = nullptr;
        {
            std::unique_lock<std::mutex> lck(gMutex);
            pData = new HostData;
            if (pData == nullptr) {
                LOGE("GetIPFromUrl new HostData error.\n");
                return -1;
            }
            gHostDataSet.emplace(pData);
            pData->strIpOrDomain = strIpOrDomain;
        }

        int iRlt = -1;
        std::thread getHost = std::thread(&GetHostThread, pData);
        getHost.detach();

        bool bTimeOut = false;
        const int timeoutMs = 150;
        {
            std::unique_lock<std::mutex> lck(gMutex);
            if (pData->mWait_timeout.wait_for(lck, std::chrono::milliseconds(timeoutMs)) == std::cv_status::timeout) {
                bTimeOut = true;
            }
        }

        if (!bTimeOut) {
            std::unique_lock<std::mutex> lck(gMutex);
            if (gHostDataSet.find(pData) == gHostDataSet.end()) {
                if (pData != nullptr) {
                    delete pData;
                    pData = nullptr;
                }
                LOGD("GetHostThread None.");
                return -1;
            }
            if (pData->bIp) {
                iRlt = 0;
                strIp = pData->strIp;
                gHostDataSet.erase(pData);
                delete pData;
                pData = nullptr;
                LOGD("Get ip ok.");
            }
        }
        return iRlt;
    } else {
        LOGI("GetIPFromUrl Url contain ip\n");
        strIp = strIpOrDomain;
        return 0;
    }
}

int HttpRequest::SocketFdCheck(const int &iSockFd) const
{
    struct timeval timeout = {0};
    fd_set rSet, wSet;
    FD_ZERO(&rSet);
    FD_ZERO(&wSet);
    FD_SET(iSockFd, &rSet);
    FD_SET(iSockFd, &wSet);
    timeout.tv_sec = SEND_HTTP_DELAY_TIME;
    timeout.tv_usec = 0;
    int iRet = select(iSockFd + 1, &rSet, &wSet, nullptr, &timeout);
    if (iRet < 0) {
        LOGE("HttpRequest::SocketFdCheck select failed");
        /* An error occurs during the selection. All descriptor sets are cleared. */
        return -1;
    } else if (iRet == 0) {
        LOGE("HttpRequest::SocketFdCheck time out");
        /* Timeout */
        return -1;
    }
    /* Check whether SocketFd is writable and unreadable. */
    int iW = FD_ISSET(iSockFd, &wSet);
    int iR = FD_ISSET(iSockFd, &rSet);
    if (iW && !iR) {
        char error[4] = "";
        socklen_t len = sizeof(error);
        int ret = getsockopt(iSockFd, SOL_SOCKET, SO_ERROR, error, &len);
        if (ret == 0) {
            if (!strcmp(error, "")) {
                /* Indicates the number of prepared descriptors. */
                return iRet;
            } else {
                LOGE("HttpRequest::SocketFdCheck getsockopt failed. error code:%{public}d", errno);
                return -1;
            }
        } else {
            LOGE("HttpRequest::SocketFdCheck getsockopt failed. error code:%{public}d", errno);
            return -1;
        }
    } else {
        LOGE("HttpRequest::SocketFdCheck "
             "Whether sockFd is in the writable character set: %{public}d, is it in the "
             "readable character set: %{public}d\t (0 means not in)\n",
            iW,
            iR);
        return -1;
    }
}
}  // namespace Wifi
}  // namespace OHOS