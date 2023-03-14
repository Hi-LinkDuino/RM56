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

#include "intl_module.h"
#if (FEATURE_INTL_MODULE == 1)
#include "ace_log.h"
#include "date_time_format_module.h"
#include "global.h"
#include "string.h"
#include "number_format_module.h"
#include "js_fwk_common.h"
namespace OHOS {
namespace ACELite {
void IntlModule::Init()
{
    jerry_value_t globalObj = jerry_get_global_object();
    jerry_value_t baseObj = jerry_create_object();
#if (FEATURE_NUMBER_FORMAT == 1)
    NumberFormatModule::Init(baseObj);
#endif
#if (FEATURE_DATE_FORMAT == 1)
    DateTimeFormatModule::Init(baseObj);
#endif
    JerrySetNamedProperty(globalObj, "Intl", baseObj);
    jerry_release_value(globalObj);
}
} // namespace ACELite
} // namespace OHOS
#endif
