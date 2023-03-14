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

#ifndef OPP_MESSAGE_H
#define OPP_MESSAGE_H

#include <string>

#include "message.h"
#include "opp_defines.h"

namespace OHOS {
namespace bluetooth {
class OppMessage : public utility::Message {
public:
    explicit OppMessage(int what = 0, int arg1 = 0, void *arg2 = nullptr) : utility::Message(what, arg1, arg2)
    {}
    ~OppMessage() = default;
    std::string dev_;
    OppSdpInformation sdpInfo_;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // OPP_MESSAGE_H
