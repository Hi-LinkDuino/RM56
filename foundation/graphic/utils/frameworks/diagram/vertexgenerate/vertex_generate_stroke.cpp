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
#include "gfx_utils/diagram/vertexgenerate/vertex_generate_stroke.h"

#include "gfx_utils/diagram/vertexprimitive/geometry_shorten_path.h"

namespace OHOS {
VertexGenerateStroke::VertexGenerateStroke()
    : stroker_(),
      srcVertices_(),
      outVertices_(),
      strokeShorten_(0.0f),
      closed_(0),
      status_(INITIAL),
      srcVertex_(0),
      outVertex_(0)
{
}

void VertexGenerateStroke::RemoveAll()
{
    srcVertices_.Clear();
    closed_ = 0;
    status_ = INITIAL;
}

void VertexGenerateStroke::AddVertex(float x, float y, uint32_t cmd)
{
    status_ = INITIAL;
    if (IsMoveTo(cmd)) {
        srcVertices_.ModifyLast(VertexDist(x, y));
    } else {
        if (IsVertex(cmd)) {
            srcVertices_.Add(VertexDist(x, y));
        } else {
            closed_ = GetCloseFlag(cmd);
        }
    }
}

void VertexGenerateStroke::Rewind(uint32_t)
{
    if (status_ == INITIAL) {
        srcVertices_.Close(closed_ != 0);
        ShortenPath(srcVertices_, strokeShorten_, closed_);
        const uint32_t verticeNum = 3;
        if (srcVertices_.Size() < verticeNum) {
            closed_ = 0;
        }
    }
    status_ = READY;
    srcVertex_ = 0;
    outVertex_ = 0;
}

uint32_t VertexGenerateStroke::GenerateVertex(float* x, float* y)
{
    const uint32_t verticesNum = 2;
    uint32_t cmd = PATH_CMD_LINE_TO;
    while (!IsStop(cmd)) {
        switch (status_) {
            case INITIAL:
                Rewind(0); // It will be ready immediately after initialization
            case READY:
                VertexReady(verticesNum, cmd);
                break;
            case LINECAP_START:
                VertexLineCapStart();
                break;
            case LINECAP_END:
                VertexLineCapEnd(verticesNum);
                break;
            case LINEJOIN_START:
                VertexLineJoinStart();
                break;
            case CLOSE_FIRST:
                VertexCloseFirst(cmd);
                break;
            case LINEJOIN_END:
                VertexLineJoinEnd();
                break;
            case OUT_VERTICES:
                if (IsVertexOutVertices(x, y)) {
                    return cmd;
                }
                break;
            case CLOCKWISE_ENDPOLY:
                status_ = prevStatus_;
                return PATH_CMD_END_POLY | PATH_FLAGS_CLOSE | PATH_FLAGS_CLOCKWISE;
            case ANTI_CLOCKWISE_ENDPOLY:
                status_ = prevStatus_;
                return PATH_CMD_END_POLY | PATH_FLAGS_CLOSE | PATH_FLAGS_ANTI_CLOCKWISE;
            case STOP:
                cmd = PATH_CMD_STOP;
                break;
            default: break;
        }
    }
    return cmd;
}

void VertexGenerateStroke::VertexReady(const uint32_t& verticesNum, uint32_t& cmd)
{
    if (srcVertices_.Size() < verticesNum + uint32_t(closed_ != 0)) {
        cmd = PATH_CMD_STOP;
        return;
    }
    if (closed_) {
        status_ = LINEJOIN_START;
    } else {
        status_ = LINECAP_START;
    }
    cmd = PATH_CMD_MOVE_TO;
    srcVertex_ = 0;
    outVertex_ = 0;
}

void VertexGenerateStroke::VertexLineCapStart()
{
#if GRAPHIC_ENABLE_LINECAP_FLAG
    stroker_.CalcCap(outVertices_,
                     srcVertices_[0],
                     srcVertices_[1],
                     srcVertices_[0].vertexDistance);
#endif
    srcVertex_ = 1;
    prevStatus_ = LINEJOIN_START;
    status_ = OUT_VERTICES;
    outVertex_ = 0;
}

void VertexGenerateStroke::VertexLineCapEnd(const uint32_t& verticesNum)
{
#if GRAPHIC_ENABLE_LINECAP_FLAG
    stroker_.CalcCap(outVertices_,
                     srcVertices_[srcVertices_.Size() - 1],
                     srcVertices_[srcVertices_.Size() - verticesNum],
                     srcVertices_[srcVertices_.Size() - verticesNum].vertexDistance);
#endif
    prevStatus_ = LINEJOIN_END;
    status_ = OUT_VERTICES;
    outVertex_ = 0;
}

void VertexGenerateStroke::VertexLineJoinStart()
{
    if (closed_ && srcVertex_ >= srcVertices_.Size()) {
        prevStatus_ = CLOSE_FIRST;
        status_ = CLOCKWISE_ENDPOLY;
        return;
    }
    if (!closed_ && srcVertex_ >= srcVertices_.Size() - 1) {
        status_ = LINECAP_END;
        return;
    }
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    stroker_.CalcJoin(outVertices_,
                      srcVertices_.Prev(srcVertex_),
                      srcVertices_.Curr(srcVertex_),
                      srcVertices_.Next(srcVertex_),
                      srcVertices_.Prev(srcVertex_).vertexDistance,
                      srcVertices_.Curr(srcVertex_).vertexDistance);
#endif
    ++srcVertex_;
    prevStatus_ = status_;
    status_ = OUT_VERTICES;
    outVertex_ = 0;
}

void VertexGenerateStroke::VertexLineJoinEnd()
{
    if (srcVertex_ <= uint32_t(closed_ == 0)) {
        status_ = ANTI_CLOCKWISE_ENDPOLY;
        prevStatus_ = STOP;
        return;
    }
    --srcVertex_;
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    stroker_.CalcJoin(outVertices_,
                      srcVertices_.Next(srcVertex_),
                      srcVertices_.Curr(srcVertex_),
                      srcVertices_.Prev(srcVertex_),
                      srcVertices_.Curr(srcVertex_).vertexDistance,
                      srcVertices_.Prev(srcVertex_).vertexDistance);
#endif
    outVertex_ = 0;
    prevStatus_ = status_;
    status_ = OUT_VERTICES;
}

void VertexGenerateStroke::VertexCloseFirst(uint32_t& cmd)
{
    status_ = LINEJOIN_END;
    cmd = PATH_CMD_MOVE_TO;
}

bool VertexGenerateStroke::IsVertexOutVertices(float* x, float* y)
{
    if (outVertex_ >= outVertices_.Size()) {
        status_ = prevStatus_;
        return false;
    } else {
        const PointF& c = outVertices_[outVertex_++];
        *x = c.x;
        *y = c.y;
        return true;
    }
}
} // namespace OHOS
