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

#include "a2dp_profile_peer.h"
#include <cstring>
#include "a2dp_avdtp.h"
#include "a2dp_codec_thread.h"
#include "a2dp_service.h"
#include "adapter_config.h"
#include "log.h"
#include "power_manager.h"
#include "profile_config.h"
#include "raw_address.h"
#include "securec.h"
#include "stub/a2dp_data_service.h"

using namespace stub;

namespace OHOS {
namespace bluetooth {
const int CODEC_BIT_MOVE8 = 8;
const int CODEC_BIT_MOVE16 = 16;
const int CODEC_SBC_SAMPLE_INDEX = 3;
const int CODEC_AAC_SAMPLE4 = 4;
const int CODEC_AAC_SAMPLE5 = 5;
const int  CODEC_AAC_BIT6 = 6;
const int  CODEC_AAC_BIT7 = 7;
const int  CODEC_AAC_BIT8 = 8;
const int  PRIORITY_DEFAULT = 1000;
const int  CODEC_INFO_LEN = 10;
const int  A2DP_PEER_3MB = 2;
std::recursive_mutex g_peerMutex {};
A2dpStream::A2dpStream()
{
    LOG_INFO("[A2dpStream] %{public}s\n", __func__);
    (void)memset_s(&codecInfo_, sizeof(CodecInfo), 0, sizeof(CodecInfo));
}

A2dpStream::~A2dpStream()
{
    LOG_INFO("[A2dpStream] %{public}s\n", __func__);
}

void A2dpStream::SetHandle(uint16_t hdl)
{
    LOG_INFO("[A2dpStream] %{public}s handle_(%u)\n", __func__, hdl);
    handle_ = hdl;
}

uint16_t A2dpStream::GetHandle() const
{
    LOG_INFO("[A2dpStream] %{public}s handle_(%u)\n", __func__, handle_);
    return handle_;
}

AvdtSepConfig A2dpStream::GetPeerSEPInformation() const
{
    LOG_INFO("[A2dpStream] %{public}s\n", __func__);
    return peerCapability_;
}

void A2dpStream::SetPeerCapability(AvdtSepConfig cap)
{
    LOG_INFO("[A2dpStream] %{public}s\n", __func__);
    (void)memcpy_s(&peerCapability_, sizeof(AvdtSepConfig), &cap, sizeof(AvdtSepConfig));
}

void A2dpStream::SetCodecInfo(CodecInfo data)
{
    LOG_INFO("[A2dpStream] %{public}s index(%u)\n", __func__, data.codecIndex);
    (void)memcpy_s(&codecInfo_, sizeof(CodecInfo), &data, sizeof(CodecInfo));
}

CodecInfo A2dpStream::GetCodecInfo() const
{
    LOG_INFO("[A2dpStream] %{public}s index(%u)\n", __func__, codecInfo_.codecIndex);
    return codecInfo_;
}

uint8_t A2dpStream::GetAcpSeid() const
{
    LOG_INFO("[A2dpStream] %{public}s acpseid(%u)\n", __func__, acpSeid_);
    return acpSeid_;
}

void A2dpStream::SetPeerCap(bool value)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpStream] %{public}s peerCap(%{public}d)\n", __func__, value);
    peerCap_ = value;
}

bool A2dpStream::GetPeerCap() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpStream] %{public}s peerCap(%{public}d)\n", __func__, peerCap_);
    return peerCap_;
}

void A2dpStream::SetAcpSeid(uint8_t seid)
{
    LOG_INFO("[A2dpStream] %{public}s acpseid(%u)\n", __func__, seid);
    acpSeid_ = seid;
}

void A2dpCodecDecoderObserver::DataAvailable(uint8_t *buf, uint32_t size)
{
    LOG_INFO("[A2dpCodecDecoderObserver] %{public}s )\n", __func__);
    /// Read encoder data
    stub::A2dpService::GetInstance()->setPCMStream((char *)buf, size);
}

A2dpProfilePeer::A2dpProfilePeer(const BtAddr &addr, uint8_t localRole)
{
    A2dpProfilePeerInit(const_cast<BtAddr &>(addr), localRole);

    return;
}

void A2dpProfilePeer::A2dpProfilePeerInit(const BtAddr &addr, const uint8_t localRole)
{
    LOG_INFO("[A2dpProfilePeer] %{public}s localRole(%u)\n", __func__, localRole);

    localRole_ = localRole;
    sourceStreamNum_ = 0;
    sinkStreamNum_ = 0;
    peerAddress_ = addr;
    CreateDefaultCodecPriority();
    RegisterSEPConfigureInfo(addr, localRole);

    auto callbackFunc = std::bind(&A2dpProfilePeer::SignalingTimeoutCallback, this, localRole_);
    signalingTimer_ = std::make_unique<utility::Timer>(callbackFunc);
    auto factoryInstance = std::make_unique<A2dpCodecFactory>(defaultCodecPriorities_);
    codecConfig_ = factoryInstance.release();
}

