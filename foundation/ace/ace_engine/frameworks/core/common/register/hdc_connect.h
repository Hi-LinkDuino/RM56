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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_REGISTER_HDC_CONNECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_REGISTER_HDC_CONNECT_H

#include <string>
#include "frameworks/core/common/register/define.h"
namespace OHOS::Ace {
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
void StartConnect(const std::string& pkgName);

void StopConnect();
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

class ConnectManagement {
public:
    ConnectManagement() = default;
    ~ConnectManagement() {}
    void SetPkgName(const std::string& pkgName);
    std::string GetPkgName();

private:
    std::string pkgName_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_REGISTER_HDC_CONNECT_H
