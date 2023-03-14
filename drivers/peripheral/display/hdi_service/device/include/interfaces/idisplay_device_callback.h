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


#ifndef _HDI_DISPLAY_REGISTER_CALLBACK_BASE_H_
#define _HDI_DISPLAY_REGISTER_CALLBACK_BASE_H_

#include <iremote_broker.h>
#include "display_device_common.h"

class DisplayRegisterCallbackBase : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Display.V1_0.DisplayComposerCallback");
    virtual ~DisplayRegisterCallbackBase() = default;
    virtual int32_t OnHotplugIn(uint32_t outputId, bool connected) = 0;
    virtual int32_t OnVBlankCallback(unsigned int sequence, uint64_t ns) = 0;
    virtual int32_t OnRefreshCallback(uint32_t devId) = 0;
};
#endif // _HDI_DISPLAY_REGISTER_CALLBACK_BASE_H_
