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

#ifndef __MINIMP3_WRAPPER_H__
#define __MINIMP3_WRAPPER_H__

#include "minimp3.h"
#include "minimp3_ex.h"

typedef mp3dec_t            Minimp3WrapperMp3dec;
typedef mp3d_sample_t       Minimp3WrapperMp3dSample;
typedef mp3dec_frame_info_t Minimp3WrapperMp3decFrameInfo;
typedef mp3dec_io_t         Minimp3WrapperMp3decIO;
typedef MP3D_ITERATE_CB     MINIMP3_WRAPPER_MP3D_ITERATE_CB;

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct {
        void (*init)(Minimp3WrapperMp3dec *dec);

        int (*decoderFrame)(Minimp3WrapperMp3dec *dec, const uint8_t *mp3, int mp3Bytes,
                            Minimp3WrapperMp3dSample *pcm, Minimp3WrapperMp3decFrameInfo *info);

        int (*detecBuf)(const uint8_t *buf, size_t bufSize);

        int (*detecCb)(Minimp3WrapperMp3decIO *io, uint8_t *buf, size_t bufSize);

        int (*iterateBuf)(const uint8_t *buf, size_t bufSize, MINIMP3_WRAPPER_MP3D_ITERATE_CB callback, void *userData);

        int (*iterateCb)(Minimp3WrapperMp3decIO *io, uint8_t *buf, size_t bufSize,
                         MINIMP3_WRAPPER_MP3D_ITERATE_CB callback, void *userData);
    } Minimp3DemuxerOp;

    void Minimp3WrapperMp3decInit(Minimp3WrapperMp3dec *dec);

    int Minimp3WrapperMp3decDecodeFrame(Minimp3WrapperMp3dec *dec, const uint8_t *mp3, int mp3Bytes,
                                        Minimp3WrapperMp3dSample *pcm, Minimp3WrapperMp3decFrameInfo *info);

    int Minimp3WrapperMp3decDetectBuf(const uint8_t *buf, size_t bufSize);

    int Minimp3WrapperMp3decDetectCb(Minimp3WrapperMp3decIO *io, uint8_t *buf, size_t bufSize);

    int Minimp3WrapperMp3decIterateBuf(const uint8_t *buf, size_t bufSize,
                                       MINIMP3_WRAPPER_MP3D_ITERATE_CB callback, void *userData);

    int Minimp3WrapperMp3decIterateCb(Minimp3WrapperMp3decIO *io, uint8_t *buf, size_t bufSize,
                                      MINIMP3_WRAPPER_MP3D_ITERATE_CB callback, void *userData);

    Minimp3DemuxerOp MiniMp3GetOpt(void);

#ifdef __cplusplus
}
#endif

#endif