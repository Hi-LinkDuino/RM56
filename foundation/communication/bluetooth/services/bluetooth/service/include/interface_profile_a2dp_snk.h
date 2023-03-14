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

/**
 * @file interface_profile_a2dp_snk.h
 *
 * @brief This class provides functions called by Framework API for a2dp sink.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_A2DP_SNK_H
#define INTERFACE_PROFILE_A2DP_SNK_H

#include "interface_profile_a2dp_src.h"

namespace OHOS {
namespace bluetooth {
class IProfileA2dpSnk : public IProfileA2dp {};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_A2DP_SNK_H