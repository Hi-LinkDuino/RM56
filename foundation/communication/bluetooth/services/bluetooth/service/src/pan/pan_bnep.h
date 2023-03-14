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

#ifndef PAN_BNEP_H
#define PAN_BNEP_H

#include <queue>

#include "base_def.h"
#include "l2cap_if.h"
#include "gap_if.h"
#include "pan_defines.h"
#include "pan_message.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Class for l2cap connection.
 */
class PanBnep {
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
     * @brief Construct a new PanBnep object.
     */
    explicit PanBnep(std::string address);

    /**
     * @brief Destroy the PanBnep object.
     */
    ~PanBnep();

    /**
     * @brief This function used to connect peer device L2CAP channel.
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
     * @param ethernetHeader The ethernet information.
     * @param length The data length.
     * @param pkt The send data.
     *
     * @return Returns the result.
     */
    int SendData(EthernetHeader ethernetHeader, uint8_t* pkt, int length);

    /**
     * @brief This function used to get the local cid.
     *
     * @return Returns the cid.
     */
    uint16_t GetLcid();

    /**
     * @brief Process l2cap event.
     *
     * @param event The event.
     */

    void ProcessEvent(const PanMessage &event);

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
    static void BnepSecurityCheckCallback(uint16_t result, GapServiceSecurityInfo security, void *context);
    void BnepSecurityCheckTask(uint16_t result);
    /**
     * @brief Incoming connect gap security callback.
     *
     * @param result Indicates the result.
     * @param security Indicates the struct GapServiceSecurityInfo.
     * @param context Indicates the pointer to context.
     */
    static void BnepIndSecurityCheckDeviceConnectCallback(uint16_t result,
        GapServiceSecurityInfo security, void *context);
    void BnepSecurityCheckDeviceConnectTask(uint16_t result);