void A2dpProfilePeer::RegisterSEPConfigureInfo(const BtAddr &addr, uint8_t role)
{
    uint16_t hdl = 0;
    AvdtStreamConfig cfg = {};
    A2dpAvdtp avdtp(role);
    CodecInfo codecData = {};
    cfg.cfg.mediaType = A2DP_MEDIA_TYPE_AUDIO;
    uint8_t *codecInfo = cfg.cfg.codecInfo;
    (void)memset_s(&codecData, sizeof(CodecInfo), 0, sizeof(CodecInfo));

    if (role == A2DP_ROLE_SOURCE) {
        for (int i = A2DP_SOURCE_CODEC_INDEX_SBC; i < A2DP_SOURCE_CODEC_INDEX_MAX; i++) {
            int value = 0x01;
            if (i == A2DP_SOURCE_CODEC_INDEX_SBC) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_CODEC_SBC_SUPPORT, value);
                LOG_INFO("[A2dpProfilePeer] %{public}s SBC value[%{public}d] \n", __func__, value);
                if (value) {
                    A2dpProfile::BuildCodecInfo(A2DP_SOURCE_CODEC_INDEX_SBC, codecInfo);
                } else {
                    continue;
                }
            } else if (i == A2DP_SOURCE_CODEC_INDEX_AAC && codecAACConfig) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_CODEC_AAC_SUPPORT, value);
                LOG_INFO("[A2dpProfilePeer] %{public}s AAC value[%{public}d] \n", __func__, value);
                if (value) {
                    A2dpProfile::BuildCodecInfo(A2DP_SOURCE_CODEC_INDEX_AAC, codecInfo);
                } else {
                    continue;
                }
            } else {
                LOG_INFO("[A2dpProfilePeer] %{public}s AAC configure[%{public}d] \n", __func__, codecAACConfig);
                break;
            }
            cfg.codecIndex = i;
            avdtp.CreateStream(const_cast<BtAddr &>(addr), cfg.codecIndex, hdl);
            streamCtrl_[i - 1].SetHandle(hdl);
            codecData.codecIndex = static_cast<A2dpCodecIndex>(i);
            (void)memcpy_s(codecData.codecInfo, A2DP_CODEC_SIZE, cfg.cfg.codecInfo, A2DP_CODEC_SIZE);
            streamCtrl_[i - 1].SetCodecInfo(codecData);
            sourceStreamNum_++;
        }
    } else {
        for (int i = 0; i < (A2DP_CODEC_INDEX_MAX - A2DP_SINK_CODEC_INDEX_MIN); i++) {
            int value = 0x01;
            if ((i + A2DP_SINK_CODEC_INDEX_MIN) == A2DP_SINK_CODEC_INDEX_SBC) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SNK_SERVICE, PROPERTY_CODEC_SBC_SUPPORT, value);
                LOG_INFO("[A2dpProfilePeer] %{public}s sink SBC value[%{public}d] \n", __func__, value);
                if (value) {
                    A2dpProfile::BuildCodecInfo(A2DP_SINK_CODEC_INDEX_SBC, codecInfo);
                } else {
                    continue;
                }
            } else if ((i + A2DP_SINK_CODEC_INDEX_MIN) == A2DP_SINK_CODEC_INDEX_AAC && codecAACConfig) {
                AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SNK_SERVICE, PROPERTY_CODEC_AAC_SUPPORT, value);
                LOG_INFO("[A2dpProfilePeer] %{public}s sink AAC value[%{public}d] \n", __func__, value);
                if (value) {
                    A2dpProfile::BuildCodecInfo(A2DP_SINK_CODEC_INDEX_AAC, codecInfo);
                } else {
                    continue;
                }
            } else {
                LOG_INFO("[A2dpProfilePeer] %{public}s AAC configure[%{public}d] \n", __func__, codecAACConfig);
                break;
            }
            cfg.codecIndex = i + A2DP_SINK_CODEC_INDEX_MIN;
            avdtp.CreateStream(const_cast<BtAddr &>(addr), cfg.codecIndex, hdl);
            streamCtrl_[i].SetHandle(hdl);
            codecData.codecIndex = static_cast<A2dpCodecIndex>(i + A2DP_SINK_CODEC_INDEX_MIN);
            (void)memcpy_s(codecData.codecInfo, A2DP_CODEC_SIZE, cfg.cfg.codecInfo, A2DP_CODEC_SIZE);
            streamCtrl_[i].SetCodecInfo(codecData);
            sinkStreamNum_++;
        }
    }
    LOG_INFO("[A2dpProfilePeer] %{public}s handle[%u] \n", __func__, GetIntSeid());
}

A2dpProfilePeer::~A2dpProfilePeer()
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);

    if (codecConfig_ != nullptr) {
        delete codecConfig_;
    }
}

void A2dpProfilePeer::SetPeerSepInfo(AvdtDiscover seps)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    for (int i = 0; i < seps.numSeps; i++) {
        peerSepInfo_[i].isUsed = (seps.seps[i]).isUsed;
        peerSepInfo_[i].mediaType = (seps.seps[i]).mediaType;
        peerSepInfo_[i].seid = (seps.seps[i]).seid;
        peerSepInfo_[i].sepType = (seps.seps[i]).sepType;
        LOG_INFO("[A2dpProfilePeer]%{public}s i[%{public}d] acpid[%u] sepNum[%u] septype[%u]\n",
            __func__,
            i,
            peerSepInfo_[i].seid,
            seps.numSeps,
            peerSepInfo_[i].sepType);
    }

    peerNumSeps_ = seps.numSeps;
    peerSepIndex_ = 0;
}

bool A2dpProfilePeer::SetPeerCapInfo(uint8_t intSeid, uint8_t acpSeid, AvdtSepConfig cap, uint8_t role)
{
    bool ret = false;
    uint8_t *codecInfo = cap.codecInfo;

    /// save peer capability
    if (role == A2DP_ROLE_SOURCE) {
        // check valid
        if (!IsPeerSinkCodecValid(codecInfo)) {
            LOG_ERROR("[A2dpProfilePeer]%{public}s IsPeerSinkCodecValid() failed  \n", __func__);
            return false;
        }
        UpdatePeerCapabilityInfo(acpSeid, cap, role);
    } else {
        // check valid
        if (!IsPeerSourceCodecValid(codecInfo)) {
            LOG_ERROR("[A2dpProfilePeer]%{public}s IsPeerSourceCodecValid failed\n", __func__);
            return false;
        }
        UpdatePeerCapabilityInfo(acpSeid, cap, role);
    }
    LOG_INFO("[A2dpProfilePeer]%{public}s SetSinkCodecConfig result(%{public}d)\n", __func__, ret);
    return ret;
}

void A2dpProfilePeer::UpdatePeerCapabilityInfo(uint8_t acpSeid, AvdtSepConfig cap, uint8_t role)
{
    uint8_t *codecInfo = cap.codecInfo;

    if (role == A2DP_ROLE_SOURCE) {
        for (int i = 0; i < sourceStreamNum_; i++) {
            LOG_INFO("[A2dpProfilePeer]%{public}s Local codec index(%u), peercapIndex(%u)\n",
                __func__,
                streamCtrl_[i].GetCodecInfo().codecIndex,
                GetSinkCodecIndex(codecInfo));
            if (streamCtrl_[i].GetCodecInfo().codecIndex == A2DP_SOURCE_CODEC_INDEX_AAC) {
                SetOptionalCodecsSupportState(A2DP_OPTIONAL_SUPPORT);
            }
            if ((streamCtrl_[i].GetCodecInfo().codecIndex + A2DP_SINK_CODEC_INDEX_MIN - 1) ==
                GetSinkCodecIndex(codecInfo)) {
                streamCtrl_[i].SetPeerCapability(cap);
                streamCtrl_[i].SetAcpSeid(acpSeid);
                selectbleStreamIndex_ = i;
                if (!streamCtrl_[i].GetPeerCap()) {
                    SetNumberOfPeerCap(true);
                    streamCtrl_[i].SetPeerCap(true);
                }
                break;
            }
        }
    } else {
        for (int i = 0; i < sinkStreamNum_; i++) {
            LOG_INFO("[A2dpProfilePeer]%{public}s Local(source) codec index(%u) peerCapIndex(%u)\n",
                __func__,
                streamCtrl_[i].GetCodecInfo().codecIndex,
                GetSourceCodecIndex(codecInfo));
            if (streamCtrl_[i].GetCodecInfo().codecIndex == A2DP_SINK_CODEC_INDEX_AAC) {
                SetOptionalCodecsSupportState(A2DP_OPTIONAL_SUPPORT);
            }

            if ((streamCtrl_[i].GetCodecInfo().codecIndex + 1 - A2DP_SINK_CODEC_INDEX_MIN) ==
                GetSourceCodecIndex(codecInfo)) {
                streamCtrl_[i].SetPeerCapability(cap);
                selectbleStreamIndex_ = i;
                streamCtrl_[i].SetAcpSeid(acpSeid);
                if (!streamCtrl_[i].GetPeerCap()) {
                    SetNumberOfPeerCap(true);
                    streamCtrl_[i].SetPeerCap(true);
                }
                break;
            }
        }
    }
}

