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

#include "a2dp_profile.h"
#include <cstring>
#include "a2dp_avdtp.h"
#include "a2dp_codec_thread.h"
#include "a2dp_encoder_sbc.h"
#include "a2dp_service.h"
#include "a2dp_sink.h"
#include "a2dp_source.h"
#include "adapter_config.h"
#include "log.h"
#include "log_util.h"
#include "profile_service_manager.h"
#include "raw_address.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const int CODEC_SBC_TYPE_INDEX = 2;
const int SAMPLE_SBC_INDEX = 3;
const int CODEC_LENGTH = 8;
const int  BIT_MOVE8 = 8;
uint8_t A2dpProfile::g_linkNum = 0;
bool A2dpProfile::g_registGap = false;
std::mutex g_profileMutex;
const std::string A2DP_IDLE_ADDRESS = "00:00:00:00:00:00";

int A2dpProfile::RegisterServiceSecurity(GAP_ServiceConnectDirection direction, GAP_Service serviceId,
    GAP_SecMultiplexingProtocol protocolId, GapSecChannel channelId, uint16_t securityMode)
{
    LOG_INFO("[A2dpGap]%{public}s\n", __func__);

    GapServiceSecurityInfo securityInfo = {direction, serviceId, protocolId, channelId};
    int ret = GAPIF_RegisterServiceSecurity(nullptr, &securityInfo, securityMode);

    return ret;
}

int A2dpProfile::DeregisterServiceSecurity(GAP_ServiceConnectDirection direction, GAP_Service serviceId)
{
    LOG_INFO("[A2dpGap]%{public}s\n", __func__);

    GapServiceSecurityInfo securityInfo = {direction, serviceId, SEC_PROTOCOL_L2CAP, {AVDT_PSM}};
    int ret = GAPIF_DeregisterServiceSecurity(nullptr, &securityInfo);

    return ret;
}

A2dpProfile::A2dpProfile(const uint8_t role)
{
    LOG_INFO("[A2dpProfile]%{public}s role(%u)\n", __func__, role);

    role_ = role;
    sdpInstance_.SetProfileRole(role_);

    packetQueue_ = QueueCreate(MAX_PCM_FRAME_NUM_PER_TICK * FRAME_THREE);
    buffer_ = new A2dpSharedBuffer();
}

A2dpProfile::~A2dpProfile()
{
    LOG_INFO("[A2dpProfile]%{public}s\n", __func__);

    for (const auto &it : peers_) {
        delete it.second;
    }
    if (packetQueue_ != nullptr) {
        QueueDelete(packetQueue_, CleanPacketData);
        packetQueue_ = nullptr;
    }
    if (buffer_ != nullptr) {
        delete buffer_;
        buffer_ = nullptr;
    }
    peers_.clear();
}

uint8_t A2dpProfile::GetRole() const
{
    std::lock_guard<std::mutex> lock(g_profileMutex);

    LOG_INFO("[A2dpProfile]%{public}s role(%u)\n", __func__, role_);

    return role_;
}

bool A2dpProfile::GetProfileEnable() const
{
    std::lock_guard<std::mutex> lock(g_profileMutex);

    LOG_INFO("[A2dpProfile]%{public}s enable(%{public}d)\n", __func__, enable_);

    return enable_;
}

void A2dpProfile::SetProfileEnable(bool value)
{
    std::lock_guard<std::mutex> lock(g_profileMutex);

    LOG_INFO("[A2dpProfile]%{public}s enableValue(%{public}d)\n", __func__, value);

    enable_ = value;
}

void A2dpProfile::SetActivePeer(const BtAddr &peer)
{
    LOG_INFO("[A2dpProfile]%{public}s\n", __func__);
    std::lock_guard<std::mutex> lock(g_profileMutex);
    activePeer_ = peer;
}

const BtAddr &A2dpProfile::GetActivePeer() const
{
    LOG_INFO("[A2dpProfile]%{public}s\n", __func__);
    std::lock_guard<std::mutex> lock(g_profileMutex);
    return activePeer_;
}

void A2dpProfile::Enable()
{
    AvdtRegisterParam avdtRcb = {};
    uint8_t role = GetRole();
    A2dpAvdtp avdtpInstance(role);
    CodecInfo codecInfo = {};

    (void)memset_s(&codecInfo, sizeof(CodecInfo), 0, sizeof(CodecInfo));
    avdtRcb.mtu = A2DP_MAX_AVDTP_MTU_SIZE;
    avdtRcb.role = AVDT_ROLE_SRC;
    avdtpInstance.RegisterAvdtp(avdtRcb);

    if (GetProfileEnable()) {
        return;
    }

    if (!GetGapRegisterInfo()) {
        UpdateGapRegisterInfo(true);
        RegisterServiceSecurity(INCOMING, GAVDP_ACP,
            SEC_PROTOCOL_L2CAP, { .l2capPsm = AVDT_PSM },
            GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_AUTHENTICATION);
        RegisterServiceSecurity(OUTGOING, GAVDP_INT,
            SEC_PROTOCOL_L2CAP, { .l2capPsm = AVDT_PSM },
            GAP_SEC_IN_AUTHENTICATION | GAP_SEC_OUT_AUTHENTICATION);
    }

    // register local stream endpoint
    if (role == A2DP_ROLE_SOURCE) {
        CreateSEPConfigureInfo(A2DP_ROLE_SOURCE);
    } else {
        CreateSEPConfigureInfo(A2DP_ROLE_SINK);
    }

    GetSDPInstance().RegisterService();
    SetProfileEnable(true);
}

