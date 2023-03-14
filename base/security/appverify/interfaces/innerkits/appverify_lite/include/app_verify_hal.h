/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SECURITY_APP_VERIFY_HAL_H
#define SECURITY_APP_VERIFY_HAL_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define INQUIRY_UDID_OK 0
#define INQUIRY_UDID_ERROR -1

typedef int (*GetDeviceUdid)(unsigned char *udid, int size);

typedef struct {
    GetDeviceUdid devUdidFunc;
} ProductDiff;

void RegistHalFunc();
int InquiryDeviceUdid(unsigned char *udid, int size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
