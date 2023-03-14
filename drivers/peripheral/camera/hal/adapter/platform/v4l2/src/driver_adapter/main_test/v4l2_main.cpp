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

#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include "securec.h"
#include "v4l2_uvc.h"
#include "v4l2_dev.h"
#include "project_v4l2_main.h"

namespace OHOS::Camera {
unsigned int g_isPreviewOn;
unsigned int g_isCaptureOn;
unsigned int g_isVideoOn;
unsigned int g_isPreviewOnUvc;
unsigned int g_isCaptureOnUvc;
unsigned int g_isVideoOnUvc;

unsigned int g_camFrameV4l2Exit = 1;
unsigned int g_camFrameV4l2Exit2 = 1;
static pthread_t g_previewThreadId;
static pthread_t g_previewThreadId2;

std::mutex g_frameBufferLock;

static constexpr uint32_t buffersCount = 4;
unsigned int g_bufCont = buffersCount;

int g_fbFd;
int g_fbInitCont;
unsigned char* g_displayBuf = nullptr;
struct fb_var_screeninfo g_vInfo;
struct fb_fix_screeninfo g_fInfo;

std::string g_devNameUvc = {};
static int g_uvcOnlineStatus;
int g_videoFd;
int g_videoFdUvc;
std::shared_ptr<HosV4L2UVC> g_myV4L2UVC;

void StoreVideo(const unsigned char *bufStart, const unsigned int size,
    const std::shared_ptr<FrameSpec>& buffer)
{
    int ret = 0;

    if (buffer->bufferPoolId_ == 0) {
        ret = write(g_videoFd, bufStart, size);
    }
    if (buffer->bufferPoolId_ == 1) {
        ret = write(g_videoFdUvc, bufStart, size);
    }

    if (ret == -1) {
        CAMERA_LOGE("write video file error %s.....\n", strerror(errno));
    }
}

void StoreImage(const unsigned char *bufStart, const unsigned int size, const int index,
    const std::shared_ptr<FrameSpec>& buffer)
{
    constexpr uint32_t pathLen = 20;
    char path[pathLen] = {0};
    int imgFD = 0;
    int ret;

    if (buffer->bufferPoolId_ == 0) {
        sprintf_s(path, sizeof(path), "./picture%d.jpeg", index);
    }

    if (buffer->bufferPoolId_ == 1) {
        sprintf_s(path, sizeof(path), "./UVC%d.jpeg", index);
    }

    imgFD = open(path, O_RDWR | O_CREAT, 00766); // 00766:file operate permission
    if (imgFD == -1) {
        CAMERA_LOGE("open image file error %s.....\n", strerror(errno));
    }

    CAMERA_LOGD("store_image  buf_start == %p size == %d index %d\n", bufStart, size, index);

    ret = write(imgFD, bufStart, size);
    if (ret == -1) {
        CAMERA_LOGE("write image file error %s.....\n", strerror(errno));
    }

    close(imgFD);
}

int DoFbMunmap(unsigned char* addr)
{
    constexpr uint32_t bit = 8;
    unsigned int size = g_vInfo.xres * g_vInfo.yres * g_vInfo.bits_per_pixel / bit;

    CAMERA_LOGD("main test:munmapped size = %d, virt_addr = 0x%p\n", size, addr);

    return munmap(addr, size);
}

unsigned char* DoFbMmap(const int pmemfd)
{
    constexpr uint32_t bit = 8;
    int screensize = g_vInfo.xres * g_vInfo.yres * g_vInfo.bits_per_pixel / bit;

    unsigned char *ret = reinterpret_cast<unsigned char*>(
        mmap(nullptr, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, pmemfd, 0));
    if (ret == reinterpret_cast<unsigned char *>(MAP_FAILED)) {
        CAMERA_LOGE("main test:do_mmap: pmem mmap() failed: %s (%d)\n", strerror(errno), errno);
        return nullptr;
    }

    CAMERA_LOGD("main test:do_mmap: pmem mmap fd %d ptr %p len %u\n", pmemfd, ret, screensize);

    return ret;
}

void FBLog()
{
    CAMERA_LOGD("the fixed information is as follow:\n");
    CAMERA_LOGD("id=%s\n", g_fInfo.id);
    CAMERA_LOGD("sem_start=%lx\n", g_fInfo.smem_start);
    CAMERA_LOGD("smem_len=%u\n", g_fInfo.smem_len);
    CAMERA_LOGD("type=%u\n", g_fInfo.type);
    CAMERA_LOGD("line_length=%u\n", g_fInfo.line_length);
    CAMERA_LOGD("mmio_start=%lu\n", g_fInfo.mmio_start);
    CAMERA_LOGD("mmio_len=%d\n", g_fInfo.mmio_len);
    CAMERA_LOGD("visual=%d\n", g_fInfo.visual);

    CAMERA_LOGD("variable information is as follow:\n");
    CAMERA_LOGD("The xres is :%u\n", g_vInfo.xres);
    CAMERA_LOGD("The yres is :%u\n", g_vInfo.yres);
    CAMERA_LOGD("xres_virtual=%u\n", g_vInfo.xres_virtual);
    CAMERA_LOGD("yres_virtual=%u\n", g_vInfo.yres_virtual);
    CAMERA_LOGD("xoffset=%u\n", g_vInfo.xoffset);
    CAMERA_LOGD("yoffset=%u\n", g_vInfo.yoffset);
    CAMERA_LOGD("bits_per_pixel is :%u\n", g_vInfo.bits_per_pixel);
    CAMERA_LOGD("red.offset=%u\n", g_vInfo.red.offset);
    CAMERA_LOGD("red.length=%u\n", g_vInfo.red.length);
    CAMERA_LOGD("red.msb_right=%u\n", g_vInfo.red.msb_right);
    CAMERA_LOGD("green.offset=%d\n", g_vInfo.green.offset);
    CAMERA_LOGD("green.length=%d\n", g_vInfo.green.length);
    CAMERA_LOGD("green.msb_right=%d\n", g_vInfo.green.msb_right);
    CAMERA_LOGD("blue.offset=%d\n", g_vInfo.blue.offset);
    CAMERA_LOGD("blue.length=%d\n", g_vInfo.blue.length);
    CAMERA_LOGD("blue.msb_right=%d\n", g_vInfo.blue.msb_right);
    CAMERA_LOGD("transp.offset=%d\n", g_vInfo.transp.offset);
    CAMERA_LOGD("transp.length=%d\n", g_vInfo.transp.length);
    CAMERA_LOGD("transp.msb_right=%d\n", g_vInfo.transp.msb_right);
    CAMERA_LOGD("height=%x\n", g_vInfo.height);
}

void FBUninit()
{
    CAMERA_LOGD("main test:FBUninit cont %d\n", g_fbInitCont);

    if (--g_fbInitCont == 0) {
        DoFbMunmap(g_displayBuf);
        close(g_fbFd);
        g_fbFd = 0;
    }
}

RetCode FBInit()
{
    g_fbInitCont++;

    if (g_fbFd)
        return RC_OK;

    g_fbFd = open("/dev/fb0", O_RDWR);
    if (g_fbFd < 0) {
        CAMERA_LOGE("main test:cannot open framebuffer %s file node\n", "/dev/fb0");
        return RC_ERROR;
    }

    if (ioctl(g_fbFd, FBIOGET_VSCREENINFO, &g_vInfo) < 0) {
        CAMERA_LOGE("main test:cannot retrieve vscreenInfo!\n");
        close(g_fbFd);
        return RC_ERROR;
    }

    if (ioctl(g_fbFd, FBIOGET_FSCREENINFO, &g_fInfo) < 0) {
        CAMERA_LOGE("main test:can't retrieve fscreenInfo!\n");
        close(g_fbFd);
        return RC_ERROR;
    }

    FBLog();

    CAMERA_LOGD("main test:allocating display buffer memory\n");
    g_displayBuf = DoFbMmap(g_fbFd);
    if (g_displayBuf == nullptr) {
        CAMERA_LOGE("main test:error g_displayBuf mmap error\n");
        close(g_fbFd);
        return RC_ERROR;
    }
    g_fbFd = -1;
    return RC_OK;
}

#define RANGE_LIMIT(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : (x)))
void ProcessImage(const unsigned char* p, unsigned char* fbp)
{
    constexpr uint32_t imageWidth = 640;
    constexpr uint32_t imageHeight = 480;

    const unsigned char* in = p;
    int width = imageWidth;
    int height = imageHeight;
    int istride = 1280;
    int x, y, j;
    int y0, u, v, r, g, b;
    long location = 0;
    int xpos = (g_vInfo.xres - width) / 2;
    int ypos = (g_vInfo.yres - height) / 2;
    int yPos, uPos, vPos;

    yPos = 0;
    uPos = 1;
    vPos = 3; // 3:v position 3
    uint32_t bitsPerByte = 8; // 8:8 bits per byte

    for (y = ypos; y < (height + ypos); y++) {
        for (j = 0, x = xpos; j < width; j++, x++) {
            location = (x + g_vInfo.xoffset) * (g_vInfo.bits_per_pixel / bitsPerByte) +
                       (y + g_vInfo.yoffset) * g_fInfo.line_length;

            y0 = in[yPos];
            u = in[uPos] - 128; // 128:offset of u
            v = in[vPos] - 128; // 128:offset of v

            r = RANGE_LIMIT(y0 + v + ((v * 103) >> 8));
            g = RANGE_LIMIT(y0 - ((u * 88) >> 8) - ((v * 183) >> 8));
            b = RANGE_LIMIT(y0 + u + ((u * 198) >> 8));
            fbp[location + 0] = (((g & 0x1C) << 3) | (b >> 3)); // 3:8 bits
            fbp[location + 1] = ((r & 0xF8) | (g >> 5)); // 5:32 bits
            yPos += 2; // 2:y position

            if (j & 0x01) {
                uPos += 4; // 4:u position
                vPos += 4; // 4:v position
            }
        }

        yPos = 0;
        uPos = 1;
        vPos = 3; // 3:v position 3
        in += istride;
    }
}

void LcdDrawScreen(unsigned char* displayBuf, const unsigned char* addr)
{
    ProcessImage(addr, displayBuf);
}

void V4L2UvcCallback(const std::string& cameraId, const std::vector<DeviceControl>& control,
    const std::vector<DeviceFormat>& fromat, const bool inOut)
{
    CAMERA_LOGD("main test:V4L2UvcCallback cameraId = %s, inOut = %d\n", cameraId.c_str(), inOut);
    if (control.empty() || fromat.empty()) {
        CAMERA_LOGD("main test:V4L2UvcCallback inOut = %d, control = %d fromat = %d\n",
            inOut, control.empty(), fromat.empty());
        return;
    }
    if (inOut) {
        g_uvcOnlineStatus = 1;
        for (auto iter = fromat.cbegin(); iter != fromat.cend(); iter++) {
            CAMERA_LOGD("main test: %s width %d height %d fps numerator %d denominator %d\n",
                iter->fmtdesc.description.c_str(), iter->fmtdesc.width, iter->fmtdesc.height,
                iter->fmtdesc.fps.numerator, iter->fmtdesc.fps.denominator);
        }
        CAMERA_LOGD("\n\n\n");
        for (auto iter = control.cbegin(); iter != control.cend(); iter++) {
            if (iter->type == V4L2_CTRL_TYPE_CTRL_CLASS) {
                CAMERA_LOGD("%s\n", iter->name.c_str());
                continue;
            }
            CAMERA_LOGD("main test: %s value %d id 0x%x ctr_type %d minimum %d maximum %d step %d \
                default_value %d\n", iter->name.c_str(), iter->value, iter->id, iter->type, iter->minimum,
                iter->maximum, iter->step, iter->default_value);
        }

        g_devNameUvc = cameraId;
    } else {
        g_uvcOnlineStatus = 0;
        g_devNameUvc = {};
        CAMERA_LOGD("main test:V4L2UvcCallback inOut = %d, control empty = %d fromat empty = %d\n",
            inOut, control.empty(), fromat.empty());
    }
}

void V4L2BufferCallback(std::shared_ptr<FrameSpec> buffer)
{
    {
        std::lock_guard<std::mutex> fb(g_frameBufferLock);
        if (g_isCaptureOn || g_isCaptureOnUvc) {
            unsigned char* addr = (unsigned char*)buffer->buffer_->GetVirAddress();
            uint32_t size = buffer->buffer_->GetSize();
            int32_t index = buffer->buffer_->GetIndex();
            StoreImage(addr, size, index, buffer);
        } else if (g_isVideoOn || g_isVideoOnUvc) {
            unsigned char* addr = (unsigned char*)buffer->buffer_->GetVirAddress();
            uint32_t size = buffer->buffer_->GetSize();
            StoreVideo(addr, size, buffer);
        } else {
            if (g_displayBuf != nullptr) {
                LcdDrawScreen(g_displayBuf, (unsigned char*)buffer->buffer_->GetVirAddress());
            }
        }
    }

    constexpr uint32_t callbackDelayTime = 8000;
    usleep(callbackDelayTime);
    buffer->buffer_->SetUsage(0);
}

RetCode V4L2CamFrame(const std::string& cameraID,
    const std::shared_ptr<HosV4L2Dev>& myV4L2Dev,
    std::shared_ptr<FrameSpec>* buffptr, uint32_t size)
{
    do {
        for (int i = 0; i < size; ++i) {
            if (!buffptr[i]->buffer_->GetUsage()) {
                myV4L2Dev->QueueBuffer(cameraID, buffptr[i]);
                buffptr[i]->buffer_->SetUsage(1);
            }
        }

        constexpr uint32_t frameDelayTime = 3000;
        usleep(frameDelayTime);
    } while ((g_camFrameV4l2Exit == 0) || (g_camFrameV4l2Exit2 == 0));

    CAMERA_LOGD("main test:Exiting V4L2CamFrame thread -- \n");

    myV4L2Dev->StopStream(cameraID);
    myV4L2Dev->ReleaseBuffers(cameraID);

    return RC_OK;
}

RetCode V4L2StartFrame(const std::string& cameraID,
    const std::shared_ptr<HosV4L2Dev>& myV4L2Dev,
    std::shared_ptr<FrameSpec>* buffptr, uint32_t size)
{
    int rc = 0;

    rc = myV4L2Dev->SetCallback(V4L2BufferCallback);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test:V4L2StartPreview Callback is nullptr\n");
        return RC_ERROR;
    }

