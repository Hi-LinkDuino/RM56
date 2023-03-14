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
#include <uv.h>

#include "bundle_status_callback.h"

#include "napi/native_common.h"

BundleStatusCallback::BundleStatusCallback(napi_env env, napi_ref addedCallback,
                                           napi_ref updatedCallback,
                                           napi_ref removeCallback)
    : env_(env), addedCallback_(addedCallback),
      updatedCallback_(updatedCallback), removeCallback_(removeCallback) {}

BundleStatusCallback::~BundleStatusCallback()
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    uv_work_t* work = new uv_work_t;
    DelRefCallbackInfo* delRefCallbackInfo = new DelRefCallbackInfo {
        .env_ = env_,
        .addedCallback_ = addedCallback_,
        .updatedCallback_ = updatedCallback_,
        .removeCallback_ = removeCallback_,
    };
    if (work == nullptr || delRefCallbackInfo == nullptr) {
        return;
    }
    work->data = (void*)delRefCallbackInfo;
    int ret = uv_queue_work(
        loop, work, [](uv_work_t* work) {},
        [](uv_work_t* work, int status) {
            // JS Thread
            DelRefCallbackInfo* delRefCallbackInfo =  reinterpret_cast<DelRefCallbackInfo*>(work->data);
            napi_delete_reference(delRefCallbackInfo->env_, delRefCallbackInfo->addedCallback_);
            napi_delete_reference(delRefCallbackInfo->env_, delRefCallbackInfo->updatedCallback_);
            napi_delete_reference(delRefCallbackInfo->env_, delRefCallbackInfo->removeCallback_);
            if (delRefCallbackInfo != nullptr) {
                delete delRefCallbackInfo;
                delRefCallbackInfo = nullptr;
            }
            if (work != nullptr) {
                delete work;
                work = nullptr;
            }
        });
    if (ret != 0) {
        if (delRefCallbackInfo != nullptr) {
            delete delRefCallbackInfo;
        }
        if (work != nullptr) {
            delete work;
        }
    }
}

void BundleStatusCallback::OnBundleAdded(const std::string& bundleName, const int userId)
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    uv_work_t* work = new uv_work_t;
    AsyncCallbackInfo* asyncCallbackInfo = new AsyncCallbackInfo {
        .env_ = env_,
        .callback_ = addedCallback_,
        .bundleName_ = bundleName,
        .userId_ = userId,
    };
    if (work == nullptr || asyncCallbackInfo == nullptr) {
        return;
    }
    work->data = (void*)asyncCallbackInfo;
    int ret = uv_queue_work(
        loop, work, [](uv_work_t* work) {},
        [](uv_work_t* work, int status) {
            // JS Thread
            AsyncCallbackInfo* asyncCallbackInfo =  reinterpret_cast<AsyncCallbackInfo*>(work->data);
            napi_value callback = nullptr;
            napi_value placeHolder = nullptr;
            napi_value result[2] = { 0 };
            napi_get_reference_value(asyncCallbackInfo->env_, asyncCallbackInfo->callback_, &callback);
            napi_create_string_utf8(
                asyncCallbackInfo->env_, asyncCallbackInfo->bundleName_.c_str(), NAPI_AUTO_LENGTH, &result[0]);
            napi_create_uint32(asyncCallbackInfo->env_, asyncCallbackInfo->userId_, &result[1]);
            napi_call_function(
                asyncCallbackInfo->env_, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder);
            if (asyncCallbackInfo != nullptr) {
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
            if (work != nullptr) {
                delete work;
                work = nullptr;
            }
        });
    if (ret != 0) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
        }
        if (work != nullptr) {
            delete work;
        }
    }
}

void BundleStatusCallback::OnBundleUpdated(const std::string& bundleName, const int userId)
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    uv_work_t* work = new uv_work_t;
    AsyncCallbackInfo* asyncCallbackInfo = new AsyncCallbackInfo {
        .env_ = env_,
        .callback_ = updatedCallback_,
        .bundleName_ = bundleName,
        .userId_ = userId,
    };
    if (work == nullptr || asyncCallbackInfo == nullptr) {
        return;
    }
    work->data = (void*)asyncCallbackInfo;
    int ret = uv_queue_work(
        loop, work, [](uv_work_t* work) {},
        [](uv_work_t* work, int status) {
            // JS Thread
            AsyncCallbackInfo* asyncCallbackInfo = reinterpret_cast<AsyncCallbackInfo*>(work->data);
            napi_value callback = nullptr;
            napi_value placeHolder = nullptr;
            napi_value result[2] = { 0 };
            napi_get_reference_value(asyncCallbackInfo->env_, asyncCallbackInfo->callback_, &callback);
            napi_create_string_utf8(
                asyncCallbackInfo->env_, asyncCallbackInfo->bundleName_.c_str(), NAPI_AUTO_LENGTH, &result[0]);
            napi_create_uint32(asyncCallbackInfo->env_, asyncCallbackInfo->userId_, &result[1]);
            napi_call_function(
                asyncCallbackInfo->env_, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder);
            if (asyncCallbackInfo != nullptr) {
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
            if (work != nullptr) {
                delete work;
                work = nullptr;
            }
        });
    if (ret != 0) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
        }
        if (work != nullptr) {
            delete work;
        }
    }
}

void BundleStatusCallback::OnBundleRemoved(const std::string& bundleName, const int userId)
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    uv_work_t* work = new uv_work_t;
    AsyncCallbackInfo* asyncCallbackInfo = new AsyncCallbackInfo {
        .env_ = env_,
        .callback_ = removeCallback_,
        .bundleName_ = bundleName,
        .userId_ = userId,
    };
    if (work == nullptr || asyncCallbackInfo == nullptr) {
        return;
    }
    work->data = (void*)asyncCallbackInfo;
    int ret = uv_queue_work(
        loop, work, [](uv_work_t* work) {},
        [](uv_work_t* work, int status) {
            // JS Thread
            AsyncCallbackInfo* asyncCallbackInfo =  reinterpret_cast<AsyncCallbackInfo*>(work->data);
            napi_value callback = nullptr;
            napi_value placeHolder = nullptr;
            napi_value result[2] = { 0 };
            napi_get_reference_value(asyncCallbackInfo->env_, asyncCallbackInfo->callback_, &callback);
            napi_create_string_utf8(
                asyncCallbackInfo->env_, asyncCallbackInfo->bundleName_.c_str(), NAPI_AUTO_LENGTH, &result[0]);
            napi_create_uint32(asyncCallbackInfo->env_, asyncCallbackInfo->userId_, &result[1]);
            napi_call_function(
                asyncCallbackInfo->env_, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder);
            if (asyncCallbackInfo != nullptr) {
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
            if (work != nullptr) {
                delete work;
                work = nullptr;
            }
        });
    if (ret != 0) {
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
        }
        if (work != nullptr) {
            delete work;
        }
    }
}