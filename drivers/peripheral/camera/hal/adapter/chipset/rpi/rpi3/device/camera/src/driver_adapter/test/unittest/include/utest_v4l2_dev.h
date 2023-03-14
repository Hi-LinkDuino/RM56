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

#ifndef HOS_CAMERA_UTEST_V4L2_H
#define HOS_CAMERA_UTEST_V4L2_H

#include <stdio.h>
#include <gtest/gtest.h>
#include "v4l2_dev.h"
#include "v4l2_uvc.h"

namespace OHOS::Camera {
class UtestV4L2Dev : public testing::Test {
public:

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp(void);
    void TearDown(void);

    static std::shared_ptr<HosV4L2UVC> V4L2UVC_;
    static std::shared_ptr<HosV4L2Dev> V4L2Dev_;

    static std::vector<std::string> cameraIDs_;
};
std::shared_ptr<HosV4L2UVC> UtestV4L2Dev::V4L2UVC_ = nullptr;
std::shared_ptr<HosV4L2Dev> UtestV4L2Dev::V4L2Dev_ = nullptr;
std::vector<std::string> UtestV4L2Dev::cameraIDs_ = {};
} // namespace OHOS::Camera
#endif