    rc = myV4L2Dev->StartStream(cameraID);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test:V4L2StartPreview StartStream error\n");
        return RC_ERROR;
    }

    rc = V4L2CamFrame(cameraID, myV4L2Dev, buffptr, size);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test:V4L2StartPreview V4L2CamFrame error\n");
        return RC_ERROR;
    }

    return RC_OK;
}

void V4L2SetDeviceFormat(DeviceFormat& format, const std::string& devname)
{
    constexpr uint32_t captureHeight = 720;
    constexpr uint32_t captureWidth = 1280;
    constexpr uint32_t width = 640;
    constexpr uint32_t height = 480;

    if (devname == "uvcvideo" || devname == "uvcvideo1") {
        if (g_isPreviewOnUvc) {
            format.fmtdesc.pixelformat = V4L2_PIX_FMT_YUYV;
            format.fmtdesc.width = width;
            format.fmtdesc.height = height;
        }
        if (g_isCaptureOnUvc) {
            format.fmtdesc.pixelformat = V4L2_PIX_FMT_MJPEG;
            format.fmtdesc.width = width;
            format.fmtdesc.height = height;
        }
        if (g_isVideoOnUvc) {
            format.fmtdesc.pixelformat = V4L2_PIX_FMT_MJPEG;
            format.fmtdesc.width = width;
            format.fmtdesc.height = height;
        }
    }

    if (devname == TEST_SENSOR_NAME) {
        if (g_isPreviewOn) {
            format.fmtdesc.pixelformat = PREVIEW_PIXEL_FORMAT;
            format.fmtdesc.width = width;
            format.fmtdesc.height = height;
        }
        if (g_isCaptureOn) {
            format.fmtdesc.pixelformat = CAPTURE_PIXEL_FORMAT;
            format.fmtdesc.width = captureWidth;
            format.fmtdesc.height = captureHeight;
        }
        if (g_isVideoOn) {
            format.fmtdesc.pixelformat = VIDEO_PIXEL_FORMAT;
            format.fmtdesc.width = captureWidth;
            format.fmtdesc.height = captureHeight;
        }
    }
}

