/*
 * Copyright (C) 2020-2021 Huawei Device Co., Ltd.
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

#include "ActsMediaCameraTest.h"

#include <components/ui_surface_view.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include "camera_kit.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
/* *
 * get current dir
 * @return  string current file path of the test suits
 */
string GetCurDir()
{
    string filePath = "";
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("get file path error");
    } else {
        printf("Current Dir: %s\r\n", buffer);
        filePath = buffer;
        free(buffer);
    }
    return filePath + "/";
}

// SetUpTestCase
void CameraLiteTest::SetUpTestCase(void)
{
    g_testPath = GetCurDir();
    cout << "SetUpTestCase" << endl;
}

// TearDownTestCase
void CameraLiteTest::TearDownTestCase(void)
{
    g_testPath = "";
    cout << "TearDownTestCase" << endl;
}

void CameraLiteTest::SetUp(void)
{
    // CameraSetUp
    g_onGetCameraAbilityFlag = false;
    g_onConfigureFlag = false;
    g_onGetSupportedSizesFlag = false;

    // CameraDeviceCallBack
    g_onCameraAvailableFlag = false;
    g_onCameraUnavailableFlag = false;

    // CameraStateCallback
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    g_onConfiguredFlag = false;
    g_onConfigureFailedFlag = false;
    g_onReleasedFlag = false;

    // FrameStateCallback
    g_onCaptureTriggerAbortedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    g_onCaptureTriggerStartedFlag = false;
    g_onFrameFinishedFlag = false;
    g_onGetFrameConfigureType = false;
    g_onFrameErrorFlag = false;
    g_onFrameProgressedFlag = false;
    g_onFrameStartedFlag = false;
    g_onRecorderFlag = false;
    cout << "SetUp" << endl;
}

// Tear down
void CameraLiteTest::TearDown(void) { cout << "TearDown." << endl; }

int32_t SetupAudioSource(const Recorder &rec)
{
    Recorder *recorder = (Recorder *)&rec;
    int32_t ret = SUCCESS;
    int32_t audioSourceId = 0;
    AudioCodecFormat audioFormat = AAC_LC;
    AudioSourceType inputSource = AUDIO_MIC;
    int32_t sampleRate = 48000;
    int32_t channelCount = 1;
    int32_t audioEncodingBitRate = sampleRate;
    if ((ret = recorder->SetAudioSource(inputSource, audioSourceId)) != SUCCESS) {
        cout << "SetAudioSource failed." << ret << endl;
        return FAIL;
    }
    if ((ret = recorder->SetAudioEncoder(audioSourceId, audioFormat)) != SUCCESS) {
        cout << "SetAudioEncoder failed." << ret << endl;
        return FAIL;
    }
    if ((ret = recorder->SetAudioSampleRate(audioSourceId, sampleRate)) != SUCCESS) {
        cout << "SetAudioSampleRate failed." << ret << endl;
        return FAIL;
    }
    if ((ret = recorder->SetAudioChannels(audioSourceId, channelCount)) != SUCCESS) {
        cout << "SetAudioChannels failed." << ret << endl;
        return FAIL;
    }
    if ((ret = recorder->SetAudioEncodingBitRate(audioSourceId, audioEncodingBitRate)) != SUCCESS) {
        cout << "SetAudioEncodingBitRate failed." << ret << endl;
        return FAIL;
    }
    return SUCCESS;
}

/* *
 * creat Recorder
 */
Recorder *SampleCreateRecorder()
{
    int ret = 0;
    VideoSourceType source = VIDEO_SOURCE_SURFACE_ES;
    int32_t frameRate = 30;
    double fps = 30;
    int32_t rate = 4096;
    int32_t sourceId = 0;
    int32_t width = 1920;
    int32_t height = 1080;
    VideoCodecFormat encoder = HEVC;
    Recorder *recorder = new Recorder();
    if ((ret = recorder->SetVideoSource(source, sourceId)) != SUCCESS) {
        cout << "SetVideoSource failed." << ret << endl;
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncoder(sourceId, encoder)) != SUCCESS) {
        cout << "SetVideoEncoder failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoSize(sourceId, width, height)) != SUCCESS) {
        cout << "SetVideoSize failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoFrameRate(sourceId, frameRate)) != SUCCESS) {
        cout << "SetVideoFrameRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncodingBitRate(sourceId, rate)) != SUCCESS) {
        cout << "SetVideoEncodingBitRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = recorder->SetCaptureRate(sourceId, fps)) != SUCCESS) {
        cout << "SetCaptureRate failed." << ret << endl;
        delete recorder;
        return nullptr;
    }
    if ((ret = SetupAudioSource(*recorder) != SUCCESS)) {
        delete recorder;
        return nullptr;
    }
    return recorder;
}

/* *
 * Create Frame StateCallback
 */
