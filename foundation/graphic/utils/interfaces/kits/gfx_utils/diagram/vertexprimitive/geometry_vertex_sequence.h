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
 * @brief Defines GeometryVertexSequence VertexDist VertexDistCmd.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LTE_GEOMETRY_VERTEX_SEQUENCE_H
#define GRAPHIC_LTE_GEOMETRY_VERTEX_SEQUENCE_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/common/common_math.h"
#include "gfx_utils/vector.h"
namespace OHOS {
struct VertexDist {
    float vertexXCoord;
    float vertexYCoord;
    float vertexDistance;

    VertexDist() {}
    /**
     * @brief Construct vertexdist.
     *
     * @param x_,y_ Vertex coordinates.
     * @since 1.0
     * @version 1.0
     */
    VertexDist(float x, float y)
        : vertexXCoord(x), vertexYCoord(y), vertexDistance(0.0f) {}
    /**
     * @brief Calculate whether the distance between the two points is very close.
     *
     * @param vertex.
     * @return If the two points are close, false is returned.
     * @since 1.0
     * @version 1.0
     */
    bool operator()(const VertexDist& val)
    {
        vertexDistance = CalcDistance(vertexXCoord, vertexYCoord, val.vertexXCoord, val.vertexYCoord);
        bool ret = vertexDistance > VERTEX_DIST_EPSILON;
        if (!ret) {
            vertexDistance = 1.0f / VERTEX_DIST_EPSILON;
        }
        return ret;
    }
};

struct VertexDistCmd : public VertexDist {
    uint32_t cmd;

    VertexDistCmd() {}
    /**
     * @brief Construct vertexdist.
     *
     * @param x_,y_ Vertex coordinates, cmd_ Connection command.
     * @since 1.0
     * @version 1.0
     */
    VertexDistCmd(float x_, float y_, uint32_t cmd_) : VertexDist(x_, y_), cmd(cmd_) {}
};

/**
 * @file geometry_vertex_sequence.h
 *
 * @brief Defines Define the GeometryVertexSequence class.
 *
 * @since 1.0
 * @version 1.0
 */
class GeometryVertexSequence : public Graphic::Vector<VertexDist> {
public:
    using BaseType = Graphic::Vector<VertexDist>;

    explicit GeometryVertexSequence(uint32_t size = BLOCK_SHIFT_SIZE) : BaseType(size) {}

    /**
     * @brief Closed vertex source.
     *
     * @param removeFlag Is it closed.
     * @since 1.0
     * @version 1.0
     */
    void Close(bool closed)
    {
        while (BaseType::Size() > 1) {
            if ((*this)[BaseType::Size() - TWO_STEP]((*this)[BaseType::Size() - 1])) {
                break;
            }
            VertexDist t = (*this)[BaseType::Size() - 1];
            BaseType::PopBack();
            ModifyLast(t);
        }

        if (closed) {
            while (BaseType::Size() > 1) {
                if ((*this)[BaseType::Size() - 1]((*this)[0])) { // Calculate the distance between two vertices
                    break;
                }
                BaseType::PopBack();
            }
        }
    }

    /**
     * @brief Add a point.
     *
     * @param val vertex.
     * @since 1.0
     * @version 1.0
     */
    void Add(const VertexDist& val)
    {
        if (BaseType::Size() > 1) {
            if (!(*this)[BaseType::Size() - TWO_STEP]((*this)[BaseType::Size() - 1])) {
                BaseType::PopBack();
            }
        }
        BaseType::PushBack(val);
    }

    /**
     * @brief Modify the last vertex.
     *
     * @param t vertex.
     * @since 1.0
     * @version 1.0
     */
    void ModifyLast(const VertexDist& val)
    {
        BaseType::Clear();
        Add(val);
    }

    /**
     * @brief Gets the next data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    const VertexDist& Next(uint32_t index)
    {
        return (*this)[(index + 1) % BaseType::size_];
    }

    /**
     * @brief Gets the previous data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    const VertexDist& Prev(uint32_t index)
    {
        return (*this)[(index + BaseType::size_ - 1) % BaseType::size_];
    }

    /**
     * @brief Gets the data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    const VertexDist& Curr(uint32_t index)
    {
        return (*this)[index];
    }
};
} // namespace OHOS
#endif
