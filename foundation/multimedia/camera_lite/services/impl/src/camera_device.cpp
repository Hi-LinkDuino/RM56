/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "camera_device.h"

#include <fcntl.h>
#include <pthread.h>
#include <string>
#include <sys/io.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <thread>
#include <unistd.h>
#include "codec_interface.h"
#include "display_layer.h"
#include "hal_camera.h"
#include "media_log.h"
#include "meta_data.h"
#include "securec.h"

#include <iostream>
#include <string>
#include <thread>

using namespace OHOS;
using namespace OHOS::Media;
using namespace std;

/** Indicates that the current frame is an Instantaneous Decoder Refresh (IDR) frame. */
const int32_t KEY_IS_SYNC_FRAME = 1;
/** Indicates the frame timestamp. */
const int32_t KEY_TIME_US = 2;

const int32_t IMAGE_WIDTH = 3;       // "DATA_PIX_FORMAT"
const int32_t IMAGE_HEIGHT = 4;       // "DATA_PIX_FORMAT"
const int32_t IMAGE_SIZE = 5;       // "DATA_PIX_FORMAT"
const int32_t DELAY_TIME_ONE_FRAME = 30000;
const int32_t VIDEO_MAX_NUM = 2;        // "video max num"
const int32_t INVALID_STREAM_ID = -1;

