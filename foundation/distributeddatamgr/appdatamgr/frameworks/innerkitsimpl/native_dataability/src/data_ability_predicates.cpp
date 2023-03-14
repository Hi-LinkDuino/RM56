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

#include "data_ability_predicates.h"
#include "predicates_utils.h"

namespace OHOS {
namespace NativeRdb {
int g_invalidObjectFlag = 0;
int g_validObjectFlag = 1;
int g_defaultSelectArgNumber = 8;
bool DataAbilityPredicates::result = false;
DataAbilityPredicates::DataAbilityPredicates()
{
    this->isRawSelection = false;
}

DataAbilityPredicates::DataAbilityPredicates(std::string rawSelection)
{
    AbsPredicates::SetWhereClause(rawSelection);
    this->isRawSelection = true;
}

DataAbilityPredicates::DataAbilityPredicates(OHOS::Parcel *source)
{
    if (source == nullptr) {
        this->judgeSource = false;
    } else {
        this->isRawSelection = source->ReadBool();
        std::string whereClause = (source->ReadInt32() != g_invalidObjectFlag) ? source->ReadString() : "";
        std::vector<std::string> whereArgs;
        if (source->ReadInt32() != g_invalidObjectFlag) {
            source->ReadStringVector(&whereArgs);
        }
        bool isDistinct = source->ReadBool();
        std::string index = (source->ReadInt32() != g_invalidObjectFlag) ? source->ReadString() : "";
        std::string group = (source->ReadInt32() != g_invalidObjectFlag) ? source->ReadString() : "";
        std::string order = (source->ReadInt32() != g_invalidObjectFlag) ? source->ReadString() : "";
        int limit = (source->ReadInt32() != g_invalidObjectFlag) ? source->ReadInt32() : -1;
        int offset = (source->ReadInt32() != g_invalidObjectFlag) ? source->ReadInt32() : -1;
        PredicatesUtils::SetWhereClauseAndArgs(this, whereClause, whereArgs);
        PredicatesUtils::SetAttributes(this, isDistinct, index, group, order, limit, offset);
    }
}
/**
 * Obtain value of variable isRawSelection.
 */
bool DataAbilityPredicates::IsRawSelection() const
{
    return isRawSelection;
}

bool DataAbilityPredicates::GetJudgeSource() const
{
    return judgeSource;
}

/**
 * Write DataAbilityPredicates object to Parcel.
 */
bool DataAbilityPredicates::Marshalling(OHOS::Parcel &parcel) const
{
    parcel.WriteBool(this->isRawSelection);
    MarshallingString(GetWhereClause(), parcel);
    MarshallingStringList(GetWhereArgs(), parcel);
    parcel.WriteBool(IsDistinct());
    MarshallingString(GetIndex(), parcel);
    MarshallingString(GetGroup(), parcel);
    MarshallingString(GetOrder(), parcel);

    int limit = GetLimit();
    int offset = GetOffset();
    if (limit != -1) {
        parcel.WriteInt32(g_validObjectFlag);
        parcel.WriteInt32(limit);
    } else {
        parcel.WriteInt32(g_invalidObjectFlag);
    }
    if (offset != -1) {
        parcel.WriteInt32(g_validObjectFlag);
        parcel.WriteInt32(offset);
    } else {
        parcel.WriteInt32(g_invalidObjectFlag);
    }

    return true;
}
/**
 * Read from Parcel object.
 */
DataAbilityPredicates* DataAbilityPredicates::Unmarshalling(OHOS::Parcel &parcel)
{
    result = true;
    return new DataAbilityPredicates(&parcel);
}

void DataAbilityPredicates::MarshallingString(std::string value, OHOS::Parcel &parcel) const
{
    if (value.length() != 0) {
        parcel.WriteInt32(g_validObjectFlag);
        parcel.WriteString(value);
    } else {
        parcel.WriteInt32(g_invalidObjectFlag);
    }
}

void DataAbilityPredicates::MarshallingStringList(std::vector<std::string> list, OHOS::Parcel &parcel) const
{
    if (list.size() != 0) {
        parcel.WriteInt32(g_validObjectFlag);
        parcel.WriteStringVector(list);
    } else {
        parcel.WriteInt32(g_invalidObjectFlag);
    }
}

DataAbilityPredicates::~DataAbilityPredicates() {}
} // namespace NativeRdb
} // namespace OHOS