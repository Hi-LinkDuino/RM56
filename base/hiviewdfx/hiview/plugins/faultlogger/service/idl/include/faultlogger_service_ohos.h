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
#ifndef HIVIEWDFX_FAULTLOGGER_SERVICE_OHOS_H
#define HIVIEWDFX_FAULTLOGGER_SERVICE_OHOS_H
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "singleton.h"
#include "system_ability.h"

#include "faultlogger_service_stub.h"
#include "faultlog_info_ohos.h"
#include "faultlog_query_result_ohos.h"
#include "faultlogger.h"

namespace OHOS {
namespace HiviewDFX {
class FaultloggerServiceOhos : public SystemAbility,
                            public FaultLoggerServiceStub,
                            public Singleton<FaultloggerServiceOhos> {
    DECLARE_SYSTEM_ABILITY(FaultloggerServiceOhos);
public:
    FaultloggerServiceOhos() {};
    virtual ~FaultloggerServiceOhos() {};

    static void StartService(OHOS::HiviewDFX::Faultlogger *service);
    static OHOS::HiviewDFX::Faultlogger *GetOrSetFaultlogger(
        OHOS::HiviewDFX::Faultlogger *service = nullptr);
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

    void AddFaultLog(const FaultLogInfoOhos& info) override;
    sptr<IRemoteObject> QuerySelfFaultLog(int32_t faultType, int32_t maxNum) override;
    void Destroy() override;

private:
    struct FaultLogQuery {
        int32_t pid = -1;
        sptr<FaultLogQueryResultOhos> ptr = nullptr;
    };

private:
    void ClearQueryStub(int32_t uid);

    std::map<int32_t, std::unique_ptr<FaultLogQuery>> queries_;
    std::mutex mutex_;
};
};      // namespace HiviewDFX
};      // namespace OHOS
#endif  // HIVIEWDFX_FAULTLOGGER_SERVICE_OHOS_H
