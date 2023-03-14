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

#ifndef OHOS_ACELITE_INTL_MODULE_H
#define OHOS_ACELITE_INTL_MODULE_H
#include "acelite_config.h"
#if (FEATURE_INTL_MODULE == 1)
#include "js_fwk_common.h"
#include "non_copyable.h"
#include "locale_info.h"
namespace OHOS {
namespace ACELite {
class IntlModule final {
public:
    ACE_DISALLOW_COPY(IntlModule);
    static IntlModule *GetInstance()
    {
        static IntlModule intlModule;
        return &intlModule;
    }

    /**
     * @brief: add Intl object to context and register numberformat and dateformat object
     * and add format method support
     */
    void Init();

private:
    IntlModule() {}

    ~IntlModule() {}
};
}
}
#endif // FEATURE_INTL_MODULE
#include "image_module.h"
namespace OHOS {
namespace ACELite {
class IntlControlModule final {
public:
    IntlControlModule() = default;
    ~IntlControlModule() = default;

    static void Load()
    {
#if (FEATURE_INTL_MODULE == 1)
        IntlModule *intlModule = IntlModule::GetInstance();
        intlModule->Init();
#endif
    }
};
}
}

#endif // INTLMODULE_H