    /**
     * @brief callback of control channel connect request.
     *
     * @param addr Indicates the pointer to  BtAddr.
     * @param lcid Indicates the lcid.
     * @param result Indicates the result.
     * @param context Indicates the pointer to context.
     */
    static void BnepL2cConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context);

   /**
     * @brief received bredr connect request.
     *
     * @param lcid Indicates the lcid.
     * @param id Indicates the id.
     * @param info Indicates the pointer to L2capConnectionInfo.
     * @param lpsm Indicates the lpsm.
     * @param ctx Indicates the pointer to context.
     */
    static void BnepRecvConnectionReqCallback(
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
    static void BnepRecvConnectionRspCallback(
        uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);

    /**
     * @brief receive config request.
     *
     * @param lcid Indicates the lcid.
     * @param id Indicates the id.
     * @param cfg Indicates the pointer to const L2capConfigInfo.
     * @param ctx Indicates the pointer to context.
     */
    static void BnepRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);

    /**
     * @brief receive config response.
     *
     * @param lcid Indicates the lcid.
     * @param cfg Indicates the pointer to const L2capConfigInfo.
     * @param result Indicates the result.
     * @param ctx Indicates the pointer to context.
     */
    static void BnepRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);

    /**
     * @brief receive disconnect request.
     *
     * @param lcid local channel id.
     * @param lcid Indicates the lcid.
     * @param id Indicates the id.
     * @param ctx Indicates the pointer to context.
     */
    static void BnepRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx);

    /**
     * @brief receive disconnect response.
     *
     * @param lcid Indicates the lcid.
     * @param ctx Indicates the pointer to context.
     */
    static void BnepRecvDisconnectionRspCallback(uint16_t lcid, void *ctx);

    /**
     * @brief receive disconnect abnormal.
     *
     * @param lcid Indicates the lcid.
     * @param reason Indicates the reason.
     * @param ctx Indicates the pointer to context.

     */
    static void BnepDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx);

    /**
     * @brief receive message from peer.
     *
     * @param lcid Indicates the lcid.
     * @param pkt  message data
     * @param ctx Indicates the pointer to context.
     */
    static void BnepRecvDataCallback(uint16_t lcid, Packet *pkt, void *ctx);

    /**
     * @brief remote peer is busy in Enhanced mode.
     *
     * @param lcid Indicates the lcid.
     * @param isBusy busy/unbusy
     * @param ctx Indicates the pointer to context.
     */
    static void BnepRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx);

    // Receive l2cap callback,do the task.
    void BnepRecvConnectionReqCallbackTask(uint16_t lcid, uint8_t id, uint16_t lpsm);
    void BnepRecvConnectionRspCallbackTask(uint16_t lcid, int result);
    void BnepRecvConfigReqCallbackTask(uint16_t lcid, uint8_t id, L2capConfigInfo cfg);
    void BnepRecvConfigRspCallbackTask(uint16_t lcid, const L2capConfigInfo cfg, uint16_t result);
    void BnepRecvDisconnectionReqCallbackTask(uint16_t lcid, uint8_t id);
    void BnepRecvDisconnectionRspCallbackTask(uint16_t lcid);
    void BnepDisconnectAbnormalCallbackTask(uint16_t lcid, uint8_t reason);
    void BnepRecvDataCallbackTask(uint16_t lcid, uint8_t *data, int dataLength);
    void BnepRemoteBusyCallbackTask(uint16_t lcid, uint8_t isBusy);
    int SendGapRequestSecurity(bool isIncoming, uint16_t lcid, uint8_t id);

    int ProcessBnepControlData(uint8_t *data, int dataLength, bool isExtension);
    int ProcessBnepExtensionData(uint8_t *data, int dataLength);
    int ProcessBnepControlCommandNotUnderstood(uint8_t *data, int dataLength);
    int ProcessBnepSetupConnectionRequest(uint8_t *data, int dataLength);
    int ProcessBnepSetupConnectionResponse(uint8_t *data, int dataLength);
    int ProcessBnepFilterNetTypeSet(uint8_t *data, int dataLength);
    int ProcessBnepFilterNetTypeResponse(uint8_t *data, int dataLength);
    int ProcessBnepFilterMultiAddrSet(uint8_t *data, int dataLength);
    int ProcessBnepFilterMultiAddrResponse(uint8_t *data, int dataLength);
    int ProcessBnepEthernetPacketHeader(uint8_t type, EthernetHeader &ethernetHeader,
        uint8_t *data, int dataLength);
    void CheckRoleUuid(Uuid localUuid, Uuid remoteUuid);
    void BnepSendControlCommandNotUnderstood(uint8_t controlCommand);
    void BnepSendSetupConnectionResponse(uint16_t responseMessage);
    void BnepSendFilterNetTypeResponse(uint16_t responseMessage);
    void BnepSendFilterMultiAddrResponse(uint16_t responseMessage);
    int BnepBuildEthernetPacketHeader(uint8_t type, EthernetHeader ethernetHeader, uint8_t *buf);
    bool CheckBnepEthernetDataFilter(EthernetHeader ethernetHeader, uint8_t* pkt, int length);
    void AddPacketToWaitingSendDataList(Packet *addPacket);
    void SendBnepDataEvent(EthernetHeader ethernetHeader, int dataLength, int offset, uint8_t *data);

    // Regist l2cap callback
    static constexpr L2capService BNEP_CALLBACK = {
        BnepRecvConnectionReqCallback,
        BnepRecvConnectionRspCallback,
        BnepRecvConfigReqCallback,
        BnepRecvConfigRspCallback,
        BnepRecvDisconnectionReqCallback,
        BnepRecvDisconnectionRspCallback,
        BnepDisconnectAbnormalCallback,
        BnepRecvDataCallback,
        BnepRemoteBusyCallback
    };

    std::string address_;
    uint8_t state_;      /* Device state */
    uint16_t lcid_;
    uint16_t id_;
    uint8_t connFlags_;
    bool isBusy_;
    std::queue<Packet *> waitingSendDataList_;
    std::vector<BnepProtocolFilterRange> protocolFilterList_;
    std::vector<BnepMultycastFilterRange> multycastFilterList_;

    BT_DISALLOW_COPY_AND_ASSIGN(PanBnep);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PAN_BNEP_H