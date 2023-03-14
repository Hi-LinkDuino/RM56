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

#include "gfx_utils/diagram/vertexprimitive/geometry_shorten_path.h"
namespace OHOS {
/**
 * @brief Long line is shortened to broken line (long line becomes short line, used in dash).
 *
 * @param vtxSeq data source,distance distance,closed is the path closed.
 * @since 1.0
 * @version 1.0
 */
void ShortenPath(GeometryVertexSequence& vtxSeq, float distance, uint32_t closed = 0)
{
    if (vtxSeq.Size() > 1 && distance > 0.0f) {
        float vtxSeqDistance;
        int32_t nSize = int32_t(vtxSeq.Size() - TWO_STEP);
        while (nSize) {
            vtxSeqDistance = vtxSeq[nSize].vertexDistance;
            if (distance < vtxSeqDistance) {
                break;
            }
            vtxSeq.PopBack();
            distance = distance - vtxSeqDistance;
            --nSize;
        }
        if (vtxSeq.Size() > 1) {
            nSize = vtxSeq.Size() - 1;
            VertexDist& prev = vtxSeq[nSize - 1];
            VertexDist& last = vtxSeq[nSize];
            vtxSeqDistance = (prev.vertexDistance - distance) / prev.vertexDistance;
            float x = prev.vertexXCoord + (last.vertexXCoord - prev.vertexXCoord) * vtxSeqDistance;
            float y = prev.vertexYCoord + (last.vertexYCoord - prev.vertexYCoord) * vtxSeqDistance;
            last.vertexXCoord = x;
            last.vertexYCoord = y;
            if (!prev(last)) {       // Calculate whether the two vertices are close
                vtxSeq.PopBack();
            }
            vtxSeq.Close(closed != 0);
        } else {
            vtxSeq.Clear();
        }
    }
}
} // namespace OHOS
