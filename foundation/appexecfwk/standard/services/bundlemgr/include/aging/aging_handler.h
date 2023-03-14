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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_HANDLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_HANDLER_H

#include <cinttypes>
#include <string>
#include <vector>

#include "aging_request.h"
#include "bundle_util.h"
#include "status_receiver_host.h"

namespace OHOS {
namespace AppExecFwk {
class AgingHandler {
public:
    AgingHandler() = default;
    virtual ~AgingHandler() = default;
    virtual bool Process(AgingRequest &request) const = 0;
    virtual const std::string &GetName() const = 0;
};

class RecentlyUnuseBundleAgingHandler : public AgingHandler {
public:
    RecentlyUnuseBundleAgingHandler() = default;
    virtual ~RecentlyUnuseBundleAgingHandler() = default;
    virtual bool Process(AgingRequest &request) const override;
    virtual bool CheckBundle(const AgingBundleInfo &bundle) const = 0;
    virtual bool NeedContinue(const AgingRequest &request) const;
    virtual bool IsRunning(const std::string bundleName, const int bundleUid) const;

private:
    bool UnInstallBundle(const std::string &bundleName) const;
};

class Over30DaysUnusedBundleAgingHandler : public RecentlyUnuseBundleAgingHandler {
public:
    bool CheckBundle(const AgingBundleInfo &bundle) const override;
    const std::string &GetName() const override;
};

class Over20DaysUnusedBundleAgingHandler : public RecentlyUnuseBundleAgingHandler {
public:
    bool CheckBundle(const AgingBundleInfo &bundle) const override;
    const std::string &GetName() const override;
};

class Over10DaysUnusedBundleAgingHandler : public RecentlyUnuseBundleAgingHandler {
public:
    bool CheckBundle(const AgingBundleInfo &bundle) const override;
    const std::string &GetName() const override;
};

class BundleDataSizeAgingHandler : public RecentlyUnuseBundleAgingHandler {
public:
    bool CheckBundle(const AgingBundleInfo &bundle) const override;
    const std::string &GetName() const override;
};

class AgingUninstallReceiveImpl : public StatusReceiverHost {
public:
    AgingUninstallReceiveImpl() = default;
    virtual ~AgingUninstallReceiveImpl() override = default;
    virtual void OnStatusNotify(const int progress) override
    {
    }
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override
    {
    }
};
}  //  namespace AppExecFwk
}  //  namespace OHOS
#endif  //  FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_HANDLER_H
