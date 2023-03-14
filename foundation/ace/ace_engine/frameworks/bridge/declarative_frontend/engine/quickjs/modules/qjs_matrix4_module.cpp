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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_matrix4_module.h"

#include "base/geometry/matrix4.h"
#include "base/log/log.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t MATRIX_LENGTH = 16;

Matrix4 ConvertToMatrix(JSContext* ctx, JSValueConst value)
{
    Matrix4 result = Matrix4::CreateIdentity();
    if (QJSUtils::JsGetArrayLength(ctx, value) != MATRIX_LENGTH) {
        LOGE("ConvertToMatrix failed, JSValueConst length != %d", MATRIX_LENGTH);
        return result;
    }
    // in column order
    for (int32_t i = 0; i < Matrix4::DIMENSION; i++) {
        for (int32_t j = 0; j < Matrix4::DIMENSION; j++) {
            auto index = i * Matrix4::DIMENSION + j;
            auto itemJSValue = JS_GetPropertyUint32(ctx, value, index);
            double num = 0.0;
            if (JS_ToFloat64(ctx, &num, itemJSValue) != 0) {
                LOGE("ConvertToMatrix failed, %s is not number", JS_ToCString(ctx, itemJSValue));
                return result;
            }
            result.Set(j, i, static_cast<float>(num));
        }
    }
    return result;
}

JSValue ConvertToJSValue(JSContext* ctx, const Matrix4& matrix)
{
    auto result = JS_NewArray(ctx);
    for (int32_t i = 0; i < Matrix4::DIMENSION; i++) {
        for (int32_t j = 0; j < Matrix4::DIMENSION; j++) {
            int32_t index = i * Matrix4::DIMENSION + j;
            JS_SetPropertyUint32(ctx, result, index, JS_NewFloat64(ctx, matrix.Get(j, i)));
        }
    }
    return result;
}

double ConvertToDouble(JSContext* ctx, JSValueConst value, double defaultValue)
{
    if (JS_IsNull(value) || JS_IsUndefined(value)) {
        return defaultValue;
    }
    double result = defaultValue;
    if (JS_ToFloat64(ctx, &result, value) != 0) {
        return defaultValue;
    }
    return result;
}

} // namespace

JSValue Init(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1 || argv == nullptr) {
        LOGE("matrix4 init argc != 1 or argv is nullptr");
        return JS_NULL;
    }

    // create new object
    JSValue other = JS_NewObject(ctx);
    // init functions
    InitMatrix4Module(ctx, other);
    int32_t length = QJSUtils::JsGetArrayLength(ctx, argv[0]);
    if (length != MATRIX_LENGTH) {
        LOGE("matrix init array length %d not equal %d", length, MATRIX_LENGTH);
        return JS_NULL;
    }
    auto matrix = ConvertToMatrix(ctx, argv[0]);
    JS_SetPropertyStr(ctx, other, MATRIX_4X4, ConvertToJSValue(ctx, matrix));
    return other;
}

JSValue Copy(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto matrix = ConvertToMatrix(ctx, JS_GetPropertyStr(ctx, value, MATRIX_4X4));
    // create new object
    JSValue other = JS_NewObject(ctx);
    // init functions
    InitMatrix4Module(ctx, other);
    // update matrix4x4
    JS_SetPropertyStr(ctx, other, MATRIX_4X4, ConvertToJSValue(ctx, matrix));
    return other;
}

JSValue Combine(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1 || argv == nullptr) {
        LOGE("Combine failed, argc != 1 or argv == nullptr");
        return JS_NULL;
    }
    if (!JS_IsObject(argv[0])) {
        LOGE("Combine failed, argv[0] is not object");
        return JS_NULL;
    }
    auto objA = JS_GetPropertyStr(ctx, argv[0], MATRIX_4X4);
    auto objB = JS_GetPropertyStr(ctx, value, MATRIX_4X4);
    auto matrixA = ConvertToMatrix(ctx, objA);
    auto matrixB = ConvertToMatrix(ctx, objB);
    auto newArrayJSValue = ConvertToJSValue(ctx, matrixA * matrixB);
    JS_SetPropertyStr(ctx, value, MATRIX_4X4, newArrayJSValue);
    JS_DupValue(ctx, value);
    return value;
}

