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


#ifndef _HDI_DISPLAY_REGISTER_CALLBACK_FRAMEWORK_H_
#define _HDI_DISPLAY_REGISTER_CALLBACK_FRAMEWORK_H_

#include "display_device_callback_stub.h"

class DisplayRegisterCallbackFramework : public DisplayDeviceCallbackStub {
public:
    DisplayRegisterCallbackFramework() = default;
    virtual ~DisplayRegisterCallbackFramework() = default;
    virtual int32_t OnHotplugIn(uint32_t outputId, bool connected) override;
    void SetHotplugInData(HotPlugCallback callback, void* data);
    virtual int32_t OnVBlankCallback(unsigned int sequence, uint64_t ns) override;
    void SetVBlankData(VBlankCallback callback, void* data);
    virtual int32_t OnRefreshCallback(uint32_t devId) override;
    void SetRefreshData(RefreshCallback callback, void* data);
private:
    HotPlugCallback hotCallback_;
    void* hotCallbackData_;
    VBlankCallback vBlankCallback_;
    void* vBlankCallbackData_;
    RefreshCallback refreshCallback_;
    void* refreshCallbackData_;
};
#endif // _HDI_DISPLAY_REGISTER_CALLBACK_FRAMEWORK_H_
