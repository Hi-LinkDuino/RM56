/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_matrix4_module.h"

#include "base/geometry/matrix4.h"
#include "base/log/log.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"

namespace OHOS::Ace::Framework {

namespace {

constexpr int32_t MATRIX_LENGTH = 16;

Matrix4 ConvertToMatrix(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    Matrix4 result = Matrix4::CreateIdentity();
    if (value->GetArrayLength(runtime) != MATRIX_LENGTH) {
        LOGE("ConvertToMatrix failed, JSValueConst length != %d", MATRIX_LENGTH);
        return result;
    }
    // in column order
    for (int32_t i = 0; i < Matrix4::DIMENSION; i++) {
        for (int32_t j = 0; j < Matrix4::DIMENSION; j++) {
            auto index = i * Matrix4::DIMENSION + j;
            auto itemJSValue = value->GetProperty(runtime, index);
            if (!itemJSValue->IsNumber(runtime)) {
                LOGE("ConvertToMatrix failed, %s is not number", itemJSValue->ToString(runtime).c_str());
                return result;
            }
            result.Set(j, i, static_cast<float>(itemJSValue->ToDouble(runtime)));
        }
    }
    return result;
}

shared_ptr<JsValue> ConvertToJSValue(const shared_ptr<JsRuntime>& runtime, const Matrix4& matrix)
{
    shared_ptr<JsValue> result = runtime->NewArray();
    for (int32_t i = 0; i < Matrix4::DIMENSION; i++) {
        for (int32_t j = 0; j < Matrix4::DIMENSION; j++) {
            int32_t index = i * Matrix4::DIMENSION + j;
            result->SetProperty(runtime, runtime->NewInt32(index), runtime->NewNumber(matrix.Get(j, i)));
        }
    }
    return result;
}

double ConvertToDouble(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value, double defaultValue)
{
    if (value == nullptr || !value->IsNumber(runtime)) {
        return defaultValue;
    }
    return value->ToDouble(runtime);
}

}

shared_ptr<JsValue> Init(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("PagePush args count is invalid");
        return runtime->NewNull();
    }

    int32_t length = argv[0]->GetArrayLength(runtime);
    if (length != MATRIX_LENGTH) {
        LOGE("matrix init array length %d not equal %d", length, MATRIX_LENGTH);
        return runtime->NewNull();
    }

    auto matrix = ConvertToMatrix(runtime, argv[0]);
    thisObj->SetProperty(runtime, MATRIX_4X4, ConvertToJSValue(runtime, matrix));
    return thisObj;
}

shared_ptr<JsValue> Combine(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("Combine failed, argc != 1");
        return runtime->NewNull();
    }
    if (!argv[0]->IsObject(runtime)) {
        LOGE("Combine failed, argv[0] is not object");
        return runtime->NewNull();
    }

    auto objA = argv[0]->GetProperty(runtime, MATRIX_4X4);
    auto objB = thisObj->GetProperty(runtime, MATRIX_4X4);
    auto matrixA = ConvertToMatrix(runtime, objA);
    auto matrixB = ConvertToMatrix(runtime, objB);
    auto newArrayJSValue = ConvertToJSValue(runtime, matrixA * matrixB);
    thisObj->SetProperty(runtime, MATRIX_4X4, newArrayJSValue);
    return thisObj;
}

shared_ptr<JsValue> Invert(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto matrixArray = thisObj->GetProperty(runtime, MATRIX_4X4);
    auto matrix = ConvertToMatrix(runtime, matrixArray);
    matrix = Matrix4::Invert(matrix);
    matrixArray = ConvertToJSValue(runtime, matrix);
    thisObj->SetProperty(runtime, MATRIX_4X4, matrixArray);
    return thisObj;
}

