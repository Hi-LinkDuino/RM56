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

#include "../include/a2dp_decoder_sbc.h"
#include "log.h"
#include "memory.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const int CODEC_BUFFER_SIZE4K = 4096;

sbc::CodecParam g_sbcDecode = {};

A2dpSbcDecoder::A2dpSbcDecoder(A2dpDecoderObserver *observer) : A2dpDecoder(observer)
{
    LOG_INFO("[SbcDecoder] %{public}s\n", __func__);
    g_sbcDecode.endian = sbc::SBC_ENDIANESS_LE;
    codecLib_ = std::make_unique<A2dpSBCDynamicLibCtrl>(false);
    codecSbcDecoderLib_ = codecLib_->LoadCodecSbcLib();
    if (codecSbcDecoderLib_ == nullptr) {
        LOG_ERROR("[SbcDecoder] %{public}s load sbc lib failed\n", __func__);
    } else {
        sbcDecoder_ = codecSbcDecoderLib_->sbcDecoder.createSbcDecode();
    }
}

A2dpSbcDecoder::~A2dpSbcDecoder()
{
    LOG_INFO("[SbcDecoder] %{public}s\n", __func__);
    if (codecSbcDecoderLib_ != nullptr) {
        codecSbcDecoderLib_->sbcDecoder.destroySbcDecode(sbcDecoder_);
        codecLib_->UnloadCodecSbcLib(codecSbcDecoderLib_);
    }
}

bool A2dpSbcDecoder::DecodePacket(uint8_t *data, uint16_t size)
{
    uint8_t pcmData[CODEC_BUFFER_SIZE4K];
    size_t len = 0;
    int pos = 0x01;
    int frameLen = 0;
    uint16_t packetSize = size;
    uint8_t buf[CODEC_BUFFER_SIZE4K] = {0};
    (void)memcpy_s(buf, CODEC_BUFFER_SIZE4K, data, packetSize);
    (void)memset_s(pcmData, CODEC_BUFFER_SIZE4K, 0, CODEC_BUFFER_SIZE4K);
    int count = len;
    while (true) {
        if (count + len > CODEC_BUFFER_SIZE4K) {
            LOG_INFO(" count > buf");
            break;
        }

        if (pos >= packetSize) {
            LOG_INFO("pos > packetSize");
            break;
        }

        frameLen = sbcDecoder_->SBCDecode(g_sbcDecode, &buf[pos], packetSize - pos,
            &pcmData[count], sizeof(pcmData) - count, &len);
        pos += frameLen;
        if (frameLen < 0) {
            LOG_ERROR(" frame  length is err");
            break;
        }
        count += len;
    }
    LOG_INFO("[DataAvailable][packetSize:%hu][count:%{public}d][len:%{public}zu][frameLen:%{public}d]",
        packetSize, count, len, frameLen);

    if (count > 0) {
        observer_->DataAvailable(pcmData, count);
    }
    return true;
}
}  // namespace bluetooth
}  // namespace OHOS