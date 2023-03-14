/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef HID_HOST_L2CAP_CONNECTION_H
#define HID_HOST_L2CAP_CONNECTION_H

#include "base_def.h"
#include "l2cap_if.h"
#include "gap_if.h"
#include "hid_host_defines.h"
#include "hid_host_message.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for l2cap connection.
 */
class HidHostL2capConnection {
public:
    /**
     * @brief start l2cap connection.
     */
    static int Startup();

    /**
     * @brief close l2cap connection.
     */
    static void Shutdown();

    /**
     * @brief Construct a new HidHostL2capConnection object.
     */
    explicit HidHostL2capConnection(std::string address);

    /**
     * @brief Destroy the HidHostL2capConnection object.
     */
    ~HidHostL2capConnection() = default;

    /**
     * @brief This function used to connect peer device L2CAP channel after SDP discovery OK (Initiator).
     *
     * @return Returns the result of connect.
     */
    int Connect();

    /**
     * @brief This function used to close L2CAP connection.
     *
     * @return Returns the result of disconnect.
     */
    int Disconnect();

    /**
     * @brief This function used to send data to remote device.
     * @param sendData The data information.
     * @param length The data length.
     * @param pkt The send data.
     *
     * @return Returns the result.
     */
    int SendData(SendHidData sendData, int length, uint8_t* pkt);

    /**
     * @brief This function used to get the control channel's local cid.
     *
     * @return Returns the cid.
     */
    uint16_t GetControlLcid();

    /**
     * @brief This function used to get the interrupt channel's local cid.
     *
     * @return Returns the cid.
     */
    uint16_t GetInterruptLcid();

    /**
     * @brief Process l2cap event.
     *
     * @param event The event.
     */

    void ProcessEvent(const HidHostMessage &event);

    /**
     * @brief Get the l2cap event name.
     *
     * @param what event number.
     * @return Returns event string name.
     */
    static std::string GetEventName(int what);

private:
    /**
     * @brief Outgoing connect gap security callback.
     *
     * @param result Indicates the result.
     * @param security Indicates the struct GapServiceSecurityInfo.
     * @param context Indicates the pointer to context.
     */
    static void HidHostSecurityCheckCallback(uint16_t result, GapServiceSecurityInfo security, void *context);
    void HidHostSecurityCheckTask(uint16_t result);
    /**
     * @brief Incoming connect gap security callback.
     *
     * @param result Indicates the result.
     * @param security Indicates the struct GapServiceSecurityInfo.
     * @param context Indicates the pointer to context.
     */
    static void HidHostIndSecurityCheckDeviceConnectCallback(uint16_t result,
        GapServiceSecurityInfo security, void *context);
    void HidHostSecurityCheckDeviceConnectTask(uint16_t result);

    /**
     * @brief callback of control channel connect request.
     *
     * @param addr Indicates the pointer to  BtAddr.
     * @param lcid Indicates the lcid.
     * @param result Indicates the result.
     * @param context Indicates the pointer to context.
     */
    static void HidHostL2cConnectControlReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);

    /**
     * @brief callback of interrupt channel connect request.
     *
     * @param addr Indicates the pointer to  BtAddr.
     * @param lcid Indicates the lcid.
     * @param result Indicates the result.
     * @param context Indicates the pointer to context.
     */
    static void HidHostL2cConnectInterruptReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);

   /**
     * @brief received bredr connect request.
     *
     * @param lcid Indicates the lcid.
     * @param id Indicates the id.
     * @param info Indicates the pointer to L2capConnectionInfo.
     * @param lpsm Indicates the lpsm.
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRecvConnectionReqCallback(
        uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx);

    /**
     * @brief received bredr connect response.
     *
     * @param lcid Indicates the lcid.
     * @param info Indicates the pointer to L2capConnectionInfo.
     * @param result Indicates the result.
     * @param status Indicates the status.
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRecvConnectionRspCallback(
        uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);

    /**
     * @brief receive config request.
     *
     * @param lcid Indicates the lcid.
     * @param id Indicates the id.
     * @param cfg Indicates the pointer to const L2capConfigInfo.
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);

    /**
     * @brief receive config response.
     *
     * @param lcid Indicates the lcid.
     * @param cfg Indicates the pointer to const L2capConfigInfo.
     * @param result Indicates the result.
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);

    /**
     * @brief receive disconnect request.
     *
     * @param lcid local channel id.
     * @param lcid Indicates the lcid.
     * @param id Indicates the id.
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx);

    /**
     * @brief receive disconnect response.
     *
     * @param lcid Indicates the lcid.
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRecvDisconnectionRspCallback(uint16_t lcid, void *ctx);

    /**
     * @brief receive disconnect abnormal.
     *
     * @param lcid Indicates the lcid.
     * @param reason Indicates the reason.
     * @param ctx Indicates the pointer to context.

     */
    static void HidHostDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx);

    /**
     * @brief receive message from peer.
     *
     * @param lcid Indicates the lcid.
     * @param pkt  message data
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRecvDataCallback(uint16_t lcid, Packet *pkt, void *ctx);

    /**
     * @brief remote peer is busy in Enhanced mode.
     *
     * @param lcid Indicates the lcid.
     * @param isBusy busy/unbusy
     * @param ctx Indicates the pointer to context.
     */
    static void HidHostRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx);

    // Receive l2cap callback,do the task.
    void HidHostRecvConnectionReqCallbackTask(uint16_t lcid, uint8_t id, uint16_t lpsm);
    void HidHostRecvConnectionRspCallbackTask(uint16_t lcid, int result);
    void HidHostRecvConfigReqCallbackTask(uint16_t lcid, uint8_t id, L2capConfigInfo cfg);
    void HidHostRecvConfigRspCallbackTask(uint16_t lcid, const L2capConfigInfo cfg, uint16_t result);
    void HidHostRecvDisconnectionReqCallbackTask(uint16_t lcid, uint8_t id);
    void HidHostRecvDisconnectionRspCallbackTask(uint16_t lcid);
    void HidHostDisconnectAbnormalCallbackTask(uint16_t lcid, uint8_t reason);
    void HidHostRemoteBusyCallbackTask(uint16_t lcid, uint8_t isBusy);
    static uint8_t HidHostGetType(uint8_t type);
    static uint8_t HidHostGetParam(uint8_t type);
    int SendGapRequestSecurity(bool isIncoming, uint16_t lcid, uint8_t id);
    void SendGetReport(SendHidData sendData);
    void SendSetReport(SendHidData sendData, int length, uint8_t* pkt);

    // Regist l2cap callback
    static constexpr L2capService L2CAP_HID_Host_CALLBACK = {
        HidHostRecvConnectionReqCallback,
        HidHostRecvConnectionRspCallback,
        HidHostRecvConfigReqCallback,
        HidHostRecvConfigRspCallback,
        HidHostRecvDisconnectionReqCallback,
        HidHostRecvDisconnectionRspCallback,
        HidHostDisconnectAbnormalCallback,
        HidHostRecvDataCallback,
        HidHostRemoteBusyCallback
    };

    std::string address_;
    uint8_t state_;      /* Device state */
    uint16_t ctrlLcid_;
    uint8_t ctrlId_;
    uint16_t intrLcid_;
    uint8_t connFlags_;

    BT_DISALLOW_COPY_AND_ASSIGN(HidHostL2capConnection);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HID_HOST_L2CAP_CONNECTION_H