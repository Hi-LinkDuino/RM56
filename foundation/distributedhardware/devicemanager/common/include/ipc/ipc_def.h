/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DEVICE_MANAGER_IPC_DEF_H
#define DEVICE_MANAGER_IPC_DEF_H

namespace OHOS {
namespace DistributedHardware {
#define DEVICE_MANAGER_SERVICE_NAME "dev_mgr_svc"
#define MAX_DM_IPC_LEN 2048

#define DECLARE_IPC_MODEL(className)                  \
public:                                               \
    className() = default;                            \
    virtual ~className() = default;                   \
                                                      \
public:                                               \
    className(const className &) = delete;            \
    className &operator=(const className &) = delete; \
    className(className &&) = delete;                 \
    className &operator=(className &&) = delete

#define DECLARE_IPC_INTERFACE(className) DECLARE_IPC_MODEL(className)

enum IpcCmdID {
    REGISTER_DEVICE_MANAGER_LISTENER = 0,
    UNREGISTER_DEVICE_MANAGER_LISTENER,
    GET_TRUST_DEVICE_LIST,
    GET_LOCAL_DEVICE_INFO,
    GET_UDID_BY_NETWORK,
    GET_UUID_BY_NETWORK,
    START_DEVICE_DISCOVER,
    STOP_DEVICE_DISCOVER,
    AUTHENTICATE_DEVICE,
    UNAUTHENTICATE_DEVICE,
    VERIFY_AUTHENTICATION,
    SERVER_DEVICE_STATE_NOTIFY,
    SERVER_DEVICE_FOUND,
    SERVER_DISCOVER_FINISH,
    SERVER_AUTH_RESULT,
    SERVER_VERIFY_AUTH_RESULT,
    SERVER_GET_DMFA_INFO,
    SERVER_USER_AUTH_OPERATION,
    SERVER_DEVICE_FA_NOTIFY,
    REGISTER_DEV_STATE_CALLBACK,
    UNREGISTER_DEV_STATE_CALLBACK,
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DEVICE_MANAGER_IPC_DEF_H
