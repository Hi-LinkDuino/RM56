
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

#ifndef MOCK_FOUNDATION_MOCK_UI_SERVICE_CALLBACK_STUB_H
#define MOCK_FOUNDATION_MOCK_UI_SERVICE_CALLBACK_STUB_H

#include <gmock/gmock.h>
#include <map>
#include "ui_service_stub.h"
#include "semaphore_ex.h"

namespace OHOS {
namespace Ace {
class MockUIServiceCallBack : public UIServiceStub {
public:
    MOCK_METHOD5(OnPushCallBack, void(const AAFwk::Want& want, const std::string& name,
        const std::string& jsonPath, const std::string& data, const std::string& extraData));
    MOCK_METHOD3(OnRequestCallBack, void(const AAFwk::Want& want, const std::string& name, const std::string& data));
    MOCK_METHOD4(OnReturnRequest, void(const AAFwk::Want& want, const std::string& source,
        const std::string& data,  const std::string& extraData));
    void Wait()
    {
        sem_.Wait();
    }

    int Post()
    {
        sem_.Post();
        return 0;
    }

    void PostVoid()
    {
        sem_.Post();
    }
private:
    Semaphore sem_;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // MOCK_FOUNDATION_MOCK_UI_SERVICE_CALLBACK_STUB_H