namespace OHOS {
namespace Media {
extern Surface *g_surface;
inline PicSize Convert2CodecSize(int32_t width, int32_t height)
{
    struct SizeMap {
        PicSize res_;
        int32_t width_;
        int32_t height_;
    };
    static SizeMap sizeMap[] = {
        {RESOLUTION_CIF, 352, 288},         {RESOLUTION_360P, 640, 360},        {RESOLUTION_D1_PAL, 720, 576},
        {RESOLUTION_D1_NTSC, 720, 480},     {RESOLUTION_720P, 1280, 720},       {RESOLUTION_1080P, 1920, 1080},
        {RESOLUTION_2560X1440, 2560, 1440}, {RESOLUTION_2592X1520, 2592, 1520}, {RESOLUTION_2592X1536, 2592, 1536},
        {RESOLUTION_2592X1944, 2592, 1944}, {RESOLUTION_2688X1536, 2688, 1536}, {RESOLUTION_2716X1524, 2716, 1524},
        {RESOLUTION_3840X2160, 3840, 2160}, {RESOLUTION_4096X2160, 4096, 2160}, {RESOLUTION_3000X3000, 3000, 3000},
        {RESOLUTION_4000X3000, 4000, 3000}, {RESOLUTION_7680X4320, 7680, 4320}, {RESOLUTION_3840X8640, 3840, 8640}
    };

    for (uint32_t i = 0; i < sizeof(sizeMap) / sizeof(SizeMap); i++) {
        if (sizeMap[i].width_ == width && sizeMap[i].height_ == height) {
            return sizeMap[i].res_;
        }
    }
    return RESOLUTION_INVALID;
}

AvCodecMime ConverFormat(ImageFormat format)
{
    if (format == FORMAT_JPEG) {
        return MEDIA_MIMETYPE_IMAGE_JPEG;
    } else if (format == FORMAT_AVC) {
        return MEDIA_MIMETYPE_VIDEO_AVC;
    } else if (format == FORMAT_HEVC) {
        return MEDIA_MIMETYPE_VIDEO_HEVC;
    } else {
        return MEDIA_MIMETYPE_INVALID;
    }
}

static int32_t SetVencSource(CODEC_HANDLETYPE codecHdl, uint32_t deviceId)
{
    Param param = {.key = KEY_DEVICE_ID, .val = (void *)&deviceId, .size = sizeof(uint32_t)};
    int32_t ret = CodecSetParameter(codecHdl, &param, 1);
    if (ret != 0) {
        MEDIA_ERR_LOG("Set enc source failed.(ret=%d)", ret);
        return ret;
    }
    return MEDIA_OK;
}

static uint32_t GetDefaultBitrate(PicSize size)
{
    uint32_t rate; /* auto calc bitrate if set 0 */
    if (size == RESOLUTION_360P) {
        rate = 0x800; /* 2048kbps */
    } else if (size == RESOLUTION_720P) {
        rate = 0x400; /* 1024kbps */
    } else if (size >= RESOLUTION_2560X1440 && size <= RESOLUTION_2716X1524) {
        rate = 0x1800; /* 6144kbps */
    } else if (size == RESOLUTION_3840X2160 || size == RESOLUTION_4096X2160) {
        rate = 0xa000; /* 40960kbps */
    } else {
        rate = 0x0;
    }
    return rate;
}

static int32_t CameraCreateVideoEnc(FrameConfig &fc,
                                    StreamAttr stream,
                                    uint32_t srcDev,
                                    CODEC_HANDLETYPE *codecHdl)
{
    const char *name = "codec.video.hardware.encoder";
    const uint32_t maxParamNum = 10;
    uint32_t paramIndex = 0;
    Param param[maxParamNum];

    CodecType domainKind = VIDEO_ENCODER;
    param[paramIndex].key = KEY_CODEC_TYPE;
    param[paramIndex].val = &domainKind;
    param[paramIndex].size = sizeof(CodecType);
    paramIndex++;

    AvCodecMime codecMime = ConverFormat(stream.format);
    param[paramIndex].key = KEY_MIMETYPE;
    param[paramIndex].val = &codecMime;
    param[paramIndex].size = sizeof(AvCodecMime);
    paramIndex++;

    VenCodeRcMode rcMode = VENCOD_RC_CBR;
    param[paramIndex].key = KEY_VIDEO_RC_MODE;
    param[paramIndex].val = &rcMode;
    param[paramIndex].size = sizeof(VenCodeRcMode);
    paramIndex++;

    VenCodeGopMode gopMode = VENCOD_GOPMODE_NORMALP;
    param[paramIndex].key = KEY_VIDEO_GOP_MODE;
    param[paramIndex].val = &gopMode;
    param[paramIndex].size = sizeof(VenCodeGopMode);
    paramIndex++;

    Profile profile = HEVC_MAIN_PROFILE;
    param[paramIndex].key = KEY_VIDEO_PROFILE;
    param[paramIndex].val = &profile;
    param[paramIndex].size = sizeof(Profile);
    paramIndex++;

#ifdef __LINUX__
    PicSize picSize = Convert2CodecSize(g_surface->GetWidth(), g_surface->GetHeight());
#else
    PicSize picSize = Convert2CodecSize(stream.width, stream.height);
#endif

    MEDIA_DEBUG_LOG("picSize=%d", picSize);
    param[paramIndex].key = KEY_VIDEO_PIC_SIZE;
    param[paramIndex].val = &picSize;
    param[paramIndex].size = sizeof(PicSize);
    paramIndex++;

    uint32_t frameRate = stream.fps;
    MEDIA_DEBUG_LOG("frameRate=%u", frameRate);
    param[paramIndex].key = KEY_VIDEO_FRAME_RATE;
    param[paramIndex].val = &frameRate;
    param[paramIndex].size = sizeof(uint32_t);
    paramIndex++;

    uint32_t bitRate = GetDefaultBitrate(picSize);
    MEDIA_DEBUG_LOG("bitRate=%u kbps", bitRate);
    param[paramIndex].key = KEY_BITRATE;
    param[paramIndex].val = &bitRate;
    param[paramIndex].size = sizeof(uint32_t);
    paramIndex++;

    int32_t ret = CodecCreate(name, param, paramIndex, codecHdl);
    if (ret != 0) {
        MEDIA_ERR_LOG("Create video encoder failed.");
        return MEDIA_ERR;
    }

    ret = SetVencSource(*codecHdl, srcDev);
    if (ret != 0) {
        CodecDestroy(codecHdl);
        return MEDIA_ERR;
    }

    return MEDIA_OK;
}

static int32_t CameraCreateJpegEnc(FrameConfig &fc, StreamAttr stream, uint32_t srcDev, CODEC_HANDLETYPE *codecHdl)
{
    const char *videoEncName = "codec.jpeg.hardware.encoder";
    const uint32_t maxParamNum = 5;
    Param param[maxParamNum];
    uint32_t paramIndex = 0;

    CodecType domainKind = VIDEO_ENCODER;
    param[paramIndex].key = KEY_CODEC_TYPE;
    param[paramIndex].val = &domainKind;
    param[paramIndex].size = sizeof(CodecType);
    paramIndex++;

    AvCodecMime codecMime = ConverFormat(stream.format);
    param[paramIndex].key = KEY_MIMETYPE;
    param[paramIndex].val = &codecMime;
    param[paramIndex].size = sizeof(AvCodecMime);
    paramIndex++;

    auto surfaceList = fc.GetSurfaces();
    Surface *surface = surfaceList.front();

    std::cout<<"------2: CameraCreateJpegEnc: surface width and height: "
        <<surface->GetWidth()<<", "<<surface->GetHeight()<<std::endl;
    PicSize picSize = Convert2CodecSize(surface->GetWidth(), surface->GetHeight());
    param[paramIndex].key = KEY_VIDEO_PIC_SIZE;
    param[paramIndex].val = &picSize;
    param[paramIndex].size = sizeof(PicSize);
    paramIndex++;

    int32_t ret = CodecCreate(videoEncName, param, paramIndex, codecHdl);
    if (ret != 0) {
        return MEDIA_ERR;
    }
    int32_t qfactor = -1;
    fc.GetParameter(PARAM_KEY_IMAGE_ENCODE_QFACTOR, qfactor);
    if (qfactor != -1) {
        Param jpegParam = {
            .key = KEY_IMAGE_Q_FACTOR,
            .val = &qfactor,
            .size = sizeof(qfactor)
        };
        ret = CodecSetParameter(*codecHdl, &jpegParam, 1);
        if (ret != 0) {
            MEDIA_ERR_LOG("CodecSetParameter set jpeg qfactor failed.(ret=%u)", ret);
        }
    }

    ret = SetVencSource(*codecHdl, srcDev);
    if (ret != 0) {
        MEDIA_ERR_LOG("Set video encoder source failed.");
        CodecDestroy(*codecHdl);
        return MEDIA_ERR;
    }

    return MEDIA_OK;
}

static int32_t CopyCodecOutput(void *dst, uint32_t *size, OutputInfo *buffer)
{
    if (dst == nullptr || size == nullptr || buffer == nullptr) {
        return MEDIA_ERR;
    }
    char *dstBuf = reinterpret_cast<char *>(dst);
    for (uint32_t i = 0; i < buffer->bufferCnt; i++) {
        uint32_t packSize = buffer->buffers[i].length - buffer->buffers[i].offset;
        errno_t ret = memcpy_s(dstBuf, *size, buffer->buffers[i].addr + buffer->buffers[i].offset, packSize);
        if (ret != EOK) {
            return MEDIA_ERR;
        }
        *size -= packSize;
        dstBuf += packSize;
    }
    return MEDIA_OK;
}

static void StreamAttrInitialize(StreamAttr *streamAttr, Surface *surface,
                                 StreamType streamType, FrameConfig &fc)
{
    if (streamAttr == nullptr || surface == nullptr) {
        return;
    }
    (void)memset_s(streamAttr, sizeof(StreamAttr), 0, sizeof(StreamAttr));
    streamAttr->type = streamType;
    fc.GetParameter(CAM_IMAGE_FORMAT, streamAttr->format);
    streamAttr->width = surface->GetWidth();
    streamAttr->height = surface->GetHeight();
    fc.GetParameter(CAM_FRAME_FPS, streamAttr->fps);
    fc.GetParameter(CAM_IMAGE_INVERT_MODE, streamAttr->invertMode);
    fc.GetParameter(CAM_IMAGE_CROP_RECT, streamAttr->crop);
}

static ImageFormat Convert2HalImageFormat(uint32_t format)
{
    if (format == CAM_IMAGE_RAW12) {
        return FORMAT_RGB_BAYER_12BPP;
    }
    return FORMAT_YVU420;
}

int32_t RecordAssistant::OnVencBufferAvailble(UINTPTR hComponent, UINTPTR dataIn, OutputInfo *buffer)
{
    CODEC_HANDLETYPE hdl = reinterpret_cast<CODEC_HANDLETYPE>(hComponent);
    RecordAssistant *assistant = reinterpret_cast<RecordAssistant *>(dataIn);
    list<Surface *> *surfaceList = nullptr;
    for (uint32_t idx = 0; idx < assistant->vencHdls_.size(); idx++) {
        if (assistant->vencHdls_[idx] == hdl) {
            surfaceList = &(assistant->vencSurfaces_[idx]);
            break;
        }
    }
    if (surfaceList == nullptr || surfaceList->empty()) {
        MEDIA_ERR_LOG("Encoder handle is illegal.");
        return MEDIA_ERR;
    }
    int32_t ret = -1;
    for (auto &surface : *surfaceList) {
#ifdef __LINUX__
        SurfaceBuffer *surfaceBuf = g_surface->RequestBuffer();
#else
        SurfaceBuffer *surfaceBuf = surface->RequestBuffer();
#endif
        if (surfaceBuf == nullptr) {
            MEDIA_ERR_LOG("No available buffer in surface.");
            break;
        }
#ifdef __LINUX__
        uint32_t size = g_surface->GetSize();
#else
        uint32_t size = surface->GetSize();
#endif
        void *buf = surfaceBuf->GetVirAddr();
        if (buf == nullptr) {
            MEDIA_ERR_LOG("Invalid buffer address.");
            break;
        }
        ret = CopyCodecOutput(buf, &size, buffer);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("No available buffer in surface.");
#ifdef __LINUX__
            g_surface->CancelBuffer(surfaceBuf);
#else
            surface->CancelBuffer(surfaceBuf);
#endif
            break;
        }
        surfaceBuf->SetInt32(KEY_IS_SYNC_FRAME, (((buffer->flag & STREAM_FLAG_KEYFRAME) == 0) ? 0 : 1));
        surfaceBuf->SetInt64(KEY_TIME_US, buffer->timeStamp);
#ifdef __LINUX__
        surfaceBuf->SetSize(g_surface->GetSize() - size);
        if (g_surface->FlushBuffer(surfaceBuf) != 0) {
            MEDIA_ERR_LOG("Flush surface failed.");
            g_surface->CancelBuffer(surfaceBuf);
            ret = -1;
            break;
        }
#else
        surfaceBuf->SetSize(surface->GetSize() - size);
        if (surface->FlushBuffer(surfaceBuf) != 0) {
            MEDIA_ERR_LOG("Flush surface failed.");
            surface->CancelBuffer(surfaceBuf);
            ret = -1;
            break;
        }
#endif
    }
    if (CodecQueueOutput(hdl, buffer, 0, -1) != 0) {
        MEDIA_ERR_LOG("Codec queue output failed.");
    }
    return ret;
}

CodecCallback RecordAssistant::recordCodecCb_ = {nullptr, nullptr, RecordAssistant::OnVencBufferAvailble};

int32_t RecordAssistant::SetFrameConfig(FrameConfig &fc, uint32_t *streamId)
{
    fc_ = &fc;
    auto surfaceList = fc.GetSurfaces();
    if (surfaceList.size() > VIDEO_MAX_NUM || surfaceList.size() == 0) {
        MEDIA_ERR_LOG("the number of surface in frame config must 1 or 2 now.\n");
        return MEDIA_ERR;
    }
    uint32_t num = 0;
    int32_t ret = MEDIA_OK;
    for (auto &surface : surfaceList) {
        CODEC_HANDLETYPE codecHdl = nullptr;
        StreamAttr stream = {};
#ifdef __LINUX__
        StreamAttrInitialize(&stream, g_surface, STREAM_VIDEO, fc);
#else
        StreamAttrInitialize(&stream, surface, STREAM_VIDEO, fc);
#endif
        ret = HalCameraStreamCreate(cameraId_, &stream, streamId);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG(" creat recorder stream failed.");
            return MEDIA_ERR;
        }
        streamIdNum_[num] = *streamId;
        num++;

        StreamInfo streamInfo;
        streamInfo.type = STERAM_INFO_PRIVATE;
        fc.GetVendorParameter(streamInfo.u.data, PRIVATE_TAG_LEN);
        HalCameraStreamSetInfo(cameraId_, *streamId, &streamInfo);

        uint32_t deviceId = 0;
        HalCameraGetDeviceId(cameraId_, *streamId, &deviceId);
        ret = CameraCreateVideoEnc(fc, stream, deviceId, &codecHdl);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("Cannot create suitble video encoder.");
            return MEDIA_ERR;
        }
        ret = CodecSetCallback(codecHdl, &recordCodecCb_, reinterpret_cast<UINTPTR>(this));
        if (ret != 0) {
            MEDIA_ERR_LOG("Set codec callback failed.(ret=%d)", ret);
            CodecDestroy(codecHdl);
            return MEDIA_ERR;
        }
        vencHdls_.emplace_back(codecHdl);
#ifdef __LINUX__
        list<Surface*> conList({g_surface});
#else
        list<Surface*> conList({surface});
#endif
        vencSurfaces_.emplace_back(conList);
    }
    state_ = LOOP_READY;
    return MEDIA_OK;
}

