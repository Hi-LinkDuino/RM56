/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_DHCP_BINDING_H
#define OHOS_DHCP_BINDING_H

#include "dhcp_define.h"
#include <stdint.h>
#include <time.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
enum BindingMode { BIND_MODE_STATIC = 0, BIND_MODE_DYNAMIC, BIND_MODE_RESERVED };

enum BindingState { BIND_NONE = 0, BIND_PENDING, BIND_ASSOCIATED, BIND_EXPIRED, BIND_RELEASED };

typedef struct AddressBinding AddressBinding;
struct AddressBinding {
    uint32_t ipAddress;
    uint8_t chaddr[16];
    uint32_t clientId;
    uint64_t bindingTime;
    uint64_t pendingTime;
    uint64_t expireIn;
    uint64_t leaseTime;
    uint64_t pendingInterval;
    int bindingStatus;
    int bindingMode;
};
typedef struct AddressBinding *PAddressBinding;

typedef struct AddressBindingNode AddressBindingNode;
struct AddressBindingNode {
    AddressBinding bind;
    AddressBindingNode *next;
};
typedef struct AddressBindingNode *PAddressBindingNode;

typedef struct AddressBindingList AddressBindingList;
struct AddressBindingList {
    int size;
    AddressBindingNode *first;
};
typedef struct AddressBindingList *PAddressBindingList;

uint64_t NextPendingInterval(uint64_t pendingInterval);
int IsExpire(AddressBinding *binding);
int WriteAddressBinding(const AddressBinding *binding, char *out, uint32_t size);
int ParseAddressBinding(AddressBinding *binding, const char *buf);

#ifdef __cplusplus
}
#endif
#endif
