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

#include "common/hap_byte_buffer_data_source.h"

namespace OHOS {
namespace Security {
namespace Verify {
HapByteBufferDataSource::HapByteBufferDataSource(HapByteBuffer& hapBuffer)
    : DataSource(), hapByteBuffer(hapBuffer)
{
}

HapByteBufferDataSource::~HapByteBufferDataSource()
{
}

bool HapByteBufferDataSource::HasRemaining() const
{
    return hapByteBuffer.HasRemaining();
}

long long HapByteBufferDataSource::Remaining() const
{
    return static_cast<long long>(hapByteBuffer.Remaining());
}

void HapByteBufferDataSource::Reset()
{
    hapByteBuffer.Clear();
}

bool HapByteBufferDataSource::ReadDataAndDigestUpdate(const DigestParameter& digestParam, int chunkSize)
{
    const unsigned char* chunk = reinterpret_cast<const unsigned char*>(hapByteBuffer.GetBufferPtr() +
        hapByteBuffer.GetPosition());
    bool res = HapVerifyOpensslUtils::DigestUpdate(digestParam, chunk, chunkSize);
    if (res) {
        hapByteBuffer.SetPosition(hapByteBuffer.GetPosition() + chunkSize);
    }
    return res;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
