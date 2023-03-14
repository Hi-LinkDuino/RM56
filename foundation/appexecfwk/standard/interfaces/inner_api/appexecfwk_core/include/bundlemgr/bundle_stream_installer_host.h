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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STREAM_INSTALLER_HOST_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STREAM_INSTALLER_HOST_H

#include <functional>
#include <map>

#include "bundle_stream_installer_interface.h"
#include "iremote_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace AppExecFwk {
class BundleStreamInstallerHost : public IRemoteStub<IBundleStreamInstaller> {
public:
    BundleStreamInstallerHost();
    virtual ~BundleStreamInstallerHost() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DISALLOW_COPY_AND_MOVE(BundleStreamInstallerHost);

    void init();
    ErrCode HandleCreateStream(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleInstall(MessageParcel &data, MessageParcel &reply);

    std::map<uint32_t, std::function<ErrCode(MessageParcel &data, MessageParcel &reply)>> funcMap_;
};
} // AppExecFwk
} // OHOS

#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STREAM_INSTALLER_HOST_H