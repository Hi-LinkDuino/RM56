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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_INSTALLD_HOST_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_INSTALLD_HOST_H

#include <string>

#include "iremote_stub.h"

#include "ipc/installd_interface.h"

namespace OHOS {
namespace AppExecFwk {
class InstalldHost : public IRemoteStub<IInstalld> {
public:
    InstalldHost();
    virtual ~InstalldHost() override;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    /**
     * @brief Handles the CreateBundleDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleCreateBundleDir(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the ExtractModuleFiles function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleExtractModuleFiles(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the RenameModuleDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleRenameModuleDir(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the CreateBundleDataDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleCreateBundleDataDir(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the RemoveBundleDataDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleRemoveBundleDataDir(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the RemoveModuleDataDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleRemoveModuleDataDir(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the RemoveDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleRemoveDir(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the CleanBundleDataDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleCleanBundleDataDir(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the CleanBundleDataDir function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleGetBundleStats(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Init private hash map funcMap_.
     */
    void init();
    /**
     * @brief Handles the set dir apl function called from a IInstalld proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleSetDirApl(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handles the all GetBundleCachePath function.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if called successfully; returns false otherwise.
     */
    bool HandleGetBundleCachePath(MessageParcel &data, MessageParcel &reply);

    using InstalldFunc = bool (InstalldHost::*)(MessageParcel &, MessageParcel &);
    std::unordered_map<uint32_t, InstalldFunc> funcMap_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_IPC_INSTALLD_HOST_H