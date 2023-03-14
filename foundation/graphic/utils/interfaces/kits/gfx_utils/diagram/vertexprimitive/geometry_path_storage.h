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

#ifndef GRAPHIC_LTE_GEOMERTY_PATH_STORAGE_H
#define GRAPHIC_LTE_GEOMERTY_PATH_STORAGE_H

#include "geometry_bezier_arc.h"
#include "gfx_utils/diagram/common/common_math.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_plaindata_array.h"

namespace OHOS {
/**
 * @brief Vertex source data block
 * @since 1.0
 * @version 1.0
 */
class VertexBlockStorage {
public:
    enum BlockScale {
        BLOCK_SHIFT = THIRTY_TWO_COLOR_NUM,
        BLOCK_SIZE = 1 << BLOCK_SHIFT,
        BLOCK_MASK = BLOCK_SIZE - 1,
        BLOCK_POOL = MAX_COLOR_SIZE
    };

    ~VertexBlockStorage()
    {
        FreeAll();
    }

    VertexBlockStorage() : totalVertices_(0), totalBlocks_(0), maxBlocks_(0), croodBlocks_(0), cmdBlocks_(0) {}

    const VertexBlockStorage& operator=(const VertexBlockStorage& vertexBlockStorage)
    {
        RemoveAll();
        for (uint32_t iIndex = 0; iIndex < vertexBlockStorage.GetTotalVertices(); iIndex++) {
            float x;
            float y;
            uint32_t cmd = vertexBlockStorage.GenerateVertex(iIndex, &x, &y);
            AddVertex(x, y, cmd);
        }
        this->totalBlocks_ = vertexBlockStorage.totalBlocks_;
        maxBlocks_ = vertexBlockStorage.maxBlocks_;
        croodBlocks_ = vertexBlockStorage.croodBlocks_;
        cmdBlocks_ = vertexBlockStorage.cmdBlocks_;
        return *this;
    }

    VertexBlockStorage(const VertexBlockStorage& vertexBlockStorage)
    {
        *this = vertexBlockStorage;
    }