int32_t RecordAssistant::Start(uint32_t streamId)
{
    if (state_ != LOOP_READY) {
        return MEDIA_ERR;
    }
    HalCameraStreamOn(cameraId_, streamId);
    int32_t ret = MEDIA_OK;
    int32_t i;
    for (i = 0; static_cast<uint32_t>(i) < vencHdls_.size(); i++) {
        ret = CodecStart(vencHdls_[i]);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("Video encoder start failed.");
            ret = MEDIA_ERR;
            break;
        }
    }
    if (ret == MEDIA_ERR) {
        /* rollback */
        for (; i >= 0; i--) {
            CodecStop(vencHdls_[i]);
        }
        return MEDIA_ERR;
    }
    state_ = LOOP_LOOPING;
    MEDIA_INFO_LOG("Start camera recording succeed.");
    return MEDIA_OK;
}

int32_t RecordAssistant::Stop()
{
    if (state_ != LOOP_LOOPING) {
        return MEDIA_ERR;
    }
    for (uint32_t i = 0; i < vencHdls_.size(); i++) {
        CodecStop(vencHdls_[i]);
        CodecDestroy(vencHdls_[i]);
    }
    vencHdls_.clear();
    vencSurfaces_.clear();
    for (uint32_t i = 0; i < VIDEO_MAX_NUM; i++) {
        if (streamIdNum_[i] != INVALID_STREAM_ID) {
            HalCameraStreamOff(cameraId_, streamIdNum_[i]);
            HalCameraStreamDestroy(cameraId_, streamIdNum_[i]);
        }
        streamIdNum_[i] = INVALID_STREAM_ID;
    }
    state_ = LOOP_STOP;
    return MEDIA_OK;
}