RetCode V4L2ConfigFormat(DeviceFormat& format, const std::string& devname,
    const std::shared_ptr<HosV4L2Dev>& myV4L2Dev)
{
    RetCode rc = RC_OK;

    rc = myV4L2Dev->ConfigSys(devname, CMD_V4L2_SET_FORMAT, format);
    if (rc == RC_ERROR) {
        return rc;
    }

    rc = myV4L2Dev->ReqBuffers(devname, g_bufCont);
    if (rc == RC_ERROR) {
        return rc;
    }

    rc = myV4L2Dev->ConfigSys(devname, CMD_V4L2_GET_FORMAT, format);
    if (rc == RC_ERROR) {
        return rc;
    }

    return rc;
}

void V4L2SetBuffers(const int32_t i, std::shared_ptr<FrameSpec> buffptr[],
    const uint32_t bufSize, const std::string& devname)
{
    buffptr[i] = std::make_shared<FrameSpec>();
    buffptr[i]->buffer_ = std::make_shared<IBuffer>();

    buffptr[i]->buffer_->SetIndex(i);
    buffptr[i]->buffer_->SetSize(bufSize);
    buffptr[i]->buffer_->SetUsage(1);

    if (devname == "uvcvideo" || devname == "uvcvideo1") {
        buffptr[i]->bufferPoolId_ = 1;
    }

    if (devname == TEST_SENSOR_NAME) {
        buffptr[i]->bufferPoolId_ = 0;
    }
}

