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
#include "test_display.h"
using namespace std;
using namespace OHOS;
using namespace Camera;

TestDisplay::TestDisplay()
{
}

uint64_t TestDisplay::GetCurrentLocalTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return tmp.count();
}

int32_t TestDisplay::SaveYUV(char* type, unsigned char* buffer, int32_t size)
{
    int ret;
    char path[PATH_MAX] = {0};
    ret = sprintf_s(path, sizeof(path) / sizeof(path[0]), "/mnt/yuv/%s_%lld.yuv", type, GetCurrentLocalTimeStamp());
    if (ret < 0) {
        CAMERA_LOGE("%s, sprintf_s failed, errno = %s.", __FUNCTION__, strerror(errno));
        return -1;
    }
    CAMERA_LOGI("%s, save yuv to file %s", __FUNCTION__, path);
    system("mkdir -p /mnt/yuv");
    int imgFd = open(path, O_RDWR | O_CREAT, 00766); // 00766: file permissions
    if (imgFd == -1) {
        CAMERA_LOGI("%s, open file failed, errno = %s.", __FUNCTION__, strerror(errno));
        return -1;
    }
    int ret = write(imgFd, buffer, size);
    if (ret == -1) {
        CAMERA_LOGI("%s, write file failed, error = %s", __FUNCTION__, strerror(errno));
        close(imgFd);
        return -1;
    }
    close(imgFd);
    return 0;
}

int TestDisplay::DoFbMunmap(unsigned char* addr)
{
    int rc;
    unsigned int size = vinfo_.xres * vinfo_.yres * vinfo_.bits_per_pixel / 8; // 8:picture size;
    CAMERA_LOGI("main test:munmapped size = %d, virt_addr = 0x%p\n", size, addr);
    rc = (munmap(addr, finfo_.smem_len));
    return rc;
}

unsigned char* TestDisplay::DoFbMmap(int* pmemfd)
{
    unsigned char* ret;
    int screensize = vinfo_.xres * vinfo_.yres * vinfo_.bits_per_pixel / 8; // 8:picture size
    ret = (unsigned char*)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, *pmemfd, 0);
    if (ret == MAP_FAILED) {
        CAMERA_LOGE("main test:do_mmap: pmem mmap() failed: %s (%d)\n", strerror(errno), errno);
        return NULL;
    }
    CAMERA_LOGI("main test:do_mmap: pmem mmap fd %d ptr %p len %u\n", *pmemfd, ret, screensize);
    return ret;
}

void TestDisplay::FBLog()
{
    CAMERA_LOGI("the fixed information is as follow:\n");
    CAMERA_LOGI("id=%s\n", finfo_.id);
    CAMERA_LOGI("sem_start=%lx\n", finfo_.smem_start);
    CAMERA_LOGI("smem_len=%u\n", finfo_.smem_len);
    CAMERA_LOGI("type=%u\n", finfo_.type);
    CAMERA_LOGI("line_length=%u\n", finfo_.line_length);
    CAMERA_LOGI("mmio_start=%lu\n", finfo_.mmio_start);
    CAMERA_LOGI("mmio_len=%d\n", finfo_.mmio_len);
    CAMERA_LOGI("visual=%d\n", finfo_.visual);

    CAMERA_LOGI("variable information is as follow:\n");
    CAMERA_LOGI("The xres is :%u\n", vinfo_.xres);
    CAMERA_LOGI("The yres is :%u\n", vinfo_.yres);
    CAMERA_LOGI("xres_virtual=%u\n", vinfo_.xres_virtual);
    CAMERA_LOGI("yres_virtual=%u\n", vinfo_.yres_virtual);
    CAMERA_LOGI("xoffset=%u\n", vinfo_.xoffset);
    CAMERA_LOGI("yoffset=%u\n", vinfo_.yoffset);
    CAMERA_LOGI("bits_per_pixel is :%u\n", vinfo_.bits_per_pixel);
    CAMERA_LOGI("red.offset=%u\n", vinfo_.red.offset);
    CAMERA_LOGI("red.length=%u\n", vinfo_.red.length);
    CAMERA_LOGI("red.msb_right=%u\n", vinfo_.red.msb_right);
    CAMERA_LOGI("green.offset=%d\n", vinfo_.green.offset);
    CAMERA_LOGI("green.length=%d\n", vinfo_.green.length);
    CAMERA_LOGI("green.msb_right=%d\n", vinfo_.green.msb_right);
    CAMERA_LOGI("blue.offset=%d\n", vinfo_.blue.offset);
    CAMERA_LOGI("blue.length=%d\n", vinfo_.blue.length);
    CAMERA_LOGI("blue.msb_right=%d\n", vinfo_.blue.msb_right);
    CAMERA_LOGI("transp.offset=%d\n", vinfo_.transp.offset);
    CAMERA_LOGI("transp.length=%d\n", vinfo_.transp.length);
    CAMERA_LOGI("transp.msb_right=%d\n", vinfo_.transp.msb_right);
    CAMERA_LOGI("height=%x\n", vinfo_.height);
}