void A2dpProfile::ProcessAvdtpCallback(const BtAddr &addr, const utility::Message &message)
{
    A2dpAvdtMsg *msg = static_cast<A2dpAvdtMsg*>(message.arg2_);
    uint8_t role = GetRole();
    uint8_t event = msg->event;
    A2dpProfilePeer *peer = FindOrCreatePeer(addr, role);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpService] %{public}s Failed to get peer instance\n", __func__);
        return;
    }
    A2dpAvdtp avdtpInstance(role);
    A2dpAvdtMsg *msgParsed =
        avdtpInstance.ParseAvdtpCallbackContent(msg->handle, addr, msg->event, msg->a2dpMsg.msg, role);

    if (msgParsed != nullptr) {
        msgParsed->role = role;
        utility::Message data(msgParsed->event, 0, msgParsed);
        LOG_INFO("[A2dpProfile] %{public}s cmd(%u)\n", __func__, msgParsed->event);
        peer->GetStateMachine()->ProcessMessage(data);
        delete msgParsed;
        msgParsed = nullptr;
    } else {
        if (event == AVDT_CONNECT_IND_EVT) {
            DeletePeer(addr);
        }
    }
}

void A2dpProfile::ConnectStateChangedNotify(const BtAddr &addr, const int state, void *context)
{
    LOG_INFO("[A2dpProfile] %{public}s state(%{public}d)\n", __func__, state);
    switch (state) {
        case STREAM_CONNECT_FAILED:
        case STREAM_DISCONNECT:
            ResetDelayValue(addr);
            DeletePeer(addr);
            if (IsActiveDevice(addr)) {
                ClearActiveDevice();
            }
            QueueFlush(packetQueue_, CleanPacketData);
            buffer_->Reset();
            break;
        case STREAM_CONNECT:
            SetActivePeer(addr);
            break;
        default:
            break;
    }
    if (a2dpSvcCBack_ != nullptr) {
        a2dpSvcCBack_->OnConnectStateChanged(addr, state, context);
    } else {
        LOG_ERROR("[A2dpProfile] %{public}s Can't find observer objest \n", __func__);
    }
}

void A2dpProfile::AudioStateChangedNotify(const BtAddr &addr, const int state, void *context) const
{
    LOG_INFO("[A2dpProfile] %{public}s state(%{public}d)\n", __func__, state);
    if (state == A2DP_IS_PLAYING) {
        buffer_->SetValid(true);
    } else {
        buffer_->SetValid(false);
    }
    if (a2dpSvcCBack_ != nullptr) {
        a2dpSvcCBack_->OnAudioStateChanged(addr, state, context);
    } else {
        LOG_ERROR("[A2dpProfile] %{public}s Can't find observer objest \n", __func__);
    }
}

void A2dpProfile::CodecChangedNotify(const BtAddr &addr, void *context) const
{
    LOG_INFO("[A2dpProfile] %{public}s", __func__);
    A2dpProfilePeer *peer = FindPeerByAddress(addr);

    if (a2dpSvcCBack_ != nullptr && peer != nullptr) {
        a2dpSvcCBack_->OnCodecStateChanged(addr, peer->GetCodecStatus(), context);
    } else {
        LOG_ERROR("[A2dpProfile] %{public}s Can't find observer objest \n", __func__);
    }
}

int A2dpProfile::SetUserCodecConfigure(const BtAddr &addr, const A2dpSrcCodecInfo &info)
{
    LOG_INFO("[A2dpProfile] %{public}s", __func__);
    int ret;
    uint8_t role = GetRole();
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpProfile] %{public}s Can't find the instance of profile\n", __func__);
        ret = RET_NO_SUPPORT;
        return ret;
    }

    A2dpProfilePeer *peer = profile->FindPeerByAddress(addr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpProfile] %{public}s Can't find the instance of peer\n", __func__);
        ret = RET_BAD_PARAM;
        return ret;
    }

    ret = peer->SetUserCodecConfigure(info);

    return ret;
}

bool A2dpProfile::IsActiveDevice(const BtAddr &addr) const
{
    bool ret = false;
    RawAddress rawAddress = RawAddress::ConvertToString(addr.addr);
    RawAddress activeAddr = RawAddress::ConvertToString(GetActivePeer().addr);
    std::lock_guard<std::mutex> lock(g_profileMutex);

    HILOGI("[A2dpProfile] current active address(%{public}s)", GetEncryptAddr(activeAddr.GetAddress()).c_str());
    if (strcmp(activeAddr.GetAddress().c_str(), rawAddress.GetAddress().c_str()) == 0) {
        HILOGI("[A2dpProfile] connect address(%{public}s)", GetEncryptAddr(rawAddress.GetAddress()).c_str());
        ret = true;
    }
    return ret;
}

void A2dpProfile::ClearActiveDevice()
{
    std::lock_guard<std::mutex> lock(g_profileMutex);

    (void)memset_s(&activePeer_, sizeof(BtAddr), 0, sizeof(BtAddr));
}

bool A2dpProfile::EnableOptionalCodec(const BtAddr &addr, bool value)
{
    LOG_INFO("[A2dpProfile] %{public}s\n", __func__);
    bool ret = true;
    A2dpProfilePeer *peer = FindPeerByAddress(addr);
    if (peer != nullptr) {
        ret = peer->EnableOptionalCodec(value);
    } else {
        LOG_ERROR("[A2dpProfile] %{public}s Can't get the device of peer\n", __func__);
    }

    return ret;
}


void A2dpProfile::NotifyEncoder(const BtAddr &addr) const
{
    LOG_INFO("[A2dpProfile] %{public}s\n", __func__);
    A2dpProfilePeer *peer = FindPeerByAddress(addr);
    if (peer != nullptr) {
        peer->NotifyEncoder();
    } else {
        LOG_ERROR("[A2dpProfile] %{public}s Can't get the device of peer\n", __func__);
    }
}

void A2dpProfile::NotifyDecoder(const BtAddr &addr) const
{
    LOG_INFO("[A2dpProfile] %{public}s\n", __func__);

    A2dpProfilePeer *peer = FindPeerByAddress(addr);
    if (peer != nullptr) {
        peer->NotifyDecoder();
    } else {
        LOG_ERROR("[A2dpProfile] %{public}s Can't get the device of peer\n", __func__);
    }
}

