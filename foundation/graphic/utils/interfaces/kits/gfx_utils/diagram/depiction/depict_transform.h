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
 * @file depict_transform.h
 *
 * @brief Defines Building vertex matrix transformation pipeline
 * ransformation matrix. Through the operation of vertex coordinates and matrix, we can get new coordinates
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_DEPICT_TRANSFORM_H
#define GRAPHIC_LITE_DEPICT_TRANSFORM_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/trans_affine.h"
namespace OHOS {
/**
 * @template<VertexSource,TransAffine> class DepictTransform
 * @brief The depicttransform class forms a 2 * 3 matrix through six variables,
 * A new coordinate is obtained after calculation with the coordinate.
 * @since 1.0
 * @version 1.0
 */
template <class VertexSource>
class DepictTransform {
public:
    /**
     * @brief DepictTransform Class constructor
     * The construction parameters are VertexSource and TransAffine attributes,
     * which determine the vertex source of the curve.
     * @since 1.0
     * @version 1.0
     */
    DepictTransform(VertexSource& source, TransAffine& tr)
        : source_(&source), trans_(&tr) {}

    void Attach(VertexSource& source)
    {
        source_ = &source;
    }

    void Rewind(uint32_t pathId)
    {
        source_->Rewind(pathId);
    }

    uint32_t GenerateVertex(float* x, float* y)
    {
        uint32_t cmd = source_->GenerateVertex(x, y);
        if (IsVertex(cmd)) {
            trans_->Transform(x, y);
        }
        return cmd;
    }

    void GetTransformer(TransAffine& tr)
    {
        trans_ = &tr;
    }

private:

    VertexSource* source_;
    TransAffine* trans_;
};
} // namespace OHOS
#endif
