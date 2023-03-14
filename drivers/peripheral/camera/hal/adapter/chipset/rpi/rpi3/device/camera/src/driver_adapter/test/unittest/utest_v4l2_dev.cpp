/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <v4l2_dev.h>
#include <v4l2_uvc.h>

#include "utest_v4l2.h"

using namespace testing::ext;
namespace OHOS::Camera {
void V4L2UvcCallback(const std::string cameraId, const std::vector<DeviceControl>& control,
    const std::vector<DeviceFormat>& fromat, const bool inOut)
{
    std::cout << "V4L2UvcCallback" << std::endl;
}

void V4L2BufferCallback(std::shared_ptr<FrameSpec> buffer)
{
    std::cout << "V4L2BufferCallback" << std::endl;
}

void UtestV4L2Dev::SetUpTestCase(void)
{
    std::cout << "SetUpTestCase.." << std::endl;

    V4L2UVC_ = std::make_shared<HosV4L2UVC>();
    EXPECT_EQ(true, V4L2UVC_ != nullptr);

    V4L2Dev_ = std::make_shared<HosV4L2Dev>();
    EXPECT_EQ(true, V4L2Dev_ != nullptr);
}

void UtestV4L2Dev::TearDownTestCase(void)
{
    std::cout << "TearDownTestCase.." << std::endl;
}

void UtestV4L2Dev::SetUp(void)
{
    std::cout << "SetUp.." << std::endl;
}

void UtestV4L2Dev::TearDown(void)
{
    std::cout << "TearDown.." << std::endl;
}

HWTEST_F(UtestV4L2Dev, InitUvc, TestSize.Level0)
{
    V4L2UVC_->V4L2UvcDetectInit(V4L2UvcCallback);
}

HWTEST_F(UtestV4L2Dev, InitCamera, TestSize.Level0)
{
    int rc = 0;

    cameraIDs_.push_back("bm2835 mmal");
    rc = HosV4L2Dev::Init(cameraIDs_);

    EXPECT_EQ(true, rc != RC_ERROR);
}

HWTEST_F(UtestV4L2Dev, SetFormat, TestSize.Level0)
{
    constexpr uint32_t width = 640;
    constexpr uint32_t height = 480;

    int rc = 0;
    std::string devname = "bm2835 mmal";
    DeviceFormat format = {};

    rc = V4L2Dev_->start(devname);
    EXPECT_EQ(RC_OK, rc);

    rc = V4L2Dev_->ConfigSys(devname, CMD_V4L2_GET_FORMAT, format);
    EXPECT_EQ(RC_OK, rc);

    format.fmtdesc.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmtdesc.width = width;
    format.fmtdesc.height = height;

    rc = V4L2Dev_->ConfigSys(devname, CMD_V4L2_SET_FORMAT, format);
    EXPECT_EQ(RC_OK, rc);

    rc = V4L2Dev_->ConfigSys(devname, CMD_V4L2_GET_FPS, format);
    EXPECT_EQ(RC_OK, rc);
}

HWTEST_F(UtestV4L2Dev, SetBuffer, TestSize.Level0)
{
    constexpr uint32_t bufferCount = 4;

    unsigned char* addr[bufferCount];
    std::shared_ptr<FrameSpec> buffptr[bufferCount];
    std::string devname = "bm2835 mmal";
    DeviceFormat format = {};
    unsigned int bufSize;
    int i;
    int rc = 0;

    rc = V4L2Dev_->ReqBuffers(devname, bufferCount);
    EXPECT_EQ(RC_OK, rc);

    rc = V4L2Dev_->ConfigSys(devname, CMD_V4L2_GET_FORMAT, format);
    EXPECT_EQ(RC_OK, rc);

    bufSize = format.fmtdesc.sizeimage;
    for (i = 0; i < bufferCount; ++i) {
        buffptr[i] = std::make_shared<FrameSpec>();
        buffptr[i]->buffer_ = std::make_shared<IBuffer>();

        buffptr[i]->buffer_->SetIndex(i);
        buffptr[i]->buffer_->SetSize(bufSize);
        buffptr[i]->buffer_->SetUsage(1);
        buffptr[i]->bufferPoolId_ = 0;
        addr[i] = (unsigned char*)malloc(bufSize);
        if (addr == nullptr) {
            std::cout << " malloc buffers fail \n" << std::endl;
            break;
        }
        buffptr[i]->buffer_->SetVirAddress(addr[i]);

        rc = V4L2Dev_->CreatBuffer(devname, buffptr[i]);
        EXPECT_EQ(RC_OK, rc);
    }

    if (i != bufferCount) {
        for (int j = 0; j < i; ++j) {
            free(addr[j]);
        }
        V4L2Dev_->stop(devname);
    }

    EXPECT_EQ(bufferCount, i);
}


HWTEST_F(UtestV4L2Dev, SetFrameCallback, TestSize.Level0)
{
    int rc = 0;

    rc = V4L2Dev_->SetCallback(V4L2BufferCallback);
    EXPECT_EQ(RC_OK, rc);
}

HWTEST_F(UtestV4L2Dev, StreamStart, TestSize.Level0)
{
    int rc = 0;
    int value;
    std::string devname = "bm2835 mmal";

    constexpr uint32_t awbValue = 8;

    rc = V4L2Dev_->StartStream(devname);
    EXPECT_EQ(RC_OK, rc);

    rc = V4L2Dev_->QuerySetting(devname, CMD_AWB_MODE, &value);
    EXPECT_EQ(RC_OK, rc);

    int setValue = awbValue;
    rc = V4L2Dev_->UpdateSetting(devname, CMD_AWB_MODE, &setValue);
    EXPECT_EQ(RC_OK, rc);

    sleep(3);
}

HWTEST_F(UtestV4L2Dev, ReleaseAll, TestSize.Level0)
{
    std::string devname = "bm2835 mmal";

    V4L2Dev_->StopStream(devname);
    V4L2Dev_->ReleaseBuffers(devname);
    V4L2Dev_->stop(devname);

    V4L2UVC_->V4L2UvcDetectUnInit();
}
} // namespace OHOS::Camera