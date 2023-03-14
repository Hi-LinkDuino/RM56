/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * @addtogroup GraphicGeometry
 * @{
 *
 * @brief Defines function ShortenPath.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LTE_GEOMETRY_SHORTEN_PATH_H
#define GRAPHIC_LTE_GEOMETRY_SHORTEN_PATH_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_vertex_sequence.h"

namespace OHOS {
void ShortenPath(GeometryVertexSequence& vtxSeq, float distence, uint32_t closed);
} // namespace OHOS
#endif
