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

#ifndef A2DP_CODEC_THREAD_H
#define A2DP_CODEC_THREAD_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "a2dp_codec/include/a2dp_codec_wrapper.h"
#include "a2dp_codec/include/a2dp_codec_config.h"
#include "a2dp_codec/include/a2dp_codec_constant.h"
#include "a2dp_profile_peer.h"
#include "base_def.h"
#include "dispatcher.h"
#include "message.h"

namespace OHOS {
namespace bluetooth {
using utility::Dispatcher;
using utility::Message;

constexpr int A2DP_AUDIO_RECONFIGURE = 1;
constexpr int A2DP_PCM_ENCODED = 2;
constexpr int A2DP_FRAME_DECODED = 3;
constexpr int A2DP_FRAME_READY = 4;
constexpr int A2DP_PCM_PUSH = 5;

class A2dpCodecThread {
public:
    /**
     * @brief A constructor used to create an <b>A2dpCodecThread</b> instance.
     *
     * @param[in] The name of the thread
     * @since 6.0
     */
    explicit A2dpCodecThread(const std::string &name);

    /**
     * @brief Get a A2dpCodecThread object.
     *
     * @param name A2dpCodecThread name.
     * @since 6.0
     */
    static A2dpCodecThread *GetInstance();

    /**
     * @brief Start a A2dpCodecThread object.
     *
     * @since 6.0
     */
    void StartA2dpCodecThread();

    /**
     * @brief Stop a A2dpCodecThread object.
     *
     * @since 6.0
     */
    void StopA2dpCodecThread();

    /**
     * @brief A destructor used to create an <b>A2dpCodecThread</b> instance.
     *
     * @since 6.0
     */
    ~A2dpCodecThread();

    /**
     * @brief Get A2dpCodecThread's name.
     * @return A2dpCodecThread's name.
     * @since 6.0
     */
    std::string &Name()
    {
        return name_;
    };

    /**
     * @brief Get the A2dpCodecThread's Dispatcher object
     * @return Dispatcher pointer.
     * @since 6.0
     */
    Dispatcher *GetDispatcher()
    {
        return dispatcher_.get();
    };

    /**
     * @brief PostMessage to A2dpCodecThread's dispatcher.
     * @return Success PostTask to dispatcher return true, else return false.
     * @since 6.0
     */
    bool PostMessage(const utility::Message msg, const A2dpEncoderInitPeerParams &peerParams, A2dpCodecConfig *config,
        A2dpDecoderObserver *decObserver) const;

    /**
     * @brief process the message of codec thread.
     * @return Success PostTask to dispatcher return true, else return false.
     * @since 6.0
     */
    void ProcessMessage(utility::Message msg, const A2dpEncoderInitPeerParams &peerParams, A2dpCodecConfig *config,
        A2dpDecoderObserver *decObserver);
    /**
     * @brief Start the timer
     * @since 6.0
     */
    void StartTimer() const;

    /**
     * @brief Stop the timer
     * @since 6.0
     */
    void StopTimer() const;

    /**
     * @brief Get the init status
     * @since 6.0
     */
    bool GetInitStatus() const;

    /**
     * @brief Get the information of the current rendered position.
     * @param[out] sendDataSize is the data size that has been sent
     * @param[out] timeStamp is the current time stamp
     * @since 6.0
     */
    void GetRenderPosition(uint16_t &sendDataSize, uint32_t &timeStamp) const;

private:
    /**
     * @brief Source side  encode
     *
     * @since 6.0
     */
    void SourceEncode(const A2dpEncoderInitPeerParams &peerParams, const A2dpCodecConfig &config);

    /**
     * @brief Source side  encode
     *
     * @since 6.0
     */
    void SinkDecode(const A2dpCodecConfig &config, A2dpDecoderObserver &observer);

    /**
     * @brief Timer to push pcm data
     *
     * @since 6.0
     */
    void SignalingTimeoutCallback() const;

    std::string name_ {};
    std::unique_ptr<Dispatcher> dispatcher_ {};
    std::unique_ptr<A2dpEncoder> encoder_ = nullptr;
    std::unique_ptr<A2dpDecoder> decoder_ = nullptr;
    std::unique_ptr<utility::Timer> signalingTimer_ = nullptr;
    static A2dpCodecThread *g_instance;
    bool threadInit = false;
    bool isSbc_ = false;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // A2DP_CODEC_THREAD_H