JSValue Identity(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // create new object
    JSValue other = JS_NewObject(ctx);
    // init functions
    InitMatrix4Module(ctx, other);
    JS_SetPropertyStr(ctx, other, MATRIX_4X4, ConvertToJSValue(ctx, Matrix4::CreateIdentity()));
    return other;
}

JSValue Invert(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto matrixArray = JS_GetPropertyStr(ctx, value, MATRIX_4X4);
    auto matrix = ConvertToMatrix(ctx, matrixArray);
    matrix = Matrix4::Invert(matrix);
    matrixArray = ConvertToJSValue(ctx, matrix);
    JS_SetPropertyStr(ctx, value, MATRIX_4X4, matrixArray);
    JS_DupValue(ctx, value);
    return value;
}

JSValue Translate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1 || argv == nullptr) {
        LOGE("Translate failed, argc != 1 or argv == nullptr");
        return JS_NULL;
    }
    if (!JS_IsObject(argv[0])) {
        LOGE("Translate failed, argv[0] is not object");
        return JS_NULL;
    }
    auto matrixArray = JS_GetPropertyStr(ctx, value, MATRIX_4X4);
    auto matrix = ConvertToMatrix(ctx, matrixArray);
    auto dxJSValue = JS_GetPropertyStr(ctx, argv[0], "x");
    double dx = ConvertToDouble(ctx, dxJSValue, 0.0);
    auto dyJSValue = JS_GetPropertyStr(ctx, argv[0], "y");
    double dy = ConvertToDouble(ctx, dyJSValue, 0.0);
    auto dzJSValue = JS_GetPropertyStr(ctx, argv[0], "z");
    double dz = ConvertToDouble(ctx, dzJSValue, 0.0);
    matrix = Matrix4::CreateTranslate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz)) * matrix;
    JS_SetPropertyStr(ctx, value, MATRIX_4X4, ConvertToJSValue(ctx, matrix));
    JS_DupValue(ctx, value);
    return value;
}

JSValue Scale(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1 || argv == nullptr) {
        LOGE("Scale failed, argc != 1 or argv == nullptr");
        return JS_NULL;
    }
    if (!JS_IsObject(argv[0])) {
        LOGE("Scale failed, argv[0] is not object");
        return JS_NULL;
    }
    auto matrixArray = JS_GetPropertyStr(ctx, value, MATRIX_4X4);
    auto matrix = ConvertToMatrix(ctx, matrixArray);
    // parse scale
    auto dxJSValue = JS_GetPropertyStr(ctx, argv[0], "x");
    double dx = ConvertToDouble(ctx, dxJSValue, 1.0);
    auto dyJSValue = JS_GetPropertyStr(ctx, argv[0], "y");
    double dy = ConvertToDouble(ctx, dyJSValue, 1.0);
    auto dzJSValue = JS_GetPropertyStr(ctx, argv[0], "z");
    double dz = ConvertToDouble(ctx, dzJSValue, 1.0);
    auto centerXJSValue = JS_GetPropertyStr(ctx, argv[0], "centerX");
    double centerX = ConvertToDouble(ctx, centerXJSValue, 0.0);
    auto centerYJSValue = JS_GetPropertyStr(ctx, argv[0], "centerY");
    double centerY = ConvertToDouble(ctx, centerYJSValue, 0.0);

    auto scaleMatrix = Matrix4::CreateScale(dx, dy, dz);
    if (!NearZero(centerX) || !NearZero(centerY)) {
        auto translate1 = Matrix4::CreateTranslate(centerX, centerY, 0.0);
        auto translate2 = Matrix4::CreateTranslate(-centerX, -centerY, 0.0);
        scaleMatrix = scaleMatrix * translate2;
        scaleMatrix = translate1 * scaleMatrix;
    }
    matrix = scaleMatrix * matrix;
    JS_SetPropertyStr(ctx, value, MATRIX_4X4, ConvertToJSValue(ctx, matrix));
    JS_DupValue(ctx, value);
    return value;
}

