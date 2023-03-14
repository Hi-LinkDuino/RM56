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

#ifndef TRANSPORT_FACTORY_H
#define TRANSPORT_FACTORY_H

#include <stdint.h>
#include "dispatcher.h"
#include "raw_address.h"
#include "transport_def.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This TransportFactory class provides a set of methods that create rfcomm transport and l2cap transport.
 */
class TransportFactory {
public:
    /**
     * @brief Create a Rfcomm transport object.
     *
     * @param addr remote device address.
     * @param scn server channel number.
     * @param mtu mtu
     * @param observer the callback to the upper layer.
     * @return std::unique_ptr<DataTransport>
     */
    static std::unique_ptr<DataTransport> CreateRfcommTransport(const RawAddress *addr, uint8_t scn, uint16_t mtu,
        DataTransportObserver &observer, utility::Dispatcher &dispatcher);

    /**
     * @brief Create a L2CAP transport object.
     *
     * @param createInfo  create info struct.
     * @return std::unique_ptr<DataTransport>
     */
    static std::unique_ptr<DataTransport> CreateL2capTransport(L2capTransportInfo &createInfo);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // TRANSPORT_FACTORY_H