void A2dpProfilePeer::SetStreamHandle(uint16_t handle)
{
    LOG_INFO("[A2dpProfilePeer] %{public}s handle(%u)\n", __func__, handle);
    streamHandle_ = handle;
}

void A2dpProfilePeer::SetAcpSeid(uint8_t seid)
{
    LOG_INFO("[A2dpProfilePeer] %{public}s seid(%u)\n", __func__, seid);
    acpId_ = seid;
}

void A2dpProfilePeer::SetIntSeid(uint8_t seid)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    LOG_INFO("[A2dpProfilePeer] %{public}s seid(%u)\n", __func__, seid);
    intId_ = seid;
}

uint8_t A2dpProfilePeer::GetAcpSeid() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    LOG_INFO("[A2dpProfilePeer] %{public}s acpId_(%u)\n", __func__, acpId_);
    return acpId_;
}

uint8_t A2dpProfilePeer::GetIntSeid() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer] %{public}s intId_(%u)\n", __func__, intId_);
    return intId_;
}

uint8_t A2dpProfilePeer::GetPeerSepNum() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer] %{public}s peerNumSeps_(%u)\n", __func__, peerNumSeps_);
    return peerNumSeps_;
}

uint16_t A2dpProfilePeer::GetStreamHandle() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer] %{public}s streamHandle_(%u)\n", __func__, streamHandle_);
    return streamHandle_;
}

AvdtSepInfo A2dpProfilePeer::GetPeerSEPInformation() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    AvdtSepInfo sepDetail = {};
    if (peerSepIndex_ < peerNumSeps_) {
        sepDetail = peerSepInfo_[peerSepIndex_];
    }
    return sepDetail;
}

void A2dpProfilePeer::UpdateSepIndex()
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer] %{public}s peerSepIndex_(%u)\n", __func__, peerSepIndex_);
    peerSepIndex_++;
}

BtAddr A2dpProfilePeer::GetPeerAddress() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    return peerAddress_;
}

AvdtSepConfig A2dpProfilePeer::GetPeerCapabilityInfo() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    return streamCtrl_[selectbleStreamIndex_].GetPeerSEPInformation();
}

A2dpStateMachine *A2dpProfilePeer::GetStateMachine()
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    return &stateMachine_;
}

void A2dpProfilePeer::SetSDPServiceCapability(bool value)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    isService_ = value;
}
bool A2dpProfilePeer::GetSDPServiceCapability() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s isService(%{public}d)\n", __func__, isService_);

    return isService_;
}
void A2dpProfilePeer::SetAvdtpVersion(bool value)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    avdtpVer132_ = value;
}

bool A2dpProfilePeer::GetAvdtpVersion() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s avdtpVer132(%{public}d)\n", __func__, avdtpVer132_);

    return avdtpVer132_;
}
void A2dpProfilePeer::SetA2dpVersion(bool value)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    a2dpVer132_ = value;
}

bool A2dpProfilePeer::GetA2dpVersion() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s a2dpVer132(%{public}d)\n", __func__, a2dpVer132_);

    return a2dpVer132_;
}

A2dpCodecFactory *A2dpProfilePeer::GetCodecConfigure() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);

    return codecConfig_;
}

bool A2dpProfilePeer::JudgeCapabilityMatched(uint8_t role)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s role(%u)\n", __func__, role);
    bool ret = false;

    if (role == A2DP_ROLE_SOURCE) {
        for (auto config : codecConfig_->GetIndexedSourceCodecs()) {
            if (FindCapabilityMatched(*config, role)) {
                return true;
            }
        }
    } else {
        for (auto config : codecConfig_->GetIndexedSinkCodecs()) {
            if (FindCapabilityMatched(*config, role)) {
                return true;
            }
        }
    }

    return ret;
}

bool A2dpProfilePeer::FindCapabilityMatched(const A2dpCodecConfig &config, const uint8_t role)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s", __func__);
    uint8_t resultConfig[A2DP_CODEC_SIZE] = {0};
    uint8_t *resConigInfo = &resultConfig[0];
    uint8_t cap[A2DP_CODEC_SIZE] = {0};
    uint8_t *codecInfo = cap;
    bool ret = false;
    if (role == A2DP_ROLE_SOURCE) {
        for (int i = 0; i < sourceStreamNum_; i++) {
            LOG_INFO("[A2dpProfilePeer]%{public}s PriorityIndex(%u), streamIndex(%u)\n",
                __func__, config.GetCodecIndex(),
                streamCtrl_[i].GetCodecInfo().codecIndex);
            if (config.GetCodecIndex() == streamCtrl_[i].GetCodecInfo().codecIndex &&
                streamCtrl_[i].GetAcpSeid() != 0 && GetInitSide()) {
                selectbleStreamIndex_ = i;
                SetIntSeid(streamCtrl_[i].GetHandle());
                SetAcpSeid(streamCtrl_[i].GetAcpSeid());
                (void)memcpy_s(cap, A2DP_CODEC_SIZE,
                    streamCtrl_[i].GetPeerSEPInformation().codecInfo, A2DP_CODEC_SIZE);
                ret = codecConfig_->SetCodecConfig(codecInfo, resConigInfo);
                LOG_INFO("[A2dpProfilePeer]%{public}s ret(%{public}d)\n", __func__, ret);
                return true;
            } else if (config.GetCodecIndex() == streamCtrl_[i].GetCodecInfo().codecIndex &&
                       streamCtrl_[i].GetAcpSeid() != 0) {
                selectbleStreamIndex_ = i;
                SetIntSeid(streamCtrl_[i].GetHandle());
                SetAcpSeid(streamCtrl_[i].GetAcpSeid());
                return true;
            } else {
                LOG_INFO("[A2dpProfilePeer]%{public}s current capability is not matched. \n", __func__);
            }
        }
    } else {
        for (int i = 0; i < sinkStreamNum_; i++) {
            if (config.GetCodecIndex() == streamCtrl_[i].GetCodecInfo().codecIndex &&
                streamCtrl_[i].GetAcpSeid() != 0) {
                selectbleStreamIndex_ = i;
                SetIntSeid(streamCtrl_[i].GetHandle());
                SetAcpSeid(streamCtrl_[i].GetAcpSeid());
                (void)memcpy_s(cap, A2DP_CODEC_SIZE,
                    streamCtrl_[i].GetPeerSEPInformation().codecInfo, A2DP_CODEC_SIZE);
                ret = codecConfig_->SetSinkCodecConfig(codecInfo, resConigInfo);
                LOG_INFO("[A2dpProfilePeer]%{public}s ret(%{public}d) cap(%u)\n", __func__, ret, cap[A2DP_CODEC_TYPE_INDEX]);
                return true;
            }
        }
    }
    return ret;
}