uint32_t A2dpProfile::SetPcmData(const uint8_t *buf, uint32_t size)
{
    LOG_INFO("[A2dpProfile] %{public}s %{public}u\n", __func__, size);

    uint32_t actualWrittenBytes = buffer_->Write(buf, size);
    LOG_INFO("[A2dpProfile] %{public}s expectedWrittenBytes(%{public}u) actualWrittenBytes(%{public}u)\n",
        __func__, size, actualWrittenBytes);
    if (actualWrittenBytes == 0) {
        LOG_ERROR("[A2dpProfile] %{public}s failed\n", __func__);
    }
    return actualWrittenBytes;
}

uint32_t A2dpProfile::GetPcmData(uint8_t *buf, uint32_t size)
{
    LOG_INFO("[A2dpProfile] %{public}s\n", __func__);

    uint32_t actualReadBytes = buffer_->Read(buf, size);
    LOG_INFO("[A2dpProfile] %{public}s expectedReadBytes(%{public}u) actualReadBytes(%{public}u)\n",
        __func__, size, actualReadBytes);
    if (actualReadBytes == 0) {
        LOG_ERROR("[A2dpProfile] %{public}s failed\n", __func__);
        return false;
    }
    return actualReadBytes;
}

void A2dpProfile::GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp)
{
    LOG_INFO("[A2dpProfile] %{public}s\n", __func__);

    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    delayValue = delayValue_;
    codecThread->GetRenderPosition(sendDataSize, timeStamp);
}

void A2dpProfile::DequeuePacket()
{
    LOG_INFO("[A2dpProfile] %{public}s \n", __func__);
    A2dpProfilePeer *peer = FindOrCreatePeer(activePeer_, A2DP_ROLE_SOURCE);
    if (peer != nullptr) {
        int32_t count = QueueGetSize(packetQueue_);
        LOG_INFO("[A2dpProfile] %{public}s QueueTryDequeue starts, count %{public}d \n", __func__, count);
        PacketData *packetData = (PacketData *)QueueTryDequeue(packetQueue_);
        if (packetData == nullptr) {
            LOG_ERROR("[A2dpProfile] %{public}s no data\n", __func__);
            return;
        }
        count = QueueGetSize(packetQueue_);
        LOG_INFO("[A2dpProfile] %{public}s  packetData is not null, QueueTryDequeue ends, count %{public}d\n",
            __func__, count);
        peer->SendPacket(packetData->packet, packetData->frames, packetData->bytes, packetData->pktTimeStamp);
        PacketFree(packetData->packet);
        free(packetData);
        packetData = nullptr;
    } else {
        LOG_ERROR("[A2dpProfile] %{public}s no peer\n", __func__);
    }
}

void A2dpProfile::EnqueuePacket(const Packet *packet, size_t frames, uint32_t bytes, uint32_t pktTimeStamp)
{
    Packet *refpkt = nullptr;
    PacketData *packetData = nullptr;
    int32_t count = QueueGetSize(packetQueue_);
    LOG_INFO("[A2dpProfile] %{public}s, frame %{public}u, count %{public}d \n", __func__, bytes, count);
    if (count >= MAX_PCM_FRAME_NUM_PER_TICK * FRAME_THREE) {
        QueueFlush(packetQueue_, CleanPacketData);
    }
    count = QueueGetSize(packetQueue_);
    LOG_INFO("[A2dpProfile] %{public}s, after flush count %{public}d \n", __func__, count);
    refpkt = PacketRefMalloc((Packet *)packet);
    packetData = (PacketData *)malloc(sizeof(PacketData));
    if (packetData == nullptr) {
        LOG_ERROR("[A2dpProfile] %{public}s, packetData is NULL\n", __func__);
        return;
    }
    packetData->packet = refpkt;
    packetData->frames = frames;
    packetData->bytes = bytes;
    packetData->pktTimeStamp = pktTimeStamp;

    QueueEnqueue(packetQueue_, (void *)packetData);
    count = QueueGetSize(packetQueue_);
    LOG_INFO("[A2dpProfile] %{public}s ends count %{public}d \n", __func__, count);
}

void A2dpProfile::CreateSEPConfigureInfo(uint8_t role)
{
    AvdtStreamConfig cfg[AVDT_NUM_SEPS] = {};
    A2dpAvdtp avdtp(role);
    uint8_t number = 0;

    for (int i = A2DP_SOURCE_CODEC_INDEX_SBC; i < A2DP_CODEC_INDEX_MAX; i++) {
        int value = 0x01;
        LOG_INFO("[A2dpProfile]%{public}s index (%{public}d)\n", __func__, i);
        if (i < A2DP_SINK_CODEC_INDEX_MIN) {
            cfg[number].sepType = AVDT_ROLE_SRC;
            cfg[number].cfg.mediaType = A2DP_MEDIA_TYPE_AUDIO;
            if (A2DP_SOURCE_CODEC_INDEX_SBC == i) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_CODEC_SBC_SUPPORT, value);
                LOG_INFO("[A2dpProfile] %{public}s SRC SBC value[%{public}d] \n", __func__, value);
                if (!value) {
                    continue;
                }
                BuildCodecInfo(A2DP_SOURCE_CODEC_INDEX_SBC, cfg[number].cfg.codecInfo);
                cfg[number].cfg.numCodec = A2DP_CODEC_SBC_INFO_LEN;
            } else if (A2DP_SOURCE_CODEC_INDEX_AAC == i) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_CODEC_AAC_SUPPORT, value);
                LOG_INFO("[A2dpProfile] %{public}s SRC AAC value[%{public}d] \n", __func__, value);
                if (!value) {
                    continue;
                }
                BuildCodecInfo(A2DP_SOURCE_CODEC_INDEX_AAC, cfg[number].cfg.codecInfo);
                cfg[number].cfg.numCodec = A2DP_CODEC_AAC_INFO_LEN;
            }
            cfg[number].codecIndex = i;
            cfg[number].sinkDataCback = ProcessSinkStream;
            cfg[number].cfg.pscMask = (AVDT_PSC_MSK_TRANS | AVDT_PSC_MSK_DELAY_RPT | AVDT_PSC_MSK_CODEC);
        } else {
            cfg[number].sepType = AVDT_ROLE_SNK;
            cfg[number].cfg.mediaType = A2DP_MEDIA_TYPE_AUDIO;
            if (A2DP_SINK_CODEC_INDEX_SBC == i) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SNK_SERVICE, PROPERTY_CODEC_SBC_SUPPORT, value);
                LOG_INFO("[A2dpProfile] %{public}s SNK SBC value[%{public}d] \n", __func__, value);
                if (!value) {
                    continue;
                }
                BuildCodecInfo(A2DP_SINK_CODEC_INDEX_SBC, cfg[number].cfg.codecInfo);
                cfg[number].cfg.numCodec = A2DP_CODEC_SBC_INFO_LEN;
            } else if (A2DP_SINK_CODEC_INDEX_AAC == i) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SNK_SERVICE, PROPERTY_CODEC_AAC_SUPPORT, value);
                LOG_INFO("[A2dpProfile] %{public}s SNK AAC value[%{public}d] \n", __func__, value);
                if (!value) {
                    continue;
                }
                BuildCodecInfo(A2DP_SINK_CODEC_INDEX_AAC, cfg[number].cfg.codecInfo);
                cfg[number].cfg.numCodec = A2DP_CODEC_AAC_INFO_LEN;
            }
            cfg[number].codecIndex = i;
            cfg[number].sinkDataCback = ProcessSinkStream;
            cfg[number].cfg.pscMask = (AVDT_PSC_MSK_TRANS | AVDT_PSC_MSK_DELAY_RPT | AVDT_PSC_MSK_CODEC);
        }
        number++;
    }
    avdtp.RegisterLocalSEP(cfg, number);
}

