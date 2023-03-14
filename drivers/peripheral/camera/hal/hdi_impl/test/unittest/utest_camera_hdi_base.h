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

#ifndef UTEST_CAMERA_HDI_BASE_H
#define UTEST_CAMERA_HDI_BASE_H

#include <thread>
#include <unistd.h>
#include <vector>
#include <map>
#include <gtest/gtest.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include "securec.h"

#include "camera.h"
#include "camera_metadata_info.h"
#include "ibuffer.h"
#include "ioffline_stream_operator.h"
#include <surface.h>
#include <display_type.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "icamera_host.h"
#include "camera_host_callback.h"
#include "camera_device_callback.h"
#include "icamera_device.h"
#include "stream_operator_callback.h"
#include "istream_operator_callback.h"

#ifdef CAMERA_BUILT_ON_OHOS_LITE
#include "camera_device.h"
#include "camera_host.h"
#include "stream_operator.h"
#else
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "camera_host_proxy.h"
#endif

using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::Camera;

class CameraHdiBaseTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);

protected:
    virtual bool InitCameraHost();
    virtual bool GetCameraDevice();
    virtual bool GetStreamOperator();
    virtual bool GetCameraIds();
    int32_t SaveToFile(const std::string path, const void* buffer, int32_t size);
    uint64_t GetCurrentLocalTimeStamp();

protected:
#ifdef CAMERA_BUILT_ON_OHOS_LITE
    std::shared_ptr<CameraHost> cameraHost_ = nullptr;
    std::shared_ptr<ICameraDevice> cameraDevice_ = nullptr;
    std::shared_ptr<IStreamOperator> streamOperator_ = nullptr;
#else
    sptr<ICameraHost> cameraHost_ = nullptr;
    sptr<ICameraDevice> cameraDevice_ = nullptr;
    sptr<IStreamOperator> streamOperator_ = nullptr;
#endif

    std::vector<std::string> cameraIds_;
};
#endif // UTEST_CAMERA_HDI_BASE_H