shared_ptr<JsValue> Translate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("Translate failed, argc != 1");
        return runtime->NewNull();
    }
    if (!argv[0]->IsObject(runtime)) {
        LOGE("Translate failed, argv[0] is not object");
        return runtime->NewNull();
    }

    auto matrixArray = thisObj->GetProperty(runtime, MATRIX_4X4);
    auto matrix = ConvertToMatrix(runtime, matrixArray);
    auto dxJSValue = argv[0]->GetProperty(runtime, "x");
    double dx = ConvertToDouble(runtime, dxJSValue, 0.0);
    auto dyJSValue = argv[0]->GetProperty(runtime, "y");
    double dy = ConvertToDouble(runtime, dyJSValue, 0.0);
    auto dzJSValue = argv[0]->GetProperty(runtime, "z");
    double dz = ConvertToDouble(runtime, dzJSValue, 0.0);

    matrix = Matrix4::CreateTranslate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz)) * matrix;
    thisObj->SetProperty(runtime, MATRIX_4X4, ConvertToJSValue(runtime, matrix));
    return thisObj;
}

shared_ptr<JsValue> Scale(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("Scale failed, argc != 1");
        return runtime->NewNull();
    }
    if (!argv[0]->IsObject(runtime)) {
        LOGE("Scale failed, argv[0] is not object");
        return runtime->NewNull();
    }

    auto matrixArray = thisObj->GetProperty(runtime, MATRIX_4X4);
    auto matrix = ConvertToMatrix(runtime, matrixArray);
    auto dxJSValue = argv[0]->GetProperty(runtime, "x");
    double dx = ConvertToDouble(runtime, dxJSValue, 1.0);
    auto dyJSValue = argv[0]->GetProperty(runtime, "y");
    double dy = ConvertToDouble(runtime, dyJSValue, 1.0);
    auto dzJSValue = argv[0]->GetProperty(runtime, "z");
    double dz = ConvertToDouble(runtime, dzJSValue, 1.0);
    auto centerXJSValue = argv[0]->GetProperty(runtime, "centerX");
    double centerX = ConvertToDouble(runtime, centerXJSValue, 0.0);
    auto centerYJSValue = argv[0]->GetProperty(runtime, "centerY");
    double centerY = ConvertToDouble(runtime, centerYJSValue, 0.0);

    auto scaleMatrix = Matrix4::CreateScale(dx, dy, dz) * matrix;
    if (!NearZero(centerX) || !NearZero(centerY)) {
        auto translate1 = Matrix4::CreateTranslate(centerX, centerY, 0.0);
        auto translate2 = Matrix4::CreateTranslate(-centerX, -centerY, 0.0);
        scaleMatrix = scaleMatrix * translate2;
        scaleMatrix = translate1 * scaleMatrix;
    }
    matrix = scaleMatrix * matrix;
    thisObj->SetProperty(runtime, MATRIX_4X4, ConvertToJSValue(runtime, matrix));
    return thisObj;
}

shared_ptr<JsValue> Rotate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("Rotate failed, argc != 1");
        return runtime->NewNull();
    }
    if (!argv[0]->IsObject(runtime)) {
        LOGE("Rotate failed, argv[0] is not object");
        return runtime->NewNull();
    }

    auto matrixArray = thisObj->GetProperty(runtime, MATRIX_4X4);
    auto matrix = ConvertToMatrix(runtime, matrixArray);
    auto dxJSValue = argv[0]->GetProperty(runtime, "x");
    double dx = ConvertToDouble(runtime, dxJSValue, 0.0);
    auto dyJSValue = argv[0]->GetProperty(runtime, "y");
    double dy = ConvertToDouble(runtime, dyJSValue, 0.0);
    auto dzJSValue = argv[0]->GetProperty(runtime, "z");
    double dz = ConvertToDouble(runtime, dzJSValue, 0.0);
    auto angleJSValue = argv[0]->GetProperty(runtime, "angle");
    double angle = ConvertToDouble(runtime, angleJSValue, 0.0);
    auto centerXJSValue = argv[0]->GetProperty(runtime, "centerX");
    double centerX = ConvertToDouble(runtime, centerXJSValue, 0.0);
    auto centerYJSValue = argv[0]->GetProperty(runtime, "centerY");
    double centerY = ConvertToDouble(runtime, centerYJSValue, 0.0);

    auto rotateMatrix = Matrix4::CreateRotate(angle, dx, dy, dz);
    if (!NearZero(centerX) || !NearZero(centerY)) {
        auto translate1 = Matrix4::CreateTranslate(centerX, centerY, 0.0);
        auto translate2 = Matrix4::CreateTranslate(-centerX, -centerY, 0.0);
        rotateMatrix = rotateMatrix * translate2;
        rotateMatrix = translate1 * rotateMatrix;
    }
    matrix = rotateMatrix * matrix;
    matrixArray = ConvertToJSValue(runtime, matrix);
    thisObj->SetProperty(runtime, MATRIX_4X4, ConvertToJSValue(runtime, matrix));
    return thisObj;
}

