/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/common/properties/shadow_config.h"

namespace OHOS::Ace {

const Shadow ShadowConfig::DefaultShadowXS = Shadow(4.0f, Offset(0.0, 0.0), Color(0x2600001E));
const Shadow ShadowConfig::DefaultShadowS = Shadow(20.0f, Offset(0.0, 0.0), Color(0x1400001E));
const Shadow ShadowConfig::DefaultShadowM = Shadow(50.0f, Offset(0.0, 10.0), Color(0x2600001E));
const Shadow ShadowConfig::DefaultShadowL = Shadow(50.0f, Offset(0.0, 10.0), Color(0x4C00001E));
const Shadow ShadowConfig::FloatingShadowS = Shadow(50.0f, Offset(0.0, 10.0), Color(0x2600001E));
const Shadow ShadowConfig::FloatingShadowM = Shadow(50.0f, Offset(0.0, 10.0), Color(0x4C00001E));

} // namespace OHOS::Ace
