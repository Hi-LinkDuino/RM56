/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define MINIMP3_IMPLEMENTATION
#define MAX_FRAME_SYNC_MATCHES 3
#include "minimp3.h"
#include "minimp3_ex.h"
#undef MINIMP3_IMPLEMENTATION

#include "minimp3_wrapper.h"

void Minimp3WrapperMp3decInit(Minimp3WrapperMp3dec *dec)
{
    mp3dec_init(dec);
}

int Minimp3WrapperMp3decDecodeFrame(Minimp3WrapperMp3dec *dec, const uint8_t *mp3, int mp3Bytes,
                                    Minimp3WrapperMp3dSample *pcm, Minimp3WrapperMp3decFrameInfo *info)
{
    return mp3dec_decode_frame(dec, mp3, mp3Bytes, pcm, info);
}

int Minimp3WrapperMp3decDetectBuf(const uint8_t *buf, size_t bufSize)
{
    return mp3dec_detect_buf(buf, bufSize);
}

int Minimp3WrapperMp3decDetectCb(Minimp3WrapperMp3decIO *io, uint8_t *buf, size_t bufSize)
{
    return mp3dec_detect_cb(io, buf, bufSize);
}

int Minimp3WrapperMp3decIterateBuf(const uint8_t *buf, size_t bufSize, MINIMP3_WRAPPER_MP3D_ITERATE_CB callback,
                                   void *userData)
{
    return mp3dec_iterate_buf(buf, bufSize, callback, userData);
}

int Minimp3WrapperMp3decIterateCb(Minimp3WrapperMp3decIO *io, uint8_t *buf, size_t bufSize,
                                  MINIMP3_WRAPPER_MP3D_ITERATE_CB callback, void *userData)
{
    return mp3dec_iterate_cb(io, buf, bufSize, callback, userData);
}

Minimp3DemuxerOp minimp3DemuxerOp = {
    .init         = Minimp3WrapperMp3decInit,
    .decoderFrame = Minimp3WrapperMp3decDecodeFrame,
    .detecBuf     = Minimp3WrapperMp3decDetectBuf,
    .detecCb      = Minimp3WrapperMp3decDetectCb,
    .iterateBuf   = Minimp3WrapperMp3decIterateBuf,
    .iterateCb    = Minimp3WrapperMp3decIterateCb
};

Minimp3DemuxerOp MiniMp3GetOpt(void)
{
    return minimp3DemuxerOp;
}