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

#ifndef DATAABILITY_I_SHARED_RESULT_SET_H
#define DATAABILITY_I_SHARED_RESULT_SET_H
#include <memory>
#include "iremote_broker.h"
#include "abs_shared_result_set.h"
namespace OHOS::NativeRdb {
class ISharedResultSet : public AbsSharedResultSet, public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NativeRdb.ISharedResultSet")
    static std::shared_ptr<AbsSharedResultSet> ReadFromParcel(MessageParcel &parcel);
    static sptr<ISharedResultSet> WriteToParcel(std::shared_ptr<AbsSharedResultSet> resultSet, MessageParcel &parcel);

protected:
    enum {
        FUNC_GET_ROW_COUNT,
        FUNC_GET_ALL_COLUMN_NAMES,
        FUNC_ON_GO,
        FUNC_CLOSE,
        FUNC_GET_BLOB,
        FUNC_GET_STRING,
        FUNC_GET_INT,
        FUNC_GET_LONG,
        FUNC_GET_DOUBLE,
        FUNC_IS_COLUMN_NULL,
        FUNC_GO_TO,
        FUNC_GO_TO_ROW,
        FUNC_GO_TO_FISTR_ROW,
        FUNC_GO_TO_LAST_ROW,
        FUNC_GO_TO_NEXT_ROW,
        FUNC_GO_TO_PREV_ROW,
        FUNC_IS_AT_FIRST_ROW,
        FUNC_IS_AT_LAST_ROW,
        FUNC_IS_STARTED_ROW,
        FUNC_IS_ENDED_ROW,
        FUNC_IS_CLOSED,
        FUNC_GET_COLUMN_COUNT,
        FUNC_GET_COLUMN_INDEX,
        FUNC_GET_COLUMN_NAME,
        FUNC_GET_COLUMN_TYPE,
        FUNC_GET_ROW_INDEX,
        FUNC_BUTT
    };
    
private:
    static std::function<std::shared_ptr<AbsSharedResultSet>(MessageParcel &parcel)> consumerCreator_;
    static std::function<sptr<ISharedResultSet>(std::shared_ptr<AbsSharedResultSet>, MessageParcel &)> providerCreator_;
};
} // namespace OHOS::NativeRdb

#endif // DATAABILITY_I_SHARED_RESULT_SET_H