void* V4L2FrameThread(void* data)
{
    int rc, i;
    const std::string devname((const char*)data);
    unsigned char* addr[g_bufCont];
    std::shared_ptr<FrameSpec> buffptr[buffersCount];
    DeviceFormat format = {};
    unsigned int bufSize;

    auto myV4L2Dev = std::make_shared<HosV4L2Dev>();
    if (myV4L2Dev == nullptr) {
        CAMERA_LOGE("main test:std::make_shared<HosV4L2Dev> is nullptr\n");
        return nullptr;
    }

    rc = myV4L2Dev->start(devname);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test:start %s fail rc = %d\n", devname.c_str(), rc);
        return nullptr;
    }

    V4L2SetDeviceFormat(format, devname);
    rc = V4L2ConfigFormat(format, devname, myV4L2Dev);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test:V4L2ConfigFormat fail rc = %d\n", rc);
        goto done1;
    }

    bufSize = format.fmtdesc.sizeimage;
    for (i = 0; i < g_bufCont; ++i) {
        V4L2SetBuffers(i, buffptr, bufSize, devname);

        addr[i] = (unsigned char*)malloc(bufSize);
        if (addr == nullptr) {
            CAMERA_LOGE("main test:V4L2PreviewThread malloc buffers fail \n");
            goto done;
        }
        buffptr[i]->buffer_->SetVirAddress(addr[i]);

        rc = myV4L2Dev->CreatBuffer(devname, buffptr[i]);
        if (rc == RC_ERROR) {
            CAMERA_LOGE("main test:V4L2PreviewThread CreatBuffer fail i = %d\n", i);
            goto done;
        }
    }

    if (V4L2StartFrame(devname, myV4L2Dev, buffptr, buffersCount) == RC_ERROR)
        CAMERA_LOGE("main test:V4L2PreviewThread V4L2StartPreview fail\n");

