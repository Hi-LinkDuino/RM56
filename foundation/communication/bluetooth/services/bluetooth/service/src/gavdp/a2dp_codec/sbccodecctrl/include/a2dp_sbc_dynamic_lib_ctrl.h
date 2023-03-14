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

#ifndef A2DP_SBC_DYNAMIC_LIB_CTRL_H
#define A2DP_SBC_DYNAMIC_LIB_CTRL_H
#include "../../sbclib/include/sbc_codec.h"

namespace OHOS {
namespace bluetooth {
struct CODECSbcEncoder {
    sbc::createSbcEncoder createSbcEncode;
    sbc::destroySbcEncoder destroySbcEncode;
};

struct CODECSbcDecoder {
    sbc::createSbcDecoder createSbcDecode;
    sbc::destroySbcDecoder destroySbcDecode;
};

struct CODECSbcLib {
    CODECSbcEncoder sbcEncoder;
    CODECSbcDecoder sbcDecoder;
    void *lib;
};

class A2dpSBCDynamicLibCtrl {
public:
    explicit A2dpSBCDynamicLibCtrl(bool isEncoder);
    ~A2dpSBCDynamicLibCtrl();
    CODECSbcLib *LoadCodecSbcLib() const;
    void UnloadCodecSbcLib(CODECSbcLib *lib) const;

private:
    bool isEncoder_ = false;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_SBC_DYNAMIC_LIB_CTRL_H