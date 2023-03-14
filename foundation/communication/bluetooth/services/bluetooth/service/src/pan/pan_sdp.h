/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef PAN_SDP_H
#define PAN_SDP_H
#include <cstdint>
#include "pan_defines.h"
namespace OHOS {
namespace bluetooth {
/**
 * @brief pan sdp service
 * pan sdp service
 */
class PanSdp {
public:
    /**
     * @brief constructor
     * @details constructor
     * @return
     */
    PanSdp();
    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     */
    virtual ~PanSdp() = default;
    /**
     * @brief register sdp
     * @details register sdp service
     * @return int @c 0 success
     *             @c not 0 failure
     */
    int Register();
    /**
     * @brief deregister sdp
     * @details deregister sdp service
     */
    void Deregister();

private:
    static const uint16_t NAP_UUID16 = 0X1116;                     /* PAN profile */
    static const uint16_t PAN_VERSION_NUMBER = 0x0100;
    static const uint16_t PAN_LANG_ID_CODE_ENGLISH = 0x656e;     /* "en" */
    static const uint16_t PAN_LANG_ID_CHAR_ENCODE_UTF8 = 0x006a; /* UTF-8 */
    static const uint16_t PAN_LANGUAGE_BASE_ID = 0x0100;
    static const uint16_t PAN_SERVICE_DESCRIPTION_ID = 0X0101;
    static const uint16_t PAN_SECURITY_DESCRIPTION_ID = 0x030A;
    static const uint16_t PAN_NET_ACCESS_TYPE_ID = 0x030B;
    static const uint16_t PAN_MAX_NET_ACCESS_RATE_ID = 0x030C;
    static const int UINT_DESC_TYPE = 1;
    static const int PAN_NET_ACCESS_TYPE_LENGTH = 2;
    static const int PAN_NET_ACCESS_RATE_LENGTH = 4;
    int AddServiceClassIdList();
    int AddPanSecurityDescriptionId();
    int AddPanNetAccessRate();
    int AddPanNetAccessType();
    int AddPanDescriptionList();
    int AddBluetoothProfileDescriptorList();
    int AddServiceName();
    int AddLanguageBaseAttributeIdList();
    int AddBrowseGroupList();
    int AddServiceDescription();
    int RegisterServiceRecord();
    int RegisterNetInfo();
    int RegisterServiceInfo();
    uint32_t sdpHandle_ = 0;  // sdp handler
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PAN_SDP_H