    /**
     * @brief Remove all vertices.
     *
     * @since 1.0
     * @version 1.0
     */
    void RemoveAll()
    {
        totalVertices_ = 0;
    }
    /**
     * @brief Free up space
     * @since 1.0
     * @version 1.0
     */
    void FreeAll()
    {
        if (totalBlocks_ > 0) {
            float** coordBLK = croodBlocks_ + totalBlocks_ - 1;
            for (; totalBlocks_ > 0; totalBlocks_--) {
                GeometryArrayAllocator<float>::Deallocate(
                    *coordBLK,
                    BLOCK_SIZE * TWO_TIMES +
                        BLOCK_SIZE / (sizeof(float) / sizeof(uint8_t)));
                --coordBLK;
            }
            GeometryArrayAllocator<float*>::Deallocate(croodBlocks_, maxBlocks_ * TWO_TIMES);
            totalBlocks_ = 0;
            maxBlocks_ = 0;
            croodBlocks_ = 0;
            cmdBlocks_ = 0;
            totalVertices_ = 0;
        }
    }
    /**
     * @brief add vertex.
     *
     * @param x Vertex X-axis coordinates.
     * @param y Vertex Y-axis coordinates.
     * @param cmd Instruction type.
     * @since 1.0
     * @version 1.0
     */
    void AddVertex(float x, float y, uint32_t cmd)
    {
        float* coordPtr = 0;
        *StoragePtrs(&coordPtr) = static_cast<uint8_t>(cmd);
        coordPtr[0] = float(x);
        coordPtr[1] = float(y);
        totalVertices_++;
    }
    /**
     * @brief Returns the last instruction.
     * @return Returns the last instruction type.
     * @since 1.0
     * @version 1.0
     */
    uint32_t LastCommand() const
    {
        if (totalVertices_) {
            return Command(totalVertices_ - 1);
        }
        return PATH_CMD_STOP;
    }
    /**
     * @brief Returns the coordinates of the last vertex.
     * @param x Used to obtain the x-axis coordinates of the last vertex.
     * @param y Used to obtain the y-axis coordinates of the last vertex.
     * @return Returns the instruction type corresponding to the vertex.
     * @since 1.0
     * @version 1.0
     */
    uint32_t LastVertex(float* x, float* y) const
    {
        if (totalVertices_) {
            return GenerateVertex(totalVertices_ - 1, x, y);
        }
        return PATH_CMD_STOP;
    }
    /**
     * @brief Returns the number of vertices.
     * @return Returns the number of vertices.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetTotalVertices() const
    {
        return totalVertices_;
    }
    /**
     * @brief Gets the coordinates of a particular vertex.
     * @param idx Vertex subscript.
     * @param x Used to obtain the x-axis coordinates of vertices.
     * @param y Used to obtain the y-axis coordinates of vertices.
     * @return Returns the instruction type corresponding to the vertex.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GenerateVertex(uint32_t idx, float* x, float* y) const
    {
        uint32_t nb = idx >> BLOCK_SHIFT;
        const float* pv = croodBlocks_[nb] + ((idx & BLOCK_MASK) << 1);
        *x = pv[0];
        *y = pv[1];
        return cmdBlocks_[nb][idx & BLOCK_MASK];
    }
    /**
     * @brief ets the instruction type corresponding to a specific vertex.
     * @param index Vertex subscript.
     * @return Returns the instruction type corresponding to the vertex.
     * @since 1.0
     * @version 1.0
     */
    uint32_t Command(uint32_t index) const
    {
        return cmdBlocks_[index >> BLOCK_SHIFT][index & BLOCK_MASK];
    }

private:
    void AllocateBlock(uint32_t nb)
    {
        if (nb >= maxBlocks_) {
            float** new_coords = GeometryArrayAllocator<float*>::Allocate(
                (maxBlocks_ + BLOCK_POOL) * TWO_TIMES);

            uint8_t** new_cmds =
                (uint8_t**)(new_coords + maxBlocks_ + BLOCK_POOL);

            if (croodBlocks_) {
                if (memcpy_s(new_coords, maxBlocks_ * sizeof(float*),
                             croodBlocks_, maxBlocks_ * sizeof(float*)) != EOK) {
                }
                if (memcpy_s(new_cmds, maxBlocks_ * sizeof(float*),
                             cmdBlocks_, maxBlocks_ * sizeof(uint8_t*)) != EOK) {
                }
                GeometryArrayAllocator<float*>::Deallocate(croodBlocks_, maxBlocks_ * TWO_TIMES);
            }
            croodBlocks_ = new_coords;
            cmdBlocks_ = new_cmds;
            maxBlocks_ += BLOCK_POOL;
        }
        croodBlocks_[nb] =
            GeometryArrayAllocator<float>::Allocate(BLOCK_SIZE * TWO_TIMES +
                                                     BLOCK_SIZE / (sizeof(float) / sizeof(uint8_t)));

        cmdBlocks_[nb] =
            (uint8_t*)(croodBlocks_[nb] + BLOCK_SIZE * TWO_TIMES);

        totalBlocks_++;
    }
    uint8_t* StoragePtrs(float** xy_ptr)
    {
        uint32_t nb = totalVertices_ >> BLOCK_SHIFT;
        if (nb >= totalBlocks_) {
            AllocateBlock(nb);
        }
        *xy_ptr = croodBlocks_[nb] + ((totalVertices_ & BLOCK_MASK) << 1);
        return cmdBlocks_[nb] + (totalVertices_ & BLOCK_MASK);
    }

private:
    uint32_t totalVertices_;
    uint32_t totalBlocks_;
    uint32_t maxBlocks_;
    float** croodBlocks_; // Input points
    uint8_t** cmdBlocks_; // Mark point status
};

class UICanvasVertices : public HeapBase {
public:
    UICanvasVertices() : vertices_(), iterator_(0) {}

