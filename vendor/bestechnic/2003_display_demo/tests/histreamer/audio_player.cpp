/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#include <unistd.h>
#include "histreamer/hiplayer.h"
#include "cmsis_os2.h"
#include "ohos_init.h"

using namespace OHOS::Media;

#define HTTP_PLAY_OPEN

#ifdef HTTP_PLAY_OPEN
#define LOG_TAG "AUD"
#include "log.h"
#include "bwifi_interface.h"
#endif
std::string GetMusicUri()
{
#ifdef HTTP_PLAY_OPEN
    std::string uri = "http://img.51miz.com/preview/sound/00/26/73/51miz-S267356-423D33372.mp3";
#else
    std::string uri = "/data/dream_it_possible.mp3";
#endif
    return uri;
}

static void AudioPlayerTask(void *unused)
{
#ifdef HTTP_PLAY_OPEN
    while(bwifi_get_current_status() != BWIFI_STATUS_GOT_IP){
        HILOG_INFO(HILOG_MODULE_APP, "%s WIFI NOT connected, pls run: AT+WSCONN=<ssid>,<passwd> and wait ...\r\n", __func__);
        osDelay(100);
    }
    osDelay(500);
    HILOG_INFO(HILOG_MODULE_APP, "%s CONNECTED and ready to play...\r\n", __func__);
#endif
    auto player = OHOS::Media::CreateHiPlayer();
    player->Init();
    OHOS::Media::Source source(GetMusicUri());
    player->SetSource(source);
    player->SetLoop(true);
    player->Prepare();
    player->Play();
    // player->SetVolume(100.0, 100.0);
    while (1) {
        osDelay(100);
    }
}

void AudioPlayerStart(void)
{
    osThreadAttr_t attr = {0};
    attr.stack_size = 1024 * 32;
    attr.priority = osPriorityNormal;
    attr.name = "AudioPlayer";
    if (osThreadNew((osThreadFunc_t)AudioPlayerTask, NULL, &attr) == NULL) {
        printf("Failed to create AudioPlayerTask");
        return;
    }
    return;
}

APP_FEATURE_INIT(AudioPlayerStart);