class SampleFrameStateCallback : public FrameStateCallback {
    /* *
     * check file exist
     *
     * @param filename the filename
     * @return  check result
     */
    int32_t FileCheck(const string &filename)
    {
        fstream fileTmp;
        fileTmp.open(filename);
        if (!fileTmp) {
            cout << "file is not exist!" << endl;
            return RET_ERR;
        } else {
            cout << "file is exist!" << endl;
            fileTmp.close();
            return RET_OK;
        }
    }

    /* *
     * Save Capture
     *
     * @param testPath the file path
     * @param buffer the buffer
     * @param size the size
     * @return save result
     */
    int32_t SampleSaveCapture(string testPath, char *buffer, uint32_t size)
    {
        cout << "Start saving picture" << endl;
        string filePath = "";
        struct timeval tv = {0};

        gettimeofday(&tv, NULL);
        struct tm *ltm = localtime(&tv.tv_sec);
        if (ltm != nullptr) {
            ostringstream ss("Capture_");
            ss << "Capture" << ltm->tm_hour << "_" << ltm->tm_min << "_" << ltm->tm_sec << ".jpg";
            filePath = testPath + ss.str();
            ofstream pic(testPath + ss.str(), ofstream::out | ofstream::trunc);
            cout << "write " << size << " bytes" << endl;
            pic.write(buffer, size);
            cout << "Saving picture end" << endl;
        }
        const char *filename = filePath.data();
        int32_t ret = FileCheck(string(filename));
        return ret;
    }

    void OnFrameFinished(Camera &cam, FrameConfig &fc, FrameResult &res) override
    {
        g_onFrameStartedFlag = true;
        g_onFrameProgressedFlag = true;
        cout << "Receive frame complete inform." << endl;
        if (fc.GetFrameConfigType() == FRAME_CONFIG_CAPTURE) {
            g_onGetFrameConfigureType = true;
            cout << "Capture frame received." << endl;
            list<Surface *> surfaceList = fc.GetSurfaces();
            for (Surface *surface : surfaceList) {
                SurfaceBuffer *buffer = surface->AcquireBuffer();
                if (buffer != nullptr) {
                    char *virtAddr = static_cast<char *>(buffer->GetVirAddr());
                    if (virtAddr != nullptr) {
                        SampleSaveCapture(g_testPath, virtAddr, buffer->GetSize());
                    } else {
                        g_onFrameErrorFlag = true;
                    }
                    surface->ReleaseBuffer(buffer);
                } else {
                    g_onFrameErrorFlag = true;
                }
                delete surface;
            }
            delete &fc;
        } else {
            g_onFrameErrorFlag = true;
        }
        g_onFrameFinishedFlag = true;
    }
};

/* *
 * create CameraStateCallback
 */
class SampleCameraStateMng : public CameraStateCallback {
public:
    SampleCameraStateMng() = delete;

    explicit SampleCameraStateMng(EventHandler &eventHdlr) : eventHandler_(eventHdlr) {}

    ~SampleCameraStateMng()
    {
        if (recorder_ != nullptr) {
            recorder_->Release();
        }
        if (cam_ != nullptr) {
            cam_->Release();
        }
    }

    void OnCreated(Camera &c) override
    {
        g_onCreatedFlag = true;
        cout << "Sample recv OnCreate camera." << endl;
        auto config = CameraConfig::CreateCameraConfig();
        config->SetFrameStateCallback(&fsCb, &eventHandler_);
        c.Configure(*config);
        g_onConfigureFlag = true;
        cam_ = &c;
    }

    void OnCreateFailed(const std::string cameraId, int32_t errorCode) override
    {
        g_onCreateFailedFlag = true;
        cout << "Camera  ID: " << cameraId << endl;
        cout << "Sample recv OnCreateFailed camera.: " << errorCode << endl;
    }

    void OnReleased(Camera &c) override
    {
        g_onReleasedFlag = true;
        cout << "Sample recv OnReleased camera." << endl;
    }