void* PreviewAssistant::YuvCopyProcess(void *arg)
{
    return nullptr;
}

int32_t PreviewAssistant::SetFrameConfig(FrameConfig &fc, uint32_t *streamId)
{
    fc_ = &fc;
    auto surfaceList = fc.GetSurfaces();
    if (surfaceList.size() != 1) {
        MEDIA_ERR_LOG("Only support one surface in frame config now.");
        return MEDIA_ERR;
    }
    Surface *surface = surfaceList.front();
    StreamAttr stream = {};
    StreamAttrInitialize(&stream, surface, STREAM_PREVIEW, fc);
    int32_t ret = HalCameraStreamCreate(cameraId_, &stream, streamId);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG(" creat preview stream failed.");
        return MEDIA_ERR;
    }
    StreamInfo streamInfo;
    streamInfo.type = STREAM_INFO_POS;
    streamInfo.u.pos.x = std::stoi(surface->GetUserData(string("region_position_x")));
    streamInfo.u.pos.y = std::stoi(surface->GetUserData(string("region_position_y")));

    HalCameraStreamSetInfo(cameraId_, *streamId, &streamInfo);
    streamId_ = *streamId;
    return MEDIA_OK;
}

int32_t PreviewAssistant::Start(uint32_t streamId)
{
    if (state_ == LOOP_LOOPING) {
        return MEDIA_ERR;
    }
    state_ = LOOP_LOOPING;

    int32_t retCode = pthread_create(&threadId, nullptr, YuvCopyProcess, this);
    if (retCode != 0) {
        MEDIA_ERR_LOG("fork thread YuvCopyProcess failed: %d.", retCode);
    }

    int32_t ret = HalCameraStreamOn(cameraId_, streamId);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("Preview start failed of HalCameraStreamOn.(ret=%d)", ret);
        Stop();
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t PreviewAssistant::Stop()
{
    if (state_ != LOOP_LOOPING) {
        return MEDIA_ERR;
    }
    state_ = LOOP_STOP;
    pthread_join(threadId, NULL);
    HalCameraStreamOff(cameraId_, streamId_);
    HalCameraStreamDestroy(cameraId_, streamId_);
    return MEDIA_OK;
}

int32_t CaptureAssistant::SetFrameConfig(FrameConfig &fc, uint32_t *streamId)
{
    auto surfaceList = fc.GetSurfaces();
    if (surfaceList.size() != 1) {
        MEDIA_ERR_LOG("Only support one surface in frame config now.");
        return MEDIA_ERR;
    }
    if (surfaceList.empty()) {
        MEDIA_ERR_LOG("Frame config with empty surface list.");
        return MEDIA_ERR;
    }
    if (surfaceList.size() > 1) {
        MEDIA_WARNING_LOG("Capture only fullfill the first surface in frame config.");
    }
    Surface *surface = surfaceList.front();

    StreamAttr stream = {};
    StreamAttrInitialize(&stream, surface, STREAM_CAPTURE, fc);

    uint32_t deviceId = 0;
    int32_t ret = HalCameraStreamCreate(cameraId_, &stream, streamId);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG(" creat capture stream failed.");
        return MEDIA_ERR;
    }
    streamId_ = *streamId;
    HalCameraGetDeviceId(cameraId_, *streamId, &deviceId);
    ret = CameraCreateJpegEnc(fc, stream, deviceId, &vencHdl_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("Create capture venc failed.");
        return MEDIA_ERR;
    }

    capSurface_ = surface;
    state_ = LOOP_READY;
    return MEDIA_OK;
}

