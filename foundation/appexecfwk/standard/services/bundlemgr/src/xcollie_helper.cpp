/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "xcollie_helper.h"

#include "app_log_wrapper.h"

#ifdef HICOLLIE_ENABLE
#include "xcollie/xcollie.h"
#include "xcollie/xcollie_define.h"
#endif

namespace OHOS {
namespace AppExecFwk {
int XCollieHelper::SetTimer(const std::string &name, unsigned int timeout, std::function<void(void *)> func, void *arg)
{
#ifdef HICOLLIE_ENABLE
    return HiviewDFX::XCollie::GetInstance().SetTimer(name, timeout, func, arg, HiviewDFX::XCOLLIE_FLAG_LOG);
#else
    APP_LOGI("HICOLLIE_ENABLE is false");
    // HICOLLIE_ENABLE is false, do nothing and return -1.
    return -1;
#endif
}

void XCollieHelper::CancelTimer(int id)
{
#ifdef HICOLLIE_ENABLE
    HiviewDFX::XCollie::GetInstance().CancelTimer(id);
#endif
}
}  // namespace AppExecFwk
}  // namespace OHOS
