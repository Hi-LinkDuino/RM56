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

#include "values_bucket.h"

namespace OHOS {
namespace NativeRdb {
ValuesBucket::ValuesBucket()
{
}

ValuesBucket::ValuesBucket(std::map<std::string, ValueObject> &valuesMap) : valuesMap(valuesMap)
{
}

ValuesBucket::~ValuesBucket()
{
}

void ValuesBucket::PutString(const std::string &columnName, const std::string &value)
{
    valuesMap.insert(std::make_pair(columnName, ValueObject(value)));
}

void ValuesBucket::PutInt(const std::string &columnName, int value)
{
    valuesMap.insert(std::make_pair(columnName, ValueObject(value)));
}

void ValuesBucket::PutLong(const std::string &columnName, int64_t value)
{
    valuesMap.insert(std::make_pair(columnName, ValueObject(value)));
}

void ValuesBucket::PutDouble(const std::string &columnName, double value)
{
    valuesMap.insert(std::make_pair(columnName, ValueObject(value)));
}

void ValuesBucket::PutBool(const std::string &columnName, bool value)
{
    valuesMap.insert(std::make_pair(columnName, ValueObject(value)));
}

void ValuesBucket::PutBlob(const std::string &columnName, const std::vector<uint8_t> &value)
{
    valuesMap.insert(std::make_pair(columnName, ValueObject(value)));
}

void ValuesBucket::PutNull(const std::string &columnName)
{
    valuesMap.insert(std::make_pair(columnName, ValueObject()));
}

void ValuesBucket::Delete(const std::string &columnName)
{
    valuesMap.erase(columnName);
}

void ValuesBucket::Clear()
{
    valuesMap.clear();
}

int ValuesBucket::Size() const
{
    return valuesMap.size();
}

bool ValuesBucket::IsEmpty() const
{
    return valuesMap.empty();
}

bool ValuesBucket::HasColumn(const std::string &columnName) const
{
    auto iter = valuesMap.find(columnName);
    if (iter == valuesMap.end()) {
        return false;
    }
    return true;
}

bool ValuesBucket::GetObject(const std::string &columnName, ValueObject &value) const
{
    auto iter = valuesMap.find(columnName);
    if (iter == valuesMap.end()) {
        return false;
    }
    value = iter->second;
    return true;
}

void ValuesBucket::GetAll(std::map<std::string, ValueObject> &outValuesMap) const
{
    outValuesMap = valuesMap;
}

bool ValuesBucket::Marshalling(Parcel &parcel) const
{
    parcel.WriteInt32(valuesMap.size());
    for (auto &it : valuesMap) {
        parcel.WriteString(it.first);
        parcel.WriteParcelable(&it.second);
    }
    return true;
}

ValuesBucket *ValuesBucket::Unmarshalling(Parcel &parcel)
{
    int mapSize = parcel.ReadInt32();
    std::map<std::string, ValueObject> valuesMap;
    ValueObject *value = nullptr;
    for (int i = 0; i < mapSize; i++) {
        std::string key = parcel.ReadString();
        value = parcel.ReadParcelable<ValueObject>();
        valuesMap.insert(std::make_pair(key, *value));
    }
    value = nullptr;
    return new ValuesBucket(valuesMap);
}
} // namespace NativeRdb
} // namespace OHOS
