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

#include "core/components/svg/svg_transform.h"

#include "base/utils/string_utils.h"

namespace OHOS::Ace {
namespace {

const char TRANSFORM_MATRIX[] = "matrix";
const char TRANSFORM_ROTATE[] = "rotate";
const char TRANSFORM_SCALE[] = "scale";
const char TRANSFORM_SKEW[] = "skew";
const char TRANSFORM_SKEWX[] = "skewX";
const char TRANSFORM_SKEWY[] = "skewY";
const char TRANSFORM_TRANSLATE[] = "translate";

} // namespace

using namespace StringUtils;

Matrix4 SvgTransform::CreateMatrix4(const std::string& transform)
{
    auto retMat = Matrix4::CreateIdentity();
    std::vector<std::string> attrs;
    SplitStr(transform, ")", attrs);
    for (auto& attr : attrs) {
        std::string type = attr.substr(0, attr.find_first_of("("));
        std::string values = attr.substr(attr.find_first_of("(") + 1);
        std::vector<std::string> numVec;
        std::string tag = (values.find(",") != std::string::npos) ? "," : " ";
        SplitStr(values, tag, numVec);
        if (numVec.empty()) {
            continue;
        }

        Matrix4 mat = Matrix4::CreateIdentity();
        TrimStr(attr);
        if (type == TRANSFORM_TRANSLATE) {
            if (numVec.size() == 1) {
                mat = Matrix4::CreateTranslate(StringToFloat(numVec[0].c_str()), 0, 0);
            } else {
                mat = Matrix4::CreateTranslate(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), 0);
            }
        } else if (type == TRANSFORM_SCALE) {
            if (numVec.size() == 1) {
                mat = Matrix4::CreateScale(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[0].c_str()), 1);
            } else {
                mat = Matrix4::CreateScale(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), 1);
            }
        } else if (type == TRANSFORM_ROTATE) {
            mat = Matrix4::CreateRotate(StringToFloat(numVec[0].c_str()), 0, 0, 1);
        } else if (type == TRANSFORM_SKEWX) {
            mat = Matrix4::CreateSkew(StringToFloat(numVec[0].c_str()), 0);
        } else if (type == TRANSFORM_SKEWY) {
            mat = Matrix4::CreateSkew(0, StringToFloat(numVec[0].c_str()));
        } else if (type == TRANSFORM_MATRIX && numVec.size() == 6) {
            mat = Matrix4::CreateMatrix2D(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()),
                                          StringToFloat(numVec[2].c_str()), StringToFloat(numVec[3].c_str()),
                                          StringToFloat(numVec[4].c_str()), StringToFloat(numVec[5].c_str()));
        } else {
            continue;
        }
        retMat = retMat * mat;
    }
    return retMat;
}

TransformInfo SvgTransform::CreateTransformInfo(const std::string& transform)
{
    auto retMat = Matrix4::CreateIdentity();
    std::vector<std::string> attrs;
    SplitStr(transform, ")", attrs);
    TransformInfo transformInfo;
    for (auto& attr : attrs) {
        std::string type = attr.substr(0, attr.find_first_of("("));
        std::string values = attr.substr(attr.find_first_of("(") + 1);
        std::vector<std::string> numVec;
        std::string tag = (values.find(",") != std::string::npos) ? "," : " ";
        SplitStr(values, tag, numVec);
        if (numVec.empty()) {
            continue;
        }

        Matrix4 mat = Matrix4::CreateIdentity();
        TrimStr(attr);
        if (type == TRANSFORM_TRANSLATE) {
            if (numVec.size() == 1) {
                mat = Matrix4::CreateTranslate(StringToFloat(numVec[0].c_str()), 0, 0);
            } else {
                mat = Matrix4::CreateTranslate(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), 0);
            }
        } else if (type == TRANSFORM_SCALE) {
            if (numVec.size() == 1) {
                mat = Matrix4::CreateScale(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[0].c_str()), 1);
            } else {
                mat = Matrix4::CreateScale(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), 1);
            }
        } else if (type == TRANSFORM_ROTATE) {
            mat = Matrix4::CreateRotate(StringToFloat(numVec[0].c_str()), 0, 0, 1);
            if (numVec.size() >= 3) {
                transformInfo.hasRotateCenter = true;
                transformInfo.rotateCenter = Offset(StringToFloat(numVec[1]), StringToFloat(numVec[2]));
            }
        } else if (type == TRANSFORM_SKEWX) {
            mat = Matrix4::CreateSkew(StringToFloat(numVec[0].c_str()), 0);
        } else if (type == TRANSFORM_SKEWY) {
            mat = Matrix4::CreateSkew(0, StringToFloat(numVec[0].c_str()));
        } else if (type == TRANSFORM_MATRIX && numVec.size() == 6) {
            mat = Matrix4::CreateMatrix2D(StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()),
                                          StringToFloat(numVec[2].c_str()), StringToFloat(numVec[3].c_str()),
                                          StringToFloat(numVec[4].c_str()), StringToFloat(numVec[5].c_str()));
        } else {
            continue;
        }
        retMat = retMat * mat;
    }
    transformInfo.matrix4 = retMat;
    return transformInfo;
}