void A2dpProfile::BuildCodecInfo(A2dpCodecIndex index, uint8_t *data)
{
    LOG_INFO("[A2dpProfile]%{public}s index(%u)\n", __func__, index);
    switch (index) {
        case A2DP_SINK_CODEC_INDEX_SBC:
            *data++ = A2DP_CODEC_SBC_INFO_LEN;
            *data++ = A2DP_MEDIA_TYPE_AUDIO;
            *data++ = A2DP_CODEC_TYPE_SBC;
            *data++ = A2DP_SBC_SINK_CAPS.sampleFreq | A2DP_SBC_SINK_CAPS.channelMode;
            *data++ = A2DP_SBC_SINK_CAPS.blockLen | A2DP_SBC_SINK_CAPS.numSubbands |
                A2DP_SBC_SINK_CAPS.allocatedMethod;
            *data++ = A2DP_SBC_SINK_CAPS.minBitpool;
            *data = A2DP_SBC_SINK_CAPS.maxBitpool;
            break;
        case A2DP_SOURCE_CODEC_INDEX_SBC:
            *data++ = A2DP_CODEC_SBC_INFO_LEN;
            *data++ = A2DP_MEDIA_TYPE_AUDIO;
            *data++ = A2DP_CODEC_TYPE_SBC;
            *data++ = A2DP_SBC_SOURCE_CAPS.sampleFreq | A2DP_SBC_SOURCE_CAPS.channelMode;
            *data++ = A2DP_SBC_SOURCE_CAPS.blockLen | A2DP_SBC_SOURCE_CAPS.numSubbands |
                A2DP_SBC_SOURCE_CAPS.allocatedMethod;
            *data++ = A2DP_SBC_SOURCE_CAPS.minBitpool;
            *data = A2DP_SBC_SOURCE_CAPS.maxBitpool;
            break;
        case A2DP_SINK_CODEC_INDEX_AAC:
        case A2DP_SOURCE_CODEC_INDEX_AAC:
            *data++ = A2DP_CODEC_AAC_INFO_LEN;
            *data++ = A2DP_MEDIA_TYPE_AUDIO;
            *data++ = A2DP_CODEC_TYPE_AAC;
            *data++ = A2DP_AAC_MPEG2_OBJECT_TYPE;         // object type
            *data++ = A2DP_AAC_SAMPLE_RATE_OCTET1_44100;  // octet1
            *data++ = (A2DP_AAC_SAMPLE_RATE_OCTET2_48000 >> BIT_MOVE8) | A2DP_AAC_CHANNEL_MODE_OCTET2_DOUBLE |
                      A2DP_AAC_CHANNEL_MODE_OCTET2_SINGLE;
            // octet2: b4~7:sampleFrequency b2: dual channels b3: single channel
            *data++ = A2DP_AAC_VARIABLE_BIT_RATE_DISABLED;  // octect3 b7: vbr   b6 ~b0: bitrate22 ~ 16
            *data++ = 0x00;                                 // octet 4 bit rate 8 ~ 15
            *data = A2DP_SAMPLE_BITS_16; // octet 5 bitrate 0 ~7
            break;
        default:
            break;
    }
}

void A2dpProfile::Disable()
{
    LOG_INFO("[A2dpProfile]%{public}s enable(%{public}d) role(%u)\n", __func__, GetProfileEnable(), GetRole());

    A2dpAvdtMsg data = {};
    utility::Message msg(EVT_DISCONNECT_REQ, 0, &data);
    A2dpProfilePeer *peer = nullptr;
    uint8_t role = GetRole();

    if (!GetProfileEnable()) {
        return;
    }
    SetProfileEnable(false);

    for (const auto &it : peers_) {
        peer = it.second;
        LOG_INFO("[A2dpProfile]%{public}s matched peers_addr(%{public}s) [role%u]\n", __func__, it.first.c_str(), role);
        if (peer != nullptr && it.first.c_str() != nullptr) {
            data.a2dpMsg.connectInfo.addr = peer->GetPeerAddress();
            data.role = role;
            msg.arg2_ = &data;
            peer->GetStateMachine()->ProcessMessage(msg);
            peer->SetCurrentCmd(EVT_DISCONNECT_REQ);
            break;
        }
    }
    if (GetGapRegisterInfo()) {
        UpdateGapRegisterInfo(false);
        DeregisterServiceSecurity(INCOMING, GAVDP_ACP);
        DeregisterServiceSecurity(OUTGOING, GAVDP_INT);
    }

    GetSDPInstance().UnregisterService();
    ClearNumberPeerDevice();

    QueueFlush(packetQueue_, CleanPacketData);
    buffer_->Reset();
}

