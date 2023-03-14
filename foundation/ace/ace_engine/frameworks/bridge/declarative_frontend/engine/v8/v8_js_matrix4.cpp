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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_js_matrix4.h"

#include <utility>

#include "frameworks/base/geometry/matrix4.h"
#include "frameworks/base/json/json_util.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"

namespace OHOS::Ace::Framework {
namespace {

Matrix4 ArrayToMatrix4(const v8::Local<v8::Array>& array, v8::Local<v8::Context> context)
{
    Matrix4 ret = Matrix4::CreateIdentity();
    if (array->Length() != Matrix4::DIMENSION * Matrix4::DIMENSION) {
        LOGE("ArrayToMatrix4 array length != 16");
        return ret;
    }

    // in column order
    for (int32_t i = 0; i < Matrix4::DIMENSION; i++) {
        for (int32_t j = 0; j < Matrix4::DIMENSION; j++) {
            auto num =
                array->Get(context, i * Matrix4::DIMENSION + j).ToLocalChecked()->ToNumber(context).ToLocalChecked();
            ret.Set(j, i, static_cast<float>(num->Value()));
        }
    }
    return ret;
}

void Matrix4ToArray(const Matrix4& matrix, v8::Isolate* isolate, v8::Local<v8::Array>& out)
{
    auto context = isolate->GetCurrentContext();
    for (int32_t i = 0; i < Matrix4::DIMENSION; i++) {
        for (int32_t j = 0; j < Matrix4::DIMENSION; j++) {
            // output in column order
            auto success = out->Set(context, i * Matrix4::DIMENSION + j, v8::Number::New(isolate, matrix.Get(j, i)));
            if (!success.ToChecked()) {
                LOGE("Matrix4ToArray Set failed! %{public}d, %{public}d", i, j);
            }
        }
    }
}

} // namespace

void JSMatrix4::CreateMatrix4Object(v8::Local<v8::Object> moduleObj, v8::Isolate* isolate)
{
    if (moduleObj.IsEmpty() || isolate == nullptr) {
        return;
    }

    auto context = isolate->GetCurrentContext();

    Matrix4 matrix;
    v8::Local<v8::Array> arr = v8::Array::New(isolate);
    Matrix4ToArray(matrix, isolate, arr);

    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), arr).ToChecked();

    using FunctionType = std::pair<std::string, void (*)(const v8::FunctionCallbackInfo<v8::Value>&)>;
    static FunctionType functions[] = {
        { MATRIX_INIT, &JSMatrix4::Init },
        { MATRIX_IDENTITY, &JSMatrix4::Identity },
        { MATRIX_COPY, &JSMatrix4::Copy },
        { MATRIX_COMBINE, &JSMatrix4::Combine },
        { MATRIX_INVERT, &JSMatrix4::Invert },
        { MATRIX_TRANSLATE, &JSMatrix4::Translate },
        { MATRIX_SCALE, &JSMatrix4::Scale },
        { MATRIX_ROTATE, &JSMatrix4::Rotate },
        { MATRIX_TRANSFORM_POINT, &JSMatrix4::TransformPoint },
    };
    for (const auto& func : functions) {
        moduleObj
            ->Set(context, v8::String::NewFromUtf8(isolate, func.first.c_str()).ToLocalChecked(),
                  v8::Function::New(context, func.second).ToLocalChecked())
            .ToChecked();
    }
}

void JSMatrix4::Init(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Init");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();

    auto copy = v8::Object::New(isolate);
    CreateMatrix4Object(copy, isolate);

    if (info.Length() == 1 && info[0]->IsArray()) {
        v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(info[0]);
        if (array->Length() == Matrix4::DIMENSION * Matrix4::DIMENSION) {
            auto success =
                copy->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), array);
            if (!success.ToChecked()) {
                LOGE("JSMatrix4::Init Set failed!");
            }
        }
    }
    info.GetReturnValue().Set(copy);
}

void JSMatrix4::Identity(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Identity");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);

    auto context = isolate->GetCurrentContext();
    auto copy = v8::Object::New(isolate);
    CreateMatrix4Object(copy, isolate);
    v8::Local<v8::Array> array = v8::Array::New(isolate);
    Matrix4 matrix = Matrix4::CreateIdentity();
    Matrix4ToArray(matrix, isolate, array);
    auto success = copy->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), array);
    if (!success.ToChecked()) {
        LOGE("JSMatrix4::Identity Set failed!");
    }

    info.GetReturnValue().Set(copy);
}