/* Block method, waiting for capture completed */
int32_t CaptureAssistant::Start(uint32_t streamId)
{
    int32_t retCode = MEDIA_ERR;
    state_ = LOOP_LOOPING;
    HalCameraStreamOn(cameraId_, streamId);
    if (capSurface_ == nullptr) {
        MEDIA_ERR_LOG("Create capture venc failed.");
        return retCode;
    }
    int pictures = capSurface_->GetQueueSize();
    int32_t ret = CodecStart(vencHdl_);
    if (ret != 0) {
        MEDIA_ERR_LOG("Start capture encoder failed.(ret=%d)", ret);
        goto FREE_RESOURCE;
    }

    do {
        SurfaceBuffer *surfaceBuf = capSurface_->RequestBuffer();
        if (surfaceBuf == nullptr) {
            MEDIA_ERR_LOG("No available buffer in surface.");
            break;
        }

        OutputInfo outInfo;
        ret = CodecDequeueOutput(vencHdl_, 0, nullptr, &outInfo);
        if (ret != 0) {
            capSurface_->CancelBuffer(surfaceBuf);
            MEDIA_ERR_LOG("Dequeue capture frame failed.(ret=%d)", ret);
            break;
        }

        uint32_t size = capSurface_->GetSize();
        void *buf = surfaceBuf->GetVirAddr();
        if (buf == nullptr) {
            MEDIA_ERR_LOG("Invalid buffer address.");
            break;
        }
        if (CopyCodecOutput(buf, &size, &outInfo) != MEDIA_OK) {
            MEDIA_ERR_LOG("No available buffer in capSurface_.");
            capSurface_->CancelBuffer(surfaceBuf);
            break;
        }
        surfaceBuf->SetSize(capSurface_->GetSize() - size);

        if (capSurface_->FlushBuffer(surfaceBuf) != 0) {
            MEDIA_ERR_LOG("Flush surface buffer failed.");
            capSurface_->CancelBuffer(surfaceBuf);
            break;
        }

        CodecQueueOutput(vencHdl_, &outInfo, 0, -1); // 0:no timeout -1:no fd
        retCode = MEDIA_OK;
    } while (--pictures);

    CodecStop(vencHdl_);

FREE_RESOURCE:
    CodecDestroy(vencHdl_);
    HalCameraStreamOff(cameraId_, streamId);
    HalCameraStreamDestroy(cameraId_, streamId);
#ifndef ENABLE_PASSTHROUGH_MODE
    delete capSurface_;
#endif
    capSurface_ = nullptr;
    state_ = LOOP_STOP;

    return retCode;
}