JSValue Rotate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1 || argv == nullptr) {
        LOGE("Rotate failed, argc != 1 or argv == nullptr");
        return JS_NULL;
    }
    if (!JS_IsObject(argv[0])) {
        LOGE("Rotate failed, argv[0] is not object");
        return JS_NULL;
    }
    auto matrixArray = JS_GetPropertyStr(ctx, value, MATRIX_4X4);
    auto matrix = ConvertToMatrix(ctx, matrixArray);
    // parse scale
    auto dxJSValue = JS_GetPropertyStr(ctx, argv[0], "x");
    double dx = ConvertToDouble(ctx, dxJSValue, 0.0);
    auto dyJSValue = JS_GetPropertyStr(ctx, argv[0], "y");
    double dy = ConvertToDouble(ctx, dyJSValue, 0.0);
    auto dzJSValue = JS_GetPropertyStr(ctx, argv[0], "z");
    double dz = ConvertToDouble(ctx, dzJSValue, 0.0);
    auto angleJSValue = JS_GetPropertyStr(ctx, argv[0], "angle");
    double angle = ConvertToDouble(ctx, angleJSValue, 0.0);
    auto centerXJSValue = JS_GetPropertyStr(ctx, argv[0], "centerX");
    double centerX = ConvertToDouble(ctx, centerXJSValue, 0.0);
    auto centerYJSValue = JS_GetPropertyStr(ctx, argv[0], "centerY");
    double centerY = ConvertToDouble(ctx, centerYJSValue, 0.0);
    auto rotateMatrix = Matrix4::CreateRotate(angle, dx, dy, dz);
    if (!NearZero(centerX) || !NearZero(centerY)) {
        auto translate1 = Matrix4::CreateTranslate(centerX, centerY, 0.0);
        auto translate2 = Matrix4::CreateTranslate(-centerX, -centerY, 0.0);
        rotateMatrix = rotateMatrix * translate2;
        rotateMatrix = translate1 * rotateMatrix;
    }
    matrix = rotateMatrix * matrix;
    matrixArray = ConvertToJSValue(ctx, matrix);
    JS_SetPropertyStr(ctx, value, MATRIX_4X4, matrixArray);
    JS_DupValue(ctx, value);
    return value;
}

JSValue TransformPoint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    JSValue result = JS_NewArray(ctx);

    if (argc != 1 || argv == nullptr) {
        LOGE("TransformPoint failed, argc != 1 or argv == nullptr");
        return JS_NULL;
    }
    if (!JS_IsArray(ctx, argv[0]) || QJSUtils::JsGetArrayLength(ctx, argv[0]) != 2) {
        LOGE("TransformPoint failed, argv[0] is not array or length != 2");
        return JS_NULL;
    }
    auto matrix = ConvertToMatrix(ctx, JS_GetPropertyStr(ctx, value, MATRIX_4X4));

    auto pointXJSValue = JS_GetPropertyUint32(ctx, argv[0], 0);
    double pointX = ConvertToDouble(ctx, pointXJSValue, 0.0);

    auto pointYJSValue = JS_GetPropertyUint32(ctx, argv[0], 1);
    double pointY = ConvertToDouble(ctx, pointYJSValue, 0.0);

    Point point { pointX, pointY };
    Point target = matrix * point;
    JS_SetPropertyUint32(ctx, result, 0, JS_NewFloat64(ctx, target.GetX()));
    JS_SetPropertyUint32(ctx, result, 1, JS_NewFloat64(ctx, target.GetY()));
    return result;
}

void InitMatrix4Module(JSContext* ctx, JSValue& moduleObj)
{
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_4X4, ConvertToJSValue(ctx, Matrix4::CreateIdentity()));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_INIT, JS_NewCFunction(ctx, Init, MATRIX_INIT, 1));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_IDENTITY, JS_NewCFunction(ctx, Identity, MATRIX_IDENTITY, 0));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_COPY, JS_NewCFunction(ctx, Copy, MATRIX_COPY, 0));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_COMBINE, JS_NewCFunction(ctx, Combine, MATRIX_COMBINE, 1));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_INVERT, JS_NewCFunction(ctx, Invert, MATRIX_INVERT, 0));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_TRANSLATE, JS_NewCFunction(ctx, Translate, MATRIX_TRANSLATE, 1));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_SCALE, JS_NewCFunction(ctx, Scale, MATRIX_SCALE, 1));
    JS_SetPropertyStr(ctx, moduleObj, MATRIX_ROTATE, JS_NewCFunction(ctx, Rotate, MATRIX_ROTATE, 1));
    JS_SetPropertyStr(
        ctx, moduleObj, MATRIX_TRANSFORM_POINT, JS_NewCFunction(ctx, TransformPoint, MATRIX_TRANSFORM_POINT, 1));
}

} // namespace OHOS::Ace::Framework