    /**
     * @brief Remove all vertices
     *
     * @since 1.0
     * @version 1.0
     */
    void RemoveAll()
    {
        vertices_.RemoveAll();
        iterator_ = 0;
    }
    /**
     * @brief Free up space
     * @since 1.0
     * @version 1.0
     */
    void FreeAll()
    {
        vertices_.FreeAll();
        iterator_ = 0;
    }

    /**
     * @brief Move a point to the set coordinates
     * @param x Vertex X-axis coordinates
     * @param y Vertex Y-axis coordinates
     * @since 1.0
     * @version 1.0
     */
    void MoveTo(float x, float y)
    {
        vertices_.AddVertex(x, y, PATH_CMD_MOVE_TO);
    }
    /**
     * @brief Connect the line to the entered coordinates
     * @param x Vertex X-axis coordinates
     * @param y Vertex Y-axis coordinates
     * @since 1.0
     * @version 1.0
     */
    void LineTo(float x, float y)
    {
        vertices_.AddVertex(x, y, PATH_CMD_LINE_TO);
    }

    void CubicBezierCurve(double xCtrl1, double yCtrl1,
                          double xCtrl2, double yCtrl2,
                          double xEnd,    double yEnd)
    {
        vertices_.AddVertex(xCtrl1, yCtrl1, PATH_CMD_CURVE4);
        vertices_.AddVertex(xCtrl2, yCtrl2, PATH_CMD_CURVE4);
        vertices_.AddVertex(xEnd, yEnd, PATH_CMD_CURVE4);
    }

    /**
     * @brief Draw an arc
     * @param rx Long and short axle
     * @param ry Long and short axle
     * @param angle angle
     * @param largeArcFlag Superior inferior arc
     * @param sweepFlag Clockwise and counterclockwise
     * @param x End X coordinate
     * @param y End Y-axis coordinates
     * @since 1.0
     * @version 1.0
     */
    void ArcTo(float rx, float ry,
               float angle,
               bool largeArcFlag,
               bool sweepFlag,
               float x, float y)
    {
        if (vertices_.GetTotalVertices() && IsVertex(vertices_.LastCommand())) {
            const float epsilon = 1e-30;
            float x0 = 0.0f;
            float y0 = 0.0f;
            vertices_.LastVertex(&x0, &y0);

            rx = MATH_ABS(rx);
            ry = MATH_ABS(ry);
            if (rx < epsilon || ry < epsilon) {
                LineTo(x, y);
                return;
            }
            if (CalcDistance(x0, y0, x, y) < epsilon) {
                return;
            }
#if GRAPHIC_ENABLE_BEZIER_ARC_FLAG
            BezierArcSvg bezierArcSvg(x0, y0, rx, ry, angle, largeArcFlag, sweepFlag, x, y);
            if (bezierArcSvg.RadiiOK()) {
                JoinPath(bezierArcSvg);
            } else {
                LineTo(x, y);
            }
#else
            LineTo(x, y);
#endif
        } else {
            MoveTo(x, y);
        }
    }

    void EndPoly(uint32_t flags = PATH_FLAGS_CLOSE)
    {
        if (IsVertex(vertices_.LastCommand())) {
            vertices_.AddVertex(0.0f, 0.0f, PATH_CMD_END_POLY | flags);
        }
    }
    /**
     * @brief ClosePolygon Closed polygon path
     * @param flags
     */
    void ClosePolygon(uint32_t flags = PATH_FLAGS_NONE)
    {
        EndPoly(PATH_FLAGS_CLOSE | flags);
    }