int32_t CaptureAssistant::Stop()
{
    MEDIA_DEBUG_LOG("No support method.");
    return MEDIA_OK;
}

int32_t CallbackAssistant::SetFrameConfig(FrameConfig &fc, uint32_t *streamId)
{
    fc_ = &fc;
    auto surfaceList = fc.GetSurfaces();
    if (surfaceList.size() != 1) {
        MEDIA_ERR_LOG("Only support one surface in frame config now.");
        return MEDIA_ERR;
    }
    uint32_t imageFormat = 0;
    fc.GetParameter(CAM_IMAGE_FORMAT, imageFormat);
    ImageFormat halImageFormat = Convert2HalImageFormat(imageFormat);
    MEDIA_INFO_LOG("Imageformat is %d", imageFormat);
    Surface *surface = surfaceList.front();
    StreamAttr stream = {};
    StreamAttrInitialize(&stream, surface, STREAM_CALLBACK, fc);
    stream.format = halImageFormat;
    int32_t ret = HalCameraStreamCreate(cameraId_, &stream, streamId);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG(" creat callback stream failed.");
        return MEDIA_ERR;
    }
    streamId_ = *streamId;
    capSurface_ = surface;
    state_ = LOOP_READY;
    return MEDIA_OK;
}

int32_t CallbackAssistant::Start(uint32_t streamId)
{
    if (state_ == LOOP_LOOPING) {
        return MEDIA_ERR;
    }
    state_ = LOOP_LOOPING;
    int32_t retCode = pthread_create(&threadId, nullptr, StreamCopyProcess, this);
    if (retCode != 0) {
        MEDIA_ERR_LOG("fork thread StreamCopyProcess failed: %d.", retCode);
    }
    HalCameraStreamOn(cameraId_, streamId);
    return MEDIA_OK;
}

