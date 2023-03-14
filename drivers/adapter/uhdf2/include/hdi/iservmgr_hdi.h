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
#ifndef HDI_ISERVICE_MANAGER_INF_H
#define HDI_ISERVICE_MANAGER_INF_H

#include <refbase.h>
#include <iremote_broker.h>
#include "iservstat_listener_hdi.h"

namespace OHOS {
namespace HDI {
namespace ServiceManager {
namespace V1_0 {
class IServiceManager : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.IServiceManager.V1_0");
    static ::OHOS::sptr<IServiceManager> Get();
    virtual ::OHOS::sptr<IRemoteObject> GetService(const char* serviceName) = 0;
    virtual int32_t RegisterServiceStatusListener(::OHOS::sptr<IServStatListener> listener, uint16_t deviceClass) = 0;
    virtual int32_t UnregisterServiceStatusListener(::OHOS::sptr<IServStatListener> listener) = 0;
};
} // namespace V1_0
} // namespace ServiceManager
} // namespace HDI
} // namespace OHOS

#endif /* HDI_ISERVICE_MANAGER_INF_H */