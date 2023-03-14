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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_I_SERVICE_CENTER_STATUS_CALLBACK_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_I_SERVICE_CENTER_STATUS_CALLBACK_H

#include "ability_info.h"
#include "element_name.h"
#include "install_result.h"
#include "iremote_broker.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class IServiceCenterStatusCallback
 * IServiceCenterStatusCallback is used to notify caller ability that free install is complete.
 */
class IServiceCenterStatusCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.ServiceCenterStatusCallback");

    /**
     * @brief Will be execute when free install is complete.
     * @param installResult the json of InstallResult
     */
    virtual int32_t OnInstallFinished(std::string installResult) = 0;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_I_SERVICE_CENTER_STATUS_CALLBACK_H