done:
    for (int j = 0; j < i; ++j) {
        free(addr[j]);
    }

done1:
    myV4L2Dev->stop(devname);

    return nullptr;
}

static void Usage(FILE* fp)
{
    (void)fprintf(fp,
            "Options:\n"
            "-h | --help          Print this message\n"
            "-p | --preview       start preview on platform sensor\n"
            "-c | --capture       capture one picture\n"
            "-w | --set WB        Set white balance Cloudy\n"
            "-e | --Set AE        Set exposure time\n"
            "-v | --video         capture Viedeo of 10s\n"
            "-u | --uvc           start preview on uvc preview\n"
            "-a | --Set ATE       Set Auto exposure\n"
            "-q | --quit          stop preview and quit this app\n");
}

const struct option g_longOptions[] = {
    {"help", no_argument, nullptr, 'h'}, {"preview", no_argument, nullptr, 'p'},
    {"capture", no_argument, nullptr, 'c'}, {"WB", no_argument, nullptr, 'w'},
    {"AE", no_argument, nullptr, 'e'}, {"video", no_argument, nullptr, 'v'},
    {"uvc", no_argument, nullptr, 'u'}, {"quit", no_argument, nullptr, 'q'},
    {"AE Auto", no_argument, nullptr, 'a'}, {0, 0, 0, 0}
};

