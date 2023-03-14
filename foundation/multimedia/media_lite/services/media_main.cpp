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
#include "hal_camera.h"
#include "samgr_lite.h"
#ifndef ENABLE_PASSTHROUGH_MODE
#include "player_server.h"
#endif
#include "camera_server.h"
#include "recorder_service.h"
#include "audio_capturer_server.h"
#include "ohos_init.h"

#include <csignal>
#include <cstdint>
#include <iostream>
#include <unistd.h>

using namespace OHOS;
using namespace OHOS::Media;
extern "C" void __attribute__((weak)) OHOS_SystemInit(void)
{
    SAMGR_Bootstrap();
}

using namespace std;
int main()
{
    OHOS_SystemInit();

    cout << "Camera server start." << endl;
    CameraServer::GetInstance()->InitCameraServer();
#ifndef ENABLE_PASSTHROUGH_MODE
    cout << "Player server start" << endl;
    PlayerServer::GetInstance()->PlayerServerInit();

    cout << "AudioCapturer server start" << endl;
    AudioCapturerServer::GetInstance()->AudioCapturerServerInit();
#endif
    cout << "Media server initialize succeed." << endl;

    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGABRT);
    sigaddset(&signalSet, SIGINT);
    sigaddset(&signalSet, SIGTERM);

    int32_t sig;
    sigwait(&signalSet, &sig);
    return 0;
}
#ifndef ENABLE_PASSTHROUGH_MODE
SYSEX_SERVICE_INIT(RecorderServiceReg);
SYSEX_SERVICE_INIT(AudioCapturerServiceReg);
#endif