void A2dpProfilePeer::SDPServiceCallback(
    const BtAddr *addr, const SdpService *serviceArray, const uint16_t serviceNum, void *context)
{
    uint8_t role = ((A2dpProfile *)context)->GetRole();
    A2dpService *service = GetServiceInstance(role);
    BtAddr address = *addr;
    int result = static_cast<int>(A2DP_SUCCESS);

    LOG_INFO("[A2dpProfilePeer] %{public}s role(%u) serviceNum(%u)\n", __func__, role, serviceNum);

    if (service == nullptr) {
        LOG_ERROR("[A2dpProfilePeer] %{public}s role(%u): Can't find the service\n", __func__, role);
        return;
    }
    if (serviceNum == 0) {
        result = A2DP_BADPARAM;
    } else {
        /// save the sdp search attributes
        ParseSDPInformation(address, serviceArray, serviceNum, role);
    }
    utility::Message msgData(A2DP_SDP_EVT, result, context);
    RawAddress peerAddr = RawAddress::ConvertToString(address.addr);

    service->PostEvent(msgData, peerAddr);
}

void A2dpProfilePeer::ParseSDPInformation(
    const BtAddr &addr, const SdpService *serviceArray, uint16_t serviceNum, uint8_t role)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s serviceNum(%u)\n", __func__, serviceNum);

    A2dpProfile *instance = GetProfileInstance(role);
    A2dpProfilePeer *profilePeer = instance->FindPeerByAddress(addr);
    if (profilePeer == nullptr) {
        LOG_ERROR("[A2dpProfilePeer]%{public}s Not find peer instance", __func__);
        return;
    }

    for (int i = 0; i < serviceNum; i++) {
        if ((role == A2DP_ROLE_SOURCE && (serviceArray[i].classId->uuid16 == A2DP_SINK_SERVICE_CLASS_UUID)) ||
            (role == A2DP_ROLE_SINK && (serviceArray[i].classId->uuid16 == A2DP_SERVICE_CLASS_UUID))) {
            profilePeer->SetSDPServiceCapability(true);
        } else {
            profilePeer->SetSDPServiceCapability(false);
        }
        if (serviceArray[i].descriptor[0].protocolUuid.uuid16 == A2DP_PROTOCOL_UUID_L2CAP &&
            serviceArray[i].descriptor[0].parameter[0].value == A2DP_PROTOCOL_UUID_AVDTP &&
            serviceArray[i].descriptor[1].protocolUuid.uuid16 == A2DP_PROTOCOL_UUID_AVDTP &&
            serviceArray[i].descriptor[1].parameter[0].value == A2DP_PROFILE_REV_1_3) {
            profilePeer->SetAvdtpVersion(true);
        } else {
            profilePeer->SetAvdtpVersion(false);
        }
        if (serviceArray[i].profileDescriptor[0].profileUuid.uuid16 == A2DP_PROFILE_UUID &&
            serviceArray[i].profileDescriptor[0].versionNumber == A2DP_PROFILE_REV_1_3) {
            profilePeer->SetA2dpVersion(true);
        } else {
            profilePeer->SetA2dpVersion(false);
        }
    }
}

void A2dpProfilePeer::SetSignalingTimer(int ms, bool isPeriodic) const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    if (signalingTimer_ != nullptr) {
        signalingTimer_->Start(ms, isPeriodic);
    }
}

void A2dpProfilePeer::StopSignalingTimer() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    if (signalingTimer_ != nullptr) {
        signalingTimer_->Stop();
    }
}

void A2dpProfilePeer::ProcessSignalingTimeoutCallback()
{
    LOG_INFO("[A2dpProfilePeer]%{public}s failed to cmd(%u)\n", __func__, GetCurrentCmd());

    A2dpAvdtMsg msg = {};
    utility::Message msgData((int)EVT_TIME_OUT, (int)0, &msg);

    if (GetCurrentCmd() != EVT_DISCONNECT_REQ) {
        msg.a2dpMsg.stream.addr = peerAddress_;
        msg.role = localRole_;
        msgData.arg2_ = &msg;
        GetStateMachine()->ProcessMessage(msgData);
    } else {
        /// Do nothing
        LOG_INFO("[A2dpProfilePeer]%{public}s Disconnect Req\n", __func__);
    }
}

void A2dpProfilePeer::SetCurrentCmd(uint8_t cmd)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s cmd(%u)\n", __func__, cmd);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    currentCmd_ = cmd;
}

uint8_t A2dpProfilePeer::GetCurrentCmd() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    LOG_INFO("[A2dpProfilePeer]%{public}s currentCmd(%u)\n", __func__, currentCmd_);
    return currentCmd_;
}

int A2dpProfilePeer::SetUserCodecConfigure(const A2dpSrcCodecInfo &info)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    int ret;
    A2dpCodecCapability userConfig = {};
    uint8_t capability[A2DP_CODEC_SIZE] = {0};
    uint8_t resultCapability[A2DP_CODEC_SIZE] = {0};
    bool reconfig = false;
    A2dpCodecType preCodecIndex = A2DP_CODEC_TYPE_SBC;
    A2dpCodecType codecIndexChanged = A2DP_CODEC_TYPE_SBC;
    uint8_t *codecInfo = capability;

    if (codecConfig_->GetCodecConfig() == nullptr) {
        return RET_BAD_PARAM;
    }
    if (codecConfig_->GetCodecConfig()->GetCodecIndex() != A2DP_SOURCE_CODEC_INDEX_SBC) {
        preCodecIndex = A2DP_CODEC_TYPE_AAC;
    }
    ChangeUserConfigureInfo(userConfig, codecInfo, info);

    uint8_t *resultInfo = resultCapability;
    if (codecConfig_->SetCodecUserConfig(userConfig, codecInfo, resultInfo, &reconfig)) {
        LOG_INFO("[A2dpProfilePeer]%{public}s reconfig(%{public}d) \n", __func__, reconfig);
        if (info.codecType == A2DP_CODEC_TYPE_AAC) {
            codecReconfig_.numCodec = A2DP_CODEC_AAC_INFO_LEN;
        } else {
            codecReconfig_.numCodec = A2DP_CODEC_SBC_INFO_LEN;
        }
        if (reconfig) {
            bool closeStream = false;
            (void)memcpy_s(codecReconfig_.codecInfo, A2DP_CODEC_SIZE, resultCapability, A2DP_CODEC_SIZE);
            codecReconfig_.mediaType = A2DP_MEDIA_TYPE_AUDIO;
            if (codecConfig_->FindSourceCodec(resultInfo) == nullptr) {
                return RET_BAD_PARAM;
            }
            if (codecConfig_->FindSourceCodec(resultInfo)->GetCodecIndex() != A2DP_SOURCE_CODEC_INDEX_SBC) {
                codecIndexChanged = A2DP_CODEC_TYPE_AAC;
            }
            closeStream = (codecIndexChanged == preCodecIndex) ? false : true;
            if (closeStream) {
                codecReconfig_.pscMask = (AVDT_PSC_MSK_TRANS | AVDT_PSC_MSK_DELAY_RPT | AVDT_PSC_MSK_CODEC);
            } else {
                codecReconfig_.pscMask = AVDT_PSC_MSK_CODEC;
            }
            Reconfigure(closeStream);
            NotifyAudioConfigChanged();
        }
        ret = RET_NO_ERROR;
    } else {
        LOG_ERROR("[A2dpProfilePeer]%{public}s Failed to setCodecUserConfig\n", __func__);
        ret = RET_BAD_PARAM;
    }
    return ret;
}

