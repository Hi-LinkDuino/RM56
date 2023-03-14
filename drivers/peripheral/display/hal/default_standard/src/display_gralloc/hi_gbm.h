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

#ifndef HI_GBM_H
#define HI_GBM_H
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct gbm_device;
struct gbm_bo;

enum gbm_bo_flags {
    /* *
     * Buffer is going to be presented to the screen using an API such as KMS
     */
    GBM_BO_USE_SCANOUT = (1 << 0),
    /* *
     * Buffer is going to be used as cursor
     */
    GBM_BO_USE_CURSOR = (1 << 1),
    /* *
     * Deprecated
     */
    GBM_BO_USE_CURSOR_64X64 = GBM_BO_USE_CURSOR,
    /* *
     * Buffer is to be used for rendering - for example it is going to be used
     * as the storage for a color buffer
     */
    GBM_BO_USE_RENDERING = (1 << 2),
    /* *
     * Deprecated
     */
    GBM_BO_USE_WRITE = (1 << 3),
    /* *
     * Buffer is guaranteed to be laid out linearly in memory. That is, the
     * buffer is laid out as an array with 'height' blocks, each block with
     * length 'stride'. Each stride is in the same order as the rows of the
     * buffer. This is intended to be used with buffers that will be accessed
     * via dma-buf mmap().
     */
    GBM_BO_USE_LINEAR = (1 << 4),
    /* *
     * The buffer will be used as a texture that will be sampled from.
     */
    GBM_BO_USE_TEXTURING = (1 << 5),
    /* *
     * The buffer will be written to by a camera subsystem.
     */
    GBM_BO_USE_CAMERA_WRITE = (1 << 6),
    /* *
     * The buffer will be read from by a camera subsystem.
     */
    GBM_BO_USE_CAMERA_READ = (1 << 7),
    /* *
     * Buffer inaccessible to unprivileged users.
     */
    GBM_BO_USE_PROTECTED = (1 << 8),
    /* *
     * These flags specify the frequency of software access. These flags do not
     * guarantee the buffer is linear, but do guarantee gbm_bo_map(..) will
     * present a linear view.
     */
    GBM_BO_USE_SW_READ_OFTEN = (1 << 9),
    GBM_BO_USE_SW_READ_RARELY = (1 << 10),
    GBM_BO_USE_SW_WRITE_OFTEN = (1 << 11),
    GBM_BO_USE_SW_WRITE_RARELY = (1 << 12),
    /* *
     * The buffer will be written by a video decode accelerator.
     */
    GBM_BO_USE_HW_VIDEO_DECODER = (1 << 13),
};

struct gbm_device *hdi_gbm_create_device(int fd);
void hdi_gbm_device_destroy(struct gbm_device *gbm);
struct gbm_bo *hdi_gbm_bo_create(struct gbm_device *gbm, uint32_t width, uint32_t height, uint32_t format, uint32_t usage);
uint32_t hdi_gbm_bo_get_stride(struct gbm_bo *bo);
uint32_t hdi_gbm_bo_get_width(struct gbm_bo *bo);
uint32_t hdi_gbm_bo_get_height(struct gbm_bo *bo);
void hdi_gbm_bo_destroy(struct gbm_bo *bo);
int hdi_gbm_bo_get_fd(struct gbm_bo *bo);

#if defined(__cplusplus)
}
#endif
#endif // HI_GBM_H