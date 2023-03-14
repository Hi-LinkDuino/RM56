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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_SERVICE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_SERVICE_H

#include <memory>

#include "installd/installd_host_impl.h"
#include "ipc/installd_host.h"

namespace OHOS {
namespace AppExecFwk {
class InstalldService {
public:
    InstalldService();
    virtual ~InstalldService();
    /**
     * @brief Start the installd service.
     * @return
     */
    void Start();
    /**
     * @brief Stop the installd service.
     * @return
     */
    void Stop();
    /**
     * @brief Check whether if the installd service is ready.
     * @return Returns true if the installd service is ready; returns false otherwise.
     */
    bool IsServiceReady() const
    {
        return isReady_;
    }

private:
    /**
     * @brief Initialize the installd service context.
     * @return Returns true if initialized successfully; returns false otherwise.
     */
    bool Init();
    /**
     * @brief Initialize to create a directory.
     * @param path Indicates the directory path to create.
     * @return Returns true if directory created successfully; returns false otherwise.
     */
    bool InitDir(const std::string &path);

private:
    bool isReady_ = false;
    sptr<InstalldHostImpl> hostImpl_;

    DISALLOW_COPY_AND_MOVE(InstalldService);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_SERVICE_H