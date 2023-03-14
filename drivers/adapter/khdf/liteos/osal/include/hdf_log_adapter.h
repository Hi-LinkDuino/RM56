/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HDF_LOG_ADAPTER_H
#define HDF_LOG_ADAPTER_H

#ifdef LOSCFG_BASE_CORE_HILOG
#include "hilog/log.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_BASE_CORE_HILOG
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002500

#define HDF_LOGV_WRAPPER(fmt, arg...) HILOG_DEBUG(LOG_DOMAIN, fmt, ##arg)

#define HDF_LOGD_WRAPPER(fmt, arg...) HILOG_DEBUG(LOG_DOMAIN, fmt, ##arg)

#define HDF_LOGI_WRAPPER(fmt, arg...) HILOG_INFO(LOG_DOMAIN, fmt, ##arg)

#define HDF_LOGW_WRAPPER(fmt, arg...) HILOG_WARN(LOG_DOMAIN, fmt, ##arg)

#define HDF_LOGE_WRAPPER(fmt, arg...) HILOG_ERROR(LOG_DOMAIN, fmt, ##arg)
#else
static inline void hdf_print_none(const char *format, ...)
{
    (void)format;
}

#define HDF_LOGV_WRAPPER(fmt, arg...) hdf_print_none("[HDF:V/" LOG_TAG "]" fmt "\r\n", ##arg)

#define HDF_LOGD_WRAPPER(fmt, arg...) hdf_print_none("[HDF:D/" LOG_TAG "]" fmt "\r\n", ##arg)

#define HDF_LOGI_WRAPPER(fmt, arg...) hdf_print_none("[HDF:I/" LOG_TAG "]" fmt "\r\n", ##arg)

#define HDF_LOGW_WRAPPER(fmt, arg...) hdf_print_none("[HDF:W/" LOG_TAG "]" fmt "\r\n", ##arg)

#define HDF_LOGE_WRAPPER(fmt, arg...) hdf_print_none("[HDF:E/" LOG_TAG "]" fmt "\r\n", ##arg)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_LOG_ADAPTER_H */

