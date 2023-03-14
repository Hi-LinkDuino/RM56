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
#ifndef OHOS_HARDWARE_NFC_V1_0_ICONNECTED_TAG_HDI_H
#define OHOS_HARDWARE_NFC_V1_0_ICONNECTED_TAG_HDI_H
#include <cstdint>
#include <hdf_log.h>
#include <iservmgr_hdi.h>
#include <string>

namespace OHOS {
namespace HDI {
namespace NFC {
namespace V1_0 {
using namespace OHOS;
class IConnectedTagHdi : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.hdi.nfc.v1_0.IConnectedTagHdi");

    virtual ~IConnectedTagHdi() = default;

    static sptr<IConnectedTagHdi> Get();

    virtual int32_t Init() = 0;

    virtual int32_t Uninit() = 0;

    virtual std::string ReadNdefTag() = 0;

    virtual int32_t WriteNdefTag(std::string data) = 0;
};
}  // namespace V1_0
}  // namespace NFC
}  // namespace HDI
}  // namespace OHOS

#endif  // OHOS_HARDWARE_NFC_V1_0_ICONNECTED_TAG_HDI_H