    void StartRecord()
    {
        int ret;
        if (isRecording_) {
            cout << "Camera is already recording." << endl;
            return;
        }
        if (recorder_ != nullptr) {
            cout << "Recorder is not null" << endl;
        }
        recorder_ = SampleCreateRecorder();
        if (recorder_ == nullptr) {
            cout << "Recorder not available" << endl;
            return;
        }
        string path = GetCurDir();
        ret = recorder_->SetOutputPath(path);
        if (ret != SUCCESS) {
            cout << "SetOutputPath failed :" << ret << std::endl;
            return;
        }

        ret = recorder_->Prepare();
        if (ret != SUCCESS) {
            cout << "Prepare failed.=" << ret << endl;
            return;
        }
        auto surface = recorder_->GetSurface(0);
        surface->SetWidthAndHeight(WIDTH, HEIGHT);
        surface->SetQueueSize(QUEUE_SIZE);
        surface->SetSize(BUFFER_SIZE * BUFFER_SIZE);
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_RECORD);
        fc->AddSurface(*surface);
        ret = recorder_->Start();
        if (ret != SUCCESS) {
            cout << "recorder start failed. ret=" << ret << endl;
            delete fc;
            return;
        }
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }

        ret = cam_->TriggerLoopingCapture(*fc);
        if (ret != 0) {
            cout << "camera start recording failed. ret=" << ret << endl;
            return;
        }
        isRecording_ = true;
        g_onRecorderFlag = true;
        cout << "camera start recording succeed." << endl;
    }

    void StartPreview()
    {
        if (isPreviewing_) {
            cout << "Camera is already previewing." << endl;
            return;
        }
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_PREVIEW);
        UISurfaceView *surface = new UISurfaceView();
        int width = 960;
        int height = 480;
        surface->SetPosition(0, 0, width, height);
        surface->GetSurface()->SetWidthAndHeight(WIDTH, HEIGHT);
        if (surface == nullptr) {
            cout << "CreateSurface failed" << endl;
            delete fc;
            return;
        }
        fc->AddSurface(*(surface->GetSurface()));
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }
        int32_t ret = cam_->TriggerLoopingCapture(*fc);
        if (ret != 0) {
            cout << "camera start preview failed. ret=" << ret << endl;
            delete fc;
            return;
        }
        isPreviewing_ = true;
        g_onPreviewFlag = true;
        cout << "camera start preview succeed." << endl;
    }

    void Capture(void)
    {
        FrameConfig *fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
        Surface *surface = Surface::CreateSurface();
        if (surface == nullptr) {
            cout << "CreateSurface failed" << endl;
            delete fc;
            return;
        }
        surface->SetWidthAndHeight(WIDTH, HEIGHT);
        fc->AddSurface(*surface);
        static int cnt = 3;
        while (cam_ == nullptr) {
            if (cnt-- < 0)
                break;
            cout << "Wait camera created success" << endl;
            sleep(1);
        }
        g_onCaptureTriggerStartedFlag = true;
        cam_->TriggerSingleCapture(*fc);
        g_onCaptureTriggerCompletedFlag = true;
    }

    void Stop()
    {
        if (recorder_ != nullptr) {
            int32_t ret = recorder_->Stop(true);
            if (ret != 0) {
                cout << "recorder_ stop failed. ret=" << ret << endl;
            }
        }

        while (cam_ == nullptr) {
            cout << "Camera is not ready." << endl;
            return;
        }
        cam_->StopLoopingCapture(-1);
        isPreviewing_ = false;
        isRecording_ = false;
        g_onRecorderFlag = false;
        g_onPreviewFlag = false;
    }

    bool isPreviewing_ = false;
    bool isRecording_ = false;
    EventHandler &eventHandler_;
    Camera *cam_ = nullptr;
    Recorder *recorder_ = nullptr;
    SampleFrameStateCallback fsCb;
};

/* *
 * create CameraStateCallback for state test
 */
class SampleCameraStateCallback : public CameraStateCallback {
public:
    SampleCameraStateCallback() = delete;

    explicit SampleCameraStateCallback(EventHandler &eventHdlr) : eventHandler_(eventHdlr) {}

    ~SampleCameraStateCallback()
    {
        if (cam_ != nullptr) {
            cam_->Release();
        }
    }

    void OnCreated(Camera &c) override
    {
        g_onCreatedFlag = true;
        cout << "camera Create success." << endl;
        auto config = CameraConfig::CreateCameraConfig();
        config->SetFrameStateCallback(&fsCb, &eventHandler_);
        c.Configure(*config);
        cam_ = &c;
    }

    void OnCreateFailed(const std::string cameraId, int32_t errorCode) override
    {
        g_onCreateFailedFlag = true;
        cout << "Camera  ID: " << cameraId << endl;
        cout << "Camera Create Failed: " << errorCode << endl;
    }

    void OnReleased(Camera &c) override
    {
        g_onReleasedFlag = true;
        cout << "camera Releasedsuccess." << endl;
    }

    void OnConfigured(Camera &c) override
    {
        g_onConfiguredFlag = true;
        cout << "Camera Configured success." << endl;
    }

    void OnConfigureFailed(const std::string cameraId, int32_t errorCode) override
    {
        g_onConfigureFailedFlag = true;
        cout << "Camera  ID: " << cameraId << endl;
        cout << "Camera Configured failed: " << errorCode << endl;
    }

    EventHandler &eventHandler_;
    Camera *cam_ = nullptr;
    SampleFrameStateCallback fsCb;
};

/* *
 * Creat camera device callback
 */
class SampleCameraDeviceCallback : public CameraDeviceCallback {
public:
    SampleCameraDeviceCallback() {}

    ~SampleCameraDeviceCallback() {}

    // camera device status changed
    void OnCameraStatus(std::string cameraId, int32_t status) override
    {
        cout << "SampleCameraDeviceCallback OnCameraStatus cam Id: \n" << cameraId << endl;
        if (status == CAMERA_DEVICE_STATE_AVAILABLE) {
            g_onCameraAvailableFlag = true;
            cout << "SampleCameraDeviceCallback onCameraAvailable\n" << endl;
        } else if (status == CAMERA_DEVICE_STATE_UNAVAILABLE) {
            g_onCameraUnavailableFlag = true;
            cout << "SampleCameraDeviceCallback onCameraUnavailable\n" << endl;
        }
    }
};

