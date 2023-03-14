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

#ifndef HDF_LOAD_HDI_H
#define HDF_LOAD_HDI_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdiBase {
    uint32_t moduleVersion;
    const char *moduleName;
    int (*OpenHdi)();
    int (*CloseHdi)();
};

struct HdiObject {
    uintptr_t dlHandler;
    struct HdiBase *hdiBase;
};

struct HdiObject *LoadHdi(const char *name, uint32_t version);
void CloseHdi(struct HdiObject *hdi);

#define HDF_HDI_INIT(module) struct HdiBase *hdfHdiDesc = (struct HdiBase *)&(module)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_LOAD_HDI_H */

