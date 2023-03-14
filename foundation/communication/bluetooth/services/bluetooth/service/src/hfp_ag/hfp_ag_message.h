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

#ifndef HFP_AG_MESSAGE_H
#define HFP_AG_MESSAGE_H

#include <string>

#include "hfp_ag_defines.h"
#include "message.h"

namespace OHOS {
namespace bluetooth {
typedef struct {
    bool service {false};
    bool roam {false};
    bool signal {false};
    bool battery {false};
} HfpAgTransferData;

class HfpAgMessage : public utility::Message {
public:
    explicit HfpAgMessage(int what, int arg1 = 0, void *arg2 = nullptr) : utility::Message(what, arg1, arg2)
    {}
    ~HfpAgMessage() = default;
    int arg3_ {0};
    int type_ {0};
    std::string str_ {""};
    std::string dev_ {""};
    HfpAgTransferData data_ {};
    HfpAgPhoneState state_ {};
    HfpAgCallList call_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HFP_AG_MESSAGE_H
