/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hos_camera_demo.h"
#include <cstdio>
#include <getopt.h>

namespace OHOS::Camera {
static void Usage(FILE* fp)
{
    fprintf(fp,
            "Options:\n"
            "-h | --help          Print this message\n"
            "-o | --offline       stream offline test\n"
            "-c | --capture       capture one picture\n"
            "-w | --set WB        Set white balance Cloudy\n"
            "-v | --video         capture Viedeo of 10s\n"
            "-a | --Set AE        Set Auto exposure\n"
            "-f | --Set Flashlight        Set flashlight ON 5s OFF\n"
            "-q | --quit          stop preview and quit this app\n");
}

const static struct option longOptions[] = {
    {"help", no_argument, nullptr, 'h'}, {"capture", no_argument, nullptr, 'c'},
    {"WB", no_argument, nullptr, 'w'}, {"video", no_argument, nullptr, 'v'},
    {"quit", no_argument, nullptr, 'q'}, {"AE", no_argument, nullptr, 'a'},
    {"OL", no_argument, nullptr, 'o'}, {"flashlight", no_argument, nullptr, 'f'},
    {0, 0, 0, 0}
};

static int PutMenuAndGetChr(void)
{
    constexpr uint32_t inputCount = 50;
    int c = 0;
    char strs[inputCount];

    Usage(stdout);
    CAMERA_LOGD("pls input command(input -q exit this app)\n");
    fgets(strs, inputCount, stdin);

    for (int i = 0; i < inputCount; i++) {
        if (strs[i] != '-') {
            c = strs[i];
            break;
        }
    }

    return c;
}

static RetCode PreviewOn(int mode, const std::shared_ptr<HosCameraDemo>& mainDemo)
{
    RetCode rc = RC_OK;
    CAMERA_LOGD("main test: PreviewOn enter");

    rc = mainDemo->StartPreviewStream();
    if (rc != RC_OK) {
        CAMERA_LOGE("main test: PreviewOn StartPreviewStream error");
        return RC_ERROR;
    }

    if (mode == 0) {
        rc = mainDemo->StartCaptureStream();
        if (rc != RC_OK) {
            CAMERA_LOGE("main test: PreviewOn StartCaptureStream error");
            return RC_ERROR;
        }
    } else {
        rc = mainDemo->StartVideoStream();
        if (rc != RC_OK) {
            CAMERA_LOGE("main test: PreviewOn StartVideoStream error");
            return RC_ERROR;
        }
    }

    rc = mainDemo->CaptureON(STREAM_ID_PREVIEW, CAPTURE_ID_PREVIEW, CAPTURE_PREVIEW);
    if (rc != RC_OK) {
        CAMERA_LOGE("main test: PreviewOn mainDemo->CaptureON() preview error");
        return RC_ERROR;
    }

    CAMERA_LOGD("main test: PreviewOn exit");
    return RC_OK;
}

static void PreviewOff(const std::shared_ptr<HosCameraDemo>& mainDemo)
{
    CAMERA_LOGD("main test: PreviewOff enter");

    mainDemo->CaptureOff(CAPTURE_ID_PREVIEW, CAPTURE_PREVIEW);
    mainDemo->ReleaseAllStream();

    CAMERA_LOGD("main test: PreviewOff exit");
}

static void FlashLightTest(const std::shared_ptr<HosCameraDemo>& mainDemo)
{
    constexpr size_t delayTime = 5;

    PreviewOff(mainDemo);
    mainDemo->ReleaseCameraDevice();
    sleep(1);
    mainDemo->FlashlightOnOff(true);
    sleep(delayTime);
    mainDemo->FlashlightOnOff(false);
    mainDemo->InitCameraDevice();
    PreviewOn(0, mainDemo);
}

static void OfflineTest(const std::shared_ptr<HosCameraDemo>& mainDemo)
{
    RetCode rc = RC_OK;

    PreviewOff(mainDemo);

    mainDemo->StartDualStreams(STREAM_ID_CAPTURE);
    mainDemo->CaptureOnDualStreams(STREAM_ID_CAPTURE);
    sleep(1);

    rc = mainDemo->StreamOffline(STREAM_ID_CAPTURE);
    if (rc != RC_OK) {
        CAMERA_LOGE("main test: mainDemo->StreamOffline error");
    } else {
        sleep(1);
        mainDemo->InitCameraDevice();
        rc = PreviewOn(0, mainDemo);
        if (rc != RC_OK) {
            CAMERA_LOGE("main test: PreviewOn() error");
        }
    }
}

static void CaptureTest(const std::shared_ptr<HosCameraDemo>& mainDemo)
{
    RetCode rc = RC_OK;
    constexpr size_t delayTime = 5;

    rc = mainDemo->CaptureON(STREAM_ID_CAPTURE, CAPTURE_ID_CAPTURE, CAPTURE_SNAPSHOT);
    if (rc != RC_OK) {
        CAMERA_LOGE("main test: mainDemo->CaptureON() capture error");
        return;
    }

    sleep(delayTime);
    rc = mainDemo->CaptureOff(CAPTURE_ID_CAPTURE, CAPTURE_SNAPSHOT);
    if (rc != RC_OK) {
        CAMERA_LOGE("main test: mainDemo->CaptureOff() capture error");
        return;
    }
}

static void VideoTest(const std::shared_ptr<HosCameraDemo>& mainDemo)
{
    RetCode rc = RC_OK;
    constexpr size_t delayTime = 5;

    PreviewOff(mainDemo);
    mainDemo->StartDualStreams(STREAM_ID_VIDEO);
    mainDemo->CaptureOnDualStreams(STREAM_ID_VIDEO);

    sleep(delayTime);
    mainDemo->CaptureOff(CAPTURE_ID_PREVIEW, CAPTURE_PREVIEW);
    mainDemo->CaptureOff(CAPTURE_ID_VIDEO, CAPTURE_VIDEO);
    mainDemo->ReleaseAllStream();

    rc = PreviewOn(0, mainDemo);
    if (rc != RC_OK) {
        CAMERA_LOGE("main test: video PreviewOn() error plese -q exit demo");
    }
}

static void ManuList(const std::shared_ptr<HosCameraDemo>& mainDemo,
    const int argc, char** argv)
{
    int idx, c;
    int awb = 1;
    const char *shortOptions = "h:cwvaqof:";

    c = getopt_long(argc, argv, shortOptions, longOptions, &idx);
    while(1) {
        switch (c) {
            case 'h':
                c = PutMenuAndGetChr();
                break;

            case 'f':
                FlashLightTest(mainDemo);
                c = PutMenuAndGetChr();
                break;

            case 'o':
                OfflineTest(mainDemo);
                c = PutMenuAndGetChr();
                break;

            case 'c':
                CaptureTest(mainDemo);
                c = PutMenuAndGetChr();
                break;

            case 'w':
                if (awb) {
                    mainDemo->SetAwbMode(OHOS_CAMERA_AWB_MODE_INCANDESCENT);
                } else {
                    mainDemo->SetAwbMode(OHOS_CAMERA_AWB_MODE_OFF);
                }
                awb = !awb;
                c = PutMenuAndGetChr();
                break;

            case 'a':
                mainDemo->SetAeExpo();
                c = PutMenuAndGetChr();
                break;

            case 'v':
                VideoTest(mainDemo);
                c = PutMenuAndGetChr();
                break;

            case 'q':
                PreviewOff(mainDemo);
                mainDemo->QuitDemo();
                return;

            default:
                CAMERA_LOGE("main test: command error please retry input command");
                c = PutMenuAndGetChr();
                break;
        }
    }
}

int main(int argc, char** argv)
{
    RetCode rc = RC_OK;

    auto mainDemo = std::make_shared<HosCameraDemo>();
    rc = mainDemo->InitSensors();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test: mainDemo->InitSensors() error\n");
        return -1;
    }
    rc = mainDemo->InitCameraDevice();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test: mainDemo->InitCameraDevice() error\n");
        return -1;
    }

    rc = PreviewOn(0, mainDemo);
    if (rc != RC_OK) {
        CAMERA_LOGE("main test: PreviewOn() error demo exit");
        return -1;
    }

    ManuList(mainDemo, argc, argv);

    return RC_OK;
}
} // namespace Camera

int main(int argc, char** argv)
{
    OHOS::Camera::main(argc, argv);

    return 0;
}
