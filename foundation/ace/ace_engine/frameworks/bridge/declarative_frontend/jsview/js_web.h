/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_WEB_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_WEB_H

#include <string>

#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"

namespace OHOS::Ace::Framework {
class JSWeb : public JSContainerBase {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);
    static void OnAlert(const JSCallbackInfo& args);
    static void OnBeforeUnload(const JSCallbackInfo& args);
    static void OnConfirm(const JSCallbackInfo& args);
    static void OnConsoleLog(const JSCallbackInfo& args);
    static void OnPageStart(const JSCallbackInfo& args);
    static void OnPageFinish(const JSCallbackInfo& args);
    static void OnProgressChange(const JSCallbackInfo& args);
    static void OnTitleReceive(const JSCallbackInfo& args);
    static void OnGeolocationHide(const JSCallbackInfo& args);
    static void OnGeolocationShow(const JSCallbackInfo& args);
    static void OnRequestFocus(const JSCallbackInfo& args);
    static void OnError(const JSCallbackInfo& args);
    static void OnMessage(const JSCallbackInfo& args);
    static void OnDownloadStart(const JSCallbackInfo& args);
    static void OnErrorReceive(const JSCallbackInfo& args);
    static void OnHttpErrorReceive(const JSCallbackInfo& args);
    static void JsEnabled(bool isJsEnabled);
    static void ContentAccessEnabled(bool isContentAccessEnabled);
    static void FileAccessEnabled(bool isFileAccessEnabled);
    static void OnFocus(const JSCallbackInfo& args);
    static void OnLineImageAccessEnabled(bool isOnLineImageAccessEnabled);
    static void DomStorageAccessEnabled(bool isDomStorageAccessEnabled);
    static void ImageAccessEnabled(bool isImageAccessEnabled);
    static void MixedMode(int32_t MixedModeNum);
    static void ZoomAccessEnabled(bool isZoomAccessEnabled);
    static void GeolocationAccessEnabled(bool isGeolocationAccessEnabled);
    static void JavaScriptProxy(const JSCallbackInfo& args);
    static void UserAgent(const std::string& userAgent);

protected:
    static void OnCommonDialog(const JSCallbackInfo& args, int dialogEventType);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_WEB_H