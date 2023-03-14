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

#ifndef TEST_DISPLAY_H
#define TEST_DISPLAY_H
#include <gtest/gtest.h>
#include "camera.h"
#include "camera_host.h"
#include "stream_operator.h"
#include "camera_device.h"
#include "utils.h"
#include <thread>
#include <map>
#include <stdio.h>
#include <climits>
#include "types.h"
#include "camera_device_impl.h"
#include "stream_operator_impl.h"
#include "idevice_manager.h"
#include "camera_metadata_info.h"
#include <display_type.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include "buffer_queue_producer.h"
#include "buffer_manager.h"
#include "test_stream_operator_impl.h"
#include "ibuffer.h"
#include <algorithm>
#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <mutex>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <securec.h>
#include <surface_buffer.h>
#include <ibuffer_producer.h>
#include <fstream>
#define PATH_MAX 128
#define BUFFERSCOUNT 8
#define CAMERA_BUFFER_QUEUE_IPC 654320
#define RANGE_LIMIT(x) (x > 255 ? 255 : (x < 0 ? 0 : x))

class TestDisplay {
public:
    // This should get the size setting according to the bottom layer
    unsigned int bufSize_ = 614400; // 614400:bufSize
    unsigned char* displayBuf_;
    unsigned int camframeV4l2Exit_ = 1;
    unsigned int numOfReadyFrames_ = 0;
    unsigned int bufCont_ = BUFFERSCOUNT;
    pthread_cond_t sub_thread_ready_cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t sub_thread_ready_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t previewThreadId_;
    std::mutex readyFrameLock_;

    int fbFd_, readIndex_;
    struct fb_var_screeninfo vinfo_;
    struct fb_fix_screeninfo finfo_;

    std::shared_ptr<OHOS::Camera::IStreamOperator> streamOperator = nullptr;
    std::shared_ptr<OHOS::Camera::IStreamOperatorCallback> streamOperatorCallback = nullptr;
    std::shared_ptr<OHOS::Camera::CaptureInfo> captureInfo = nullptr;
    std::vector<std::shared_ptr<OHOS::Camera::StreamInfo>> streamInfos;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfo = nullptr;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfoPre = nullptr;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfoVideo = nullptr;
    std::shared_ptr<OHOS::Camera::StreamInfo> streamInfoCapture = nullptr;
    std::shared_ptr<IBufferProducer> producer = nullptr;
    std::shared_ptr<IBufferProducer> producerCapture = nullptr;
    std::shared_ptr<IBufferProducer> producerVideo = nullptr;
    std::shared_ptr<OHOS::Camera::CameraHost> cameraHost = nullptr;
    std::shared_ptr<OHOS::Camera::ICameraDevice> cameraDevice = nullptr;
    std::shared_ptr<CameraAbility> ability = nullptr;
    std::vector<int> captureIds;
    std::vector<std::string> cameraIds;
    std::vector<int> streamIds;
    std::vector<OHOS::Camera::StreamIntent> intents;
    enum {
        streamId_preview = 1000, // 1000:preview streamID
        streamId_capture,
        streamId_video,
        captureId_preview = 2000, // 2000:preview captureId
        captureId_capture,
        captureId_video
    };
    enum {
        preview_mode = 0,
        capture_mode,
        video_mode
    };
    OHOS::Camera::CamRetCode rc;
    int init_flag = 0;
    bool status;

public:
    TestDisplay();
    uint64_t GetCurrentLocalTimeStamp();
    int32_t SaveYUV(char* type, unsigned char* buffer, int32_t size);
    int DoFbMunmap(unsigned char* addr);
    unsigned char* DoFbMmap(int* pmemfd);
    void FBLog();
    RetCode FBInit();
    void ProcessImage(const unsigned char* p, unsigned char* fbp);
    void LcdDrawScreen(unsigned char* displayBuf_, unsigned char* addr);
    void BufferCallback(std::shared_ptr<SurfaceBuffer> buffer, int choice);
    void Init();
    void UsbInit();
    void Close();
    void OpenCamera();
    void AchieveStreamOperator();
    void StartStream(std::vector<OHOS::Camera::StreamIntent> intents);
    void StopStream(std::vector<int>& captureIds, std::vector<int>& streamIds);
    void StartCapture(int streamId, int captureId, bool shutterCallback, bool isStreaming);
    float calTime(struct timeval start, struct timeval end);
};
#endif