void* CallbackAssistant::StreamCopyProcess(void *arg)
{
    CallbackAssistant *assistant = (CallbackAssistant *)arg;
    if (assistant == nullptr) {
        MEDIA_ERR_LOG("CallbackAssistant create failed.");
        return nullptr;
    }
    if (assistant->capSurface_ == nullptr) {
        MEDIA_ERR_LOG("capSurface_ is null.\n");
        return nullptr;
    }

    int32_t ret;
    HalBuffer streamBuffer;
    (void)memset_s(&streamBuffer, sizeof(HalBuffer), 0, sizeof(HalBuffer));
    while (assistant->state_ == LOOP_LOOPING) {
        SurfaceBuffer *surfaceBuf = assistant->capSurface_->RequestBuffer();
        if (surfaceBuf == nullptr) {
            usleep(DELAY_TIME_ONE_FRAME);
            continue;
        }

        if (streamBuffer.size != 0x0) {
            HalCameraQueueBuf(assistant->cameraId_, assistant->streamId_, &streamBuffer);
            (void)memset_s(&streamBuffer, sizeof(HalBuffer), 0, sizeof(HalBuffer));
        }
        streamBuffer.format = FORMAT_PRIVATE;
        streamBuffer.size = assistant->capSurface_->GetSize();
        if (surfaceBuf->GetVirAddr() == NULL) {
            MEDIA_ERR_LOG("Invalid buffer address.");
            break;
        }
        streamBuffer.virAddr = surfaceBuf->GetVirAddr();

        ret = HalCameraDequeueBuf(assistant->cameraId_, assistant->streamId_, &streamBuffer);
        if (ret != MEDIA_OK) {
            usleep(DELAY_TIME_ONE_FRAME);
            continue;
        }

        if (assistant->capSurface_->FlushBuffer(surfaceBuf) != 0) {
            MEDIA_ERR_LOG("Flush surface failed.");
            assistant->capSurface_->CancelBuffer(surfaceBuf);
            break;
        }
        usleep(DELAY_TIME_ONE_FRAME);
    }
    if (streamBuffer.size != 0x0) {
        HalCameraQueueBuf(assistant->cameraId_, assistant->streamId_, &streamBuffer);
    }
    MEDIA_DEBUG_LOG(" yuv thread joined \n");
    return nullptr;
}

