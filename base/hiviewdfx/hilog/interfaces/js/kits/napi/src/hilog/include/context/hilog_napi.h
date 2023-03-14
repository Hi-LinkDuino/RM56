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

#ifndef HIVIEWDFX_NAPI_HILOG
#define HIVIEWDFX_NAPI_HILOG

#include "hilog_napi_base.h"
#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace HiviewDFX {
class HilogNapi : public HilogNapiBase, public NExporter {
public:
    inline static const std::string className = "hilog";
    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    HilogNapi(napi_env env, napi_value exports) : NExporter(env, exports) {}
    explicit HilogNapi() {}
    ~HilogNapi() override {};
};
}  // namespace HiviewDFX
}  // namespace OHOS

#endif // HIVIEWDFX_NAPI_HILOG
