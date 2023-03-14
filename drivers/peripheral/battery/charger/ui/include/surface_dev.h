/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CHARGER_UI_SURFACE_DEV_H
#define CHARGER_UI_SURFACE_DEV_H

#include <cstdint>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class SurfaceDev {
public:
    enum class DevType {
        FB_DEVICE = 0,
        DRM_DEVICE,
    };

    explicit SurfaceDev(SurfaceDev::DevType devType);
    ~SurfaceDev();
    void Flip(char* buf);
    void GetScreenSize(int32_t& w, int32_t& h);
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_SURFACE_DEV_H
