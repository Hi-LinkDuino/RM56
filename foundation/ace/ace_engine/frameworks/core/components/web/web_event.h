/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_WEB_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_WEB_EVENT_H

namespace OHOS::Ace {

class WebConsoleLog : public AceType {
    DECLARE_ACE_TYPE(WebConsoleLog, AceType)
public:
   WebConsoleLog() = default;
    ~WebConsoleLog() = default;

    virtual int GetLineNumber() = 0;
    virtual std::string GetLog() = 0;
    virtual int GetLogLevel() = 0;
    virtual std::string GetSourceId() = 0;
};

class ACE_EXPORT WebError : public AceType {
    DECLARE_ACE_TYPE(WebError, AceType)

public:
    WebError(const std::string& info, int32_t code) : info_(info), code_(code) {}
    ~WebError() = default;

    const std::string& GetInfo() const
    {
        return info_;
    }

    int32_t GetCode() const
    {
        return code_;
    }

private:
    std::string info_;
    int32_t code_;
};

class ACE_EXPORT WebResponse : public AceType {
    DECLARE_ACE_TYPE(WebResponse, AceType)

public:
    WebResponse(const std::map<std::string, std::string>& headers, const std::string& data, const std::string& encoding,
        const std::string& mimeType, const std::string& reason, int32_t statusCode)
        : headers_(headers), data_(data), encoding_(encoding), mimeType_(mimeType), reason_(reason),
          statusCode_(statusCode)
    {}
    ~WebResponse() = default;

    const std::map<std::string, std::string>& GetHeaders() const
    {
        return headers_;
    }

    const std::string& GetData() const
    {
        return data_;
    }

    const std::string& GetEncoding() const
    {
        return encoding_;
    }

    const std::string& GetMimeType() const
    {
        return mimeType_;
    }

    const std::string& GetReason() const
    {
        return reason_;
    }

    int32_t GetStatusCode() const
    {
        return statusCode_;
    }

private:
    std::map<std::string, std::string> headers_;
    std::string data_;
    std::string encoding_;
    std::string mimeType_;
    std::string reason_;
    int32_t statusCode_;
};

class ACE_EXPORT WebRequest : public AceType {
    DECLARE_ACE_TYPE(WebRequest, AceType)

public:
    WebRequest(const std::map<std::string, std::string>& headers, const std::string& method, const std::string& url,
        bool hasGesture, bool isMainFrame, bool isRedirect)
        : headers_(headers), method_(method), url_(url), hasGesture_(hasGesture), isMainFrame_(isMainFrame),
          isRedirect_(isRedirect)
    {}
    ~WebRequest() = default;

    const std::map<std::string, std::string>& GetHeaders() const
    {
        return headers_;
    }

    const std::string& GetMethod() const
    {
        return method_;
    }

    const std::string& GetUrl() const
    {
        return url_;
    }

    bool HasGesture() const
    {
        return hasGesture_;
    }

    bool IsMainFrame() const
    {
        return isMainFrame_;
    }

    bool IsRedirect() const
    {
        return isRedirect_;
    }

private:
    std::map<std::string, std::string> headers_;
    std::string method_;
    std::string url_;
    bool hasGesture_;
    bool isMainFrame_;
    bool isRedirect_;
};

class ACE_EXPORT Result : public AceType {
    DECLARE_ACE_TYPE(Result, AceType)

public:
    Result() = default;
    ~Result() = default;

    virtual void Confirm() = 0;
    virtual void Confirm(const std::string& message) = 0;
    virtual void Cancel() = 0;
};

class ACE_EXPORT WebDialogEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(WebDialogEvent, BaseEventInfo);

public:
    WebDialogEvent(const std::string& url, const std::string& message, const RefPtr<Result>& result)
        : BaseEventInfo("WebDialogEvent"), url_(url), result_(result) {}
    ~WebDialogEvent() = default;

    const std::string& GetUrl() const
    {
        return url_;
    }

    const std::string& GetMessage() const
    {
        return message_;
    }

    const RefPtr<Result>& GetResult() const
    {
        return result_;
    }

private:
    std::string url_;
    std::string message_;
    RefPtr<Result> result_;
};

class ACE_EXPORT WebGeolocation : public AceType {
    DECLARE_ACE_TYPE(WebGeolocation, AceType)

public:
    WebGeolocation() = default;
    ~WebGeolocation() = default;

    virtual void Invoke(const std::string& origin, const bool& allow, const bool& retain) = 0;
};

class ACE_EXPORT LoadWebPageStartEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebPageStartEvent, BaseEventInfo);

public:
    explicit LoadWebPageStartEvent(const std::string& url) : BaseEventInfo
        ("LoadWebPageStartEvent"), loadedUrl_(url) {}
    ~LoadWebPageStartEvent() = default;

    const std::string& GetLoadedUrl() const
    {
        return loadedUrl_;
    }

private:
    std::string loadedUrl_;
};

class ACE_EXPORT LoadWebPageFinishEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebPageFinishEvent, BaseEventInfo);

public:
    explicit LoadWebPageFinishEvent(const std::string& url) : BaseEventInfo
        ("LoadWebPageFinishEvent"), loadedUrl_(url) {}
    ~LoadWebPageFinishEvent() = default;

    const std::string& GetLoadedUrl() const
    {
        return loadedUrl_;
    }

private:
    std::string loadedUrl_;
};

class ACE_EXPORT LoadWebProgressChangeEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebProgressChangeEvent, BaseEventInfo);

public:
    explicit LoadWebProgressChangeEvent(const int& newProgress)
        : BaseEventInfo("LoadWebProgressChangeEvent"), newProgress_(newProgress) {}
    ~LoadWebProgressChangeEvent() = default;

    const int& GetNewProgress() const
    {
        return newProgress_;
    }

