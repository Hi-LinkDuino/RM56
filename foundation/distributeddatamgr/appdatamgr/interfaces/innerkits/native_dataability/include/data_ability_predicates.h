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

#ifndef NATIVE_RDB_DATD_ABILITY_PREDICATES_H
#define NATIVE_RDB_DATD_ABILITY_PREDICATES_H

#include <string>

#include "abs_predicates.h"
#include "parcel.h"

namespace OHOS {
namespace NativeRdb {
class DataAbilityPredicates : public AbsPredicates, public virtual OHOS::Parcelable {
public:
    DataAbilityPredicates();
    explicit DataAbilityPredicates(std::string rawSelection);
    explicit DataAbilityPredicates(OHOS::Parcel *source);
    ~DataAbilityPredicates() override;
    bool IsRawSelection() const;
    bool GetJudgeSource() const;
    bool Marshalling(OHOS::Parcel &parcel) const override;
    static DataAbilityPredicates *Unmarshalling(OHOS::Parcel &parcel);
    static bool result;

private:
    void MarshallingString(std::string value, OHOS::Parcel &parcel) const;
    void MarshallingStringList(std::vector<std::string> list, OHOS::Parcel &parcel) const;

    bool isRawSelection;
    bool judgeSource;
};
} // namespace NativeRdb
} // namespace OHOS

#endif