    /**
     * @brief Returns the number of vertices.
     * @return Returns the number of vertices.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetTotalVertices() const
    {
        return vertices_.GetTotalVertices();
    }

    /**
     * @brief Returns the coordinates of the last vertex.
     * @param x Used to obtain the x-axis coordinates of the last vertex.
     * @param y Used to obtain the y-axis coordinates of the last vertex.
     * @return Returns the instruction type corresponding to the vertex.
     * @since 1.0
     * @version 1.0
     */
    uint32_t LastVertex(float* x, float* y) const
    {
        return vertices_.LastVertex(x, y);
    }
    /**
     * @brief Gets the coordinates of a particular vertex.
     * @param idx Vertex subscript.
     * @param x Used to obtain the x-axis coordinates of vertices.
     * @param y Used to obtain the y-axis coordinates of vertices.
     * @return Returns the instruction type corresponding to the vertex.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GenerateVertex(uint32_t idx, float* x, float* y) const;

    /**
     * @brief Iterator fallback to a vertex。
     * @param pathId The vertex sequence number of the fallback.
     * @since 1.0
     * @version 1.0
     */
    void Rewind(uint32_t pathId)
    {
        iterator_ = pathId;
    }

    /**
     * @brief Gets the coordinates of the next vertex.
     * @param x Used to obtain the x-axis coordinates of vertices.
     * @param y Used to obtain the y-axis coordinates of vertices.
     * @return Returns the instruction type corresponding to the next vertex.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GenerateVertex(float* x, float* y)
    {
        if (iterator_ >= vertices_.GetTotalVertices()) {
            return PATH_CMD_STOP;
        }
        return vertices_.GenerateVertex(iterator_++, x, y);
    }
#if GRAPHIC_ENABLE_BEZIER_ARC_FLAG
    /**
     * @brief Connection path.
     * @param vs Vertex source to connect.
     * @param pathId Connection location.
     * @since 1.0
     * @version 1.0
     */
    void ConcatPath(BezierArc& path, uint32_t pathId = 0)
    {
        float x;
        float y;
        uint32_t cmd;
        path.Rewind(pathId);
        while (!IsStop(cmd = path.GenerateVertex(&x, &y))) {
            vertices_.AddVertex(x, y, cmd);
        }
    }

    /**
     * @brief Add vertex source to existing source.
     * @param vs Vertex source to join.
     * @param pathId Join location.
     * @since 1.0
     * @version 1.0
     */
    void JoinPath(BezierArcSvg& path, uint32_t pathId = 0)
    {
        float x;
        float y;
        uint32_t pathCommand;
        path.Rewind(pathId);
        pathCommand = path.GenerateVertex(&x, &y);
        if (!IsStop(pathCommand)) {
            if (IsVertex(pathCommand)) {
                float x0;
                float y0;
                uint32_t cmd0 = LastVertex(&x0, &y0);
                if (IsVertex(cmd0) && (CalcDistance(x, y, x0, y0) > VERTEX_DIST_EPSILON)) {
                    if (IsMoveTo(pathCommand)) {
                        pathCommand = PATH_CMD_LINE_TO;
                    }
                    vertices_.AddVertex(x, y, pathCommand);
                } else if (IsStop(cmd0)) {
                    pathCommand = PATH_CMD_MOVE_TO;
                    vertices_.AddVertex(x, y, pathCommand);
                } else {
                    if (IsMoveTo(pathCommand)) {
                        pathCommand = PATH_CMD_LINE_TO;
                    }
                    vertices_.AddVertex(x, y, pathCommand);
                }
            }
            for (; !IsStop(pathCommand = path.GenerateVertex(&x, &y));) {
                uint32_t pathCmd;
                if (IsMoveTo(pathCommand)) {
                    pathCmd = uint32_t(PATH_CMD_LINE_TO);
                } else {
                    pathCmd = pathCommand;
                }
                vertices_.AddVertex(x, y, pathCmd);
            }
        }
    }
#endif
private:
    VertexBlockStorage vertices_;
    uint32_t iterator_;
};
} // namespace OHOS
#endif
