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

#ifndef HID_HOST_MESSAGE_H
#define HID_HOST_MESSAGE_H

#include <string>

#include "hid_host_defines.h"
#include "message.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
class HidHostMessage : public utility::Message {
public:
    explicit HidHostMessage(int what = 0, int arg1 = 0, void *arg2 = nullptr) : utility::Message(what, arg1, arg2)
    {}
    HidHostMessage(const HidHostMessage &src) : utility::Message(src.what_, src.arg1_, src.arg2_),
        dev_(src.dev_),
        sendData_(src.sendData_),
        l2capInfo_(src.l2capInfo_),
        data_(nullptr),
        dataLength_(src.dataLength_)
    {
        if ((dataLength_ > 0) && (src.data_ != nullptr)) {
            data_ = std::make_unique<uint8_t[]>(dataLength_);
            if (memcpy_s(data_.get(), dataLength_, src.data_.get(), dataLength_) != EOK) {
                data_.reset(nullptr);
                dataLength_ = 0;
            }
        } else {
            data_.reset(nullptr);
            dataLength_ = 0;
        }
    }
    ~HidHostMessage() = default;
    std::string dev_ {""};
    SendHidData sendData_ {};
    HidHostL2capConnectionInfo l2capInfo_ {};
    std::unique_ptr<uint8_t[]> data_ = nullptr;
    int dataLength_ = 0;

    HidHostMessage operator=(const HidHostMessage &src)
    {
        if (this != &src) {
            dev_ = src.dev_;
            sendData_ = src.sendData_;
            l2capInfo_ = src.l2capInfo_;
            data_ = nullptr;
            dataLength_ = src.dataLength_;

            if ((dataLength_ > 0) && (src.data_ != nullptr)) {
                data_ = std::make_unique<uint8_t[]>(dataLength_);
                if (memcpy_s(data_.get(), dataLength_, src.data_.get(), dataLength_) != EOK) {
                    data_.reset(nullptr);
                    dataLength_ = 0;
                }
            } else {
                data_.reset(nullptr);
                dataLength_ = 0;
            }
        }
        return *this;
    }
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HID_HOST_MESSAGE_H