/* *
 * Get camera Id
 */
void GetCameraId(CameraKit *cameraKit, list<string> &cameraList, string &cameraId)
{
    cameraKit = CameraKit::GetInstance();
    cameraList = cameraKit->GetCameraIds();
    for (auto &cam : cameraList) {
        cout << "camera name:" << cam << endl;
        const CameraAbility *ability = cameraKit->GetCameraAbility(cam);
        g_onGetCameraAbilityFlag = true;
        /* find camera which fits user's ability */
        list<CameraPicSize> sizeList = ability->GetSupportedSizes(0);
        if (sizeList.size() != 0) {
            g_onGetSupportedSizesFlag = true;
        }
        for (auto &pic : sizeList) {
            cout << "Pic size: " << pic.width << "x" << pic.height << endl;
            if (pic.width == WIDTH && pic.height == HEIGHT) {
                /* 1920:width,1080:height */
                cameraId = cam;
                break;
            }
        }
    }

    if (cameraId.empty()) {
        cout << "No available camera.(1080p wanted)" << endl;
        return;
    }
}

/* start camera kit interface test */
/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0100
 * @tc.name      : Get Instance
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetInstance_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    EXPECT_NE(cameraKit, nullptr);
}


HWTEST_F(CameraLiteTest, PrfTest_GetInstance_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraKit = CameraKit::GetInstance();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0200
 * @tc.name      : Get cameraIDs
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetCameraIds_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    list<string> cameraList = cameraKit->GetCameraIds();
    EXPECT_TRUE(cameraList.size() > 0);
}

HWTEST_F(CameraLiteTest, PrfTest_GetCameraIds_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    list<string> cameraList;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraList = cameraKit->GetCameraIds();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0300
 * @tc.name      : Get cameraAbility
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetCameraAbility_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraAbility *ability = cameraKit->GetCameraAbility(cameraId);
    EXPECT_NE(ability, nullptr);
}

HWTEST_F(CameraLiteTest, Test_GetCameraAbility_002, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string errorCameraId = "-1";
    const CameraAbility *ability = cameraKit->GetCameraAbility(errorCameraId);
    EXPECT_EQ(ability, nullptr);
}

HWTEST_F(CameraLiteTest, Test_GetCameraAbility_003, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string errorCameraId = "main";
    const CameraAbility *ability = cameraKit->GetCameraAbility(errorCameraId);
    EXPECT_EQ(ability, nullptr);
}

HWTEST_F(CameraLiteTest, PrfTest_GetCameraAbility_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    const CameraAbility *ability = nullptr;
    string cameraId = "0";
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        ability = cameraKit->GetCameraAbility(cameraId);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0400
 * @tc.name      : CameraKit get Info
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetCameraInfo_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    list<string> cameraList = cameraKit->GetCameraIds();
    for (auto &cameraId : cameraList) {
        const CameraInfo *cameraInfo = cameraKit->GetCameraInfo(cameraId);
        EXPECT_NE(cameraInfo, nullptr);
    }
}

HWTEST_F(CameraLiteTest, Test_GetCameraInfo_002, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string errorCameraId = "-1";
    const CameraInfo *cameraInfo = cameraKit->GetCameraInfo(errorCameraId);
    EXPECT_EQ(cameraInfo, nullptr);
}

HWTEST_F(CameraLiteTest, Test_GetCameraInfo_003, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string errorCameraId = "main";
    const CameraInfo *cameraInfo = cameraKit->GetCameraInfo(errorCameraId);
    EXPECT_EQ(cameraInfo, nullptr);
}

HWTEST_F(CameraLiteTest, PrfTest_GetCameraInfo_001, Level1)
{
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraKit->GetCameraInfo(cameraId);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0500
 * @tc.name      : Register Camera device callback
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_RegisterCameraDeviceCallback_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback deviceCallback = SampleCameraDeviceCallback();
    g_onCameraAvailableFlag = false;
    cameraKit->RegisterCameraDeviceCallback(deviceCallback, eventHdlr);
    sleep(2);
    EXPECT_TRUE(g_onCameraAvailableFlag);
}

HWTEST_F(CameraLiteTest, PrfTest_RegisterCameraDeviceCallback_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback deviceCallback = SampleCameraDeviceCallback();
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraKit->RegisterCameraDeviceCallback(deviceCallback, eventHdlr);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0600
 * @tc.name      : Unregister Camera Device Callback
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_UnregisterCameraDeviceCallback_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback deviceCallback = SampleCameraDeviceCallback();
    g_onCameraAvailableFlag = false;
    cameraKit->RegisterCameraDeviceCallback(deviceCallback, eventHdlr);
    sleep(2);
    g_onCameraAvailableFlag = false;
    cameraKit->UnregisterCameraDeviceCallback(deviceCallback);
    sleep(2);
    EXPECT_EQ(g_onCameraAvailableFlag, false);
}