static int PutMenuAndGetChr(void)
{
    constexpr uint32_t inputCount = 50;
    int c = 0;
    char str[inputCount];

    Usage(stdout);
    CAMERA_LOGD("please input command(input -q exit this app)\n");
    char* ret = fgets(str, inputCount, stdin);
    if (ret == nullptr) {
        return -1;
    }

    for (int i = 0; i < inputCount; i++) {
        if (str[i] != '-') {
            c = str[i];
            break;
        }
    }

    return c;
}

void StopAllFrame(bool freeFb)
{
    if (!g_camFrameV4l2Exit && !g_camFrameV4l2Exit2) {
        g_camFrameV4l2Exit = 1;
        g_camFrameV4l2Exit2 = 1;

        CAMERA_LOGD("main test:StopAllFrame pthread_join g_previewThreadId2 %ld g_previewThreadId %ld\n",
            g_previewThreadId2, g_previewThreadId);
        pthread_join(g_previewThreadId2, nullptr);
        pthread_join(g_previewThreadId, nullptr);

        if (freeFb) {
            FBUninit();
        }
    } else {
        if (!g_camFrameV4l2Exit2) {
            g_camFrameV4l2Exit2 = 1;

            CAMERA_LOGD("main test:StopAllFrame pthread_join g_previewThreadId2 %ld\n", g_previewThreadId2);
            pthread_join(g_previewThreadId2, nullptr);
            if (freeFb) {
                FBUninit();
            }
        }
        if (!g_camFrameV4l2Exit) {
            g_camFrameV4l2Exit = 1;
            CAMERA_LOGD("main test:StopAllFrame pthread_join g_previewThreadId %ld\n", g_previewThreadId);
            pthread_join(g_previewThreadId, nullptr);

            if (freeFb) {
                FBUninit();
            }
        }
    }
}

RetCode V4L2InitSensors()
{
    int rc;
    std::vector<std::string> cameraIDs;

    cameraIDs.push_back(TEST_SENSOR_NAME);

    // Init platform sensors
    rc = HosV4L2Dev::Init(cameraIDs);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("main test:main myV4L2Dev->Init error rc = %d\n", rc);
        return RC_ERROR;
    }

    g_myV4L2UVC = std::make_shared<HosV4L2UVC>();
    g_myV4L2UVC->V4L2UvcDetectInit(V4L2UvcCallback);

    return RC_OK;
}

void StartFrame()
{
    std::string devName;

    devName = TEST_SENSOR_NAME;
    g_camFrameV4l2Exit = 0;
    g_isPreviewOn = 1;
    pthread_create(&g_previewThreadId, nullptr, V4L2FrameThread, (void*)devName.c_str());

    sleep(1);
}

void StartUvcFrame()
{
    if (g_uvcOnlineStatus) {
        FBInit();

        g_camFrameV4l2Exit2 = 0;
        g_isPreviewOnUvc = 1;
        pthread_create(&g_previewThreadId2, nullptr, V4L2FrameThread, (void*)g_devNameUvc.c_str());
    } else {
        CAMERA_LOGD("main test:uvcOnlineStatus = % d, please inset UVC Camera\n", g_uvcOnlineStatus);
    }

    sleep(1);
}