TransformInfo SvgTransform::CreateMatrix4(const std::map<std::string, std::vector<float>>& transfrom)
{
    auto retMat = Matrix4::CreateIdentity();
    auto mat = Matrix4::CreateIdentity();
    TransformInfo transformInfo;
    for (auto& [type, values] : transfrom) {
        if (values.empty()) {
            continue;
        }
        if (type == TRANSFORM_TRANSLATE && values.size() >= 2) {
            mat = Matrix4::CreateTranslate(values[0], values[1], 0);
        } else if (type == TRANSFORM_SCALE && values.size() >= 2) {
            mat = Matrix4::CreateScale(values[0], values[1], 1);
        } else if (type == TRANSFORM_ROTATE) {
            mat = Matrix4::CreateRotate(values[0], 0, 0, 1);
            if (values.size() >= 3) {
                transformInfo.hasRotateCenter = true;
                transformInfo.rotateCenter = Offset(values[1], values[2]);
            }
        } else if (type == TRANSFORM_SKEW && values.size() >= 2) {
            mat = Matrix4::CreateSkew(values[0], values[1]);
        } else {
            continue;
        }
        retMat = retMat * mat;
    }
    transformInfo.matrix4 = retMat;
    return transformInfo;
}

std::map<std::string, std::vector<float>> SvgTransform::CreateMap(const std::string& transform)
{
    std::map<std::string, std::vector<float>> mapTrans;
    std::vector<std::string> attrs;
    SplitStr(transform, ")", attrs);
    for (auto& attr : attrs) {
        std::string type = attr.substr(0, attr.find_first_of("("));
        std::string values = attr.substr(attr.find_first_of("(") + 1);
        std::vector<std::string> numVec;
        std::string tag = (values.find(",") != std::string::npos) ? "," : " ";
        SplitStr(values, tag, numVec);
        if (numVec.empty()) {
            continue;
        }
        TrimStr(attr);
        if (type == TRANSFORM_TRANSLATE) {
            if (numVec.size() == 1) {
                mapTrans[TRANSFORM_TRANSLATE] = { StringToFloat(numVec[0].c_str()), 0, 0 };
            } else {
                mapTrans[TRANSFORM_TRANSLATE] = {
                    StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), 0
                };
            }
        } else if (type == TRANSFORM_SCALE) {
            if (numVec.size() == 1) {
                mapTrans[TRANSFORM_SCALE] = { StringToFloat(numVec[0].c_str()), StringToFloat(numVec[0].c_str()), 1 };
            } else {
                mapTrans[TRANSFORM_SCALE] = { StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), 1 };
            }
        } else if (type == TRANSFORM_ROTATE) {
            if (numVec.size() == 1) {
                mapTrans[TRANSFORM_ROTATE] = { StringToFloat(numVec[0].c_str())};
            } else if (numVec.size() >= 3) {
                mapTrans[TRANSFORM_ROTATE] = {
                    StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), StringToFloat(numVec[2].c_str())
                };
            }
        } else if (type == TRANSFORM_SKEWX) {
            if (mapTrans.find(TRANSFORM_SKEW) != mapTrans.end()) {
                mapTrans[TRANSFORM_SKEW][0] += StringToFloat(numVec[0].c_str());
                continue;
            }
            mapTrans[TRANSFORM_SKEW] = { StringToFloat(numVec[0].c_str()), 0 };
        } else if (type == TRANSFORM_SKEWY) {
            if (mapTrans.find(TRANSFORM_SKEW) != mapTrans.end()) {
                mapTrans[TRANSFORM_SKEW][1] += StringToFloat(numVec[0].c_str());
                continue;
            }
            mapTrans[TRANSFORM_SKEW] = { 0, StringToFloat(numVec[0].c_str()) };
        } else if (type == TRANSFORM_MATRIX && numVec.size() >= 6) {
            mapTrans[TRANSFORM_MATRIX] = {
                StringToFloat(numVec[0].c_str()), StringToFloat(numVec[1].c_str()), StringToFloat(numVec[2].c_str()),
                StringToFloat(numVec[3].c_str()), StringToFloat(numVec[4].c_str()), StringToFloat(numVec[5].c_str())
            };
        }
    }
    return mapTrans;
}