HWTEST_F(CameraLiteTest, PrfTest_UnregisterCameraDeviceCallback_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    SampleCameraDeviceCallback deviceCallback = SampleCameraDeviceCallback();
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        cameraKit->RegisterCameraDeviceCallback(deviceCallback, eventHdlr);
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraKit->UnregisterCameraDeviceCallback(deviceCallback);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0700
 * @tc.name      : Create Camera
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_CreateCamera_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    EXPECT_EQ(g_onCreatedFlag, true);
    EXPECT_EQ(g_onCreateFailedFlag, false);
}

HWTEST_F(CameraLiteTest, Test_CreateCamera_002, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "main";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    EXPECT_EQ(g_onCreatedFlag, false);
    EXPECT_EQ(g_onCreateFailedFlag, true);
}

HWTEST_F(CameraLiteTest, Test_CreateCamera_003, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    list<string> cameraList;
    string cameraId = "0";
    EventHandler eventHdlr;
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    EXPECT_EQ(g_onCreatedFlag, true);
}

HWTEST_F(CameraLiteTest, Test_CreateCamera_004, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    cameraId = "-1";
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    EXPECT_EQ(g_onCreatedFlag, true);
    EXPECT_EQ(g_onCreateFailedFlag, true);
}

HWTEST_F(CameraLiteTest, PrfTest_CreateCamera_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t totalTime = 0;
    int64_t usecTimes = 1000000;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 500000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
} /* end camera kit interface test */

/* start camera ability interface test */
/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0800
 * @tc.name      : Get cameraKit supported Size
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetSupportedSizes_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    list<string> cameraList = cameraKit->GetCameraIds();
    for (auto &cameraId : cameraList) {
        const CameraAbility *ability = cameraKit->GetCameraAbility(cameraId);
        list<CameraPicSize> cameraPicSizes = ability->GetSupportedSizes(0);
        EXPECT_TRUE(cameraPicSizes.size() > 0);
    }
}

HWTEST_F(CameraLiteTest, PrfTest_GetSupportedSizes_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraAbility *ability = cameraKit->GetCameraAbility(cameraId);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        ability->GetSupportedSizes(0);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_0900
 * @tc.name      : Get supported AfModes
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetSupportedAfModes_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    list<string> cameraList = cameraKit->GetCameraIds();
    for (auto &cameraId : cameraList) {
        const CameraAbility *ability = cameraKit->GetCameraAbility(cameraId);
        list<int32_t> afModes = ability->GetSupportedAfModes();
        EXPECT_TRUE(afModes.size() >= 0);
    }
}

HWTEST_F(CameraLiteTest, PrfTest_GetSupportedAfModes_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraAbility *ability = cameraKit->GetCameraAbility(cameraId);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        ability->GetSupportedAfModes();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1000
 * @tc.name      : Get supported AeModes
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetSupportedAeModes_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    list<string> cameraList = cameraKit->GetCameraIds();
    for (auto &cameraId : cameraList) {
        const CameraAbility *ability = cameraKit->GetCameraAbility(cameraId);
        list<int32_t> aeModes = ability->GetSupportedAeModes();
        EXPECT_TRUE(aeModes.size() >= 0);
    }
}

HWTEST_F(CameraLiteTest, PrfTest_GetSupportedAeModes_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraAbility *ability = cameraKit->GetCameraAbility(cameraId);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t totalTime = 0;
    int64_t usecTimes = 1000000;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        ability->GetSupportedAeModes();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
} /* end camera ability interface test*/

/* start camera state callback test */
/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1100
 * @tc.name      : On Create
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_OnCreate_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    EXPECT_EQ(g_onCreatedFlag, true);
    EXPECT_EQ(g_onCreateFailedFlag, false);
}

HWTEST_F(CameraLiteTest, Test_OnCreate_002, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCreatedFlag = false;
    g_onCreateFailedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Stop();
    EXPECT_EQ(g_onCreatedFlag, true);
    EXPECT_NE(g_onCreateFailedFlag, true);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1200
 * @tc.name      : On Create Failed
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_OnCreateFailed_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "-1";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCreateFailedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    EXPECT_EQ(g_onCreateFailedFlag, true);
}

HWTEST_F(CameraLiteTest, Test_OnCreateFailed_002, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "main";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCreateFailedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    EXPECT_EQ(g_onCreateFailedFlag, true);
} /* end camera state callback test */

/* start frame config & frame state call back interface test */
/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1300
 * @tc.name      : Frame Config
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_FrameConfig_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    EXPECT_NE(frameConfig.get(), nullptr);
}

HWTEST_F(CameraLiteTest, Test_FrameConfig_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_RECORD);
    EXPECT_NE(frameConfig.get(), nullptr);
}

HWTEST_F(CameraLiteTest, Test_FrameConfig_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    EXPECT_NE(frameConfig.get(), nullptr);
}

HWTEST_F(CameraLiteTest, Test_FrameConfig_004, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CALLBACK);
    EXPECT_NE(frameConfig.get(), nullptr);
}

