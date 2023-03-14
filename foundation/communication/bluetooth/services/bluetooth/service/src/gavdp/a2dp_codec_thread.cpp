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

#include "a2dp_codec_thread.h"
#include <sys/time.h>
#include "a2dp_decoder_aac.h"
#include "a2dp_encoder_aac.h"
#include "a2dp_decoder_sbc.h"
#include "a2dp_encoder_sbc.h"
#include "log.h"
#include "a2dp_service.h"

namespace OHOS {
namespace bluetooth {
const int ENCODE_TIMER_SBC = 20;
const int ENCODE_TIMER_AAC = 25;
#define PCM_DATA_ENCODED_TIMER(isSbc) ((isSbc) ? ENCODE_TIMER_SBC : ENCODE_TIMER_AAC)
A2dpCodecThread *A2dpCodecThread::g_instance = nullptr;
std::recursive_mutex g_codecMutex {};
A2dpCodecThread::A2dpCodecThread(const std::string &name) : name_(name)
{
    LOG_INFO("[A2dpCodecThread]%{public}s\n", __func__);
    dispatcher_ = std::make_unique<Dispatcher>(name);
    auto callbackFunc = std::bind(&A2dpCodecThread::SignalingTimeoutCallback, this);
    signalingTimer_ = std::make_unique<utility::Timer>(callbackFunc);
}

A2dpCodecThread::~A2dpCodecThread()
{
    encoder_ = nullptr;
    decoder_ = nullptr;
    signalingTimer_ = nullptr;
    dispatcher_ = nullptr;
    g_instance = nullptr;
    isSbc_ = false;
}

bool A2dpCodecThread::PostMessage(const utility::Message msg, const A2dpEncoderInitPeerParams &peerParams,
    A2dpCodecConfig *config, A2dpDecoderObserver *decObserver) const
{
    dispatcher_->PostTask(
        std::bind(&A2dpCodecThread::ProcessMessage, g_instance, msg, peerParams, config, decObserver));

    return true;
}

A2dpCodecThread *A2dpCodecThread::GetInstance()
{
    LOG_INFO("[A2dpCodecThread]%{public}s\n", __func__);
    std::lock_guard<std::recursive_mutex> lock(g_codecMutex);
    if (g_instance == nullptr) {
        auto instance = std::make_unique<A2dpCodecThread>("a2dpCodec");
        g_instance = instance.release();
    }
    return g_instance;
}

void A2dpCodecThread::StartA2dpCodecThread()
{
    LOG_INFO("[A2dpCodecThread]%{public}s\n", __func__);
    dispatcher_->Initialize();
    threadInit = true;
}

void A2dpCodecThread::StopA2dpCodecThread()
{
    LOG_INFO("[A2dpCodecThread]%{public}s\n", __func__);

    dispatcher_->Uninitialize();

    if (signalingTimer_ != nullptr) {
        signalingTimer_->Stop();
    }
    threadInit = false;
}

void A2dpCodecThread::ProcessMessage(utility::Message msg, const A2dpEncoderInitPeerParams &peerParams,
    A2dpCodecConfig *config, A2dpDecoderObserver *decObserver)
{
    LOG_INFO("[A2dpCodecThread]%{public}s msg is %{public}d\n", __func__, msg.what_);
    struct timeval tv = {};
    struct timezone tz = {};
    gettimeofday(&tv, &tz);
    std::lock_guard<std::recursive_mutex> lock(g_codecMutex);
    switch (msg.what_) {
        case A2DP_AUDIO_RECONFIGURE:
            if (encoder_ != nullptr) {
                encoder_->UpdateEncoderParam();
            }
            break;
        case A2DP_PCM_PUSH:
            if (encoder_ != nullptr) {
                encoder_->SendFrames(tv.tv_usec);
            }
            break;
        case A2DP_FRAME_READY:
            if (msg.arg2_ != nullptr && decoder_ != nullptr) {
                decoder_->DecodePacket((uint8_t *)msg.arg2_, msg.arg1_);
            }
            if (msg.arg2_ != nullptr) {
                free((uint8_t *)msg.arg2_);
            }
            break;
        case A2DP_PCM_ENCODED:
            if (config == nullptr) {
                return;
            }
            if (encoder_ != nullptr) {
                A2dpEncoder* encoder = encoder_.release();
                delete encoder;
            }
            SourceEncode(peerParams, *config);
            break;
        case A2DP_FRAME_DECODED:
            if (config == nullptr) {
                return;
            }
            if (decoder_ != nullptr) {
                A2dpDecoder* decoder = decoder_.release();
                delete decoder;
            }
            SinkDecode(*config, *decObserver);
            break;
        default:
            break;
    }
}

void A2dpCodecThread::StartTimer() const
{
    LOG_INFO("[A2dpCodecThread]%{public}s\n", __func__);
    if (signalingTimer_ != nullptr) {
        signalingTimer_->Start(PCM_DATA_ENCODED_TIMER(isSbc_), true);
    }
}

void A2dpCodecThread::StopTimer() const
{
    LOG_INFO("[A2dpCodecThread]%{public}s\n", __func__);
    if (signalingTimer_ != nullptr) {
        signalingTimer_->Stop();
    }
    if (encoder_ != nullptr) {
        encoder_->ResetFeedingState();
    }
}

bool A2dpCodecThread::GetInitStatus() const
{
    return threadInit;
}


void A2dpCodecThread::GetRenderPosition(uint16_t &sendDataSize, uint32_t &timeStamp) const
{
    LOG_INFO("[A2dpCodecThread]%{public}s\n", __func__);
    if (encoder_ != nullptr) {
        encoder_->GetRenderPosition(sendDataSize, timeStamp);
    }
}

void A2dpCodecThread::SourceEncode(
    const A2dpEncoderInitPeerParams &peerParams, const A2dpCodecConfig &config)
{
    LOG_INFO("[A2dpCodecThread]%{public}s index:%u\n", __func__, config.GetCodecIndex());
    switch (config.GetCodecIndex()) {
        case A2DP_SINK_CODEC_INDEX_SBC:
        case A2DP_SOURCE_CODEC_INDEX_SBC:
            isSbc_ = true;
            encoder_ = std::make_unique<A2dpSbcEncoder>(&const_cast<A2dpEncoderInitPeerParams &>(peerParams),
                &const_cast<A2dpCodecConfig &>(config));
            if (signalingTimer_ != nullptr) {
                signalingTimer_->Stop();
                signalingTimer_->Start(PCM_DATA_ENCODED_TIMER(isSbc_), true);
            }
            break;
        case A2DP_SOURCE_CODEC_INDEX_AAC:
        case A2DP_SINK_CODEC_INDEX_AAC:
            isSbc_ = false;
            encoder_ = std::make_unique<A2dpAacEncoder>(&const_cast<A2dpEncoderInitPeerParams &>(peerParams),
                &const_cast<A2dpCodecConfig &>(config));
            if (signalingTimer_ != nullptr) {
                signalingTimer_->Stop();
                signalingTimer_->Start(PCM_DATA_ENCODED_TIMER(isSbc_), true);
            }
            break;
        default:
            break;
    }
}

void A2dpCodecThread::SignalingTimeoutCallback() const
{
    LOG_INFO("[A2dpCodecThread]%{public}s", __func__);
    A2dpProfile *profile = GetProfileInstance(A2DP_ROLE_SOURCE);
    profile->DequeuePacket();
    utility::Message msg(A2DP_PCM_PUSH, 0, nullptr);
    A2dpEncoderInitPeerParams peerParams = {};
    PostMessage(msg, peerParams, nullptr, nullptr);
}

void A2dpCodecThread::SinkDecode(const A2dpCodecConfig &config, A2dpDecoderObserver &observer)
{
    LOG_INFO("[A2dpCodecThread]%{public}s index:%u\n", __func__, config.GetCodecIndex());

    switch (config.GetCodecIndex()) {
        case A2DP_SINK_CODEC_INDEX_SBC:
        case A2DP_SOURCE_CODEC_INDEX_SBC:
            if (decoder_ == nullptr) {
                decoder_ = std::make_unique<A2dpSbcDecoder>(&observer);
            }
            break;
        case A2DP_SOURCE_CODEC_INDEX_AAC:
        case A2DP_SINK_CODEC_INDEX_AAC:
            if (decoder_ == nullptr) {
                decoder_ = std::make_unique<A2dpAacDecoder>(&observer);
            }

            break;
        default:
            break;
    }
}
}  // namespace bluetooth
}  // namespace OHOS