shared_ptr<JsValue> TransformPoint(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("TransformPoint failed, argc != 1");
        return runtime->NewNull();
    }
    if (!argv[0]->IsArray(runtime) || argv[0]->GetArrayLength(runtime) != 2) {
        LOGE("TransformPoint failed, argv[0] is not array or length != 2");
        return runtime->NewNull();
    }
    auto matrix = ConvertToMatrix(runtime, thisObj->GetProperty(runtime, MATRIX_4X4));

    auto pointXJSValue = argv[0]->GetProperty(runtime, 0);
    double pointX = ConvertToDouble(runtime, pointXJSValue, 0.0);

    auto pointYJSValue = argv[0]->GetProperty(runtime, 1);
    double pointY = ConvertToDouble(runtime, pointYJSValue, 0.0);

    Point point { pointX, pointY };
    Point target = matrix * point;
    shared_ptr<JsValue> result = runtime->NewArray();
    result->SetProperty(runtime, runtime->NewInt32(0), runtime->NewNumber(target.GetX()));
    result->SetProperty(runtime, runtime->NewInt32(1), runtime->NewNumber(target.GetY()));
    return result;
}

shared_ptr<JsValue> Copy(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto matrix = ConvertToMatrix(runtime, thisObj->GetProperty(runtime, MATRIX_4X4));
    // create new object
    shared_ptr<JsValue> other = runtime->NewObject();
    // init functions
    InitMatrix4Module(runtime, other);
    // update matrix4x4
    other->SetProperty(runtime, MATRIX_4X4, ConvertToJSValue(runtime, matrix));
    other->SetProperty(runtime, MATRIX_COPY, runtime->NewFunction(Copy));
    other->SetProperty(runtime, MATRIX_COMBINE, runtime->NewFunction(Combine));
    other->SetProperty(runtime, MATRIX_INVERT, runtime->NewFunction(Invert));
    other->SetProperty(runtime, MATRIX_TRANSLATE, runtime->NewFunction(Translate));
    other->SetProperty(runtime, MATRIX_SCALE, runtime->NewFunction(Scale));
    other->SetProperty(runtime, MATRIX_ROTATE, runtime->NewFunction(Rotate));
    other->SetProperty(runtime, MATRIX_TRANSFORM_POINT, runtime->NewFunction(TransformPoint));
    return other;
}

shared_ptr<JsValue> Identity(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    shared_ptr<JsValue> matrixObj = runtime->NewObject();
    matrixObj->SetProperty(runtime, MATRIX_4X4, ConvertToJSValue(runtime, Matrix4::CreateIdentity()));
    matrixObj->SetProperty(runtime, MATRIX_COPY, runtime->NewFunction(Copy));
    matrixObj->SetProperty(runtime, MATRIX_COMBINE, runtime->NewFunction(Combine));
    matrixObj->SetProperty(runtime, MATRIX_INVERT, runtime->NewFunction(Invert));
    matrixObj->SetProperty(runtime, MATRIX_TRANSLATE, runtime->NewFunction(Translate));
    matrixObj->SetProperty(runtime, MATRIX_SCALE, runtime->NewFunction(Scale));
    matrixObj->SetProperty(runtime, MATRIX_ROTATE, runtime->NewFunction(Rotate));
    matrixObj->SetProperty(runtime, MATRIX_TRANSFORM_POINT, runtime->NewFunction(TransformPoint));
    return matrixObj;
}

void InitMatrix4Module(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& moduleObj)
{
    moduleObj->SetProperty(runtime, MATRIX_4X4, ConvertToJSValue(runtime, Matrix4::CreateIdentity()));
    moduleObj->SetProperty(runtime, MATRIX_INIT, runtime->NewFunction(Init));
    moduleObj->SetProperty(runtime, MATRIX_IDENTITY, runtime->NewFunction(Identity));
}

} // namespace OHOS::Ace::Framework
