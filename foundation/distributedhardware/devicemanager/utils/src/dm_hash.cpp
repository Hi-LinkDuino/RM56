/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dm_log.h"
#include "dm_constants.h"
#include "dm_hash.h"

namespace OHOS {
namespace DistributedHardware {
int32_t GetUdidHash(uint8_t *udid, int32_t udidDataLen, uint8_t outudidData[32])
{
    LOGI("GetUdidHash");
    if (udid == nullptr || *udid < 0) {
        LOGE("udid is nullptr or dataLen: %d", udidDataLen);
        return DM_POINT_NULL;
    }
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t  *info;

    mbedtls_md_init(&ctx);
    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (info == nullptr) {
        LOGE("info is nullptr");
        return DM_POINT_NULL;
    }
    int32_t ret = DM_OK;
    do {
        ret = mbedtls_md_setup(&ctx, info, 0);
        if (ret != 0) {
            LOGE("mbedtls_md_setup is fail");
            ret = DM_FAILED;
            break;
        }

        ret = mbedtls_md_starts(&ctx);
        if (ret != 0) {
            LOGE("mbedtls_md_starts is fail");
            ret = DM_FAILED;
            break;
        }

        ret = mbedtls_md_update(&ctx, udid, udidDataLen);
        if (ret != 0) {
            LOGE("mbedtls_md_update is fail");
            ret = DM_FAILED;
            break;
        }

        ret = mbedtls_md_finish(&ctx, outudidData);
        if (ret != 0) {
            LOGE("mbedtls_md_finish is fail");
            ret = DM_FAILED;
            break;
        }
        ret = DM_OK;
    } while (0);

    mbedtls_md_free(&ctx);
    return ret;
}
} // namespace DistributedHardware
} // namespace OHOS