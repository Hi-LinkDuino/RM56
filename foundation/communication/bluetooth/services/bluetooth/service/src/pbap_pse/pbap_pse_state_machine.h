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

#ifndef PBAP_PSE_STATE_MACHINE_H
#define PBAP_PSE_STATE_MACHINE_H

#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>
#include "../obex/obex_headers.h"
#include "../obex/obex_session.h"
#include "../obex/obex_types.h"
#include "base_def.h"
#include "base_observer_list.h"
#include "bt_def.h"
#include "interface_profile_pbap_pse.h"
#include "pbap_pse_def.h"
#include "pbap_pse_header_msg.h"
#include "pbap_pse_vcard_manager.h"
#include "raw_address.h"
#include "state_machine.h"
#include "timer.h"

namespace OHOS {
namespace bluetooth {
struct PbapPseStateMachineInfo {
    ObexServerSession *obexSession_ = nullptr;
    uint32_t connectId_ = 0;
    // If the PbapPceSupportedFeatures parameter is not present, 0x00000003 shall be assumed for a remote PCE.
    uint32_t pceFeatures_ = 0x00000003;
    bool accepted_ = false;  // is accepted?
    bool isBusy_ = false;    // is busy?
};

struct PbapPsePhoneBookResOpt {
    bool newMissedCall_ = false;
    bool phonebookSize_ = false;
    bool dbIdentifier_ = false;
    bool folderVer_ = false;
};

class PbapPseStateMachine : public utility::StateMachine {
public:
    // const for state machine start
    static const std::string PSE_WAITING_FOR_CONNECT_STATE;
    static const std::string PSE_CONNECTED_STATE;
    static const std::string PSE_CONNECTING_STATE;
    static const std::string PSE_DISCONNECTED_STATE;
    static const std::string PSE_DISCONNECTING_STATE;
    /**
     * @brief constructor
     * construct PseConnecting instance
     */
    explicit PbapPseStateMachine(const RawAddress &device, BaseObserverList<IPbapPseObserver> &observerMgrList,
        std::function<uint32_t()> getNextConnectIdFun);

    virtual ~PbapPseStateMachine();
    /**
     * @brief Init PbapPseStateMachine
     * @details Init PbapPseStateMachine
     */
    void Init();

    /**
     * Start Timer
     * @brief callback callback
     * @details ms time out
     */
    bool StartTimer(std::function<void()> callback, int ms);

    /**
     * Stop Timer
     * @brief callback callback
     * @details ms time out
     */
    void StopTimer();

    /**
     * @brief Get the Connect State object
     *
     * @return BTConnectState
     */
    BTConnectState GetConnectState() const;

    /**
     * @brief get remote address
     * @details get remote address
     * @return RawAddress&
     */
    const RawAddress &GetDevice() const;
    /**
     * @brief Is Connected
     * @details Is Connected
     * @return bool
     */
    bool IsConnected() const;
    /**
     * @brief Is Accepted
     * @details Is Accepted
     * @return bool
     */
    bool IsAccepted() const;
    void SetBusy(bool isBusy);
    bool IsBusy() const;
    PbapPseStateMachineInfo &GetStmInfo();

private:
    PbapPseStateMachineInfo stmInfo_ {};
    const RawAddress device_ {""};  // remote device
    BaseObserverList<IPbapPseObserver> &observerMgrList_;
    std::unique_ptr<utility::Timer> timer_ = nullptr;
    std::function<uint32_t()> getNextConnectIdFun_ {};
    BT_DISALLOW_COPY_AND_ASSIGN(PbapPseStateMachine);
};

/**
 * @brief disconnected statemachine
 * control disconnected state
 */
class PseDisConnected : public utility::StateMachine::State {
public:
    /**
     * @brief constructor
     * construct PceDisconnected instance
     */
    explicit PseDisConnected(
        const std::string &name, PbapPseStateMachine &stm, BaseObserverList<IPbapPseObserver> &observerMgrList);
    virtual ~PseDisConnected() = default;
    /**
     * @brief entry
     * @details when become PseDisConnecting, call this
     * @return void
     */
    void Entry() override;
    /**
     * @brief exit
     * @details when leave PseDisConnecting, call this
     * @return void
     */
    void Exit() override;
    /**
     * @brief Dispatch
     * @details when Dispatch PseDisConnecting, call this
     * @param msg message
     * @return bool true:success false:failure
     */
    bool Dispatch(const utility::Message &msg) override;

private:
    PbapPseStateMachine &stm_;
    BaseObserverList<IPbapPseObserver> &observerMgrList_;
};
/**
 * @brief Connecting statemachine
 * control Connecting state
 */
class PseDisConnecting : public utility::StateMachine::State {
public:
    /**
     * @brief constructor
     * construct PceDisconnected instance
     */
    explicit PseDisConnecting(
        const std::string &name, PbapPseStateMachine &stm, BaseObserverList<IPbapPseObserver> &observerMgrList);
    virtual ~PseDisConnecting() = default;
    /**
     * @brief entry
     * @details when become PseDisConnecting, call this
     * @return void
     */
    void Entry() override;
    /**
     * @brief exit
     * @details when leave PseDisConnecting, call this
     * @return void
     */
    void Exit() override;
    /**
     * @brief Dispatch
     * @details when Dispatch PseDisConnecting, call this
     * @param msg message
     * @return bool true:success false:failure
     */
    bool Dispatch(const utility::Message &msg) override;

private:
    PbapPseStateMachine &stm_;
    BaseObserverList<IPbapPseObserver> &observerMgrList_;
};
/**
 * @brief WaitingForConnect statemachine
 * control WaitingForConnect state
 */
class PseWaitingForConnect : public utility::StateMachine::State {
public:
    /**
     * @brief constructor
     * construct PceDisconnected instance
     */
    explicit PseWaitingForConnect(const std::string &name, PbapPseStateMachine &stm);
    virtual ~PseWaitingForConnect() = default;
    /**
     * @brief entry
     * @details when become WaitingForConnect, call this
     * @return void
     */
    void Entry() override;
    /**
     * @brief exit
     * @details when leave WaitingForConnect, call this
     * @return void
     */
    void Exit() override;
    /**
     * @brief Dispatch
     * @details when Dispatch WaitingForConnect, call this
     * @param msg message
     * @return bool true:success false:failure
     */
    bool Dispatch(const utility::Message &msg) override;

private:
    PbapPseStateMachine &stm_;
    BT_DISALLOW_COPY_AND_ASSIGN(PseWaitingForConnect);
};
/**
 * @brief Connecting statemachine
 * control Connecting state
 */
class PseConnecting : public utility::StateMachine::State {
public:
    /**
     * @brief constructor
     * construct PseConnecting instance
     */
    explicit PseConnecting(const std::string &name, PbapPseStateMachine &stm,
        BaseObserverList<IPbapPseObserver> &observerMgrList, std::function<uint32_t()> &getNextConnectIdFun);
    virtual ~PseConnecting() = default;
    /**
     * @brief entry
     * @details when become PseConnecting, call this
     * @return void
     */
    void Entry() override;
    /**
     * @brief exit
     * @details when leave PseConnecting, call this
     * @return void
     */
    void Exit() override;
    /**
     * @brief Dispatch
     * @details when Dispatch PseConnecting, call this
     * @param msg message
     * @return bool true:success false:failure
     */
    bool Dispatch(const utility::Message &msg) override;

private:
    void CheckConnectReq(const PbapPseObexMessage &obexMsg);
    void CheckAuthChallenges(const ObexHeader &req, const ObexOptionalTlvHeader &authChallenges);
    void PasswordInput(const PbapPsePasswordInputMsg &pwdInputMsg);

