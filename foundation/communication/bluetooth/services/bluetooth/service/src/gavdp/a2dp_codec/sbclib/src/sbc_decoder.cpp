/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "../include/sbc_decoder.h"
#include <memory>
#include "../include/sbc_constant.h"
#include "../include/sbc_tables.h"
#include "foundation/communication/bluetooth/services/bluetooth/service/src/gavdp/a2dp_codec/sbclib/include/sbc_frame.h"
#include "foundation/communication/bluetooth/services/bluetooth/service/src/gavdp/a2dp_codec/sbclib/include/sbc_math.h"
#include "new"
#include "securec.h"


namespace sbc {
const int BIT16_BYTE2 = 2;
const int SUBBANDS_NUM4 = 4;
const int SUBBANDS_NUM8 = 8;
const int MAX_CHANNEL_NUM = 2;
const int MULTIPLE_TEN = 10;
const int HIGH_BYTE_16BIT = 0xff00;
const int LOW_BYTE_16BIT = 0x00ff;
const int MOVE_RIGHT_BIT8 = 8;
const int INDEX_0 = 0;
const int INDEX_1 = 1;
const int INDEX_2 = 2;
const int INDEX_3 = 3;
const int INDEX_4 = 4;
const int INDEX_5 = 5;
const int INDEX_6 = 6;
const int INDEX_7 = 7;
const int INDEX_8 = 8;
const int INDEX_9 = 9;
const int VALUE_9 = 9;
const int VALUE_79 = 79;
const int VALUE_80 = 80;
const int VALUE_159 = 159;
const int VALUE_160 = 160;
const int VALUE_16 = 16;
const int VALUE_4 = 4;
const int VALUE_5 = 5;

Decoder::Decoder()
{
    initialized_ = false;
}

extern "C" Decoder* CreateDecode()
{
    std::unique_ptr<Decoder> decoder = std::make_unique<Decoder>();
    return decoder.release();
}

extern "C" void DestroyDecode(Decoder *p)
{
    delete p;
}

void Decoder::Init(const Frame& frame)
{
    (void)memset_s(v_, sizeof(v_), 0, sizeof(v_));
    for (int ch = 0; ch < MAX_CHANNEL_NUM; ch++) {
        for (int i = 0; i < frame.subbands_ * BIT16_BYTE2; i++) {
            offset_[ch][i] = (MULTIPLE_TEN * i + MULTIPLE_TEN);
        }
    }
    initialized_ = true;
}

ssize_t Decoder::SBCDecode(const CodecParam& codecParam, const uint8_t* in, size_t iLength,
        uint8_t* out, size_t oLength, size_t* written)
{
    int length = frame_.Unpack(in, iLength);
    if (length < 0) {
        return length;
    }
    if (!initialized_) {
        Init(frame_);
    }
    frame_.length_ = length;
    if (!out) {
        return length;
    }

    uint8_t *data = out;
    auto samples = Synthesize(frame_);

    if (oLength < static_cast<size_t>(samples * frame_.channels_ * BIT16_BYTE2)) {
        samples = oLength / (frame_.channels_ * BIT16_BYTE2);
    }

    for (int sample  = 0; sample < samples; sample++) {
        for (int channel = 0; channel < frame_.channels_; channel++) {
            int16_t val = pcmSamples_[channel][sample];
            if (codecParam.endian == SBC_ENDIANESS_BE) {
                *data++ = (val & HIGH_BYTE_16BIT) >> MOVE_RIGHT_BIT8;
                *data++ = (val & LOW_BYTE_16BIT);
            } else {
                *data++ = (val & LOW_BYTE_16BIT);
                *data++ = (val & HIGH_BYTE_16BIT) >> MOVE_RIGHT_BIT8;
            }
        }
    }
    if (written) {
        *written = samples * frame_.channels_ * BIT16_BYTE2;
    }
    return length;
}

int Decoder::Synthesize(const Frame& frame)
{
    switch (frame.subbands_) {
        case SUBBANDS_NUM4:
            for (int channel = 0; channel < frame.channels_; channel++) {
                for (int blk = 0; blk < frame.blocks_; blk++) {
                    Synthesize4(frame, channel, blk);
                }
            }
            break;
        case SUBBANDS_NUM8:
            for (int channel = 0; channel < frame.channels_; channel++) {
                for (int blk = 0; blk < frame.blocks_; blk++) {
                    Synthesize8(frame, channel, blk);
                }
            }
            break;
        default:
            return -1;
    }
    return frame.subbands_ * frame.blocks_;
}

inline int16_t Clip16(int32_t val)
{
    if (val > 0x7FFF) {
        return 0x7FFF;
    } else if (val < -0x8000) {
        return -0x8000;
    } else {
        return val;
    }
}

void Decoder::Synthesize4(const Frame &frame, int ch, int blk)
{
    int32_t *v = v_[ch];
    int *offset = offset_[ch];

    for (auto i = 0; i < INDEX_8; i++) {
        offset[i]--;
        if (offset[i] < 0) {
            offset[i] = VALUE_79;
            (void)memcpy_s(v + VALUE_80, VALUE_9 * sizeof(*v), v, VALUE_9 * sizeof(*v));
        }

        v[offset[i]] = Scale4Staged1(MULA(SYNMATRIX4[i][INDEX_0], frame.samples_[blk][ch][INDEX_0],
            MULA(SYNMATRIX4[i][INDEX_1], frame.samples_[blk][ch][INDEX_1],
            MULA(SYNMATRIX4[i][INDEX_2], frame.samples_[blk][ch][INDEX_2],
            MUL(SYNMATRIX4[i][INDEX_3], frame.samples_[blk][ch][INDEX_3])))));
    }

    for (auto idx = 0, i = 0; i < INDEX_4; i++, idx += VALUE_5) {
        int k = (i + VALUE_4) & 0xf;
        pcmSamples_[ch][blk * VALUE_4 + i] = Clip16(Scale4Staged1(MULA(v[offset[i] + INDEX_0],
                                                                        PROTO_4_40M0[idx + INDEX_0],
                                                                        MULA(v[offset[k] + INDEX_1],
                                                                             PROTO_4_40M1[idx + INDEX_0],
                                                                        MULA(v[offset[i] + INDEX_2],
                                                                             PROTO_4_40M0[idx + INDEX_1],
                                                                        MULA(v[offset[k] + INDEX_3],
                                                                             PROTO_4_40M1[idx + INDEX_1],
                                                                        MULA(v[offset[i] + INDEX_4],
                                                                             PROTO_4_40M0[idx + INDEX_2],
                                                                        MULA(v[offset[k] + INDEX_5],
                                                                             PROTO_4_40M1[idx + INDEX_2],
                                                                        MULA(v[offset[i] + INDEX_6],
                                                                             PROTO_4_40M0[idx + INDEX_3],
                                                                        MULA(v[offset[k] + INDEX_7],
                                                                             PROTO_4_40M1[idx + INDEX_3],
                                                                        MULA(v[offset[i] + INDEX_8],
                                                                             PROTO_4_40M0[idx + INDEX_4],
                                                                        MUL(v[offset[k] + INDEX_9],
                                                                        PROTO_4_40M1[idx + INDEX_4]))))))))))));
    }
}

void Decoder::Synthesize8(const Frame &frame, int ch, int blk)
{
    int i = 0;
    int idx = 0;
    int *offset = offset_[ch];
    for (i = 0; i < VALUE_16; i++) {
        offset[i]--;
        if (offset[i] < 0) {
            offset[i] = VALUE_159;
            for (int j = 0; j < INDEX_9; j++) {
                v_[ch][j + VALUE_160] = v_[ch][j];
            }
        }
        v_[ch][offset[i]] = Scale8Staged1(MULA(SYNMATRIX8[i][INDEX_0], frame.samples_[blk][ch][INDEX_0],
                                                MULA(SYNMATRIX8[i][INDEX_1], frame.samples_[blk][ch][INDEX_1],
                                                MULA(SYNMATRIX8[i][INDEX_2], frame.samples_[blk][ch][INDEX_2],
                                                MULA(SYNMATRIX8[i][INDEX_3], frame.samples_[blk][ch][INDEX_3],
                                                MULA(SYNMATRIX8[i][INDEX_4], frame.samples_[blk][ch][INDEX_4],
                                                MULA(SYNMATRIX8[i][INDEX_5], frame.samples_[blk][ch][INDEX_5],
                                                MULA(SYNMATRIX8[i][INDEX_6], frame.samples_[blk][ch][INDEX_6],
                                                MUL(SYNMATRIX8[i][INDEX_7], frame.samples_[blk][ch][INDEX_7])))))))));
    }
    for (idx = 0, i = 0; i < INDEX_8; i++, idx += VALUE_5) {
        int k = (i + INDEX_8) & 0xf;
        pcmSamples_[ch][blk * INDEX_8 + i] = Clip16(Scale8Staged1(MULA(v_[ch][offset[i] + INDEX_0],
                                                                        PROTO_8_80M0[idx + INDEX_0],
                                                                        MULA(v_[ch][offset[k] + INDEX_1],
                                                                             PROTO_8_80M1[idx + INDEX_0],
                                                                        MULA(v_[ch][offset[i] + INDEX_2],
                                                                             PROTO_8_80M0[idx + INDEX_1],
                                                                        MULA(v_[ch][offset[k] + INDEX_3],
                                                                             PROTO_8_80M1[idx + INDEX_1],
                                                                        MULA(v_[ch][offset[i] + INDEX_4],
                                                                             PROTO_8_80M0[idx + INDEX_2],
                                                                        MULA(v_[ch][offset[k] + INDEX_5],
                                                                             PROTO_8_80M1[idx + INDEX_2],
                                                                        MULA(v_[ch][offset[i] + INDEX_6],
                                                                             PROTO_8_80M0[idx + INDEX_3],
                                                                        MULA(v_[ch][offset[k] + INDEX_7],
                                                                             PROTO_8_80M1[idx + INDEX_3],
                                                                        MULA(v_[ch][offset[i] + INDEX_8],
                                                                             PROTO_8_80M0[idx + INDEX_4],
                                                                        MUL(v_[ch][offset[k] + INDEX_9],
                                                                        PROTO_8_80M1[idx +INDEX_4]))))))))))));
    }
}
} // namespace sbc