bool SvgTransform::SetProperty(const std::string& type, const std::vector<float>& from, const std::vector<float>& to,
    double value, std::map<std::string, std::vector<float>>& transformAttrs)
{
    if ((type == TRANSFORM_SCALE || type == TRANSFORM_TRANSLATE) && from.size() >= 2 && to.size() >= 2) {
        float x = from[0] + (to[0] - from[0]) * value;
        float y = from[1] + (to[1] - from[1]) * value;
        transformAttrs[type] = { x, y, 1};
    } else if (type == TRANSFORM_ROTATE && from.size() >= 3 && to.size() >= 3) {
        float rotate = from[0] + (to[0] - from[0]) * value;
        float centerX = from[1] + (to[1] - from[1]) * value;
        float centerY = from[2] + (to[2] - from[2]) * value;
        transformAttrs[TRANSFORM_ROTATE] = { rotate, centerX, centerY};
    } else if (type == TRANSFORM_SKEWX && from.size() >= 1 && to.size() >= 1) {
        float skewX = from[0] + (to[0] - from[0]) * value;
        if (transformAttrs.find(TRANSFORM_SKEW) != transformAttrs.end()) {
            transformAttrs[TRANSFORM_SKEW][0] = skewX;
        } else {
            transformAttrs[TRANSFORM_SKEW] = { skewX, 0.0f};
        }
    } else if (type == TRANSFORM_SKEWY && from.size() >= 1 && to.size() >= 1) {
        float skewY = from[0] + (to[0] - from[0]) * value;
        if (transformAttrs.find(TRANSFORM_SKEW) != transformAttrs.end()) {
            transformAttrs[TRANSFORM_SKEW][1] = skewY;
        } else {
            transformAttrs[TRANSFORM_SKEW] = { 0.0f, skewY };
        }
    } else {
        return false;
    }

    return true;
}

bool SvgTransform::AlignmentValues(const std::string& type, std::vector<float>& from, std::vector<float>& to)
{
    auto fromSize = from.size();
    auto toSize = to.size();
    if (type == TRANSFORM_SCALE) {
        if (fromSize == 0) {
            from = { 0.0f, 0.0f };
        } else if (fromSize == 1) {
            from.push_back(from[0]);
        }
        if (toSize == 1) {
            to.push_back(to[0]);
        }
    } else if (type == TRANSFORM_SKEWX || type == TRANSFORM_SKEWY) {
        if (fromSize == 0) {
            from = { 0.0f };
        }
    } else if (type == TRANSFORM_TRANSLATE) {
        if (fromSize == 0) {
            from = { 0.0f, 0.0f };
        } else if (fromSize == 1) {
            from.push_back(0.0f);
        }
        if (toSize == 1) {
            to.push_back(0.0f);
        }
    } else if (type == TRANSFORM_ROTATE) {
        if (fromSize == 0) {
            from = { 0.0f, 0.0f, 0.0f };
        } else if (fromSize < 3) {
            from = { from[0], 0.0f, 0.0f };
        }
        if (toSize < 3) {
            to = { to[0], 0.0f, 0.0f };
        }
    } else {
        return false;
    }
    return true;
}

bool SvgTransform::AlignmentFrame(const std::string& type, std::vector<float>& frame)
{
    auto size = frame.size();
    if (size == 0) {
        return false;
    }

    if (type == TRANSFORM_SCALE) {
        if (size == 1) {
            frame.push_back(frame[0]);
        }
    } else if (type == TRANSFORM_TRANSLATE) {
        if (size == 1) {
            frame.push_back(0.0f);
        }
    } else if (type == TRANSFORM_ROTATE) {
        if (size < 3) {
            frame = { frame[0], 0.0f, 0.0f };
        }
    } else if (type == TRANSFORM_SKEWX || type == TRANSFORM_SKEWY) {
    } else {
        return false;
    }
    return true;
}

} // namespace OHOS::Ace