A2dpSdpManager A2dpProfile::GetSDPInstance(void) const
{
    std::lock_guard<std::mutex> lock(g_profileMutex);

    return sdpInstance_;
}
const ConfigureStream &A2dpProfile::GetConfigure() const
{
    LOG_INFO("[A2dpProfile]%{public}s\n", __func__);
    std::lock_guard<std::mutex> lock(g_profileMutex);
    return configureStream_;
}

void A2dpProfile::SetConfigure(const BtAddr &addr)
{
    std::lock_guard<std::mutex> lock(g_profileMutex);

    A2dpProfilePeer *peer = FindPeerByAddress(addr);
    if (peer != nullptr) {
        (void)memset_s(&configureStream_, sizeof(ConfigureStream), 0, sizeof(ConfigureStream));
        configureStream_.cfg = peer->GetPeerCapabilityInfo();
        configureStream_.intSeid = peer->GetIntSeid();
        configureStream_.acpSeid = peer->GetAcpSeid();
        configureStream_.addr = addr;
    }

    LOG_INFO(
        "[A2dpProfile]%{public}s AcpSeid(%u) intseid(%u)\n", __func__, configureStream_.acpSeid, configureStream_.intSeid);
}

int A2dpProfile::Connect(const BtAddr &device)
{
    LOG_INFO("[A2dpProfile]%{public}s\n", __func__);

    A2dpProfile *a2dpInstance = nullptr;
    uint8_t role = GetRole();
    if (role == A2DP_ROLE_SOURCE) {
        a2dpInstance = A2dpSrcProfile::GetInstance();
    } else {
        a2dpInstance = A2dpSnkProfile::GetInstance();
    }
    FindOrCreatePeer(device, role);
    if (GetSDPInstance().FindSnkService(device, a2dpInstance, A2dpProfilePeer::SDPServiceCallback) != RET_NO_ERROR) {
        LOG_WARN("[A2dpProfile]%{public}s SDP_ServiceSearch Error\n", __func__);
    }
    return A2DP_SUCCESS;
}

void A2dpProfile::ProcessSDPCallback(const BtAddr &addr, uint8_t result)
{
    if (result != A2DP_SUCCESS) {
        CallbackParameter param = {A2DP_ROLE_INT, true, 0};
        ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
        LOG_INFO("[A2dpProfile]%{public}s Failed to get SDPresult(%hhu)\n", __func__, result);
        return;
    }

    /// Check sdp information
    uint8_t role = GetRole();
    A2dpAvdtMsg data = {};
    A2dpProfilePeer *peer = FindOrCreatePeer(addr, role);
    utility::Message msg(EVT_CONNECT_REQ, 0, &data);

    data.a2dpMsg.connectInfo.addr = addr;
    data.role = role;
    msg.arg2_ = &data;
    if (peer == nullptr) {
        LOG_ERROR("[A2dpProfile]%{public}s Not resources", __func__);
    } else {
        if (!peer->GetSDPServiceCapability()) {
            msg.arg1_ = EVT_SDP_CFM;
            LOG_ERROR("[A2dpProfile]%{public}s Can't find peer service matched. role(%u)", __func__, data.role);
        }
        peer->GetStateMachine()->ProcessMessage(msg);
        peer->SetCurrentCmd(EVT_CONNECT_REQ);
        peer->SetInitSide(true);
    }
}

int A2dpProfile::Disconnect(const BtAddr &device)
{
    int ret = 0;
    A2dpProfilePeer *peer = nullptr;
    A2dpAvdtMsg data = {};
    utility::Message msg(EVT_DISCONNECT_REQ, 0, &data);

    data.a2dpMsg.connectInfo.addr = device;
    data.role = GetRole();
    msg.arg2_ = &data;
    peer = FindPeerByAddress(device);
    if (peer == nullptr) {
        ret = AVDT_NO_RESOURCES;
        LOG_ERROR("[A2dpProfile]%{public}s Not resources", __func__);
    } else {
        if (peer->GetCurrentCmd() != EVT_SDP_DISC && peer->GetCurrentCmd() != EVT_CONNECT_REQ) {
            ret = peer->GetStateMachine()->ProcessMessage(msg);
            peer->SetCurrentCmd(EVT_DISCONNECT_REQ);
        } else {
            peer->SetDisconnectIndication(true);
        }
    }

    return ret;
}

int A2dpProfile::Stop(const uint16_t handle, const bool suspend)
{
    LOG_INFO("[A2dpProfile]%{public}s handle(%u) suspend(%{public}d)\n", __func__, handle, suspend);
    int ret = RET_NO_ERROR;
    A2dpProfilePeer *peer = nullptr;
    A2dpAvdtMsg data = {};
    utility::Message msg(EVT_SUSPEND_REQ, 0, &data);

    data.a2dpMsg.stream.handle = handle;
    data.role = GetRole();
    msg.arg2_ = &data;

    peer = FindPeerByHandle(handle);
    if (peer == nullptr) {
        ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[A2dpProfile]%{public}s invalid handle(%u) suspend(%{public}d)\n", __func__, handle, suspend);
        return ret;
    }

    if (strcmp(A2DP_PROFILE_STREAMING.c_str(), peer->GetStateMachine()->GetStateName().c_str()) == 0) {
        LOG_INFO("[A2dpProfile]%{public}s streaming exited now\n", __func__);
        peer->GetStateMachine()->ProcessMessage(msg);
    } else {
        LOG_ERROR("[A2dpProfile]%{public}s Not streaming status now\n", __func__);
        ret = AVDT_ERR_UNSUPPORTED_COMMAND;
    }

    QueueFlush(packetQueue_, CleanPacketData);
    buffer_->Reset();
    return ret;
}

int A2dpProfile::Start(const uint16_t handle)
{
    LOG_INFO("[A2dpProfile]%{public}s handle(%hu)\n", __func__, handle);

    int ret = RET_NO_ERROR;
    uint8_t role = GetRole();
    A2dpProfilePeer *peer = nullptr;
    A2dpAvdtMsg data = {};
    utility::Message msg(EVT_START_REQ, 0, &data);

    data.a2dpMsg.stream.handle = handle;
    data.role = role;
    msg.arg2_ = &data;
    peer = FindPeerByHandle(handle);
    if (peer == nullptr) {
        ret = AVDT_BAD_PARAMS;
        return ret;
    }
    if (strcmp(A2DP_PROFILE_STREAMING.c_str(), peer->GetStateMachine()->GetStateName().c_str()) == 0) {
        LOG_INFO("[A2dpProfile]%{public}s The state of stream is streaming now\n", __func__);
        if (role == A2DP_ROLE_SOURCE) {
            peer->NotifyEncoder();
        } else if (role == A2DP_ROLE_SINK) {
            peer->NotifyDecoder();
        } else {
            LOG_ERROR("[A2dpProfile]%{public}s Audio data is not ready\n", __func__);
        }
        return RET_NO_ERROR;
    }
    if (!JudgeAllowedStreaming()) {
        return AVDT_NO_RESOURCES;
    }
    peer->GetStateMachine()->ProcessMessage(msg);

    return ret;
}

bool A2dpProfile::JudgeAllowedStreaming() const
{
    bool ret = true;
    A2dpProfilePeer *peerProfile = nullptr;

    for (const auto &it : peers_) {
        peerProfile = it.second;
        if (peerProfile != nullptr) {
            LOG_INFO("[A2dpProfile]%{public}s state(%{public}s)\n", __func__, peerProfile->GetStateMachine()->GetStateName().c_str());
            if (strcmp(A2DP_PROFILE_STREAMING.c_str(), peerProfile->GetStateMachine()->GetStateName().c_str()) == 0) {
                LOG_ERROR("[A2dpProfile]%{public}s there is one streaming exited now\n", __func__);
                ret = false;
                break;
            }
        }
    }
    return ret;
}

int A2dpProfile::Close(const uint16_t handle) const
{
    int ret = RET_NO_ERROR;
    A2dpProfilePeer *peer = nullptr;
    A2dpAvdtMsg data = {};
    utility::Message msg(EVT_CLOSE_REQ, 0, &data);

    data.a2dpMsg.stream.handle = handle;
    data.role = GetRole();
    msg.arg2_ = &data;
    peer = FindPeerByHandle(handle);
    if (peer == nullptr) {
        ret = AVDT_BAD_PARAMS;
        return ret;
    }

    if (strcmp(A2DP_PROFILE_OPEN.c_str(), peer->GetStateMachine()->GetStateName().c_str()) == 0 ||
        strcmp(A2DP_PROFILE_STREAMING.c_str(), peer->GetStateMachine()->GetStateName().c_str()) == 0) {
        LOG_INFO("[A2dpProfile]%{public}s open/streaming exited now\n", __func__);
        peer->GetStateMachine()->ProcessMessage(msg);
    } else {
        LOG_ERROR("[A2dpProfile]%{public}s Not open/streaming status now\n", __func__);
        ret = AVDT_ERR_UNSUPPORTED_COMMAND;
    }
    return ret;
}


bool A2dpProfile::HasStreaming()
{
    A2dpProfilePeer *peer = nullptr;
    for (const auto &it : peers_) {
        LOG_INFO("[A2dpProfile]%{public}s handle(%u)\n", __func__, it.second->GetStreamHandle());
        peer = it.second;
        if (peer != nullptr) {
            if (strcmp(A2DP_PROFILE_STREAMING.c_str(), peer->GetStateMachine()->GetStateName().c_str()) == 0) {
                LOG_INFO("[A2dpProfile]%{public}s streaming exited now\n", __func__);
                return true;
            }
        }
    }
    return false;
}

bool A2dpProfile::HasOpen()
{
    A2dpProfilePeer *peer = nullptr;
    for (const auto &it : peers_) {
        LOG_INFO("[A2dpProfile]%{public}s \n", __func__);
        peer = it.second;
        if (peer != nullptr) {
            if (strcmp(A2DP_PROFILE_OPEN.c_str(), peer->GetStateMachine()->GetStateName().c_str()) == 0) {
                LOG_INFO("[A2dpProfile]%{public}s open exited now\n", __func__);
                return true;
            }
        }
    }
    return false;
}

void A2dpProfile::SetDisalbeTag(bool tag)
{
    LOG_INFO("[A2dpProfile]%{public}s isDoDisable(%u)\n", __func__, tag);
    isDoDisable_ = tag;
}

bool A2dpProfile::GetDisalbeTag()
{
    LOG_INFO("[A2dpProfile]%{public}s isDoDisable(%u)\n", __func__, isDoDisable_);
    return isDoDisable_;
}

void A2dpProfile::CloseAll()
{
    for (const auto &it : peers_) {
        LOG_INFO("[A2dpProfile]%{public}s handle(%u)\n", __func__, it.second->GetStreamHandle());
        if (it.second != nullptr) {
            Close(it.second->GetStreamHandle());
        }
    }
}

int A2dpProfile::SendDelay(const uint16_t handle, const uint16_t delayValue)
{
    LOG_INFO("[A2dpProfile]%{public}s handle(%u)\n", __func__, handle);
    A2dpAvdtp avdtp(A2DP_ROLE_SINK);
    uint8_t label = 0;

    int ret = avdtp.DelayReq(handle, label, delayValue);
    return ret;
}