void A2dpProfilePeer::ChangeUserConfigureInfo(
    A2dpCodecCapability &userConfig, uint8_t *codecInfo, const A2dpSrcCodecInfo info)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    A2dpCodecIndex sourceCodecIndex = A2DP_SOURCE_CODEC_INDEX_SBC;

    if (A2DP_CODEC_TYPE_AAC == info.codecType) {
        sourceCodecIndex = A2DP_SOURCE_CODEC_INDEX_AAC;
    }
    userConfig.bitsPerSample = info.bitsPerSample;
    userConfig.channelMode_ = info.channelMode;
    userConfig.codecIndex_ = sourceCodecIndex;
    userConfig.codecPriority_ = A2DP_CODEC_PRIORITY_HIGHEST;
    userConfig.sampleRate_ = info.sampleRate;

    for (int i = 0; i < sourceStreamNum_; i++) {
        if (sourceCodecIndex == streamCtrl_[i].GetCodecInfo().codecIndex) {
            selectbleStreamIndex_ = i;
            (void)memcpy_s(codecInfo, A2DP_CODEC_SIZE,
                streamCtrl_[i].GetPeerSEPInformation().codecInfo, A2DP_CODEC_SIZE);
            if (info.codecType == A2DP_CODEC_TYPE_SBC) {
                *(codecInfo + CODEC_SBC_SAMPLE_INDEX) = (userConfig.sampleRate_ | userConfig.channelMode_);
                LOG_INFO("[A2dpProfilePeer]%{public}s  SBC Freq&chan(%u)\n", __func__, *(codecInfo + CODEC_SBC_SAMPLE_INDEX));
            } else {
                *(codecInfo + CODEC_AAC_SAMPLE4) = userConfig.sampleRate_;
                *(codecInfo + CODEC_AAC_SAMPLE5) =
                    ((userConfig.sampleRate_ >> CODEC_BIT_MOVE8) | userConfig.channelMode_);
                *(codecInfo + CODEC_AAC_BIT6) = userConfig.bitsPerSample >> CODEC_BIT_MOVE16;
                *(codecInfo + CODEC_AAC_BIT7) = userConfig.bitsPerSample >> CODEC_BIT_MOVE8;
                *(codecInfo + CODEC_AAC_BIT8) = userConfig.bitsPerSample;
                LOG_INFO("[A2dpProfilePeer]%{public}s  AAC Freq(%u)\n", __func__, *(codecInfo + CODEC_AAC_SAMPLE4));
                LOG_INFO("[A2dpProfilePeer]%{public}s  AAC Freq&chan(%u)\n", __func__,
                    *(codecInfo + CODEC_AAC_SAMPLE5));
            }
            break;
        }
    }
}

bool A2dpProfilePeer::EnableOptionalCodec(bool value)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s value(%{public}d)\n", __func__, value);
    bool ret = true;
    A2dpCodecCapability userConfig = {};
    bool reconfig = false;
    uint8_t capability[A2DP_CODEC_SIZE] = {0};
    uint8_t resultCapability[A2DP_CODEC_SIZE] = {0};
    uint8_t *capInfo = nullptr;
    uint8_t *resultInfo = nullptr;
    capInfo = capability;
    resultInfo = resultCapability;

    if (value) {
        SelectCodecInfo(userConfig, capInfo, A2DP_SOURCE_CODEC_INDEX_AAC);
    } else {
        SelectCodecInfo(userConfig, capInfo, A2DP_SOURCE_CODEC_INDEX_SBC);
    }
    userConfig.codecPriority_ = A2DP_CODEC_PRIORITY_HIGHEST;
    ret = codecConfig_->SetCodecUserConfig(userConfig, capInfo, resultInfo, &reconfig);
    for (int i = 0; i < CODEC_INFO_LEN; i++) {
        LOG_INFO("[A2dpProfilePeer]%{public}s capInfo(%u)\n", __func__, *(capInfo + i));
        LOG_INFO("[A2dpProfilePeer]%{public}s resultInfo(%u)\n", __func__, *(resultInfo + i));
    }

    if (ret && reconfig) {
        (void)memcpy_s(codecReconfig_.codecInfo, A2DP_CODEC_SIZE, resultCapability, A2DP_CODEC_SIZE);
        codecReconfig_.mediaType = A2DP_MEDIA_TYPE_AUDIO;
        if (value) {
            codecReconfig_.numCodec = A2DP_CODEC_AAC_INFO_LEN;
        } else {
            codecReconfig_.numCodec = A2DP_CODEC_SBC_INFO_LEN;
        }
        codecReconfig_.pscMask = (AVDT_PSC_MSK_TRANS | AVDT_PSC_MSK_DELAY_RPT | AVDT_PSC_MSK_CODEC);
        Reconfigure(true);
    }
    return ret;
}

void A2dpProfilePeer::SelectCodecInfo(A2dpCodecCapability &userConfig, uint8_t *codecInfo, A2dpCodecIndex index)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s \n", __func__);

    userConfig.codecIndex_ = index;
    for (int i = 0; i < sourceStreamNum_; i++) {
        if (index == streamCtrl_[i].GetCodecInfo().codecIndex) {
            selectbleStreamIndex_ = i;
            (void)memcpy_s(codecInfo, A2DP_CODEC_SIZE,
                streamCtrl_[i].GetPeerSEPInformation().codecInfo, A2DP_CODEC_SIZE);
            if (index == A2DP_SOURCE_CODEC_INDEX_SBC) {
                userConfig.bitsPerSample = *(codecInfo + CODEC_AAC_SAMPLE5);
                userConfig.channelMode_ = *(codecInfo + CODEC_SBC_SAMPLE_INDEX) & 0x0F;
                userConfig.sampleRate_ = *(codecInfo + CODEC_SBC_SAMPLE_INDEX) & 0xF0;
            } else {
                userConfig.bitsPerSample = *(codecInfo + CODEC_AAC_BIT6) & 0x7F;
                userConfig.bitsPerSample =
                    (userConfig.bitsPerSample << CODEC_BIT_MOVE8) | *(codecInfo + CODEC_AAC_BIT7);
                userConfig.bitsPerSample =
                    (userConfig.bitsPerSample << CODEC_BIT_MOVE8) | *(codecInfo + CODEC_AAC_BIT8);
                userConfig.channelMode_ = *(codecInfo + CODEC_AAC_SAMPLE5) & 0x0C;
                userConfig.sampleRate_ = *(codecInfo + CODEC_AAC_SAMPLE5) & 0xF0;
                userConfig.sampleRate_ =
                    (userConfig.sampleRate_ << CODEC_BIT_MOVE8) | *(codecInfo + CODEC_AAC_SAMPLE4);
            }
            break;
        }
    }
}