void JSMatrix4::Copy(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Copy");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();

    auto copy = v8::Object::New(isolate);
    CreateMatrix4Object(copy, isolate);
    auto object = info.This()->Get(context,
                                   v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked()).ToLocalChecked();
    if (object->IsArray()) {
        auto success = copy->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), object);
        if (!success.ToChecked()) {
            LOGE("JSMatrix4::Copy failed!");
        }
    }

    info.GetReturnValue().Set(copy);
}

void JSMatrix4::Combine(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Combine");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();

    if (info.Length() == 1 && info[0]->IsObject()) {
        auto arg = v8::Local<v8::Object>::Cast(info[0]);
        auto objA =
            arg->Get(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked()).ToLocalChecked();
        auto objB = info.This()
                        ->Get(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked())
                        .ToLocalChecked();
        auto matrixA = ArrayToMatrix4(v8::Local<v8::Array>::Cast(objA), context);
        auto matrixB = ArrayToMatrix4(v8::Local<v8::Array>::Cast(objB), context);

        v8::Local<v8::Array> arr = v8::Array::New(isolate);
        Matrix4ToArray(matrixA * matrixB, isolate, arr);
        auto success = info.This()->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), arr);
        if (!success.ToChecked()) {
            LOGE("JSMatrix4::Combine set failed");
        }
    }
    info.GetReturnValue().Set(info.This());
}

void JSMatrix4::Invert(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Invert");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();

    auto obj =
        info.This()->Get(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked()).ToLocalChecked();
    if (obj->IsArray()) {
        auto array = v8::Local<v8::Array>::Cast(obj);
        auto m = ArrayToMatrix4(array, context);
        m = Matrix4::Invert(m);
        v8::Local<v8::Array> arr = v8::Array::New(isolate);
        Matrix4ToArray(m, isolate, arr);
        auto success = info.This()->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), arr);
        if (!success.ToChecked()) {
            LOGE("JSMatrix4::Invert set failed");
        }
    }
    return info.GetReturnValue().Set(info.This());
}

void JSMatrix4::Translate(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Translate");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();

    if (info.Length() == 1 && info[0]->IsObject()) {
        auto args = JsonUtil::ParseJsonString(
            V8Utils::ScopedString(v8::JSON::Stringify(context, info[0]).ToLocalChecked()).get());
        if (args) {
            auto dx = args->GetDouble("x", 0.0);
            auto dy = args->GetDouble("y", 0.0);
            auto dz = args->GetDouble("z", 0.0);

            Matrix4 translateMatrix = Matrix4::CreateTranslate(dx, dy, dz);
            auto obj =
                info.This()->Get(context,
                                 v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked()).ToLocalChecked();
            if (obj->IsArray()) {
                auto array = v8::Local<v8::Array>::Cast(obj);
                Matrix4 matrix = ArrayToMatrix4(array, context);
                matrix = translateMatrix * matrix;
                v8::Local<v8::Array> arr = v8::Array::New(isolate);
                Matrix4ToArray(matrix, isolate, arr);
                auto success =
                    info.This()->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), arr);
                if (!success.ToChecked()) {
                    LOGE("JSMatrix4::Translate set failed");
                }
            }
        }
    }
    info.GetReturnValue().Set(info.This());
}

