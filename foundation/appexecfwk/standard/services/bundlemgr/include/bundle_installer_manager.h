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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_MANAGER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_MANAGER_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "nocopyable.h"
#include "thread_pool.h"

#include "bundle_installer.h"
#include "event_handler.h"
#include "status_receiver_interface.h"

namespace OHOS {
namespace AppExecFwk {
class BundleInstallerManager : public EventHandler {
public:
    explicit BundleInstallerManager(const std::shared_ptr<EventRunner> &runner);
    virtual ~BundleInstallerManager() override;
    /**
     * @brief Process the event of destroy bundle installer object.
     * @param event Indicates the event to be processed.
     * @return
     */
    virtual void ProcessEvent(const InnerEvent::Pointer &event) override;
    /**
     * @brief Create a bundle installer object for installing a bundle.
     * @param bundleFilePath Indicates the path for storing the HAP of the bundle to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateInstallTask(const std::string &bundleFilePath, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for installing a bundle by bundleName.
     * @param bundleName Indicates the bundleName.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateInstallByBundleNameTask(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for installing a bundle by bundleName.
     * @param bundleFilePath Indicates the path for storing the HAP of the bundle to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateRecoverTask(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for installing multiple haps of a bundle.
     * @param bundleFilePaths Indicates the paths for storing the HAPs of the bundle to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return
     */
    void CreateInstallTask(const std::vector<std::string> &bundleFilePaths, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for uninstalling an bundle.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return
     */
    void CreateUninstallTask(
        const std::string &bundleName, const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Create a bundle installer object for uninstalling a module.
     * @param bundleName Indicates the bundle name of the module to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return
     */
    void CreateUninstallTask(const std::string &bundleName, const std::string &modulePackage,
        const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver);
    enum {
        REMOVE_BUNDLE_INSTALLER = 1,
    };

private:
    /**
     * @brief Create a bundle installer object internal.
     * @param statusReceiver Indicates the callback object for this installer.
     * @return Returns a pointers to BundleInstaller object.
     */
    std::shared_ptr<BundleInstaller> CreateInstaller(const sptr<IStatusReceiver> &statusReceiver);
    /**
     * @brief Remove an installer object with the installer ID.
     * @param installerId Indicates the installer ID.
     * @return
     */
    void RemoveInstaller(const int64_t installerId);

private:
    const int MAX_TASK_NUMBER = 10;
    const int THREAD_NUMBER = std::thread::hardware_concurrency();
    // Thread pool used to start multipule installer in parallel.
    ThreadPool installersPool_;
    std::mutex mutex_;
    // map key will use timestamp.
    std::unordered_map<int64_t, std::shared_ptr<BundleInstaller>> installers_;

    DISALLOW_COPY_AND_MOVE(BundleInstallerManager);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_MANAGER_H