int32_t CallbackAssistant::Stop()
{
    if (state_ != LOOP_LOOPING) {
        return MEDIA_ERR;
    }
    state_ = LOOP_STOP;
    pthread_join(threadId, NULL);
    HalCameraStreamOff(cameraId_, streamId_);
    HalCameraStreamDestroy(cameraId_, streamId_);
    return MEDIA_OK;
}

CameraDevice::CameraDevice() {}
CameraDevice::CameraDevice(uint32_t cameraId)
{
    this->cameraId = cameraId;
}

CameraDevice::~CameraDevice() {}

int32_t CameraDevice::Initialize()
{
    // Need to be Refactored when delete config file
    int32_t ret = CodecInit();
    if (ret != 0) {
        MEDIA_ERR_LOG("Codec module init failed.(ret=%d)", ret);
        return MEDIA_ERR;
    }
    MEDIA_INFO_LOG("Codec module init succeed.");
    captureAssistant_.state_ = LOOP_READY;
    previewAssistant_.state_ = LOOP_READY;
    recordAssistant_.state_ = LOOP_READY;
    callbackAssistant_.state_ = LOOP_READY;
    captureAssistant_.cameraId_ = cameraId;
    previewAssistant_.cameraId_ = cameraId;
    recordAssistant_.cameraId_ = cameraId;
    callbackAssistant_.cameraId_ = cameraId;
    return MEDIA_OK;
}

int32_t CameraDevice::UnInitialize()
{
    return MEDIA_OK;
}

int32_t CameraDevice::TriggerLoopingCapture(FrameConfig &fc, uint32_t *streamId)
{
    MEDIA_DEBUG_LOG("Camera device start looping capture.");
    DeviceAssistant *assistant = nullptr;
    int32_t fcType = fc.GetFrameConfigType();
    switch (fcType) {
        case FRAME_CONFIG_RECORD:
            assistant = &recordAssistant_;
            break;
        case FRAME_CONFIG_PREVIEW:
            assistant = &previewAssistant_;
            break;
        case FRAME_CONFIG_CAPTURE:
            assistant = &captureAssistant_;
            break;
        case FRAME_CONFIG_CALLBACK:
            assistant = &callbackAssistant_;
            break;
        default:
            break;
    }
    if (assistant == nullptr) {
        MEDIA_ERR_LOG("Invalid frame config type.(type=%d)", fcType);
        return MEDIA_ERR;
    }
    if (assistant->state_ == LOOP_IDLE || assistant->state_ == LOOP_LOOPING || assistant->state_ == LOOP_ERROR) {
        MEDIA_ERR_LOG("Device state is %d, cannot start looping capture.", assistant->state_);
        return MEDIA_ERR;
    }

    int32_t ret = assistant->SetFrameConfig(fc, streamId);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("Check and set frame config failed.(ret=%d)", ret);
        return MEDIA_ERR;
    }

    ret = assistant->Start(*streamId);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("Start looping capture failed.(ret=%d)", ret);
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

void CameraDevice::StopLoopingCapture(int32_t type)
{
    MEDIA_INFO_LOG("Stop looping capture in camera_device.cpp");

    switch (type) {
        case FRAME_CONFIG_RECORD:
            MEDIA_INFO_LOG("Stop recorder");
            recordAssistant_.Stop();;
            break;
        case FRAME_CONFIG_PREVIEW:
            MEDIA_INFO_LOG("Stop preview");
            previewAssistant_.Stop();
            break;
        case FRAME_CONFIG_CALLBACK:
            MEDIA_INFO_LOG("Stop callback");
            callbackAssistant_.Stop();
            break;
        default:
            MEDIA_INFO_LOG("Stop all");
            previewAssistant_.Stop();
            recordAssistant_.Stop();
            callbackAssistant_.Stop();
            break;
    }
}

int32_t CameraDevice::TriggerSingleCapture(FrameConfig &fc, uint32_t *streamId)
{
    return TriggerLoopingCapture(fc, streamId);
}

int32_t CameraDevice::SetCameraConfig()
{
    return MEDIA_OK;
}
} // namespace Media
} // namespace OHOS
