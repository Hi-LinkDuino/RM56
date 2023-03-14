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

#ifndef PAN_MESSAGE_H
#define PAN_MESSAGE_H

#include <string>

#include "message.h"
#include "securec.h"
#include "pan_defines.h"

namespace OHOS {
namespace bluetooth {
class PanMessage : public utility::Message {
public:
    explicit PanMessage(int what = 0, int arg1 = 0, void *arg2 = nullptr) : utility::Message(what, arg1, arg2)
    {}
    PanMessage(const PanMessage &src) : utility::Message(src.what_, src.arg1_, src.arg2_),
        dev_(src.dev_),
        ethernetHeader_(src.ethernetHeader_),
        l2capInfo_(src.l2capInfo_),
        data_(src.data_),
        dataLength_(src.dataLength_)
    {
    }
    ~PanMessage() = default;
    std::string dev_;
    EthernetHeader ethernetHeader_ {};
    PanL2capConnectionInfo l2capInfo_ {};
    std::shared_ptr<std::unique_ptr<uint8_t[]>> data_ = nullptr;
    int dataLength_ = 0;

    PanMessage operator=(const PanMessage &src)
    {
        if (this != &src) {
            dev_ = src.dev_;
            ethernetHeader_ = src.ethernetHeader_;
            l2capInfo_ = src.l2capInfo_;
            data_ = src.data_;
            dataLength_ = src.dataLength_;
        }
        return *this;
    }
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PAN_MESSAGE_H
