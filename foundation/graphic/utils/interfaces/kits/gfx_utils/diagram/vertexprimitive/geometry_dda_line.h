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
 * @file geometryL_dda_line.h
 * @brief DDA2 Line Algorithm
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GEOMETRY_DDA_LINE_H
#define GRAPHIC_LITE_GEOMETRY_DDA_LINE_H

#include "gfx_utils/diagram/common/common_basics.h"

namespace OHOS {
/**
 * @brief Dda2 algorithm
 * @see Dda2LineInterpolator
 * @since 1.0
 * @version 1.0
 */
class GeometryDdaLine {
public:
    GeometryDdaLine() {}

    GeometryDdaLine(int32_t coordinate1, int32_t coordinate2, int32_t count)
        : dividCount_(count <= 0 ? 1 : count),
          leftSideCoordinate_((coordinate2 - coordinate1) / dividCount_),
          remainderValue_((coordinate2 - coordinate1) % dividCount_),
          moduloOperate_(remainderValue_),
          coordinateData_(coordinate1)
    {
        if (moduloOperate_ <= 0) {
            moduloOperate_ += count;
            remainderValue_ += count;
            leftSideCoordinate_--;
        }
        moduloOperate_ -= count;
    }

    void operator++()
    {
        moduloOperate_ += remainderValue_;
        coordinateData_ += leftSideCoordinate_;
        if (moduloOperate_ > 0) {
            moduloOperate_ -= dividCount_;
            coordinateData_++;
        }
    }

    void operator--()
    {
        if (moduloOperate_ <= remainderValue_) {
            moduloOperate_ += dividCount_;
            coordinateData_--;
        }
        moduloOperate_ -= remainderValue_;
        coordinateData_ -= leftSideCoordinate_;
    }

    int32_t GetCoordinate() const
    {
        return coordinateData_;
    }

private:
    int32_t dividCount_;
    int32_t leftSideCoordinate_;
    int32_t remainderValue_;
    int32_t moduloOperate_;
    int32_t coordinateData_;
};
} // namespace OHOS
#endif
