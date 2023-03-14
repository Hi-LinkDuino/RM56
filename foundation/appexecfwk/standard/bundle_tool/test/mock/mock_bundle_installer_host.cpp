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

#include "mock_bundle_installer_host.h"

using namespace OHOS::AAFwk;
namespace OHOS {
namespace AppExecFwk {
bool MockBundleInstallerHost::Install(
    const std::string &bundleFilePath, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    APP_LOGI("enter");

    APP_LOGI("bundleFilePath: %{private}s", bundleFilePath.c_str());
    APP_LOGI("installParam.installFlag: %{public}d", installParam.installFlag);

    statusReceiver->OnFinished(OHOS::ERR_OK, MSG_SUCCESS);

    return true;
}

bool MockBundleInstallerHost::Install(const std::vector<std::string> &bundleFilePath, const InstallParam &installParam,
    const sptr<IStatusReceiver> &statusReceiver)
{
    APP_LOGI("enter");

    for_each(bundleFilePath.begin(), bundleFilePath.end(), [](const auto &path)->decltype(auto) {
        APP_LOGI("bundleFilePath: %{private}s", path.c_str());
    });
    APP_LOGI("installParam.installFlag: %{public}d", installParam.installFlag);

    statusReceiver->OnFinished(OHOS::ERR_OK, MSG_SUCCESS);

    return true;
}

bool MockBundleInstallerHost::Uninstall(
    const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    APP_LOGI("enter");

    APP_LOGI("bundleName: %{public}s", bundleName.c_str());
    APP_LOGI("installParam.installFlag: %{public}d", installParam.installFlag);

    statusReceiver->OnFinished(OHOS::ERR_OK, MSG_SUCCESS);

    return true;
}

bool MockBundleInstallerHost::Uninstall(const std::string &bundleName, const std::string &modulePackage,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    APP_LOGI("enter");

    APP_LOGI("bundleName: %{public}s", bundleName.c_str());
    APP_LOGI("modulePackage: %{public}s", modulePackage.c_str());
    APP_LOGI("installParam.installFlag: %{public}d", installParam.installFlag);

    statusReceiver->OnFinished(OHOS::ERR_OK, MSG_SUCCESS);

    return true;
}

bool MockBundleInstallerHost::Recover(const std::string &bundleName, const InstallParam &installParam,
    const sptr<IStatusReceiver> &statusReceiver)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    APP_LOGD("installParam.installFlag: %{public}d", installParam.installFlag);
    statusReceiver->OnFinished(OHOS::ERR_OK, MSG_SUCCESS);
    return true;
}

ErrCode MockBundleInstallerHost::StreamInstall(const std::vector<std::string> &bundleFilePaths,
    const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver)
{
    APP_LOGD("enter");
    statusReceiver->OnFinished(OHOS::ERR_OK, MSG_SUCCESS);
    return OHOS::ERR_OK;
}
} // namespace AppExecFwk
} // namespace OHOS