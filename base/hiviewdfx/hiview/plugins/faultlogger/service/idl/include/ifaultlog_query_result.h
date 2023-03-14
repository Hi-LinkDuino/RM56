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
#ifndef OHOS_HIVIEWDFX_IFAULTLOG_QUERY_RESULT_H
#define OHOS_HIVIEWDFX_IFAULTLOG_QUERY_RESULT_H

#include <string>

#include "iremote_broker.h"
#include "iremote_object.h"

#include "faultlog_info_ohos.h"

namespace OHOS {
namespace HiviewDFX {
class IFaultLogQueryResult : public IRemoteBroker {
public:
    enum {
        IFaultLogQueryResult_HASNEXT = 0,
        IFaultLogQueryResult_GETNEXT,
    };

    virtual bool HasNext()  = 0;
    virtual sptr<FaultLogInfoOhos> GetNext() = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.hiviewdfx.IFaultLogQueryResult");
};
} // space HiviewDFX
} // namespace OHOS

#endif // OHOS_HIVIEWDFX_IFAULTLOG_QUERY_RESULT_H