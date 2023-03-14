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

#ifndef HI_GBM_INTERNEL_H
#define HI_GBM_INTERNEL_H

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define ALIGN_UP(x, a) ((((x) + ((a)-1)) / (a)) * (a))
#define HEIGHT_ALIGN 2U
#define WIDTH_ALIGN 8U

#define MAX_PLANES 3

struct gbm_device {
    int fd;
};

struct gbm_bo {
    struct gbm_device *gbm;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t handle;
    uint32_t stride;
    uint32_t size;
};

#endif // HI_GBM_INTERNEL_H