RetCode TestDisplay::FBInit()
{
    fbFd_ = open("/dev/fb0", O_RDWR);
    if (fbFd_ < 0) {
        CAMERA_LOGE("main test:cannot open framebuffer %s file node\n", "/dev/fb0");
        return RC_ERROR;
    }

    if (ioctl(fbFd_, FBIOGET_VSCREENINFO, &vinfo_) < 0) {
        CAMERA_LOGE("main test:cannot retrieve vscreenInfo!\n");
        close(fbFd_);
        return RC_ERROR;
    }

    if (ioctl(fbFd_, FBIOGET_FSCREENINFO, &finfo_) < 0) {
        CAMERA_LOGE("main test:can't retrieve fscreenInfo!\n");
        close(fbFd_);
        return RC_ERROR;
    }

    FBLog();

    CAMERA_LOGI("main test:allocating display buffer memory\n");
    displayBuf_ = DoFbMmap(&fbFd_);
    if (displayBuf_ == NULL) {
        CAMERA_LOGE("main test:error displayBuf_ mmap error\n");
        close(fbFd_);
        return RC_ERROR;
    }
    return RC_OK;
}

void TestDisplay::ProcessImage(const unsigned char* p, unsigned char* fbp)
{
    unsigned char* in = (unsigned char*)p;
    int width = 640; // 640:Displays the size of the width
    int height = 480; // 480:Displays the size of the height
    int istride = 1280; // 1280:Initial value of span
    int x, y, j;
    int y0, u, v, r, g, b;
    long location = 0;
    int xpos = (vinfo_.xres - width) / 2;
    int ypos = (vinfo_.yres - height) / 2;
    int y_pos, u_pos, v_pos;

    y_pos = 0; // 0:Pixel initial value
    u_pos = 1; // 1:Pixel initial value
    v_pos = 3; // 3:Pixel initial value

    for (y = ypos; y < (height + ypos); y++) {
        for (j = 0, x = xpos; j < width; j++, x++) {
            location = (x + vinfo_.xoffset) * (vinfo_.bits_per_pixel / 8) + // 8: The bytes for each time
            (y + vinfo_.yoffset) * finfo_.line_length; // add one y number of rows at a time

            y0 = in[y_pos];
            u = in[u_pos] - 128; // 128:display size
            v = in[v_pos] - 128; // 128:display size

            r = RANGE_LIMIT(y0 + v + ((v * 103) >> 8)); // 103,8:display range
            g = RANGE_LIMIT(y0 - ((u * 88) >> 8) - ((v * 183) >> 8)); // 88,8,183:display range
            b = RANGE_LIMIT(y0 + u + ((u * 198) >> 8)); // 198,8:display range

            fbp[location + 1] = ((r & 0xF8) | (g >> 5)); // 5:display range
            fbp[location + 0] = (((g & 0x1C) << 3) | (b >> 3)); // 3:display range

            y_pos += 2;

            if (j & 0x01) {
                u_pos += 4;
                v_pos += 4;
            }
        }

        y_pos = 0; // 0:Pixel initial value
        u_pos = 1; // 1:Pixel initial value
        v_pos = 3; // 3:Pixel initial value
        in += istride; // add one y number of rows at a time
    }
}