void JSMatrix4::Scale(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Scale");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();

    if (info.Length() == 1 && info[0]->IsObject()) {
        auto args = JsonUtil::ParseJsonString(
            V8Utils::ScopedString(v8::JSON::Stringify(context, info[0]).ToLocalChecked()).get());
        if (args) {
            auto dx = args->GetDouble("x", 1.0);
            auto dy = args->GetDouble("y", 1.0);
            auto dz = args->GetDouble("z", 1.0);
            auto centerX = args->GetDouble("centerX", 0.0);
            auto centerY = args->GetDouble("centerY", 0.0);
            auto obj =
                info.This()->Get(context,
                                 v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked()).ToLocalChecked();
            if (obj->IsArray()) {
                auto array = v8::Local<v8::Array>::Cast(obj);
                auto matrix = ArrayToMatrix4(array, context);
                auto scaleMatrix = Matrix4::CreateScale(dx, dy, dz) * matrix;

                if (!NearZero(centerX) || !NearZero(centerY)) {
                    auto translate1 = Matrix4::CreateTranslate(centerX, centerY, 0.0);
                    auto translate2 = Matrix4::CreateTranslate(-centerX, -centerY, 0.0);
                    scaleMatrix = scaleMatrix * translate2;
                    scaleMatrix = translate1 * scaleMatrix;
                }
                matrix = scaleMatrix * matrix;
                v8::Local<v8::Array> arr = v8::Array::New(isolate);
                Matrix4ToArray(matrix, isolate, arr);

                auto success =
                    info.This()->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), arr);
                if (!success.ToChecked()) {
                    LOGE("JSMatrix4::Translate set failed");
                }
            }
        }
    }
    info.GetReturnValue().Set(info.This());
}

void JSMatrix4::Rotate(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::Rotate");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();

    if (info.Length() == 1 && info[0]->IsObject()) {
        auto args = JsonUtil::ParseJsonString(
            V8Utils::ScopedString(v8::JSON::Stringify(context, info[0]).ToLocalChecked()).get());
        if (args) {
            auto dx = args->GetDouble("x", 0.0);
            auto dy = args->GetDouble("y", 0.0);
            auto dz = args->GetDouble("z", 0.0);
            auto angle = args->GetDouble("angle", 0.0);
            auto centerX = args->GetDouble("centerX", 0.0);
            auto centerY = args->GetDouble("centerY", 0.0);
            auto obj =
                info.This()->Get(context,
                                 v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked()).ToLocalChecked();
            if (obj->IsArray()) {
                auto array = v8::Local<v8::Array>::Cast(obj);
                auto matrix = ArrayToMatrix4(array, context);
                auto rotateMatrix = Matrix4::CreateRotate(angle, dx, dy, dz) * matrix;

                if (!NearZero(centerX) || !NearZero(centerY)) {
                    auto translate1 = Matrix4::CreateTranslate(centerX, centerY, 0.0);
                    auto translate2 = Matrix4::CreateTranslate(-centerX, -centerY, 0.0);
                    rotateMatrix = rotateMatrix * translate2;
                    rotateMatrix = translate1 * rotateMatrix;
                }
                matrix = rotateMatrix * matrix;
                v8::Local<v8::Array> arr = v8::Array::New(isolate);
                Matrix4ToArray(matrix, isolate, arr);
                auto success =
                    info.This()->Set(context, v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked(), arr);
                if (!success.ToChecked()) {
                    LOGE("JSMatrix4::Translate set failed");
                }
            }
        }
    }
    info.GetReturnValue().Set(info.This());
}

void JSMatrix4::TransformPoint(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    LOGD("JSMatrix4::TransformPoint");
    auto isolate = info.GetIsolate();
    v8::HandleScope scp(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Array> arr = v8::Array::New(isolate);

    if (info.Length() == 1 && info[0]->IsArray()) {
        auto args = v8::Local<v8::Array>::Cast(info[0]);
        if (args->Length() == 2) {
            auto x = args->Get(context, 0).ToLocalChecked()->ToNumber(context).ToLocalChecked();
            auto y = args->Get(context, 1).ToLocalChecked()->ToNumber(context).ToLocalChecked();

            Point p { x->Value(), y->Value() };

            auto obj =
                info.This()->Get(context,
                                 v8::String::NewFromUtf8(isolate, MATRIX_4X4).ToLocalChecked()).ToLocalChecked();
            if (obj->IsArray()) {
                auto array = v8::Local<v8::Array>::Cast(obj);
                auto matrix = ArrayToMatrix4(array, context);

                Point target = matrix * p;
                auto success = arr->Set(context, 0, v8::Number::New(isolate, target.GetX()));
                if (!success.ToChecked()) {
                    LOGE("TransformPoint failed!");
                }
                success = arr->Set(context, 1, v8::Number::New(isolate, target.GetY()));
                if (!success.ToChecked()) {
                    LOGE("TransformPoint failed!");
                }
            }
        }
    }
    info.GetReturnValue().Set(arr);
}

} // namespace OHOS::Ace::Framework
