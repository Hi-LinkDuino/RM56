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

#ifndef USBD_FUNCTION_H
#define USBD_FUNCTION_H
#include "usbd.h"

#define USB_FUNCTION_NONE 0
#define USB_FUNCTION_ACM 1
#define USB_FUNCTION_ECM 2
#define USB_FUNCTION_HDC 4

int32_t UsbdSetFunction(int32_t funcs);
int32_t UsbdGetFunction();

#endif
