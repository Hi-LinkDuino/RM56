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

#ifndef A2DP_AVDTP_H
#define A2DP_AVDTP_H

#include <cstdint>

#include "a2dp_def.h"
#include "avdtp.h"
#include "btstack.h"
#include "message.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief The class supply the method between A2dp profile with the AVDT protocal
 *
* @since 6.0
 */
class A2dpAvdtp {
public:
    /**
     * @brief A constructor used to create an <b>A2dpAvdtp</b> instance.
     *
     * @param[in] role The role of the local profile
     * @since 6.0
     */
    explicit A2dpAvdtp(const uint8_t role);

    /**
     * @brief A destructor used to delete an <b>A2dpAvdtp</b> instance.
     *
     * @param[in] role The role of the local profile
     * @since 6.0
     */
    ~A2dpAvdtp() = default;

    /**
     * @brief A function to register a2dp information to the avdtp.
     *
     * @param[in] rcb The register information of a2dp profile
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t RegisterAvdtp(const AvdtRegisterParam &rcb);

    /**
     * @brief A function to register a2dp SEP to the avdtp.
     *
     * @param[in] addr The address of peer device
     * @param[in] cfg The sep information of a2dp profile
     * @param[out] handle The handle/seid of SEP of local profile
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t CreateStream(const BtAddr &addr, const uint8_t codecIndex, uint16_t &handle);

    /**
     * @brief A function to register a2dp SEP to the avdtp. Used usually for acceptor of avdtp
     *
     * @param[in] cfg The sep information of a2dp profile
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    uint16_t RegisterLocalSEP(AvdtStreamConfig cfg[], const uint8_t number) const;

    /**
     * @brief A function to create signalling channel to  peer device.
     *
     * @param[in] addr The address of peer device
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    uint16_t ConnectReq(const BtAddr &addr) const;

    /**
     * @brief A function to disconnect the stream.
     *
     * @param[in] addr The address of peer device
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t DisconnectReq(const BtAddr &addr);

    /**
     * @brief A function to get sep list of peer device.
     *
     * @param[in] addr The address of peer device
     * @param[in] maxNum The max number of seps
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t DiscoverReq(const BtAddr &addr, const uint8_t maxNum, uint8_t &label);

    /**
     * @brief A function to get capability of peer device.
     *
     * @param[in] addr The address of peer device
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t GetCapabilityReq(const BtAddr &addr, const uint8_t seid, uint8_t &label);

    /**
     * @brief A function to get all capabilities of peer device.
     *
     * @param[in] addr The address of peer device
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t GetAllCapabilityReq(const BtAddr &addr, const uint8_t seid, uint8_t &label);

    /**
     * @brief A function to configure the stream
     *
     * @param[in] addr The address of peer device
     * @param[in] seid The seid of peer device
     * @param[in] handle The seid of local profile
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t SetConfigureReq(
        const BtAddr &addr, const uint16_t handle, const uint8_t seid, const AvdtSepConfig &cfg, uint8_t &label);

    /**
     * @brief A function to establish stream.
     *
     * @param[in] handle The handle of stream
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t OpenReq(const uint16_t handle, uint8_t &label);

    /**
     * @brief A function to send delay value to source.
     *
     * @param[in] handle The handle of stream
     * @param[in] delayValue The value of delay
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t DelayReq(const uint16_t handle, uint8_t &label, uint16_t delayValue);

    /**
     * @brief A function to close the stream.
     *
     * @param[in] handle The handle of stream
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t CloseReq(const uint16_t handle);

    /**
     * @brief A function to response to the close req.
     *
     * @param[in] handle The handle of stream
     * @param[in] label The label of signalling received
     * @param[in] errCode AVDT_SUCCESS: close ok; otherwise refuse the signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t CloseRsp(const uint16_t handle, const uint8_t label, const uint8_t errCode);

    /**
     * @brief A function to suspend the stream.
     *
     * @param[in] handle The handle of stream
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t SuspendReq(const uint16_t handle, uint8_t &label);

    /**
     * @brief A function to start the stream.
     *
     * @param[in] handle The handle of stream
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t StartReq(const uint16_t handle, uint8_t &label);

    /**
     * @brief A function to response to the suspend req.
     *
     * @param[in] handle The handle of stream
     * @param[in] label The label of signalling received
     * @param[in] errHandle The first handle refused
     * @param[in] errCode AVDT_SUCCESS: close ok; otherwise refuse the signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t SuspendRsp(
        const uint16_t handle, const uint8_t label, const uint16_t errHandle, const uint8_t errCode);

    /**
     * @brief A function to response to the start req.
     *
     * @param[in] handle The handle of stream
     * @param[in] label The label of signalling received
     * @param[in] errHandle The first handle refused
     * @param[in] errCode AVDT_SUCCESS: close ok; otherwise refuse the signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t StartRsp(
        const uint16_t handle, const uint8_t label, const uint16_t errHandle, const uint8_t errCode);

    /**
     * @brief A function to reconfigure the stream.
     *
     * @param[in] handle The handle of stream
     * @param[in] cfg The configure information
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t ReconfigureReq(const uint16_t handle, const AvdtSepConfig &cfg, uint8_t &label);

    /**
     * @brief A function to abort the stream.
     *
     * @param[in] handle The handle of stream
     * @param[out] label The label of signalling
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t AbortReq(const uint16_t handle, uint8_t &label);

    /**
     * @brief A function to response to the configure req.
     *
     * @param[in] handle The handle of stream
     * @param[in] label The label of signalling received
     * @param[in] category Errorcode is zero, configure ok;
     *            Otherwise returen the first category failed and error cause
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t SetConfigureRsp(const uint16_t handle, const uint8_t label, const AvdtCatetory category);

    /**
     * @brief A function to response to the configure req.
     *
     * @param[in] handle The handle of stream
     * @param[in] label The label of signalling received
     * @param[in] category Errorcode is zero, configure ok;
     *            Otherwise return the first category failed and error cause
     * @return A2DP_SUCCESS: Successful; otherwise failed to call avdtp api.
     * @since 6.0
     */
    static uint16_t ReconfigureRsp(const uint16_t handle, const uint8_t label, const AvdtCatetory category);