void TestDisplay::LcdDrawScreen(unsigned char* displayBuf_, unsigned char* addr)
{
    ProcessImage(addr, displayBuf_);
}

void TestDisplay::BufferCallback(std::shared_ptr<SurfaceBuffer> buffer, int choice)
{
    if (choice == preview_mode) {
        LcdDrawScreen(displayBuf_, (unsigned char*)buffer->GetVirAddr());
        return;
    } else {
        LcdDrawScreen(displayBuf_, (unsigned char*)buffer->GetVirAddr());
        std::cout << "==========[test log] capture start saveYuv......" << std::endl;
        SaveYUV("capture", (unsigned char*)buffer->GetVirAddr(), bufSize_);
        std::cout << "==========[test log] capture end saveYuv......" << std::endl;
        return;
    }
}

void TestDisplay::Init()
{
    std::shared_ptr<Camera::IDeviceManager> deviceManager = Camera::IDeviceManager::GetInstance();
    if (!init_flag) {
        deviceManager->Init();
        init_flag = 1;
    }
    if (cameraHost == nullptr) {
        cameraHost = Camera::CameraHost::CreateCameraHost();
        if (cameraHost == nullptr) {
            std::cout << "==========[test log] CreateCameraHost failed." << std::endl;
            return;
        }
        std::cout << "==========[test log] CreateCameraHost success." << std::endl;
    }
    if (cameraDevice == nullptr) {
        cameraHost->GetCameraIds(cameraIds);
        const std::shared_ptr<OHOS::Camera::ICameraDeviceCallback> callback =
            std::make_shared<OHOS::Camera::ICameraDeviceCallback>();
        rc = cameraHost->OpenCamera(cameraIds.front(), callback, cameraDevice);
        if (rc != Camera::NO_ERROR || cameraDevice == nullptr) {
            std::cout << "==========[test log] OpenCamera failed, rc = " << rc << std::endl;
            return;
        }
        std::cout << "==========[test log]  OpenCamera success." << std::endl;
    }
}

void TestDisplay::UsbInit()
{
    std::shared_ptr<OHOS::Camera::IDeviceManager> deviceManager = Camera::IDeviceManager::GetInstance();
    if (!init_flag) {
        deviceManager->Init();
        init_flag = 1;
    }
    if (cameraHost == nullptr) {
        cameraHost = Camera::CameraHost::CreateCameraHost();
        if (cameraHost == nullptr) {
            std::cout << "==========[test log] CreateCameraHost failed." << std::endl;
            return;
        }
        std::cout << "==========[test log] CreateCameraHost success." << std::endl;
    }

    std::shared_ptr<OHOS::Camera::ICameraHostCallback> cameraHostCallback =
        std::make_shared<OHOS::Camera::ICameraHostCallback>();
    cameraHostCallback->OnCameraStatus = nullptr;
    Camera::CamRetCode ret = cameraHost->SetCallback(cameraHostCallback);
    if (ret != Camera::NO_ERROR) {
        std::cout << "==========[test log] SetCallback failed." << std::endl;
        return;
    } else {
        std::cout << "==========[test log] SetCallback success." << std::endl;
    }
}

void TestDisplay::Close()
{
    std::cout << "==========[test log] cameraDevice->Close()." << std::endl;
    if (cameraDevice != nullptr) {
        cameraDevice->Close();
        cameraDevice = nullptr;
        }
}

