/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_HIVIEWDFX_ISYS_EVENT_CALLBACK_H
#define OHOS_HIVIEWDFX_ISYS_EVENT_CALLBACK_H

#include "iremote_broker.h"

namespace OHOS {
namespace HiviewDFX {
class ISysEventCallback : public IRemoteBroker {
public:
    virtual void Handle(const std::u16string& domain, const std::u16string& eventName, uint32_t eventType,
        const std::u16string& eventDetail) = 0;

    enum {
        HANDLE = 0,
    };

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.hiviewdfx.ISysEventCallback");
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // OHOS_HIVIEWDFX_ISYS_EVENT_CALLBACK_H