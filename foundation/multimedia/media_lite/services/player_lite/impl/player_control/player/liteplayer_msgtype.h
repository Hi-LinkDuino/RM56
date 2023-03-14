/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef PLAYERCONTROL_MSG_TYPE_H
#define PLAYERCONTROL_MSG_TYPE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum PlayerControlMsgType {
    PLAYERCONTROL_MSG_SETATTR = 0,  // setmedia
    PLAYERCONTROL_MSG_REGCALLBACK,
    PLAYERCONTROL_MSG_SET_DATASOURCE_FD,
    PLAYERCONTROL_MSG_SET_DATASOURCE_URI,
    PLAYERCONTROL_MSG_SET_DATASOURCE_STREAM,
    PLAYERCONTROL_MSG_PREPARE,
    PLAYERCONTROL_MSG_PLAY,
    PLAYERCONTROL_MSG_SEEK,
    PLAYERCONTROL_MSG_PAUSE,
    PLAYERCONTROL_MSG_TPLAY,
    PLAYERCONTROL_MSG_STOP,
    PLAYERCONTROL_MSG_GETFILEINFO,
    PLAYERCONTROL_MSG_HANDLEDATA,
    PLAYERCONTROL_MSG_ERROR,
    PLAYERCONTROL_MSG_SET_VOLUME,
    PLAYERCONTROL_MSG_INVOKE,
    PLAYERCONTROL_MSG_SET_AUDIOSTREAM_TYPE,
    PLAYERCONTROL_MSG_BUTT
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLAYERCONTROL_MSG_TYPE_H */