private:
    int newProgress_;
};

class ACE_EXPORT LoadWebTitleReceiveEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebTitleReceiveEvent, BaseEventInfo);

public:
    explicit LoadWebTitleReceiveEvent(const std::string& title) : BaseEventInfo
        ("LoadWebTitleReceiveEvent"), title_(title) {}
    ~LoadWebTitleReceiveEvent() = default;

    const std::string& GetTitle() const
    {
        return title_;
    }

private:
    std::string title_;
};

class ACE_EXPORT LoadWebGeolocationHideEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebGeolocationHideEvent, BaseEventInfo);

public:
    explicit LoadWebGeolocationHideEvent(const std::string& origin)
        : BaseEventInfo("LoadWebGeolocationHideEvent"), origin_(origin) {}
    ~LoadWebGeolocationHideEvent() = default;

    const std::string& GetOrigin() const
    {
        return origin_;
    }

private:
    std::string origin_;
};

class ACE_EXPORT LoadWebGeolocationShowEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebGeolocationShowEvent, BaseEventInfo);

public:
    LoadWebGeolocationShowEvent(const std::string& origin, const RefPtr<WebGeolocation>& webGeolocation)
        : BaseEventInfo("LoadWebGeolocationShowEvent"), origin_(origin), webGeolocation_(webGeolocation) {}
    ~LoadWebGeolocationShowEvent() = default;

    const std::string& GetOrigin() const
    {
        return origin_;
    }

    const RefPtr<WebGeolocation>& GetWebGeolocation() const
    {
        return webGeolocation_;
    }

private:
    std::string origin_;
    RefPtr<WebGeolocation> webGeolocation_;
};

class ACE_EXPORT DownloadStartEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(DownloadStartEvent, BaseEventInfo);

public:
    DownloadStartEvent(const std::string& url, const std::string& userAgent, const std::string& contentDisposition,
        const std::string& mimetype, long contentLength) : BaseEventInfo("DownloadStartEvent"), url_(url),
        userAgent_(userAgent), contentDisposition_(contentDisposition), mimetype_(mimetype),
        contentLength_(contentLength) {}
    ~DownloadStartEvent() = default;

    const std::string& GetUrl() const
    {
        return url_;
    }

    const std::string& GetUserAgent() const
    {
        return userAgent_;
    }

    const std::string& GetContentDisposition() const
    {
        return contentDisposition_;
    }

    const std::string& GetMimetype() const
    {
        return mimetype_;
    }

    long GetContentLength() const
    {
        return contentLength_;
    }

private:
    std::string url_;
    std::string userAgent_;
    std::string contentDisposition_;
    std::string mimetype_;
    long contentLength_;
};

class ACE_EXPORT ReceivedErrorEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(ReceivedErrorEvent, BaseEventInfo);

public:
    ReceivedErrorEvent(const RefPtr<WebRequest>& request, const RefPtr<WebError>& error)
        : BaseEventInfo("ReceivedErrorEvent"), request_(request), error_(error)
    {
        LOGI("ReceivedErrorEvent constructor");
    }
    ~ReceivedErrorEvent() = default;

    const RefPtr<WebRequest>& GetRequest() const
    {
        return request_;
    }

    const RefPtr<WebError>& GetError() const
    {
        return error_;
    }

private:
    RefPtr<WebRequest> request_;
    RefPtr<WebError> error_;
};

class ACE_EXPORT ReceivedHttpErrorEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(ReceivedHttpErrorEvent, BaseEventInfo);

public:
    ReceivedHttpErrorEvent(const RefPtr<WebRequest>& request, const RefPtr<WebResponse>& response)
        : BaseEventInfo("ReceivedHttpErrorEvent"), request_(request), response_(response)
    {
        LOGI("ReceivedHttpErrorEvent constructor");
    }
    ~ReceivedHttpErrorEvent() = default;

    const RefPtr<WebRequest>& GetRequest() const
    {
        return request_;
    }

    const RefPtr<WebResponse>& GetResponse() const
    {
        return response_;
    }

private:
    RefPtr<WebRequest> request_;
    RefPtr<WebResponse> response_;
};

class ACE_EXPORT LoadWebRequestFocusEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebRequestFocusEvent, BaseEventInfo);

public:
    explicit LoadWebRequestFocusEvent(const std::string& url) : BaseEventInfo
        ("LoadWebRequestFocusEvent"), focusUrl_(url) {}
    ~LoadWebRequestFocusEvent() = default;

    const std::string& GetRequestFocus() const
    {
        return focusUrl_;
    }
private:
    std::string focusUrl_;
};

class ACE_EXPORT LoadWebOnFocusEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebOnFocusEvent, BaseEventInfo);

public:
    explicit LoadWebOnFocusEvent(const std::string& url) : BaseEventInfo
        ("LoadWebOnFocusEvent"), focusUrl_(url) {}
    ~LoadWebOnFocusEvent() = default;

    const std::string& GetOnFocus() const
    {
        return focusUrl_;
    }
private:
    std::string focusUrl_;
};

class ACE_EXPORT LoadWebConsoleLogEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LoadWebConsoleLogEvent, BaseEventInfo);

public:
    LoadWebConsoleLogEvent(RefPtr<WebConsoleLog> message) : BaseEventInfo("LoadWebConsoleLogEvent"),
                                                            message_(message) {}
    ~LoadWebConsoleLogEvent() = default;

    const RefPtr<WebConsoleLog> GetMessage() const
    {
        return message_;
    }

private:
    RefPtr<WebConsoleLog> message_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_WEB_EVENT_H