    /**
     *
     * @brief Function WriteStream
     * @details Send a media packet from SOURCE to the SINK, pts only.
     * @param[in] handle       Handle of stream
     * @param[in] pkt          Stream data
     * @param[in] timeStamp    Timestamp of this stream data sent
     * @param[in] payloadType  Payload type
     * @param[in] marker       Marker: such ad frame boundaries in the data stream
     * @return AVDT_SUCCESS if successful, otherwise error.
     *
     */
    static uint16_t WriteStream(uint16_t handle, Packet *pkt, uint32_t timeStamp, uint8_t payloadType, uint16_t marker);

    /**
     * @brief A function to process the event(indication/confirm) from avdtp.
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] event The indication or confirm from avdtp
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @since 6.0
     */
    static void ProcAvdtpEvent(
        uint16_t handle, const BtAddr *bdAddr, uint8_t event, const AvdtCtrlData *data, uint8_t role);

    /**
     * @brief A function to process event from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] event The indication or confirm from avdtp
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @since 6.0
     */
    A2dpAvdtMsg *ParseAvdtpCallbackContent(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t event, const AvdtCtrlData &data, const uint8_t role) const;

private:
    /**
     * @brief A function to switch from avdtp thread to a2dp service.
     *
     * @param[in] bdAddr The address of peer device
     * @param[in] msgData The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @since 6.0
     */
    static void SwitchThreadToA2dpService(uint8_t role, BtAddr bdAddr, utility::Message msgData);

    /**
     * @brief A function to parse disconnect indication from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] msg The message sent to a2dp profile state machine
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @since 6.0
     */
    static uint8_t ParseAvdtpDisconnectInd(
        const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg);

    /**
     * @brief A function to parse configure indication from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpConfigureInd(const uint16_t handle, const BtAddr bdAddr,
        const AvdtCtrlData &data, const uint8_t role, A2dpAvdtMsg &msg);

    /**
     * @brief A function to parse reconfigure indication from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpReconfigureInd(const uint16_t handle, const BtAddr bdAddr,
        const AvdtCtrlData &data, const uint8_t role, A2dpAvdtMsg &msg);

    /**
     * @brief A function to parse open indication from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpOpenInd(
        const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg);

    /**
     * @brief A function to parse close indication from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpCloseInd(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse close stream channel indication from avdtp
     *
     * @param[in] bdAddr The address of peer device
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpCloseChannelInd(
        const BtAddr bdAddr, const uint16_t handle, const uint8_t role, A2dpAvdtMsg &msg);

    /**
     * @brief A function to parse start indication from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpStartInd(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse suspend indication from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpSuspendInd(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse delay report indication from avdtp
     *
     * @param[in] bdAddr The address of peer device
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     */
    static uint8_t ParseAvdtpDelayReportInd(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);
    /**
     * @brief A function to parse connect confirm from avdtp
     *
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpConnectCFM(const BtAddr bdAddr, const uint8_t role,
        A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse disconnect confirm from avdtp
     *
     * @param[in] bdAddr The address of peer device
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpDisconnectCFM(const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg);

    /**
     * @brief A function to parse discover confirm from avdtp
     *
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpDiscoverCFM(
        const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse get capability confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpGetCapabilityCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse get all capability confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpGetALLCapCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse configure confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpConfigureCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse get configure confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpGetConfigureCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, const A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse open confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpOpenCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse start confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpStartCFM(
        uint16_t handle, BtAddr bdAddr, uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse reconfigure confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpReconfigureCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse suspend confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpSuspendCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse stream close confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpStreamCloseCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse abort confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpAbortCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, const A2dpAvdtMsg &msg, const AvdtCtrlData &data);

    /**
     * @brief A function to parse write confirm from avdtp
     *
     * @param[in] handle The handle of stream(after configured)
     * @param[in] bdAddr The address of peer device
     * @param[in] data The detail information matched event
     * @param[in] role The role of local profile(Acceptor: is confirmed after configure)
     * @param[in] msg The message sent to a2dp profile state machine
     * @since 6.0
     */
    static uint8_t ParseAvdtpWriteCFM(const uint16_t handle, const BtAddr bdAddr,
        const uint8_t role, const A2dpAvdtMsg &msg, const AvdtCtrlData &data);
    A2dpAvdtp() = delete;
    uint8_t peerRole_ = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_AVDTP_H