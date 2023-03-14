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
#ifndef HDI_SERVICE_STATUS_LISTENER_INF_H
#define HDI_SERVICE_STATUS_LISTENER_INF_H

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef HDF_HDI_SERVICE_STATUS_H
#define HDF_HDI_SERVICE_STATUS_H

#include "hdf_types.h"
#include "hdf_service_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ServiceStatusListener *HdiServiceStatusListenerNewInstance(void);
void HdiServiceStatusListenerFree(struct ServiceStatusListener *listener);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_HDI_SERVICE_STATUS_H */

#endif /* HDI_SERVICE_STATUS_LISTENER_INF_H */