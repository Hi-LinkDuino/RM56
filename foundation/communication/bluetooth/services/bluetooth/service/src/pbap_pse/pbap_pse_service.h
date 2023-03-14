/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef PBAP_PSE_SERVICE_H
#define PBAP_PSE_SERVICE_H

#include <cstring>
#include <list>
#include <map>
#include <vector>
#include "../obex/obex_headers.h"
#include "../obex/obex_session.h"
#include "../obex/obex_transport.h"
#include "base_def.h"
#include "context.h"
#include "interface_profile_pbap_pse.h"
#include "message.h"
#include "pbap_pse_def.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief phone book server service
 * process connect event
 */
class PbapPseService : public IProfilePbapPse, public utility::Context {
public:
    /**
     * @brief register observer
     * @details register observer for the service of phone book serve
     * @param observer the pointer that point to a PbapPseObserver
     * @return void
     */
    void RegisterObserver(IPbapPseObserver &observer) override;

    /**
     * @brief deregister observer
     * @details deregister observer for the service of phone book serve
     * @param observer the pointer that point to a PbapPseObserver
     * @return void
     */
    void DeregisterObserver(IPbapPseObserver &observer) override;

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return std::vector remote devices
     */
    std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &states) const override;

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return int @c not -1 state of the specified remote device
     *             @c -1 device is not exist
     */
    int GetDeviceState(const RawAddress &device) const override;

    /**
     * @brief constructor
     * @details constructor
     */
    PbapPseService();

    /**
     * @brief deconstructor
     * @details deconstructor
     */
    virtual ~PbapPseService();

    /**
     * @brief get context
     * @details get context
     * @return Context*
     */
    utility::Context *GetContext() override;

    /**
     * @brief start server
     * @details start phone book server
     */
    void Enable() override;

    /**
     * @brief shutdown server
     * @details shutdown phone book server
     */
    void Disable() override;

    /**
     * @brief not used
     * @details not used
     */
    int Connect(const RawAddress &device) override;

    /**
     * @brief disconnect device
     * @details disconnect from remote device
     * @return bool @c  0 success
     *              @c -1 failure
     */
    int Disconnect(const RawAddress &device) override;

    /**
     * @brief not used
     * @details not used
     */
    std::list<RawAddress> GetConnectDevices() override;

    /**
     * @brief Get Connect State
     * @details Get Connect State for base service
     */
    int GetConnectState() override;

    /**
     * @brief Get Max Connect Num
     * @details Get Max Connect Num for base service
     */
    int GetMaxConnectNum() override;

    /**
     * @brief Set the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @param strategy Reference to the connection policy,
     *     @c UNKNOWN : the connection policy for unkown state.
     *     @c ALLOWED : the connection policy for allowed state.
     *     @c FORBIDDEN : the connection policy for forbidden state.
     * @return Returns true if the operation is successful;returns false if the operation fails.
     */
    int SetConnectionStrategy(const RawAddress &device, int strategy) override;

    /**
     * @brief Get the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @return Returns the connection police of the specified bluetooth address.
     */
    int GetConnectionStrategy(const RawAddress &device) const override;

    /**
     * @brief Grant connect permission to device
     *
     * @param device device
     * @param allow allow
     * @param save save
     */
    void GrantPermission(const RawAddress &device, bool allow, bool save = false) override;

    /**
     * @brief Set device's password
     *
     * @param device device
     * @param password device's password
     * @param userId device's userId
     */
    int SetDevicePassword(const RawAddress &device, const std::string &password, std::string userId = "") override;

    /**
     * @brief post message
     * @details post message to self thread
     * @param msg message from other threads
     * @return void
     */
    void PostMessage(utility::Message msg);
    /**
     * @brief Process Obex Request
     *
     * @param what msgId
     * @param device remote address
     */
    void ProcessObexRequest(int what, const RawAddress &device) const;
    /**
     * @brief Process Obex Request
     *
     * @param what msgId
     * @param device remote address
     * @param incomingConnect incoming connect
     */
    void ProcessObexRequest(int what, const RawAddress &device, ObexIncomingConnect &incomingConnect);
    /**
     * @brief Process Obex Request
     *
     * @param what msgId
     * @param device remote address
     * @param session obex session
     * @param req obex request header
     */
    void ProcessObexRequest(int what, const RawAddress &device, ObexServerSession &session, const ObexHeader &req);
    /**
     * @brief Process Obex Busy
     *
     * @param device remote address
     * @param isBusy isBusy
     */
    void ProcessObexBusy(const RawAddress &device, bool isBusy) const;
    /**
     * @brief Get the Next Connect Id object
     *
     * @return uint32_t
     */
    uint32_t GetNextConnectId() const;

private:
    /**
     * @brief enable service
     * @details enable service
     * @return void
     */
    void EnableService();

    /**
     * @brief disable service
     * @details disable service
     * @return void
     */
    void DisableService();

    /**
     * @brief disconnect to server
     * @details disconnect to phone book server
     * @param device  remote device
     */
    void DisConnectInternal(RawAddress device);

    /**
     * @brief incoming process
     * @details when incoming obex server, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessConnectIncoming(const RawAddress &device, const utility::Message &msg);

    /**
     * @brief incoming grant permission process
     * @details when decide accept/reject incoming
     * @param device remote address
     * @param allow accept:true reject:false
     * @param save save
     * @return void
     */
    void ProcessGrantPermission(const RawAddress &device, bool allow, bool save) const;

    /**
     * @brief disconnect after connected
     * @details disconnect after connected
     * @param device remote address
     * @return void
     */
    void ProcessWaitToDisconnect(const RawAddress &device) const;

    /**
     * @brief set password process
     * @details when password set by user
     * @param device remote address
     * @param password password
     * @param userId userId
     * @return void
     */
    void ProcessSetDevicePassword(const RawAddress &device, std::string password, std::string userId) const;

    /**
     * @brief connected process
     * @details when connected obex server, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessObexConnect(const RawAddress &device, const utility::Message &msg) const;

    /**
     * @brief disconnected process
     * @details when disconnected obex server, call it
     * @param device remote address
     * @param msg message from other threads
     * @return void
     */
    void ProcessDeviceDisconnected(const RawAddress &device, const utility::Message &msg) const;

    /**
     * @brief shutdown process
     * @details when shutdown obex server, call it
     * @param msg message from other threads
     * @return void
     */
    void ProcessShutDownComplete(const utility::Message &msg);

    /**
     * @brief Incoming connect Timeout process
     * @details when incoming connect Timeout , call it
     * @param device remote address
     * @return void
     */
    void IncomingTimeout(const RawAddress &device);
    void ProcessIncomingTimeout(const RawAddress &device) const;
    /**
     * @brief save pbap connection policy
     * @details save pbap connection policy
     * @param addr remote address
     * @param strategy strategy
     * @return bool
     */
    bool SaveConnectPolicy(const std::string &addr, int strategy) const;
    /**
     * @brief load pbap connection policy
     * @details load pbap connection policy
     * @param addr remote address
     * @param strategy strategy
     * @return bool
     */
    bool LoadConnectPolicy(const std::string &addr, int &strategy) const;

    /**
     * @brief is all devices disconnected
     * @details is all devices disconnected
     * @return bool
     */
    bool IsAllDisconnected() const;
    /**
     * @brief load adapter config
     * @details load adapter config
     */
    void LoadAdapterConfig() const;
    DECLARE_IMPL();
    BT_DISALLOW_COPY_AND_ASSIGN(PbapPseService);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_SERVICE_H