void A2dpProfilePeer::NotifyEncoder()
{
    LOG_INFO("[A2dpProfilePeer]%{public}s \n", __func__);
    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    utility::Message msg(A2DP_PCM_ENCODED, localRole_, nullptr);
    A2dpEncoderInitPeerParams peerParams = {};
    A2dpCodecConfig *config = nullptr;

    if (codecThread != nullptr) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::BUSY, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(peerAddress_.addr));
        if (edr_) {
            peerParams.isPeerEdr = true;
        } else {
            peerParams.isPeerEdr = false;
        }
        if (edr_ & A2DP_PEER_3MB) {
            peerParams.peerSupports3mbps = true;
        } else {
            peerParams.peerSupports3mbps = false;
        }
        peerParams.peermtu = mtu_;
        config = codecConfig_->GetCodecConfig();
        LOG_INFO("[A2dpProfilePeer]%{public}s edr(%{public}d) 3Mb(%{public}d) \n", __func__, peerParams.isPeerEdr,
            peerParams.peerSupports3mbps);
        codecThread->ProcessMessage(msg, peerParams, config, nullptr);
    } else {
        LOG_ERROR("[A2dpProfilePeer]%{public}s Can't find the instance of codec \n", __func__);
    }
}

void A2dpProfilePeer::NotifyDecoder()
{
    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    utility::Message msg(A2DP_FRAME_DECODED, localRole_, nullptr);
    A2dpCodecConfig *config = nullptr;
    A2dpEncoderInitPeerParams peerParams = {};

    if (codecThread != nullptr) {
        config = codecConfig_->GetCodecConfig();
        if (decoderObserver_ == nullptr) {
            decoderObserver_ = std::make_unique<A2dpCodecDecoderObserver>();
        }
        codecThread->ProcessMessage(msg, peerParams, config, decoderObserver_.get());
    } else {
        LOG_ERROR("[A2dpProfilePeer]%{public}s Can't find the instance of codec \n", __func__);
    }
}

void A2dpProfilePeer::NotifyAudioConfigChanged() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s \n", __func__);
    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    utility::Message msg(A2DP_AUDIO_RECONFIGURE, localRole_, nullptr);
    A2dpEncoderInitPeerParams peerParams = {};

    if (codecThread != nullptr) {
        codecThread->ProcessMessage(msg, peerParams, nullptr, nullptr);
    } else {
        LOG_ERROR("[A2dpProfilePeer]%{public}s Can't find the instance of codec \n", __func__);
    }
}

void A2dpProfilePeer::UpdateConfigure()
{
    LOG_INFO("[A2dpProfilePeer]%{public}s intId_(%u) acpId_(%u)\n", __func__, GetIntSeid(), GetAcpSeid());

    A2dpAvdtMsg msgData = {};
    msgData.a2dpMsg.configStream.addr = peerAddress_;
    msgData.a2dpMsg.configStream.intSeid = GetIntSeid();
    msgData.a2dpMsg.configStream.acpSeid = GetAcpSeid();
    msgData.role = localRole_;
    (void)memcpy_s(&msgData.a2dpMsg.configStream.cfg,
        sizeof(ConfigureStream), &codecReconfig_, sizeof(ConfigureStream));
    utility::Message msg(EVT_SETCONFIG_REQ, localRole_, &msgData);
    restart_ = false;
    stateMachine_.ProcessMessage(msg);
}

AvdtSepConfig A2dpProfilePeer::GetReconfig() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s \n", __func__);
    return codecReconfig_;
}

bool A2dpProfilePeer::GetRestart() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s restart_(%{public}d)\n", __func__, restart_);
    return restart_;
}

void A2dpProfilePeer::SetRestart(bool value)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s value(%{public}d)\n", __func__, value);
    restart_ = value;
}

bool A2dpProfilePeer::GetReconfigTag() const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s reconfigTag_(%{public}d)\n", __func__, reconfigTag_);
    return reconfigTag_;
}

void A2dpProfilePeer::SetReconfigTag(bool value)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s value(%{public}d)\n", __func__, value);
    reconfigTag_ = value;
}

const ConfigureStream &A2dpProfilePeer::GetConfigure() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    return configureStream_;
}

void A2dpProfilePeer::SetConfigure()
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    A2dpCodecConfig *curConfig = nullptr;
    uint8_t codecInfo[A2DP_CODEC_SIZE];
    uint8_t *curCodec = codecInfo;
    (void)memset_s(codecInfo, A2DP_CODEC_SIZE, 0, A2DP_CODEC_SIZE);
    (void)memset_s(&configureStream_, sizeof(ConfigureStream), 0, sizeof(ConfigureStream));

    curConfig = codecConfig_->GetCodecConfig();
    curConfig->CopyOutOtaCodecConfig(curCodec);
    (void)memcpy_s(configureStream_.cfg.codecInfo, A2DP_CODEC_SIZE, codecInfo, A2DP_CODEC_SIZE);
    configureStream_.cfg.mediaType = streamCtrl_[selectbleStreamIndex_].GetPeerSEPInformation().mediaType;
    configureStream_.cfg.numCodec = streamCtrl_[selectbleStreamIndex_].GetPeerSEPInformation().numCodec;
    configureStream_.cfg.pscMask = streamCtrl_[selectbleStreamIndex_].GetPeerSEPInformation().pscMask;
    configureStream_.intSeid = intId_;
    configureStream_.acpSeid = acpId_;
    configureStream_.addr = peerAddress_;

    LOG_INFO(
        "[A2dpProfilePeer]%{public}s AcpSeid(%u) intseid(%u)\n", __func__, configureStream_.acpSeid, configureStream_.intSeid);
}

void A2dpProfilePeer::SetNumberOfPeerCap(bool value)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    if (value) {
        capNumber_++;
    }
}

void A2dpProfilePeer::UpdatePeerMtu(uint16_t mtu)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s peer mtu size(%u) mtu_(%u)\n", __func__, mtu, mtu_);
    if ((mtu >= 0) && (mtu < A2DP_MAX_AVDTP_MTU_SIZE)) {
        mtu_ = mtu;
    } else {
        mtu_ = A2DP_MAX_AVDTP_MTU_SIZE;
    }
}

void A2dpProfilePeer::UpdatePeerEdr(uint8_t edr)
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s peer edr(%u) edr_(%u)\n", __func__, edr, edr_);
    edr_ = edr;
}

bool A2dpProfilePeer::GetPeerCapComplete(uint8_t role) const
{
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);
    LOG_INFO("[A2dpProfilePeer]%{public}s capNumber_(%u)\n", __func__, capNumber_);

    if (role == A2DP_ROLE_SOURCE) {
        if (capNumber_ >= sourceStreamNum_) {
            return true;
        }
    } else {
        if (capNumber_ >= sinkStreamNum_) {
            return true;
        }
    }
    return false;
}

void A2dpProfilePeer::SetInitSide(bool value)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s (%{public}d)\n", __func__, value);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    isInitSide_ = value;
}

bool A2dpProfilePeer::GetInitSide() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s (%{public}d)\n", __func__, isInitSide_);
    std::lock_guard<std::recursive_mutex> lock(g_peerMutex);

    return isInitSide_;
}

void A2dpProfilePeer::SetDisconnectIndication(bool value)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s (%{public}d)\n", __func__, value);

    disconnectInd_ = value;
}

