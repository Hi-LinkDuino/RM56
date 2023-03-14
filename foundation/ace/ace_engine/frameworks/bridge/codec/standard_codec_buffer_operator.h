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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_STANDARD_CODEC_BUFFER_OPERATOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_STANDARD_CODEC_BUFFER_OPERATOR_H

#include <vector>

#include "base/utils/noncopyable.h"
#include "frameworks/bridge/codec/byte_buffer_operator.h"
#include "frameworks/bridge/codec/codec_data.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT StandardCodecBufferReader final {
public:
    explicit StandardCodecBufferReader(const std::vector<uint8_t>& buffer) : byteBufferReader_(buffer) {}
    ~StandardCodecBufferReader() = default;

    bool ReadData(CodecData& resultData);
    bool ReadDataList(std::vector<CodecData>& resultDataList);
    bool ReadMapSize(int32_t& size);

private:
    bool ReadType(BufferDataType& type);

    ByteBufferReader byteBufferReader_;

    ACE_DISALLOW_COPY_AND_MOVE(StandardCodecBufferReader);
};

class StandardCodecBufferWriter final {
public:
    explicit StandardCodecBufferWriter(std::vector<uint8_t>& buffer) : byteBufferWriter_(buffer) {}
    ~StandardCodecBufferWriter() = default;

    void WriteData(const CodecData& data);
    void WriteDataList(const std::vector<CodecData>& dataList);

private:
    void WriteType(BufferDataType type);

    ByteBufferWriter byteBufferWriter_;

    ACE_DISALLOW_COPY_AND_MOVE(StandardCodecBufferWriter);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_STANDARD_CODEC_BUFFER_OPERATOR_H
