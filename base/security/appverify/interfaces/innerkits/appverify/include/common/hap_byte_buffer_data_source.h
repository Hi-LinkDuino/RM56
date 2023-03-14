/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef HAP_BYTEBUFFER_DATASOURCE_H
#define HAP_BYTEBUFFER_DATASOURCE_H

#include "common/data_source.h"
#include "common/export_define.h"
#include "common/random_access_file.h"

namespace OHOS {
namespace Security {
namespace Verify {
class HapByteBufferDataSource : public DataSource {
public:
    DLL_EXPORT HapByteBufferDataSource(HapByteBuffer& hapBuffer);
    DLL_EXPORT ~HapByteBufferDataSource();
    bool HasRemaining() const override;
    long long Remaining() const override;
    void Reset() override;
    bool ReadDataAndDigestUpdate(const DigestParameter& digestParam, int chunkSize) override;

private:
    HapByteBuffer& hapByteBuffer;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_BYTEBUFFER_DATASOURCE_H