void StartCapture()
{
    std::string devName;

    StopAllFrame(0);
    if (g_isPreviewOn || g_isPreviewOnUvc) {
        if (g_isPreviewOn) {
            g_isPreviewOn = 0;
            g_isCaptureOn = 1;
            devName = TEST_SENSOR_NAME;
            g_camFrameV4l2Exit = 0;
            pthread_create(&g_previewThreadId, nullptr, V4L2FrameThread, (void*)devName.c_str());
        }

        if (g_isPreviewOnUvc) {
            g_isPreviewOnUvc = 0;
            g_isCaptureOnUvc = 1;
            g_camFrameV4l2Exit2 = 0;
            pthread_create(&g_previewThreadId2, nullptr, V4L2FrameThread, (void*)g_devNameUvc.c_str());
        }

        sleep(1);
        StopAllFrame(0);

        if (g_isCaptureOnUvc) {
            g_isPreviewOnUvc = 1;
            g_isCaptureOnUvc = 0;
            g_camFrameV4l2Exit2 = 0;
            pthread_create(&g_previewThreadId2, nullptr, V4L2FrameThread, (void*)g_devNameUvc.c_str());
        }
        if (g_isCaptureOn) {
            g_isPreviewOn = 1;
            g_isCaptureOn = 0;
            devName = TEST_SENSOR_NAME;
            g_camFrameV4l2Exit = 0;
            pthread_create(&g_previewThreadId, nullptr, V4L2FrameThread, (void*)devName.c_str());
        }

        sleep(1);
    } else {
        CAMERA_LOGE("please first preview Camera\n");
    }
}

void SetAWb()
{
    int value;
    std::string devName;
    constexpr uint32_t awbModeValue = 8;

    if (g_isPreviewOn || g_isPreviewOnUvc) {
        auto v4l2Dev = std::make_shared<HosV4L2Dev>();
        if (v4l2Dev != nullptr) {
            if (g_isPreviewOn) {
                devName = TEST_SENSOR_NAME;
                v4l2Dev->QuerySetting(devName, CMD_AWB_MODE, &value);
                CAMERA_LOGD("v4l2Dev->QuerySetting CMD_AWB_MODE value %d\n", value);

                int setValue = awbModeValue;
                v4l2Dev->UpdateSetting(devName, CMD_AWB_MODE, &setValue);
            }
        }
    } else {
        CAMERA_LOGE("please first preview Camera\n");
    }
}

void SetExpotime()
{
    std::string devName;
    int value;
    constexpr uint32_t aeExpotimeValue = 50;

    if (g_isPreviewOn || g_isPreviewOnUvc) {
        auto v4l2Dev = std::make_shared<HosV4L2Dev>();
        if (v4l2Dev != nullptr) {
            if (g_isPreviewOn) {
                devName = TEST_SENSOR_NAME;
                v4l2Dev->QuerySetting(devName, CMD_AE_EXPOTIME, &value);
                CAMERA_LOGD("v4l2Dev->QuerySetting CMD_AE_EXPOTIME value %d\n", value);

                int setValue = 1;
                v4l2Dev->UpdateSetting(devName, CMD_AE_EXPO, &setValue);

                setValue = aeExpotimeValue;
                v4l2Dev->UpdateSetting(devName, CMD_AE_EXPOTIME, &setValue);
            }
            if (g_isPreviewOnUvc) {
                v4l2Dev->QuerySetting(g_devNameUvc, CMD_AE_EXPOTIME, &value);
                CAMERA_LOGD("v4l2Dev->QuerySetting CMD_AE_EXPOTIME value %d\n", value);

                int setValue = 1;
                v4l2Dev->UpdateSetting(g_devNameUvc, CMD_AE_EXPO, &setValue);

                setValue = aeExpotimeValue;
                v4l2Dev->UpdateSetting(g_devNameUvc, CMD_AE_EXPOTIME, &setValue);
            }
        }
    } else {
        CAMERA_LOGE("please first preview Camera\n");
    }
}

