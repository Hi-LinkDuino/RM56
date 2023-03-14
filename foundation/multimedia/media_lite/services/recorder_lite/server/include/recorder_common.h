/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RECORDER_COMMON_H
#define RECORDER_COMMON_H

#define RECORDER_SERVICE_NAME "RecorderServer"

namespace OHOS {
namespace Media {
enum RecFunc {
    REC_FUNC_CONNECT,
    REC_FUNC_DISCONNECT,
    REC_FUNC_SET_VIDEOSOURCE,
    REC_FUNC_SET_VIDEOENCODER,
    REC_FUNC_SET_VIDEOSIZE,
    REC_FUNC_SET_VIDEOFRAMERATE,
    REC_FUNC_SET_VIDEOENCODINGBITRATE,
    REC_FUNC_SET_CAPTURERATE,
    REC_FUNC_GET_SURFACE,
    REC_FUNC_SET_AUDIOSOURCE,
    REC_FUNC_SET_AUDIOENCODER,
    REC_FUNC_SET_AUDIOSAMPLERATE,
    REC_FUNC_SET_AUDIOCHANNELS,
    REC_FUNC_SET_AUDIOENCODINGBITRATE,
    REC_FUNC_SET_MAXDURATION,
    REC_FUNC_SET_OUTPUTFORMAT,
    REC_FUNC_SET_OUTPUTPATH,
    REC_FUNC_SET_OUTPUTFILE,
    REC_FUNC_SET_NEXTOUTPUTFILE,
    REC_FUNC_SET_MAXFILESIZE,
    REC_FUNC_SET_RECORDERCALLBACK,
    REC_FUNC_PREPARE,
    REC_FUNC_START,
    REC_FUNC_PAUSE,
    REC_FUNC_RESUME,
    REC_FUNC_STOP,
    REC_FUNC_RESET,
    REC_FUNC_RELEASE,
    REC_FUNC_SET_FILESPLITDURATION,
    REC_FUNC_SET_PARAMETER,
    REC_FUNC_SET_DATASOURCE,
    REC_FUNC_BUTT,
};

enum RecAnonymousFunc {
    REC_ANONYMOUS_FUNC_ON_ERROR,
    REC_ANONYMOUS_FUNC_ON_INFO,
    REC_ANONYMOUS_FUNC_BUTT,
};

static constexpr int32_t DEFAULT_IPC_SIZE = 100;
}  // namespace Media
}  // namespace OHOS
#endif  // RECORDER_COMMON_H
