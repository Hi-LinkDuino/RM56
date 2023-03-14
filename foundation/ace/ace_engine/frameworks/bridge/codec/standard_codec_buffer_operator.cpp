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

#include "frameworks/bridge/codec/standard_codec_buffer_operator.h"

#include "base/log/log.h"

namespace OHOS::Ace::Framework {
namespace {

template<class T>
inline bool ReadDataFromByteBuffer(const ByteBufferReader& buffer, CodecData& resultData)
{
    T data;
    if (buffer.ReadData(data)) {
        resultData = CodecData(std::move(data));
        return true;
    }
    return false;
}

}

bool StandardCodecBufferReader::ReadType(BufferDataType& type)
{
    uint8_t uint8Type = 0;
    if (byteBufferReader_.ReadData(uint8Type)) {
        type = static_cast<BufferDataType>(uint8Type);
        return true;
    }
    return false;
}

bool StandardCodecBufferReader::ReadMapSize(int32_t& data)
{
    BufferDataType type = BufferDataType::TYPE_NULL;
    if (!ReadType(type)) {
        LOGW("Read type failed");
        return false;
    }
    if (type == BufferDataType::TYPE_MAP) {
        byteBufferReader_.ReadData(data);
        return true;
    }
    return false;
}

bool StandardCodecBufferReader::ReadDataList(std::vector<CodecData>& resultDataList)
{
    resultDataList.clear();
    uint8_t size = 0;
    if (!byteBufferReader_.ReadData(size)) {
        LOGW("Failed to read size of data list");
        return false;
    }

    for (uint8_t idx = 0; idx < size; ++idx) {
        CodecData data;
        if (!ReadData(data)) {
            LOGW("Failed to read data");
            resultDataList.clear();
            return false;
        }
        resultDataList.emplace_back(std::move(data));
    }
    return true;
}

bool StandardCodecBufferReader::ReadData(CodecData& resultData)
{
    BufferDataType type = BufferDataType::TYPE_NULL;
    if (!ReadType(type)) {
        LOGW("Read type failed");
        return false;
    }

    switch (type) {
        case BufferDataType::TYPE_NULL:
            resultData = CodecData();
            return true;
        case BufferDataType::TYPE_TRUE:
            resultData = CodecData(true);
            return true;
        case BufferDataType::TYPE_FALSE:
            resultData = CodecData(false);
            return true;
        case BufferDataType::TYPE_INT:
            return ReadDataFromByteBuffer<int32_t>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_LONG:
            return ReadDataFromByteBuffer<int64_t>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_DOUBLE:
            return ReadDataFromByteBuffer<double>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_STRING:
            return ReadDataFromByteBuffer<std::string>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_INT8_ARRAY:
            return ReadDataFromByteBuffer<std::vector<int8_t>>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_INT16_ARRAY:
            return ReadDataFromByteBuffer<std::vector<int16_t>>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_INT32_ARRAY:
            return ReadDataFromByteBuffer<std::vector<int32_t>>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_MAP:
            return ReadDataFromByteBuffer<std::map<std::string, std::string>>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_SET:
            return ReadDataFromByteBuffer<std::set<std::string>>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_FUNCTION:
            return ReadDataFromByteBuffer<int32_t>(byteBufferReader_, resultData);
        case BufferDataType::TYPE_OBJECT:
            return ReadDataFromByteBuffer<std::string>(byteBufferReader_, resultData);
        default:
            LOGW("Unknown type");
            return false;
    }
}

void StandardCodecBufferWriter::WriteType(BufferDataType type)
{
    byteBufferWriter_.WriteData(static_cast<uint8_t>(type));
}

void StandardCodecBufferWriter::WriteDataList(const std::vector<CodecData>& dataList)
{
    byteBufferWriter_.WriteData(static_cast<uint8_t>(dataList.size()));
    for (const auto& data : dataList) {
        WriteData(data);
    }
}

void StandardCodecBufferWriter::WriteData(const CodecData& data)
{
    WriteType(data.GetType());

    switch (data.GetType()) {
        case BufferDataType::TYPE_INT:
            byteBufferWriter_.WriteData(data.GetIntValue());
            break;
        case BufferDataType::TYPE_LONG:
            byteBufferWriter_.WriteData(data.GetLongValue());
            break;
        case BufferDataType::TYPE_DOUBLE:
            byteBufferWriter_.WriteData(data.GetDoubleValue());
            break;
        case BufferDataType::TYPE_STRING:
            byteBufferWriter_.WriteData(data.GetStringValue());
            break;
        case BufferDataType::TYPE_INT8_ARRAY:
            byteBufferWriter_.WriteData(data.GetInt8ArrayValue());
            break;
        case BufferDataType::TYPE_INT16_ARRAY:
            byteBufferWriter_.WriteData(data.GetInt16ArrayValue());
            break;
        case BufferDataType::TYPE_INT32_ARRAY:
            byteBufferWriter_.WriteData(data.GetInt32ArrayValue());
            break;
        case BufferDataType::TYPE_MAP:
            byteBufferWriter_.WriteData(data.GetMapValue());
            break;
        case BufferDataType::TYPE_SET:
            byteBufferWriter_.WriteData(data.GetSetValue());
            break;
        case BufferDataType::TYPE_FUNCTION:
            byteBufferWriter_.WriteData(data.GetFunctionValue());
            break;
        case BufferDataType::TYPE_OBJECT:
            byteBufferWriter_.WriteData(data.GetObjectValue());
            break;
        default:
            break;
    }
}

} // namespace OHOS::Ace::Framework