void SetAeExpo()
{
    std::string devName;
    int value;

    if (g_isPreviewOn || g_isPreviewOnUvc) {
        auto v4l2Dev = std::make_shared<HosV4L2Dev>();
        if (v4l2Dev != nullptr) {
            if (g_isPreviewOn) {
                devName = TEST_SENSOR_NAME;
                v4l2Dev->QuerySetting(devName, CMD_AE_EXPO, &value);
                CAMERA_LOGD("v4l2Dev->QuerySetting CMD_AE_EXPO value %d\n", value);

                int setValue = 0;
                v4l2Dev->UpdateSetting(devName, CMD_AE_EXPO, &setValue);
            }
            if (g_isPreviewOnUvc) {
                v4l2Dev->QuerySetting(g_devNameUvc, CMD_AE_EXPO, &value);
                CAMERA_LOGD("v4l2Dev->QuerySetting CMD_AE_EXPO value %d\n", value);

                int setValue = 3;
                v4l2Dev->UpdateSetting(g_devNameUvc, CMD_AE_EXPO, &setValue);
            }
        }
    } else {
        CAMERA_LOGE("please first preview Camera\n");
    }
}

void StartVideo()
{
    std::string devName;
    int value;
    constexpr uint32_t videoTime = 10;

    StopAllFrame(0);
    if (g_isPreviewOn || g_isPreviewOnUvc) {
        if (g_isPreviewOn) {
            g_isPreviewOn = 0;
            g_isVideoOn = 1;
            devName = TEST_SENSOR_NAME;
            g_camFrameV4l2Exit = 0;
            g_videoFd = open("video.h264", O_RDWR | O_CREAT, 00766); // 00766:file operate permission
            pthread_create(&g_previewThreadId, nullptr, V4L2FrameThread, (void*)devName.c_str());
        }

        if (g_isPreviewOnUvc) {
            g_isPreviewOnUvc = 0;
            g_isVideoOnUvc = 1;
            g_camFrameV4l2Exit2 = 0;
            g_videoFdUvc = open("uvc.h264", O_RDWR | O_CREAT, 00766); // 00766:file operate permission
            pthread_create(&g_previewThreadId2, nullptr, V4L2FrameThread, (void*)g_devNameUvc.c_str());
        }

        sleep(videoTime);
        StopAllFrame(0);

        if (g_isVideoOnUvc) {
            g_isPreviewOnUvc = 1;
            g_isVideoOnUvc = 0;
            g_camFrameV4l2Exit2 = 0;
            pthread_create(&g_previewThreadId2, nullptr, V4L2FrameThread, (void*)g_devNameUvc.c_str());
        }
        if (g_isVideoOn) {
            g_isPreviewOn = 1;
            g_isVideoOn = 0;
            devName = TEST_SENSOR_NAME;
            g_camFrameV4l2Exit = 0;
            pthread_create(&g_previewThreadId, nullptr, V4L2FrameThread, (void*)devName.c_str());
        }

        sleep(1);
    } else {
        CAMERA_LOGE("please first preview Camera\n");
    }
}

void QuitMain()
{
    StopAllFrame(1);
    g_isPreviewOn = 0;
    g_isPreviewOnUvc = 0;
    CAMERA_LOGD("main test:StopAllFrame(1) done\n");

    sleep(1);

    g_myV4L2UVC->V4L2UvcDetectUnInit();
}

int main(int argc, char** argv)
{
    int idx, c;
    constexpr char shortOptions[] = "h:pcwevuaq:";

    V4L2InitSensors();

    c = getopt_long(argc, argv, shortOptions, g_longOptions, &idx);
    while (1) {
        switch (c) {
            case 'h':
                c = PutMenuAndGetChr();
                break;

            case 'p':
                FBInit();
                StartFrame();
                c = PutMenuAndGetChr();
                break;

            case 'c':
                StartCapture();
                c = PutMenuAndGetChr();
                break;

            case 'w':
                SetAWb();
                c = PutMenuAndGetChr();
                break;

            case 'e':
                SetExpotime();
                c = PutMenuAndGetChr();
                break;

            case 'a':
                SetAeExpo();
                c = PutMenuAndGetChr();
                break;

            case 'v':
                StartVideo();
                c = PutMenuAndGetChr();
                break;

            case 'u':
                StartUvcFrame();
                c = PutMenuAndGetChr();
                break;

            case 'q':
                QuitMain();
                exit(EXIT_SUCCESS);

            default:
                c = PutMenuAndGetChr();
                break;
        }
    }

    return RC_OK;
}
} // namespace OHOS::Camera

int main(int argc, char** argv)
{
    OHOS::Camera::main(argc, argv);

    return 0;
}