HWTEST_F(CameraLiteTest, PrfTest_Frameconfig_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = nullptr;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

HWTEST_F(CameraLiteTest, PrfTest_Frameconfig_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = nullptr;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_RECORD);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

HWTEST_F(CameraLiteTest, PrfTest_Frameconfig_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = nullptr;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

HWTEST_F(CameraLiteTest, PrfTest_Frameconfig_004, Level1)
{
    shared_ptr<FrameConfig> frameConfig = nullptr;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CALLBACK);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1400
 * @tc.name      : Get Frame Configure Type
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetFrameConfigureType_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onGetFrameConfigureType = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_EQ(g_onGetFrameConfigureType, true);
}

HWTEST_F(CameraLiteTest, Test_GetFrameConfigureType_002, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onGetFrameConfigureType = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    camStateMng.Stop();
    EXPECT_EQ(g_onGetFrameConfigureType, true);
}

HWTEST_F(CameraLiteTest, Test_GetFrameConfigureType_003, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onGetFrameConfigureType = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    EXPECT_EQ(g_onGetFrameConfigureType, false);
}

HWTEST_F(CameraLiteTest, Test_GetFrameConfigureType_004, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onGetFrameConfigureType = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    EXPECT_EQ(g_onGetFrameConfigureType, true);
}

HWTEST_F(CameraLiteTest, Test_GetFrameConfigureType_005, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onGetFrameConfigureType = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    camStateMng.Stop();
    EXPECT_EQ(g_onGetFrameConfigureType, true);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1500
 * @tc.name      : Set Vendor Parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_SetVendorParameter_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[4] = 5;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    uint8_t *privateTagTemp = new uint8_t[10];
    frameConfig->GetVendorParameter(privateTagTemp, 10);
    EXPECT_TRUE(privateTagTemp[4] == 5);
    delete[] privateTag;
    delete[] privateTagTemp;
}

HWTEST_F(CameraLiteTest, Test_SetVendorParameter_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[10] = 10;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    uint8_t *privateTagTemp = new uint8_t[PRIVATE_TAG_LEN];
    frameConfig->GetVendorParameter(privateTagTemp, PRIVATE_TAG_LEN);
    EXPECT_TRUE(privateTagTemp[10] == 10);
    delete[] privateTag;
    delete[] privateTagTemp;
}

HWTEST_F(CameraLiteTest, Test_SetVendorParameter_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_RECORD);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[0] = 1;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    uint8_t *privateTagTemp = new uint8_t[PRIVATE_TAG_LEN];
    frameConfig->GetVendorParameter(privateTagTemp, PRIVATE_TAG_LEN);
    EXPECT_TRUE(privateTagTemp[0] == 1);
    delete[] privateTag;
    delete[] privateTagTemp;
}

HWTEST_F(CameraLiteTest, Test_SetVendorParameter_004, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CALLBACK);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[31] = 31;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    uint8_t *privateTagTemp = new uint8_t[PRIVATE_TAG_LEN];
    frameConfig->GetVendorParameter(privateTagTemp, PRIVATE_TAG_LEN);
    EXPECT_TRUE(privateTagTemp[31] == 31);
    delete[] privateTag;
    delete[] privateTagTemp;
}

HWTEST_F(CameraLiteTest, PrfTest_SetVendorParameter_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[1] = 1;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
}

HWTEST_F(CameraLiteTest, PrfTest_SetVendorParameter_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[2] = 2;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
}

HWTEST_F(CameraLiteTest, PrfTest_SetVendorParameter_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_RECORD);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[3] = 3;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
}

HWTEST_F(CameraLiteTest, PrfTest_SetVendorParameter_004, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CALLBACK);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[4] = 4;
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1600
 * @tc.name      : Get Vendor Parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, PrfTest_GetVendorParameter_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[1] = 1;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    uint8_t *privateTagTemp = new uint8_t[PRIVATE_TAG_LEN];
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetVendorParameter(privateTagTemp, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
    delete[] privateTagTemp;
}

HWTEST_F(CameraLiteTest, PrfTest_GetVendorParameter_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[2] = 2;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    uint8_t *privateTagTemp = new uint8_t[PRIVATE_TAG_LEN];
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetVendorParameter(privateTagTemp, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
    delete[] privateTagTemp;
}

HWTEST_F(CameraLiteTest, PrfTest_GetVendorParameter_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_RECORD);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[3] = 3;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    uint8_t *privateTagTemp = new uint8_t[PRIVATE_TAG_LEN];
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetVendorParameter(privateTagTemp, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
    delete[] privateTagTemp;
}

HWTEST_F(CameraLiteTest, PrfTest_GetVendorParameter_004, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CALLBACK);
    uint8_t *privateTag = new uint8_t[PRIVATE_TAG_LEN];
    privateTag[4] = 4;
    frameConfig->SetVendorParameter(privateTag, PRIVATE_TAG_LEN);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    uint8_t *privateTagTemp = new uint8_t[PRIVATE_TAG_LEN];
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetVendorParameter(privateTagTemp, PRIVATE_TAG_LEN);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
    delete[] privateTag;
    delete[] privateTagTemp;
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1700
 * @tc.name      : Get Surface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetSurface_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    surface->SetWidthAndHeight(1920, 1080);
    EXPECT_EQ(1920, surface->GetWidth());
    EXPECT_EQ(1080, surface->GetHeight());
}

