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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STREAM_INSTALLER_INTERFACE_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STREAM_INSTALLER_INTERFACE_H

#include "iremote_broker.h"
#include "status_receiver_interface.h"

namespace OHOS {
namespace AppExecFwk {
class IBundleStreamInstaller : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.BundleStreamInstaller");

    virtual int CreateStream(const std::string &hapName, long offset) = 0;
    virtual bool Install(const sptr<IStatusReceiver>& receiver) = 0;

    virtual uint32_t GetInstallerId() const = 0;
    virtual void SetInstallerId(uint32_t installerId) = 0;
    virtual void UnInit() = 0;

    enum StreamMessage : uint32_t {
        CREATE_STREAM = 0,
        STREAM_INSTALL = 1,
    };
};
} // AppExecFwk
} // OHOS

#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STREAM_INSTALLER_INTERFACE_H