    bool CheckConnectHeader(const ObexHeader &req) const;
    void ToDisconnect(ObexRspCode rspCode = ObexRspCode::NOT_ACCEPTABLE);
    void SendSuccessResponse(const std::string &userId = "", const std::string &pwd = "");
    void AcceptConnection();
    void RejectConnection();
    std::vector<uint8_t> authNonce_ {};
    std::vector<uint8_t> authDescription_ {};
    bool authChallenge_ = false;
    uint8_t authUserCharset_ = 0;
    bool authNeedUser_ = false;
    bool authFullAccess_ = false;
    bool waitingDisconnect_ = false;
    PbapPseStateMachine &stm_;
    BaseObserverList<IPbapPseObserver> &observerMgrList_;
    ObexIncomingConnect *incomeConnect_ = nullptr;
    std::function<uint32_t()> &getNextConnectIdFun_;
    BT_DISALLOW_COPY_AND_ASSIGN(PseConnecting);
};

class PseConnected : public utility::StateMachine::State {
public:
    /**
     * @brief constructor
     * construct PseConnecting instance
     */
    explicit PseConnected(
        const std::string &name, PbapPseStateMachine &stm, BaseObserverList<IPbapPseObserver> &observerMgrList);
    virtual ~PseConnected() = default;
    /**
     * @brief entry
     * @details when become disconnected, call this
     * @return void
     */
    void Entry() override;
    /**
     * @brief exit
     * @details when leave disconnected, call this
     * @return void
     */
    void Exit() override;
    /**
     * @brief Dispatch
     * @details when Dispatch PseConnecting, call this
     * @param msg message
     * @return bool true:success false:failure
     */
    bool Dispatch(const utility::Message &msg) override;

private:
    uint32_t GetSupportedFeatures() const;
    void DisconnectObex(const PbapPseObexMessage &obexMsg) const;
    void HandleGetRequest(const PbapPseObexMessage &obexMsg);
    void HandleSetPathRequest(const PbapPseObexMessage &obexMsg);

    void PullPhoneBook(ObexServerSession &obexSession, const ObexHeader &req) const;
    void PullvCardListing(ObexServerSession &obexSession, const ObexHeader &req) const;
    void PullvCardEntry(ObexServerSession &obexSession, const ObexHeader &req) const;
    bool CheckvCardEntryId(std::u16string &entryId) const;
    void SendPhoneBookResponse(ObexServerSession &obexSession, const ObexHeader &req,
        const PbapPseVcardManager::PhoneBookResult &pbResult, const PbapPsePhoneBookResOpt &opt) const;
    bool IsVCardName(std::u16string &name) const;
    std::u16string GetCurrentPath() const;
    std::u16string GetFullPath(const std::vector<std::u16string> &paths) const;
    std::vector<std::u16string> currentPath_ {};  // record current path the client are browsing
    PbapPseStateMachine &stm_;
    BaseObserverList<IPbapPseObserver> &observerMgrList_;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_STATE_MACHINE_H