HWTEST_F(CameraLiteTest, Test_GetSurface_002, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    surface->SetWidthAndHeight(480, 360);
    EXPECT_EQ(480, surface->GetWidth());
    EXPECT_EQ(360, surface->GetHeight());
}

HWTEST_F(CameraLiteTest, Test_GetSurface_003, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    surface->SetWidthAndHeight(1920, 1080);
    frameConfig->AddSurface(*surface);
    EXPECT_FALSE(frameConfig->GetSurfaces().size() == 0);
}

HWTEST_F(CameraLiteTest, Test_GetSurface_004, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    frameConfig->RemoveSurface(*surface);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 0);
}

HWTEST_F(CameraLiteTest, Test_GetSurface_005, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_RECORD);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 1);
}

HWTEST_F(CameraLiteTest, Test_GetSurface_006, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CALLBACK);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 1);
}

HWTEST_F(CameraLiteTest, PrfTest_GetSurfaces_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetSurfaces();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

HWTEST_F(CameraLiteTest, PrfTest_GetSurfaces_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CAPTURE);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetSurfaces();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

HWTEST_F(CameraLiteTest, PrfTest_GetSurfaces_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_RECORD);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetSurfaces();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

HWTEST_F(CameraLiteTest, PrfTest_GetSurfaces_004, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_CALLBACK);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->GetSurfaces();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1800
 * @tc.name      : On Frame Progressed
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_OnFrameProgressed_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_EQ(g_onFrameStartedFlag, true);
    EXPECT_EQ(g_onFrameProgressedFlag, true);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_1900
 * @tc.name      : On Frame Completed
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_OnFrameCompletedFlag_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_EQ(g_onFrameFinishedFlag, true);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2000
 * @tc.name      : On Frame Error
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_OnFrameErrorFlag_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_NE(g_onFrameErrorFlag, true);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2100
 * @tc.name      : Add Surface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_AddSurface_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 1);
}

HWTEST_F(CameraLiteTest, Test_AddSurface_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    frameConfig->AddSurface(*surface);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 2);
}

HWTEST_F(CameraLiteTest, Test_AddSurface_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    frameConfig->AddSurface(*surface);
    frameConfig->AddSurface(*surface);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 3);
}

HWTEST_F(CameraLiteTest, PrfTest_AddSurface_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->AddSurface(*surface);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2200
 * @tc.name      : Remove Surface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_RemoveSurface_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    frameConfig->RemoveSurface(*surface);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 0);
}

HWTEST_F(CameraLiteTest, Test_RemoveSurface_002, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    Surface *surface1 = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    frameConfig->AddSurface(*surface1);
    frameConfig->RemoveSurface(*surface1);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 1);
}

HWTEST_F(CameraLiteTest, Test_RemoveSurface_003, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    Surface *surface1 = Surface::CreateSurface();
    frameConfig->AddSurface(*surface);
    frameConfig->AddSurface(*surface1);
    frameConfig->RemoveSurface(*surface);
    frameConfig->RemoveSurface(*surface1);
    list<Surface *> list = frameConfig->GetSurfaces();
    EXPECT_TRUE(list.size() == 0);
}

HWTEST_F(CameraLiteTest, PrfTest_RemoveSurface_001, Level1)
{
    shared_ptr<FrameConfig> frameConfig = make_shared<FrameConfig>(FRAME_CONFIG_PREVIEW);
    Surface *surface = Surface::CreateSurface();
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        frameConfig->AddSurface(*surface);
        clock_gettime(CLOCK_REALTIME, &tv1);
        frameConfig->RemoveSurface(*surface);
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2300
 * @tc.name      : Capture
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_Capture_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_EQ(g_onCaptureTriggerStartedFlag, true);
    EXPECT_EQ(g_onCaptureTriggerCompletedFlag, true);
}

HWTEST_F(CameraLiteTest, Test_Capture_002, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    camStateMng.Stop();
    sleep(2);
    EXPECT_EQ(g_onCaptureTriggerStartedFlag, true);
    EXPECT_EQ(g_onCaptureTriggerCompletedFlag, true);
}

HWTEST_F(CameraLiteTest, Test_Capture_003, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    EventHandler eventHdlr;
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    sleep(2);
    EXPECT_EQ(g_onCaptureTriggerStartedFlag, false);
    EXPECT_EQ(g_onCaptureTriggerCompletedFlag, false);
}

HWTEST_F(CameraLiteTest, Test_Capture_004, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    camStateMng.Stop();
    sleep(2);
    EXPECT_EQ(g_onCaptureTriggerStartedFlag, true);
    EXPECT_EQ(g_onCaptureTriggerCompletedFlag, true);
}

