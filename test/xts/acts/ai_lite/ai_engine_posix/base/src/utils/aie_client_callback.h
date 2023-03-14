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

#ifndef AIE_CLIENT_CALLBACK_H
#define AIE_CLIENT_CALLBACK_H

#include "client_executor/include/i_client_cb.h"
#include "utils/log/aie_log.h"

/**
 * Defines a callback function which is used to define the input parameter of AieClientPrepare().
 */
class ClientCallback : public OHOS::AI::IClientCb {
public:
    ClientCallback() = default;
    ~ClientCallback() override = default;
    void OnResult(const DataInfo &result, int resultCode, int requestId)
    {
        HILOGI("\n[Test]ClientCallbackOnResult is executed."\
            " resultCode[%d], requestId[%d], resultData[%s], resultLength[%d].",
               resultCode, requestId, result.data, result.length);
        EXPECT_EQ(0, resultCode) << "ClientCallback is failed!";
    }
};

#endif // AIE_CLIENT_CALLBACK_H