bool A2dpProfilePeer::GetDisconnectIndication() const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s\n", __func__);

    return disconnectInd_;
}

A2dpSrcCodecStatus A2dpProfilePeer::GetCodecStatus() const
{
    A2dpSrcCodecStatus codecStatus = {};
    A2dpSrcCodecInfo localCap = {};
    A2dpSrcCodecInfo selectableCap = {};
    auto config = codecConfig_->GetCodecConfig();
    if (config != nullptr) {
        uint8_t codecType = 0;
        LOG_INFO("[A2dpProfilePeer] %{public}s [codecIndex:%u]\n", __func__, config->GetCodecConfig().codecIndex_);
        if (A2DP_SOURCE_CODEC_INDEX_SBC == config->GetCodecConfig().codecIndex_ ||
            A2DP_SINK_CODEC_INDEX_SBC == config->GetCodecConfig().codecIndex_) {
            codecType = A2DP_CODEC_TYPE_SBC;
        } else {
            codecType = A2DP_CODEC_TYPE_AAC;
        }
        codecStatus.codecInfo.bitsPerSample = config->GetCodecConfig().bitsPerSample;
        codecStatus.codecInfo.channelMode = config->GetCodecConfig().channelMode_;
        codecStatus.codecInfo.codecPriority = config->GetCodecConfig().codecPriority_;
        codecStatus.codecInfo.sampleRate = config->GetCodecConfig().sampleRate_;
        codecStatus.codecInfo.codecType = codecType;
        LOG_INFO("[A2dpProfilePeer] %{public}s bitsPerSample(%u) \n", __func__, codecStatus.codecInfo.bitsPerSample);
        LOG_INFO("[A2dpProfilePeer] %{public}s channelMode(%u) \n", __func__, codecStatus.codecInfo.channelMode);
        LOG_INFO("[A2dpProfilePeer] %{public}s sampleRate(%u) \n", __func__, codecStatus.codecInfo.sampleRate);
        LOG_INFO("[A2dpProfilePeer] %{public}s codecType(%u) \n", __func__, codecStatus.codecInfo.codecType);
        LOG_INFO("[A2dpProfilePeer] %{public}s codecPriority(%u) \n", __func__, codecStatus.codecInfo.codecPriority);
        LOG_INFO("[A2dpProfilePeer] %{public}s get local cap info\n", __func__);
        localCap.bitsPerSample = config->GetCodecLocalCapability().bitsPerSample;
        localCap.channelMode = config->GetCodecLocalCapability().channelMode_;
        localCap.codecPriority = config->GetCodecLocalCapability().codecPriority_;
        localCap.sampleRate = config->GetCodecLocalCapability().sampleRate_;
        localCap.codecType = codecType;
        codecStatus.codecInfoLocalCap.push_back(localCap);
        LOG_INFO("[A2dpProfilePeer] %{public}s bitsPerSample(%u) \n", __func__, localCap.bitsPerSample);
        LOG_INFO("[A2dpProfilePeer] %{public}s channelMode(%u) \n", __func__, localCap.channelMode);
        LOG_INFO("[A2dpProfilePeer] %{public}s sampleRate(%u) \n", __func__, localCap.sampleRate);
        LOG_INFO("[A2dpProfilePeer] %{public}s codecType(%u) \n", __func__, localCap.codecType);
        LOG_INFO("[A2dpProfilePeer] %{public}s get peer cap info\n", __func__);
        for (int i = 0; i < sourceStreamNum_; i++) {
            FindPeerSelectableCapabilityMatched(selectableCap, streamCtrl_[i].GetPeerSEPInformation());
            codecStatus.codecInfoConfirmedCap.push_back(selectableCap);
        }
    }

    return codecStatus;
}

void A2dpProfilePeer::FindPeerSelectableCapabilityMatched(A2dpSrcCodecInfo &selectableCap, AvdtSepConfig cap)
{
    uint8_t *codecInfo = cap.codecInfo;

    for (int i = 0; i < CODEC_INFO_LEN; i++) {
        LOG_INFO("[A2dpProfilePeer]%{public}s codec[%u](0x:%x) \n", __func__, i, *(codecInfo + i));
    }
    if (*codecInfo++ == A2DP_CODEC_SBC_INFO_LEN) {
        selectableCap.codecType = A2DP_CODEC_TYPE_SBC;
        selectableCap.bitsPerSample = A2DP_SAMPLE_BITS_16;
        codecInfo++;  // media type
        codecInfo++;  // codec type
        selectableCap.sampleRate = *codecInfo & A2DP_SBC_SAMPLE_RATE_MSK;
        selectableCap.channelMode = *codecInfo & A2DP_SBC_CHANNEL_MODE_MSK;
    } else {
        uint32_t sampleRate = 0;
        selectableCap.codecType = A2DP_CODEC_TYPE_AAC;
        codecInfo++;  // media type
        codecInfo++;  // codec type
        codecInfo++;  // object type
        sampleRate = (*codecInfo++ & A2DP_AAC_SAMPLE_RATE_OCTET1_MSK);
        LOG_INFO("[A2dpProfilePeer] %{public}s sampleRateLow(0x:%x) \n", __func__, sampleRate);
        selectableCap.sampleRate = ((*codecInfo) & A2DP_AAC_SAMPLE_RATE_OCTET2_MSK) << CODEC_BIT_MOVE8;
        LOG_INFO("[A2dpProfilePeer] %{public}s sampleRateHigh(0x:%x) \n", __func__, selectableCap.sampleRate);
        selectableCap.sampleRate = selectableCap.sampleRate | sampleRate;
        selectableCap.channelMode = *codecInfo++ & A2DP_AAC_CHANNEL_MODE_OCTET2_MSK;
        selectableCap.bitsPerSample = (*codecInfo++ & 0x7F) << CODEC_BIT_MOVE16;
        selectableCap.bitsPerSample = selectableCap.bitsPerSample | (*codecInfo++ << CODEC_BIT_MOVE8);
        selectableCap.bitsPerSample = selectableCap.bitsPerSample | *codecInfo;
        // need to adjust the bitpersample
    }

    LOG_INFO("[A2dpProfilePeer] %{public}s bitsPerSample(0x:%x) \n", __func__, selectableCap.bitsPerSample);
    LOG_INFO("[A2dpProfilePeer] %{public}s channelMode(0x:%x) \n", __func__, selectableCap.channelMode);
    LOG_INFO("[A2dpProfilePeer] %{public}s sampleRate(0x:%x) \n", __func__, selectableCap.sampleRate);
    LOG_INFO("[A2dpProfilePeer] %{public}s codecType(0x:%x) \n", __func__, selectableCap.codecType);
}