int A2dpProfile::CloseRsp(const uint16_t handle, const uint8_t errorCode) const
{
    LOG_INFO("[A2dpProfile]%{public}s handle(%u) role(%u)\n", __func__, handle, GetRole());
    A2dpAvdtp avdtp(GetRole());
    uint8_t label = 0;

    int ret = avdtp.CloseRsp(handle, label, errorCode);
    return ret;
}

int A2dpProfile::WriteStream(const uint16_t handle, const uint8_t config, const uint8_t bitpool)
{
    LOG_INFO("[A2dpProfile]%{public}s handle(%u)\n", __func__, handle);

    A2dpAvdtp avdtp(A2DP_ROLE_SOURCE);
    uint32_t timeStamp = 0x8FF;
    Packet *pkt = PacketMalloc(0, 0, 0);
    uint8_t pktType = 0;
    uint16_t marker = 0x0B;
    uint8_t codecInfo[CODEC_LENGTH] = {0x01, 0x9c, 0x00, 0x35, 0x00, 0x88, 0x88, 0x88};

    codecInfo[CODEC_SBC_TYPE_INDEX] = config;
    codecInfo[SAMPLE_SBC_INDEX] = bitpool;
    Buffer *encBuf = BufferMalloc(CODEC_LENGTH);
    (void)memcpy_s(BufferPtr(encBuf), CODEC_LENGTH, codecInfo, CODEC_LENGTH);
    PacketPayloadAddLast(pkt, encBuf);

    int ret = avdtp.WriteStream(handle, pkt, timeStamp, pktType, marker);
    return ret;
}

int A2dpProfile::Reconfigure(const uint16_t handle, uint8_t *codecInfo) const
{
    LOG_INFO("[A2dpProfile]%{public}s handle(%u)\n", __func__, handle);

    int ret = RET_NO_ERROR;
    A2dpProfilePeer *peer = nullptr;
    A2dpAvdtMsg data = {};
    utility::Message msg(EVT_RECONFIG_REQ, 0, &data);

    peer = FindPeerByHandle(handle);
    if (peer == nullptr) {
        ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[A2dpProfile]%{public}s invalid handle(%u) \n", __func__, handle);
        return ret;
    }

    (void)memcpy_s(data.a2dpMsg.configStream.cfg.codecInfo, A2DP_CODEC_SIZE, codecInfo,
        sizeof(data.a2dpMsg.configStream.cfg.codecInfo));
    data.a2dpMsg.configStream.cfg.mediaType = A2DP_MEDIA_TYPE_AUDIO;
    data.a2dpMsg.configStream.cfg.numCodec = A2DP_CODEC_SBC_INFO_LEN;
    data.a2dpMsg.configStream.cfg.pscMask = AVDT_PSC_MSK_CODEC;
    data.a2dpMsg.configStream.intSeid = handle;
    data.role = GetRole();
    msg.arg2_ = &data;

    if (strcmp(A2DP_PROFILE_OPEN.c_str(), peer->GetStateMachine()->GetStateName().c_str()) == 0) {
        LOG_INFO("[A2dpProfile]%{public}s streaming exited now\n", __func__);
        peer->GetStateMachine()->ProcessMessage(msg);
    } else {
        LOG_ERROR("[A2dpProfile]%{public}s Not open status now\n", __func__);
        ret = AVDT_ERR_UNSUPPORTED_COMMAND;
    }

    return ret;
}

void A2dpProfile::DelayReportNotify(const BtAddr &device, const uint16_t delayValue)
{
    LOG_INFO("[A2dpProfile]%{public}s delayValue(%{public}u)\n", __func__, delayValue);

    A2dpProfilePeer *peer = nullptr;
    peer = FindPeerByAddress(device);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpProfile]%{public}s Not resources", __func__);
        return;
    }

    delayValue_ = delayValue;
}

void A2dpProfile::ResetDelayValue(const BtAddr &device)
{
    LOG_INFO("[A2dpProfile]%{public}s", __func__);

    A2dpProfilePeer *peer = nullptr;
    peer = FindPeerByAddress(device);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpProfile]%{public}s Not resources", __func__);
        return;
    }
    delayValue_ = 0;
}

void A2dpProfile::RegisterObserver(A2dpProfileObserver *observer)
{
    LOG_INFO("[A2dpProfile]%{public}s\n", __func__);

    a2dpSvcCBack_ = observer;
    return;
}

void A2dpProfile::DeregisterObserver(A2dpProfileObserver *observer)
{
    LOG_INFO("[A2dpProfile]%{public}s\n", __func__);
    return;
}

A2dpProfilePeer *A2dpProfile::FindPeerByAddress(const BtAddr &peerAddress) const
{
    HILOGI("[A2dpProfile] addr(%{public}s)",
        GetEncryptAddr(RawAddress::ConvertToString(peerAddress.addr).GetAddress()).c_str());
    A2dpProfilePeer *peer = nullptr;

    for (const auto &it : peers_) {
        if (strcmp(it.first.c_str(), RawAddress::ConvertToString(peerAddress.addr).GetAddress().c_str()) == 0) {
            peer = it.second;
            LOG_INFO("[A2dpProfile]%{public}s matched peers_addr(%{public}s)\n", __func__, it.first.c_str());
            return peer;
        }
        LOG_INFO("[A2dpProfile]%{public}s peers_addr(%{public}s)\n", __func__, it.first.c_str());
    }
    return peer;
}

A2dpProfilePeer *A2dpProfile::FindPeerByHandle(uint16_t handle) const
{
    LOG_INFO("[A2dpProfile]%{public}s handle(%u)\n", __func__, handle);
    A2dpProfilePeer *peer = nullptr;

    for (const auto &it : peers_) {
        LOG_INFO("[A2dpProfile]%{public}s handle(%u)\n", __func__, it.second->GetStreamHandle());
        if (handle == it.second->GetStreamHandle()) {
            peer = it.second;
            break;
        }
    }
    return peer;
}

