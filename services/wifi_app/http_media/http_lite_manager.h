/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifdef HISTREAMER_LITE_MANAGER_EN

#ifndef HISTREAMER_HTTP_LITE_PLUGIN_HTTP_MANAGER_H
#define HISTREAMER_HTTP_LITE_PLUGIN_HTTP_MANAGER_H
#include <stdlib.h>
#include <string>
#include <memory>

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpLitePlugin {
using OnError = void(*)(int httpError, int localError, void *param, int supportRetry);
struct HttpLiteAttr {
    OnError callbackFunc;
    void *pluginHandle;
    int bufferSize;
    int priority;
    std::string certFile;
    bool notVerifyCert; // false for verify; true for not verify
};

enum HttpLiteStatus {
    HTTP_LITE_STATUS_IDLE = 0,
    HTTP_LITE_STATUS_PLAY,
    HTTP_LITE_STATUS_PAUSE,
    HTTP_LITE_STATUS_SEEK,
    HTTP_LITE_STATUS_END,
    HTTP_LITE_STATUS_STOP
};

struct HttpLiteRunningInfo {
    unsigned int readPos;
    unsigned int writePos;
    unsigned int lastReadTime;
    HttpLiteStatus state;
    bool isRetry;
};

enum HttpLiteUrlType {
    URL_HTTP,
    URL_HLS,
    URL_WEBSOCKET,
    URL_UNKNOWN
};

class HttpLiteManager {
public:
    HttpLiteManager() noexcept;
    virtual ~HttpLiteManager();
    bool HttpOpen(std::string &url, HttpLiteAttr &attr);
    void HttpClose();
    bool HttpRead(unsigned char *buff, unsigned int wantReadLength, unsigned int &realReadLength, int &flag);
    bool HttpPeek(unsigned char *buff, unsigned int wantReadLength, unsigned int &realReadLength);
    bool HttpSeek(int offset);
    bool HttpPause();
    bool HttpReset();
    unsigned int GetContentLength();
    HttpLiteStatus GetHttpStatus();
    unsigned int GetLastReadTime();
    void GetHttpBufferRange(unsigned int *read, unsigned int *write);
    void SetWaterline(int high, int low);
    int IsStreaming();
    HttpLiteUrlType IsHlsSource(std::string &url);
    void GetHttpRunningInfo(HttpLiteRunningInfo &info);
    void SetHttpRunningInfo(bool isRetry);

private:
    friend void ReceiveData(unsigned char *data, int len, void *priv);
    friend void OnFinished(void *priv);
    friend void OnFailed(int httpError, int localError, void *priv, int supportRetry);
    bool IsNeedRetry(int localError, int supportRetry);
    HttpLiteAttr httpAttr_ {nullptr, nullptr, 0, 0, {}};
    HttpLiteStatus status_ {HTTP_LITE_STATUS_IDLE};
    unsigned int lastReadTime_ {0};
    bool isRetry_ {false};
    int retryTimes_ {0};
    void *httpHandle_;
    long sourceLen_;
};
} // HttpLitePlugin
} // Plugin
} // Media
} // OHOS

#endif // HISTREAMER_HTTP_LITE_PLUGIN_HTTP_MANAGER_H

#endif // HISTREAMER_LITE_MANAGER_EN