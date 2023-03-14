/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef HAP_DATASOURCE_H
#define HAP_DATASOURCE_H

#include "common/hap_byte_buffer.h"
#include "util/hap_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
class DataSource {
public:
    virtual ~DataSource() {}
    virtual bool HasRemaining() const = 0;
    virtual long long Remaining() const = 0;
    virtual void Reset() = 0;
    virtual bool ReadDataAndDigestUpdate(const DigestParameter& digestParam, int chunkSize) = 0;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_DATASOURCE_H