HWTEST_F(CameraLiteTest, Test_Capture_005, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_EQ(g_onCaptureTriggerStartedFlag, true);
    EXPECT_EQ(g_onCaptureTriggerCompletedFlag, true);
}

HWTEST_F(CameraLiteTest, Test_Capture_006, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_TRUE(g_onCaptureTriggerStartedFlag);
    EXPECT_TRUE(g_onCaptureTriggerCompletedFlag);
}

HWTEST_F(CameraLiteTest, Test_Capture_007, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.Stop();
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_TRUE(g_onCaptureTriggerStartedFlag);
    EXPECT_TRUE(g_onCaptureTriggerCompletedFlag);
}

HWTEST_F(CameraLiteTest, Test_Capture_008, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    g_onCaptureTriggerStartedFlag = false;
    g_onCaptureTriggerCompletedFlag = false;
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    sleep(2);
    camStateMng.Capture();
    sleep(2);
    EXPECT_EQ(g_onCaptureTriggerStartedFlag, true);
    EXPECT_EQ(g_onCaptureTriggerCompletedFlag, true);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2400
 * @tc.name      : Preview
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_Preview_001, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    g_onPreviewFlag = false;
    camStateMng.StartPreview();
    sleep(2);
    EXPECT_TRUE(g_onPreviewFlag);
    camStateMng.Stop();
}

HWTEST_F(CameraLiteTest, Test_Preview_002, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    g_onPreviewFlag = false;
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    EXPECT_FALSE(g_onPreviewFlag);
}

HWTEST_F(CameraLiteTest, Test_Preview_003, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    g_onPreviewFlag = false;
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    camStateMng.StartPreview();
    sleep(2);
    EXPECT_TRUE(g_onPreviewFlag);
}

HWTEST_F(CameraLiteTest, Test_Preview_004, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    g_onPreviewFlag = false;
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    EXPECT_FALSE(g_onPreviewFlag);
}

HWTEST_F(CameraLiteTest, Test_Preview_005, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    g_onPreviewFlag = false;
    camStateMng.Capture();
    sleep(2);
    camStateMng.Stop();
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    EXPECT_TRUE(g_onPreviewFlag);
    camStateMng.Stop();
}

HWTEST_F(CameraLiteTest, Test_Preview_006, Level1)
{
    CameraKit *cameraKit = nullptr;
    list<string> cameraList;
    string cameraId;
    EventHandler eventHdlr;
    GetCameraId(cameraKit, cameraList, cameraId);
    SampleCameraStateMng camStateMng(eventHdlr);
    cameraKit->CreateCamera(cameraId, camStateMng, eventHdlr);
    g_onPreviewFlag = false;
    camStateMng.Capture();
    sleep(2);
    camStateMng.StartPreview();
    sleep(2);
    camStateMng.Stop();
    EXPECT_FALSE(g_onPreviewFlag);
} /* end frame config & frame state call back interface test */

/* start camera info interface test */
/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2500
 * @tc.name      : Get Camera Type
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetCameraType_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraInfo *cameraInfo = cameraKit->GetCameraInfo(cameraId);
    int32_t cameraType = cameraInfo->GetCameraType();
    EXPECT_TRUE(cameraType <= 3);
    EXPECT_TRUE(cameraType >= 0);
}

HWTEST_F(CameraLiteTest, PrfTest_GetCameraType_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraInfo *cameraInfo = cameraKit->GetCameraInfo(cameraId);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraInfo->GetCameraType();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
}

/* *
 * @tc.number    : SUB_MEDIA_CAMERA_DEV_2600
 * @tc.name      : Get Camera Facing Type
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(CameraLiteTest, Test_GetCameraFacingType_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraInfo *cameraInfo = cameraKit->GetCameraInfo(cameraId);
    int32_t cameraFacingType = cameraInfo->GetCameraFacingType();
    EXPECT_TRUE(cameraFacingType <= 2);
    EXPECT_TRUE(cameraFacingType >= 0);
}

HWTEST_F(CameraLiteTest, PrfTest_GetCameraFacingType_001, Level1)
{
    CameraKit *cameraKit = CameraKit::GetInstance();
    string cameraId = "0";
    const CameraInfo *cameraInfo = cameraKit->GetCameraInfo(cameraId);
    struct timespec tv1 = {0};
    struct timespec tv2 = {0};
    int64_t performanceTestTimes = 10;
    int64_t usecTimes = 1000000;
    int64_t totalTime = 0;
    for (int32_t i = 0; i < performanceTestTimes; i++) {
        clock_gettime(CLOCK_REALTIME, &tv1);
        cameraInfo->GetCameraFacingType();
        clock_gettime(CLOCK_REALTIME, &tv2);
        totalTime += tv2.tv_sec * usecTimes + tv2.tv_nsec - (tv1.tv_sec * usecTimes + tv1.tv_nsec);
    }
    int64_t expectTime = 1000000;
    EXPECT_TRUE(totalTime <= expectTime * performanceTestTimes);
} /* end camera info interface test */
}  // namespace OHOS
