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

#include "btm_interop.h"

#include <string.h>

typedef struct {
    uint8_t addr[BT_ADDRESS_SIZE];
    uint8_t length;
    uint8_t feature;
} BtmInteropDbEntry;

static const BtmInteropDbEntry G_INTEROP_DB[] = {
    // BMW car kits (Harman/Becker)
    {{0x9c, 0xdf, 0x03, 0, 0, 0}, 3, INTEROP_AUTO_RETRY_PAIRING},  // 9C:DF:03:XX:XX:XX
    // Bose QuiteComfort 35, SoundSport and similar
    {{0x04, 0x52, 0xc7, 0, 0, 0}, 3, INTEROP_2MBPS_LINK_ONLY},  // 04:52:C7:XX:XX:XX
    // JayBird Family
    {{0x00, 0x18, 0x91, 0, 0, 0}, 3, INTEROP_2MBPS_LINK_ONLY},  // 00:18:91:XX:XX:XX
    // Sony MBH-10
    {{0x20, 0x15, 0x06, 0, 0, 0}, 3, INTEROP_2MBPS_LINK_ONLY},  // 20:15:06:XX:XX:XX
    // Uconnect
    {{0x00, 0x54, 0xaf, 0, 0, 0}, 3, INTEROP_2MBPS_LINK_ONLY},  // 00:54:AF:XX:XX:XX
    {{0x30, 0x14, 0x4a, 0, 0, 0}, 3, INTEROP_2MBPS_LINK_ONLY},  // 30:14:4A:XX:XX:XX
};

static bool IsMatchedAddr(const uint8_t addr1[BT_ADDRESS_SIZE], const BtAddr *addr2, uint8_t length)
{
    bool isMatched = true;
    for (uint8_t i = 0; i < length; i++) {
        if (addr1[i] != addr2->addr[BT_ADDRESS_SIZE - 1 - i]) {
            isMatched = false;
            break;
        }
    }
    return isMatched;
}

bool BtmInteropIsMatchedAddr(uint8_t feature, const BtAddr *addr)
{
    if (addr == NULL) {
        return false;
    }

    const size_t dbSize = sizeof(G_INTEROP_DB) / sizeof(BtmInteropDbEntry);
    for (size_t i = 0; i < dbSize; i++) {
        if (feature != G_INTEROP_DB[i].feature) {
            continue;
        }

        if (IsMatchedAddr(G_INTEROP_DB[i].addr, addr, G_INTEROP_DB[i].length)) {
            return true;
        }
    }
    return false;
}