A2dpProfilePeer *A2dpProfile::FindOrCreatePeer(const BtAddr &peerAddress, uint8_t localRole)
{
    A2dpProfilePeer *peer = nullptr;
    std::string addr = RawAddress::ConvertToString(peerAddress.addr).GetAddress();
    peer = FindPeerByAddress(peerAddress);
    if (peer != nullptr) {
        LOG_DEBUG("[A2dpProfile]%{public}s Peer is exited", __func__);
        return peer;
    }
    if (UpdateNumberPeerDevice(true)) {
        auto peerInstance = std::make_unique<A2dpProfilePeer>(peerAddress, localRole);
        peer = peerInstance.release();
        peers_.insert(std::make_pair(addr, peer));
        HILOGI("addr: %{public}s", GetEncryptAddr(addr).c_str());
    } else {
        HILOGE("Connected device is max: %{public}u", GetConnectedPeerDevice());
    }
    return peer;
}

bool A2dpProfile::DeletePeer(const BtAddr &peerAddress)
{
    bool ret = false;
    std::map<std::string, A2dpProfilePeer *>::iterator it;

    if (IsActiveDevice(peerAddress)) {
        A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
        if (codecThread != nullptr) {
            codecThread->StopA2dpCodecThread();
            delete codecThread;
        }
    }

    for (it = peers_.begin(); it != peers_.end(); it++) {
        if (strcmp(it->first.c_str(), RawAddress::ConvertToString(peerAddress.addr).GetAddress().c_str()) == 0) {
            delete it->second;
            peers_.erase(it);
            UpdateNumberPeerDevice(false);
            ret = true;
            HILOGI("[A2dpProfile] matched peers_addr(%{public}s)",
                GetEncryptAddr(RawAddress::ConvertToString(peerAddress.addr).GetAddress()).c_str());
            break;
        }
        LOG_INFO("[A2dpProfile]%{public}s peers_addr(%{public}s)\n", __func__, it->first.c_str());
    }

    return ret;
}

bool A2dpProfile::UpdateNumberPeerDevice(bool plus)
{
    std::lock_guard<std::mutex> lock(g_profileMutex);
    LOG_INFO("[A2dpProfile]%{public}s plus(%{public}d)\n", __func__, plus);

    if (plus) {
        g_linkNum++;
    } else {
        if (g_linkNum > 0) {
            g_linkNum--;
        }
    }
    if (g_linkNum > A2DP_CONNECT_NUM_MAX) {
        LOG_ERROR("[A2dpProfile]%{public}s linkNum(%u)\n", __func__, g_linkNum);
        g_linkNum = A2DP_CONNECT_NUM_MAX;
        return false;
    }
    LOG_INFO("[A2dpProfile]%{public}s linkNum(%u)\n", __func__, g_linkNum);
    return true;
}

uint8_t A2dpProfile::GetConnectedPeerDevice()
{
    std::lock_guard<std::mutex> lock(g_profileMutex);
    LOG_INFO("[A2dpProfile]%{public}s \n", __func__);

    return g_linkNum;
}

void A2dpProfile::ClearNumberPeerDevice()
{
    std::lock_guard<std::mutex> lock(g_profileMutex);
    LOG_INFO("[A2dpProfile]%{public}s \n", __func__);

    g_linkNum = 0;
}

void A2dpProfile::UpdateGapRegisterInfo(bool value)
{
    std::lock_guard<std::mutex> lock(g_profileMutex);
    LOG_INFO("[A2dpProfile]%{public}s register(%{public}d) \n", __func__, value);

    g_registGap = value;
}

bool A2dpProfile::GetGapRegisterInfo()
{
    std::lock_guard<std::mutex> lock(g_profileMutex);
    LOG_INFO("[A2dpProfile]%{public}s register(%{public}d) \n", __func__, g_registGap);

    return g_registGap;
}

void A2dpProfile::ProcessSignalingTimeoutCallback(const BtAddr &addr) const
{
    LOG_INFO("[A2dpProfile]%{public}s  \n", __func__);

    A2dpProfilePeer *peer = FindPeerByAddress(addr);
    if (peer != nullptr) {
        peer->ProcessSignalingTimeoutCallback();
    }
}

void ProcessSinkStream(uint16_t handle, Packet *pkt, uint32_t timeStamp, uint8_t pt, uint16_t streamHandle)
{
    LOG_INFO("[A2dpProfile]%{public}s  \n", __func__);

    A2dpProfile *profile = GetProfileInstance(A2DP_ROLE_SINK);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpProfile]%{public}s Can't find the instance of profile \n", __func__);
        return;
    }
    A2dpProfilePeer *peer = profile->FindPeerByHandle(handle);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpProfile]%{public}s Can't find the instance of peer device \n", __func__);
        return;
    }
    if (strcmp(A2DP_PROFILE_STREAMING.c_str(), peer->GetStateMachine()->GetStateName().c_str()) != 0) {
        LOG_ERROR("[A2dpProfile]%{public}s Peer statemachine is not Streaming \n", __func__);
        return;
    }

    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    if (codecThread == nullptr) {
        LOG_ERROR("[A2dpProfile]%{public}s Can't find the instance of codec \n", __func__);
        return;
    }

    uint16_t packetSize = PacketSize(pkt);
    if (packetSize == 0) {
        return;
    }
    auto frameData = (uint8_t *)malloc(packetSize);
    if (frameData != nullptr) {
        (void)memset_s(frameData, packetSize, 0, packetSize);
        PacketRead(pkt, frameData, 0, packetSize);
        A2dpEncoderInitPeerParams peerParams = {};
        utility::Message msg(A2DP_FRAME_READY, packetSize, frameData);
        codecThread->PostMessage(msg, peerParams, nullptr, nullptr);
    }
}

void CleanPacketData(void *data)
{
    PacketData *packetData = (PacketData *)data;
    PacketFree(packetData->packet);
    free(packetData);
}
}  // namespace bluetooth
}  // namespace OHOS