void TestDisplay::OpenCamera()
{
    if (cameraDevice == nullptr) {
        cameraHost->GetCameraIds(cameraIds);
        const std::shared_ptr<Camera::ICameraDeviceCallback> callback =
            std::make_shared<Camera::ICameraDeviceCallback>();
        rc = cameraHost->OpenCamera(cameraIds.front(), callback, cameraDevice);
        if (rc != Camera::NO_ERROR || cameraDevice == nullptr) {
            std::cout << "==========[test log] OpenCamera failed, rc = " << rc << std::endl;
            return;
        }
        std::cout << "==========[test log]  OpenCamera success." << std::endl;
        }
}

float TestDisplay::calTime(struct timeval start, struct timeval end)
{
    float time_use = 0;
    time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec); // 1000000:time
    return time_use;
}

void TestDisplay::AchieveStreamOperator()
{
    // Create and get streamOperator information
    std::shared_ptr<OHOS::Camera::IStreamOperatorCallback> streamOperatorCallback =
        std::make_shared<OHOS::Camera::IStreamOperatorCallback>();
    rc = cameraDevice->GetStreamOperator(streamOperatorCallback, streamOperator);
    EXPECT_EQ(true, rc == Camera::NO_ERROR);
    if (rc == Camera::NO_ERROR) {
        std::cout << "==========[test log] AchieveStreamOperator success." << std::endl;
    } else {
        std::cout << "==========[test log] AchieveStreamOperator fail, rc = " << rc << std::endl;
    }
}

void TestDisplay::StartStream(std::vector<OHOS::Camera::StreamIntent> intents)
{
    streamInfo = std::make_shared<OHOS::Camera::StreamInfo>();
    streamInfoPre = std::make_shared<OHOS::Camera::StreamInfo>();
    streamInfoCapture = std::make_shared<OHOS::Camera::StreamInfo>();
    streamInfoVideo = std::make_shared<OHOS::Camera::StreamInfo>();
    for (auto& intent : intents) {
    if (intent == 0) {
        std::shared_ptr<IBufferProducer> producer = IBufferProducer::CreateBufferQueue();
        producer->SetQueueSize(8); // 8:set bufferQueue size
        if (producer->GetQueueSize() != 8) { // 8:get bufferQueue size
            std::cout << "~~~~~~~" << std::endl;
        }
        auto callback = [this](std::shared_ptr<SurfaceBuffer> Prebuffer) {
            BufferCallback(Prebuffer, preview_mode);
            return;
        };
        producer->SetCallback(callback);
        streamInfo->streamId_ = streamId_preview;
        streamInfo->width_ = 640; // 640:picture width
        streamInfo->height_ = 480; // 480:picture height
        streamInfo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
        streamInfo->datasapce_ = 8; // 8:picture datasapce
        streamInfo->intent_ = intent;
        streamInfo->tunneledMode_ = 5; // 5:tunnel mode
        streamInfo->bufferQueue_ = producer;
        streamInfos.push_back(streamInfo);
    } else if (intent == 1) {
        std::shared_ptr<IBufferProducer> producerVideo = IBufferProducer::CreateBufferQueue();
        producerVideo->SetQueueSize(8); // 8:set bufferQueue size
        if (producerVideo->GetQueueSize() != 8) { // 8:get bufferQueue size
            std::cout << "~~~~~~~" << std::endl;
        }
        auto callbackVideo = [this](std::shared_ptr<SurfaceBuffer> VideoBuffer) {
            BufferCallback(VideoBuffer, video_mode);
            return;
        };
        producerVideo->SetCallback(callbackVideo);
        streamInfoVideo->streamId_ = streamId_video;
        streamInfoVideo->width_ = 640; // 640:picture width
        streamInfoVideo->height_ = 480; // 480:picture height
        streamInfoVideo->format_ = CAMERA_FORMAT_YUYV_422_PKG;
        streamInfoVideo->datasapce_ = 8; // 8:picture datasapce
        streamInfoVideo->intent_ = intent;
        streamInfoVideo->tunneledMode_ = 5; // 5:tunnel mode
        streamInfo->bufferQueue_ = producerVideo;
        streamInfos.push_back(streamInfoVideo);
    } else {
        std::shared_ptr<IBufferProducer> producerCapture = IBufferProducer::CreateBufferQueue();
        producerCapture->SetQueueSize(8); // 8:set bufferQueue size
        if (producerCapture->GetQueueSize() != 8) { // 8:get bufferQueue size
            std::cout << "~~~~~~~" << std::endl;
        }
        auto callbackCap = [this](std::shared_ptr<SurfaceBuffer> CapBuffer) {
            BufferCallback(CapBuffer, capture_mode);
            return;
        };
        producerCapture->SetCallback(callbackCap);
        streamInfoCapture->streamId_ = streamId_capture;
        streamInfoCapture->width_ = 640; // 640:picture width
        streamInfoCapture->height_ = 480; // 480:picture height
        streamInfoCapture->format_ = CAMERA_FORMAT_YUYV_422_PKG;
        streamInfoCapture->datasapce_ = 8; // 8:picture datasapce
        streamInfoCapture->intent_ = intent;
        streamInfoCapture->tunneledMode_ = 5; // 5:tunnel mode
        streamInfo->bufferQueue_ = producerCapture;
        streamInfos.push_back(streamInfoCapture);
    }
}
    rc = streamOperator->CreateStreams(streamInfos);
    EXPECT_EQ(false, rc != Camera::NO_ERROR);
    if (rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CreateStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CreateStreams fail, rc = " << rc << std::endl;
    }
    rc = streamOperator->CommitStreams(Camera::NORMAL, ability);
    EXPECT_EQ(false, rc != Camera::NO_ERROR);
    if (rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]CommitStreams success." << std::endl;
    } else {
        std::cout << "==========[test log]CommitStreams fail, rc = " << rc << std::endl;
    }
    std::vector<std::shared_ptr<Camera::StreamInfo>>().swap(streamInfos);
}

