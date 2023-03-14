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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_CODEC_DATA_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_CODEC_DATA_H

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>

namespace OHOS::Ace::Framework {

enum class BufferDataType : uint8_t {
    TYPE_NULL = 0x10,
    TYPE_TRUE,
    TYPE_FALSE,
    TYPE_INT,
    TYPE_LONG,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_MAP,
    TYPE_SET,
    TYPE_INT8_ARRAY,
    TYPE_INT16_ARRAY,
    TYPE_INT32_ARRAY,
    TYPE_FUNCTION,
    TYPE_OBJECT,
};

class CodecData final {
public:
    CodecData() = default;
    ~CodecData() = default;
    CodecData(const CodecData& other) = default;
    CodecData(CodecData&& other) = default;
    CodecData& operator=(const CodecData& other) = default;
    CodecData& operator=(CodecData&& other) = default;

    explicit CodecData(bool val) : type_(val ? BufferDataType::TYPE_TRUE : BufferDataType::TYPE_FALSE) {}
    explicit CodecData(int32_t val) : type_(BufferDataType::TYPE_INT), data_(val) {}
    explicit CodecData(int32_t val, BufferDataType type) : type_(type), data_(val) {}
    explicit CodecData(int64_t val) : type_(BufferDataType::TYPE_LONG), data_(val) {}
    explicit CodecData(double val) : type_(BufferDataType::TYPE_DOUBLE), data_(val) {}

    explicit CodecData(const std::string& val) : type_(BufferDataType::TYPE_STRING), data_(val) {}
    explicit CodecData(const std::string& val, BufferDataType type) : type_(type), data_(val) {}
    explicit CodecData(const std::map<std::string, std::string>& val) : type_(BufferDataType::TYPE_MAP), data_(val) {}
    explicit CodecData(const std::set<std::string>& val) : type_(BufferDataType::TYPE_SET), data_(val) {}
    explicit CodecData(const std::vector<int8_t>& val) : type_(BufferDataType::TYPE_INT8_ARRAY), data_(val) {}
    explicit CodecData(const std::vector<int16_t>& val) : type_(BufferDataType::TYPE_INT16_ARRAY), data_(val) {}
    explicit CodecData(const std::vector<int32_t>& val) : type_(BufferDataType::TYPE_INT32_ARRAY), data_(val) {}

    explicit CodecData(std::string&& val) : type_(BufferDataType::TYPE_STRING), data_(std::move(val)) {}
    explicit CodecData(std::map<std::string, std::string>&& val)
        : type_(BufferDataType::TYPE_MAP), data_(std::move(val)) {}
    explicit CodecData(std::set<std::string>&& val) : type_(BufferDataType::TYPE_SET), data_(std::move(val)) {}
    explicit CodecData(std::vector<int8_t>&& val) : type_(BufferDataType::TYPE_INT8_ARRAY), data_(std::move(val)) {}
    explicit CodecData(std::vector<int16_t>&& val) : type_(BufferDataType::TYPE_INT16_ARRAY), data_(std::move(val)) {}
    explicit CodecData(std::vector<int32_t>&& val) : type_(BufferDataType::TYPE_INT32_ARRAY), data_(std::move(val)) {}

    BufferDataType GetType() const
    {
        return type_;
    }
    bool IsNull() const
    {
        return type_ == BufferDataType::TYPE_NULL;
    }
    bool IsBool() const
    {
        return type_ == BufferDataType::TYPE_TRUE || type_ == BufferDataType::TYPE_FALSE;
    }
    bool IsInt() const
    {
        return type_ == BufferDataType::TYPE_INT;
    }
    bool IsLong() const
    {
        return type_ == BufferDataType::TYPE_LONG;
    }
    bool IsDouble() const
    {
        return type_ == BufferDataType::TYPE_DOUBLE;
    }
    bool IsString() const
    {
        return type_ == BufferDataType::TYPE_STRING;
    }
    bool IsMap() const
    {
        return type_ == BufferDataType::TYPE_MAP;
    }
    bool IsSet() const
    {
        return type_ == BufferDataType::TYPE_SET;
    }
    bool IsInt8Array() const
    {
        return type_ == BufferDataType::TYPE_INT8_ARRAY;
    }
    bool IsInt16Array() const
    {
        return type_ == BufferDataType::TYPE_INT16_ARRAY;
    }
    bool IsInt32Array() const
    {
        return type_ == BufferDataType::TYPE_INT32_ARRAY;
    }

