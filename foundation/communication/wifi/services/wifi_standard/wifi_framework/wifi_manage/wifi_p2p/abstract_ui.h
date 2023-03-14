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
#ifndef MOCK_SYSTEM_UI_H
#define MOCK_SYSTEM_UI_H
#include <any>
#include "wifi_log.h"

#ifdef INCLUDE_SYSTEMUI_MODULE
std::any &GetVerityModuleV2();
#endif

namespace OHOS {
namespace Wifi {
class AlertDialog {
public:
    AlertDialog()
    {}
    virtual ~AlertDialog()
    {}
    virtual void SetTitle(const std::string value)
    {
        LOGI("value:%{public}s", value.c_str());
    }
    virtual void SetMessage(const std::string msg)
    {
        LOGI("value:%{public}s", msg.c_str());
    }
    virtual void SetInputBox(const std::string txt)
    {
        LOGI("value:%{public}s", txt.c_str());
    }
    virtual std::string GetInputBox(const std::string txt)
    {
        LOGI("value:%{public}s", txt.c_str());
        return "";
    }
    virtual void SetButton(
        const std::string txt, std::function<void(AlertDialog &, std::any)> &buttonEvent, std::any ctx)
    {
        LOGI("value:%{public}s", txt.c_str());
        AlertDialog log;
        buttonEvent(log, ctx);
    }
};

class AbstractUI {
public:
    AbstractUI()
    {}
    virtual ~AbstractUI()
    {}
    static AbstractUI &GetInstance()
    {
#ifdef INCLUDE_SYSTEMUI_MODULE
        static AbstractUI &instance = reinterpret_cast<AbstractUI &>(GetVerityModuleV2());
        instance.StartInit();
#else
        static AbstractUI instance;
#endif
        return instance;
    }
    virtual void StartInit()
    {}
    virtual AlertDialog &Build()
    {
        static AlertDialog p;
        return p;
    }
    virtual void ShowAlerDialog(AlertDialog &alertDialog)
    {
        alertDialog.SetTitle("value");
    }
    virtual void DestroyAlerDialog(AlertDialog &alertDialog)
    {
        alertDialog.SetTitle("value");
    }
    virtual void UnInit(){};
};
}  // namespace Wifi
}  // namespace OHOS
#endif  // MOCK_SYSTEM_UI_H