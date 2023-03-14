/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_HOST_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_HOST_H

#include <atomic>
#include <memory>
#include <string>

#include "iremote_stub.h"
#include "nocopyable.h"

#include "bundle_installer_interface.h"
#include "bundle_installer_manager.h"
#include "bundle_stream_installer_host_impl.h"

namespace OHOS {
namespace AppExecFwk {
class BundleInstallerHost : public IRemoteStub<IBundleInstaller> {
public:
    BundleInstallerHost();
    virtual ~BundleInstallerHost() override;

    bool Init();
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    /**
     * @brief Installs an application, the final result will be notified from the statusReceiver object.
     * @attention Notice that the bundleFilePath should be an absolute path.
     * @param bundleFilePath Indicates the path for storing the ohos Ability Package (HAP) of the application
     *                       to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Install(const std::string &bundleFilePath, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Installs an application by bundleName, the final result will be notified from the statusReceiver object.
     * @param bundleName Indicates the bundleName of the application to install.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Recover(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Installs multiple haps, the final result will be notified from the statusReceiver object.
     * @attention Notice that the bundleFilePath should be an string vector of absolute paths.
     * @param bundleFilePaths Indicates the paths for storing the ohos Ability Packages (HAP) of the application
     *                       to install or update.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Install(const std::vector<std::string> &bundleFilePaths, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Uninstalls an application, the result will be notified from the statusReceiver object.
     * @param bundleName Indicates the bundle name of the application to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Uninstall(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Uninstalls a module in an application, the result will be notified from the statusReceiver object.
     * @param bundleName Indicates the bundle name of the module to uninstall.
     * @param modulePackage Indicates the module package of the module to uninstall.
     * @param installParam Indicates the uninstall parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the uninstall result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool Uninstall(const std::string &bundleName, const std::string &modulePackage,
        const InstallParam &installParam, const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Installs an app by bundleName, only used in preInstall app.
     * @param bundleName Indicates the bundleName of the application to install.
     * @param installParam Indicates the install parameters.
     * @param statusReceiver Indicates the callback object that using for notifing the install result.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool InstallByBundleName(const std::string &bundleName, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;
    /**
     * @brief Install sandbox application.
     * @param bundleName Indicates the bundle name of the sandbox application to be install.
     * @param dlpType Indicates type of the sandbox application.
     * @param userId Indicates the sandbox application will be installed under which user id.
     * @param appIndex Indicates the appIndex of the sandbox application installed under which user id.
     * @return Returns ERR_OK if the sandbox application is installed successfully; returns errcode otherwise.
     */
    virtual ErrCode InstallSandboxApp(const std::string &bundleName, int32_t dplType, int32_t userId,
        int32_t &appIndex) override;
    /**
     * @brief Uninstall sandbox application.
     * @param bundleName Indicates the bundle name of the sandbox application to be install.
     * @param appIndex Indicates application index of the sandbox application.
     * @param userId Indicates the sandbox application will be uninstall under which user id.
     * @return Returns ERR_OK if the sandbox application is installed successfully; returns errcode otherwise.
     */
    virtual ErrCode UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId) override;

    virtual sptr<IBundleStreamInstaller> CreateStreamInstaller(const InstallParam &installParam) override;
    virtual bool DestoryBundleStreamInstaller(uint32_t streamInstallerId) override;
    virtual ErrCode StreamInstall(const std::vector<std::string> &bundleFilePaths, const InstallParam &installParam,
        const sptr<IStatusReceiver> &statusReceiver) override;

private:
    /**
     * @brief Handles the Install function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return
     */
    void HandleInstallMessage(Parcel &data);
    /**
     * @brief Handles the Install by bundleName function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @return
     */
    void HandleRecoverMessage(Parcel &data);
    /**
     * @brief Handles the Install multiple haps function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return
     */
    void HandleInstallMultipleHapsMessage(Parcel &data);
    /**
     * @brief Handles the Uninstall bundle function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return
     */
    void HandleUninstallMessage(Parcel &data);
    /**
     * @brief Handles the Uninstall module function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return
     */
    void HandleUninstallModuleMessage(Parcel &data);
    /**
     * @brief Handles the InstallSandboxApp function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent.
     * @return Returns true if the sandbox application is installed successfully; returns false otherwise.
     */
    void HandleInstallSandboxApp(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the UninstallSandboxApp function called from a IBundleInstaller proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent.
     * @return Returns true if the sandbox application is installed successfully; returns false otherwise.
     */
    void HandleUninstallSandboxApp(Parcel &data, Parcel &reply);
    /**
     * @brief Check whether the statusReceiver object is valid.
     * @param statusReceiver Indicates the IStatusReceiver object.
     * @return Returns true if the object is valid; returns false otherwise.
     */
    bool CheckBundleInstallerManager(const sptr<IStatusReceiver> &statusReceiver) const;

    void HandleCreateStreamInstaller(Parcel &data, Parcel &reply);
    void HandleDestoryBundleStreamInstaller(Parcel &data, Parcel &reply);

private:
    InstallParam CheckInstallParam(const InstallParam &installParam);
    std::shared_ptr<BundleInstallerManager> manager_;
    std::vector<sptr<IBundleStreamInstaller>> streamInstallers_;
    std::atomic<uint32_t> streamInstallerIds_ = 0;
    std::mutex streamInstallMutex_;

    DISALLOW_COPY_AND_MOVE(BundleInstallerHost);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_INSTALLER_HOST_H