    bool IsFunction() const
    {
        return type_ == BufferDataType::TYPE_FUNCTION;
    }

    bool IsObject() const
    {
        return type_ == BufferDataType::TYPE_OBJECT;
    }

    bool GetBoolValue(bool defValue = false) const
    {
        return (type_ == BufferDataType::TYPE_TRUE) ? true : (type_ == BufferDataType::TYPE_FALSE) ? false : defValue;
    }
    int32_t GetIntValue(int32_t defValue = 0) const
    {
        return IsInt() ? std::get<int32_t>(data_) : defValue;
    }
    int64_t GetLongValue(int64_t defValue = 0) const
    {
        return IsLong() ? std::get<int64_t>(data_) : IsInt() ? std::get<int32_t>(data_) : defValue;
    }
    double GetDoubleValue(double defValue = 0.0) const
    {
        return IsDouble() ? std::get<double>(data_)
                          : IsLong() ? std::get<int64_t>(data_) : IsInt() ? std::get<int32_t>(data_) : defValue;
    }

    const std::string& GetStringValue() const
    {
        return GetValue<std::string>();
    }
    const std::map<std::string, std::string>& GetMapValue() const
    {
        return GetValue<std::map<std::string, std::string>>();
    }
    const std::set<std::string>& GetSetValue() const
    {
        return GetValue<std::set<std::string>>();
    }
    const std::vector<int8_t>& GetInt8ArrayValue() const
    {
        return GetValue<std::vector<int8_t>>();
    }
    const std::vector<int16_t>& GetInt16ArrayValue() const
    {
        return GetValue<std::vector<int16_t>>();
    }
    const std::vector<int32_t>& GetInt32ArrayValue() const
    {
        return GetValue<std::vector<int32_t>>();
    }

    int32_t GetFunctionValue(int32_t defValue = 0) const
    {
        return IsFunction() ? std::get<int32_t>(data_) : defValue;
    }

    const std::string& GetObjectValue() const
    {
        return GetValue<std::string>();
    }

private:
    template<class T>
    struct CopyableUniquePtr final {
        CopyableUniquePtr() = default;
        ~CopyableUniquePtr() = default;

        CopyableUniquePtr(const CopyableUniquePtr& other) : ptr(std::make_unique<T>(*other.ptr)) {}
        CopyableUniquePtr(CopyableUniquePtr&& other) = default;

        CopyableUniquePtr& operator=(const CopyableUniquePtr& other)
        {
            ptr = std::make_unique<T>(*other.ptr);
            return *this;
        }
        CopyableUniquePtr& operator=(CopyableUniquePtr&& other) = default;

        CopyableUniquePtr(const T& data) : ptr(std::make_unique<T>(data)) {}
        CopyableUniquePtr(T&& data) : ptr(std::make_unique<T>(std::move(data))) {}

        std::unique_ptr<T> ptr;
    };

    using EncodedData = std::variant<int32_t, int64_t, double, CopyableUniquePtr<std::string>,
        CopyableUniquePtr<std::map<std::string, std::string>>, CopyableUniquePtr<std::set<std::string>>,
        CopyableUniquePtr<std::vector<int8_t>>, CopyableUniquePtr<std::vector<int16_t>>,
        CopyableUniquePtr<std::vector<int32_t>>>;

    template<class T>
    const T& GetValue() const
    {
        auto val = std::get_if<CopyableUniquePtr<T>>(&data_);
        if (val != nullptr) {
            return *(val->ptr);
        } else {
            static T defData;
            return defData;
        }
    }

    BufferDataType type_ = BufferDataType::TYPE_NULL;
    EncodedData data_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_CODEC_CODEC_DATA_H