void TestDisplay::StartCapture(int streamId, int captureId, bool shutterCallback, bool isStreaming)
{
    // Get preview
    captureInfo = std::make_shared<Camera::CaptureInfo>();
    captureInfo->streamIds_ = {streamId};
    captureInfo->captureSetting_ = ability;
    captureInfo->enableShutterCallback_ = shutterCallback;
    rc = streamOperator->Capture(captureId, captureInfo, isStreaming);
    EXPECT_EQ(true, rc == Camera::NO_ERROR);
    if (rc == Camera::NO_ERROR) {
        std::cout << "==========[test log]check Capture: Capture success, " << captureId << std::endl;
    } else {
        std::cout << "==========[test log]check Capture: Capture fail, rc = " << rc << std::endl;
    }
    sleep(2); // 2:sleep two second
}

void TestDisplay::StopStream(std::vector<int>& captureIds, std::vector<int>& streamIds)
{
    if (sizeof(captureIds) > 0) {
        for (auto &captureId : captureIds) {
            rc = streamOperator->CancelCapture(captureId);
            EXPECT_EQ(true, rc == Camera::NO_ERROR);
            if (rc == Camera::NO_ERROR) {
                std::cout << "==========[test log]check Capture: CancelCapture success," << captureId << std::endl;
            } else {
                std::cout << "==========[test log]check Capture: CancelCapture fail, rc = " << rc;
                std::cout << "captureId = " << captureId << std::endl;
            }
        }
    }
    if (sizeof(streamIds) > 0) {
        // release stream
        rc = streamOperator->ReleaseStreams(streamIds);
        EXPECT_EQ(true, rc == Camera::NO_ERROR);
        if (rc == Camera::NO_ERROR) {
            std::cout << "==========[test log]check Capture: ReleaseStreams success." << std::endl;
        } else {
            std::cout << "==========[test log]check Capture: ReleaseStreams fail, rc = " << rc << std::endl;
        }
    }
}