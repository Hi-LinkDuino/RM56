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

#ifndef HOS_CAMERA_PROJET_HARDWARE_H
#define HOS_CAMERA_PROJET_HARDWARE_H

namespace OHOS::Camera {
std::vector<HardwareConfiguration> hardware = {
    {CAMERA_FIRST, DM_M_SENSOR, DM_C_SENSOR, (std::string) "bm2835 mmal"},
    {CAMERA_FIRST, DM_M_ISP, DM_C_ISP, (std::string) "isp"},
    {CAMERA_FIRST, DM_M_FLASH, DM_C_FLASH, (std::string) "flash"},
    {CAMERA_SECOND, DM_M_SENSOR, DM_C_SENSOR, (std::string) "Imx600"},
    {CAMERA_SECOND, DM_M_ISP, DM_C_ISP, (std::string) "isp"},
    {CAMERA_SECOND, DM_M_FLASH, DM_C_FLASH, (std::string) "flash"}
};
} // namespace OHOS::Camera
#endif
