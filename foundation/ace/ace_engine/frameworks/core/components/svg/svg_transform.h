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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVGTRANSFORM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVGTRANSFORM_H

#include <map>
#include <vector>

#include "base/geometry/matrix4.h"

namespace OHOS::Ace {

struct TransformInfo {
    Matrix4 matrix4;
    Offset rotateCenter;
    bool hasRotateCenter = false;
};

class SvgTransform final {
public:
    SvgTransform() = default;
    ~SvgTransform() = default;

    // input: "rotate(-10, 50, 100) translate(-36, 45.5) skewX(40) skewY(40) scale(1, 0.5) matrix(1, 2, 3, 4, 5, 6)"
    static Matrix4 CreateMatrix4(const std::string& transform);
    static TransformInfo CreateTransformInfo(const std::string& transform);

    // input: "rotate(-10, 50, 100) translate(-36, 45.5) skewX(40) skewY(40) scale(1, 0.5) matrix(1, 2, 3, 4, 5, 6)"
    // output: "{ { "rotate" : {-10, 50, 100} }, { "translate" : {-36, 45.5} }, { "skew" : {40, 50} },
    //            { "scale", {1, 0.5} }, { "matrix", {1, 2, 3, 4, 5, 6} } }"
    static std::map<std::string, std::vector<float>> CreateMap(const std::string& transform);

    static TransformInfo CreateMatrix4(const std::map<std::string, std::vector<float>>& transfrom);

    static bool SetProperty(const std::string& type, const std::vector<float>& from, const std::vector<float>& to,
        double value, std::map<std::string, std::vector<float>>& transformAttrs);

    static bool AlignmentValues(const std::string& type, std::vector<float>& from, std::vector<float>& to);

    static bool AlignmentFrame(const std::string& type, std::vector<float>& frame);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVGTRANSFORM_H
