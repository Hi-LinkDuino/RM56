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

#ifndef OHOS_NAPI_ACE_PLUGIN_WANT_H
#define OHOS_NAPI_ACE_PLUGIN_WANT_H

#include <map>
#include <string>
#include <vector>

#include "js_plugin_data.h"
#include "ohos/aafwk/content/want_params.h"

namespace OHOS::Ace::Napi {
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

napi_value AceWrapElementName(napi_env env, const ElementName& elementName);
bool AceUnwrapElementName(napi_env env, napi_value param, ElementName& elementName);

napi_value AceWrapWantParams(napi_env env, const AAFwk::WantParams& wantParams);
bool AceUnwrapWantParams(napi_env env, napi_value param, AAFwk::WantParams& wantParams);

napi_value AceWrapWant(napi_env env, const Want& want);
bool AceUnwrapWant(napi_env env, napi_value param, Want& want);
}  // namespace OHOS::Ace::Napi
#endif  // OHOS_NAPI_ACE_PLUGIN_WANT_H