void A2dpProfilePeer::Reconfigure(bool close)
{
    LOG_INFO("[A2dpProfilePeer]%{public}s intId_(%u)\n", __func__, GetIntSeid());
    A2dpAvdtMsg msgData = {};
    msgData.role = localRole_;
    const uint8_t *codecInfo = codecReconfig_.codecInfo;
    A2dpStateMachine *stateMachine = GetStateMachine();
    SetRestart(true);
    if (close) {
        msgData.a2dpMsg.stream.handle = GetIntSeid();
        utility::Message msg(EVT_CLOSE_REQ, localRole_, &msgData);
        stateMachine->ProcessMessage(msg);
        SetCurrentCmd(EVT_CLOSE_REQ);
        SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);
        for (int i = 0; i < AVDT_NUM_SEPS; i++) {
            if (codecConfig_->FindSourceCodec(codecInfo) == nullptr) {
                continue;
            }
            if (codecConfig_->FindSourceCodec(codecInfo)->GetCodecIndex() ==
                streamCtrl_[i].GetCodecInfo().codecIndex) {
                SetIntSeid(streamCtrl_[i].GetHandle());
                SetAcpSeid(streamCtrl_[i].GetAcpSeid());
                LOG_INFO("[A2dpProfilePeer]%{public}s intId_(%u)\n", __func__, GetIntSeid());
                break;
            }
            if (i >= (AVDT_NUM_SEPS - 1)) {
                LOG_ERROR("[A2dpProfilePeer]%{public}s Can't fine stream index(%{public}d) \n", __func__, i);
            }
        }
    } else {
        if (strcmp(stateMachine->GetStateName().c_str(), A2DP_PROFILE_OPEN.c_str()) == 0) {
            msgData.a2dpMsg.configStream.intSeid = GetIntSeid();
            msgData.a2dpMsg.configStream.cfg = codecReconfig_;
            utility::Message msg(EVT_RECONFIG_REQ, localRole_, &msgData);
            SetRestart(false);
            stateMachine->ProcessMessage(msg);
            SetCurrentCmd(EVT_RECONFIG_REQ);
            SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);
        } else if (strcmp(stateMachine->GetStateName().c_str(), A2DP_PROFILE_STREAMING.c_str()) == 0) {
            msgData.a2dpMsg.stream.handle = GetIntSeid();
            utility::Message msg(EVT_SUSPEND_REQ, localRole_, &msgData);
            stateMachine->ProcessMessage(msg);
            SetCurrentCmd(EVT_SUSPEND_REQ);
            SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);
        } else {
            LOG_WARN(
                "[A2dpProfilePeer]%{public}s  The state(%{public}s) is no correct\n", __func__, stateMachine->GetStateName().c_str());
        }
    }
}

void A2dpProfilePeer::SignalingTimeoutCallback(uint8_t role) const
{
    LOG_INFO("[A2dpProfilePeer]%{public}s \n", __func__);
    A2dpService *service = GetServiceInstance(role);
    if (service != nullptr) {
        utility::Message msgData(A2DP_TIMEOUT_EVT, role, nullptr);
        RawAddress peerAddr = RawAddress::ConvertToString(peerAddress_.addr);
        service->PostEvent(msgData, peerAddr);
    }
}

void A2dpProfilePeer::SetOptionalCodecsSupportState(int state) const
{
    LOG_INFO("[A2dpService] %{public}s state(%{public}d)\n", __func__, state);

    IProfileConfig *config = ProfileConfig::GetInstance();
    if (state == (int)A2DP_OPTIONAL_SUPPORT_UNKNOWN) {
        config->RemoveProperty(RawAddress::ConvertToString(peerAddress_.addr).GetAddress().c_str(),
            SECTION_CODE_CS_SUPPORT,
            PROPERTY_A2DP_SUPPORTS_OPTIONAL_CODECS);
    } else {
        config->SetValue(RawAddress::ConvertToString(peerAddress_.addr).GetAddress().c_str(),
            SECTION_CODE_CS_SUPPORT,
            PROPERTY_A2DP_SUPPORTS_OPTIONAL_CODECS,
            state);
    }
}

void A2dpProfilePeer::CreateDefaultCodecPriority()
{
    LOG_INFO("[A2dpProfilePeer]%{public}s  \n", __func__);
    int value = 0;
    bool result = false;
    result = AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_CODEC_SBC, value);
    LOG_INFO("[A2dpProfilePeer]%{public}s SourceSBC Config(%{public}d) priority(%{public}d) \n",
        __func__, result, value);
    if (result) {
        defaultCodecPriorities_.insert(
            std::make_pair(A2DP_SOURCE_CODEC_INDEX_SBC, A2dpCodecPriority(value * PRIORITY_DEFAULT + 1)));
    } else {
        defaultCodecPriorities_.insert(
            std::make_pair(A2DP_SOURCE_CODEC_INDEX_SBC, A2dpCodecPriority(PRIORITY_DEFAULT + 1)));
    }

    result = AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SRC_SERVICE, PROPERTY_CODEC_AAC, value);
    LOG_INFO("[A2dpProfilePeer]%{public}s SourceAAC Config(%{public}d) priority(%{public}d) \n", __func__, result, value);
    if (result) {
        if (localRole_ == A2DP_ROLE_SOURCE) {
            codecAACConfig = true;
        }
        defaultCodecPriorities_.insert(
            std::make_pair(A2DP_SOURCE_CODEC_INDEX_AAC, A2dpCodecPriority(value * PRIORITY_DEFAULT + 1)));
    }

    // get sink codec configure
    result = AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SNK_SERVICE, PROPERTY_CODEC_SBC, value);
    LOG_INFO("[A2dpProfilePeer]%{public}s SinkSBC Config(%{public}d) priority(%{public}d) \n",
        __func__, result, value);
    if (result) {
        defaultCodecPriorities_.insert(
            std::make_pair(A2DP_SINK_CODEC_INDEX_SBC, A2dpCodecPriority(value * PRIORITY_DEFAULT + 1)));
    } else {
        defaultCodecPriorities_.insert(
            std::make_pair(A2DP_SINK_CODEC_INDEX_SBC,
                A2dpCodecPriority(PRIORITY_DEFAULT * A2DP_SINK_CODEC_INDEX_SBC + 1)));
    }

    result = AdapterConfig::GetInstance()->GetValue(SECTION_A2DP_SNK_SERVICE, PROPERTY_CODEC_AAC, value);
    LOG_INFO("[A2dpProfilePeer]%{public}s Sink AAC Config(%{public}d) priority(%{public}d) \n", __func__, result, value);
    if (result) {
        if (localRole_ == A2DP_ROLE_SINK) {
            codecAACConfig = true;
        }
        defaultCodecPriorities_.insert(
            std::make_pair(A2DP_SINK_CODEC_INDEX_AAC, A2dpCodecPriority(value * PRIORITY_DEFAULT + 1)));
    }
}

bool A2dpProfilePeer::SendPacket(const Packet *packet, size_t frames, uint32_t bytes,
    uint32_t pktTimeStamp) const
{
    LOG_INFO("[A2dpProfilePeer] %{public}s \n", __func__);

    bool ret = false;
    uint8_t payloadType = 0x0e;
    uint8_t marker = 0;
    A2dpAvdtp avdtp(A2DP_ROLE_SOURCE);

    if (avdtp.WriteStream(streamHandle_, const_cast<Packet *>(packet), pktTimeStamp, payloadType, marker) ==
        A2DP_SUCCESS) {
        ret = true;
    }

    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS
