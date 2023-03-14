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

#include "core/common/hdc_register.h"

#include <dlfcn.h>
#include <unistd.h>

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "core/common/ace_application_info.h"

namespace OHOS::Ace {

namespace {

using StartRegister = void (*)(const std::string& pkgName);
using StopRegister = void (*)();

} // namespace

HdcRegister::HdcRegister(): registerHandler_(nullptr)
{
    isDebugVersion_ = AceApplicationInfo::GetInstance().IsDebugVersion();
    if (!isDebugVersion_) {
        return;
    }
    pkgName_ = AceApplicationInfo::GetInstance().GetPackageName();
    LoadRegisterSo();
}

HdcRegister::~HdcRegister()
{
    StopHdcRegister();
}

HdcRegister& HdcRegister::Get()
{
    static HdcRegister hdcRegister;
    return hdcRegister;
}

void HdcRegister::LoadRegisterSo()
{
    LOGI("LoadRegisterSo");
    registerHandler_ = dlopen("libhdc_register.z.so", RTLD_LAZY);
    if (registerHandler_ == nullptr) {
        LOGE("cannot find register so");
    }
}

void HdcRegister::StartHdcRegister()
{
    LOGI("Start Hdc Register");
    if (registerHandler_ == nullptr) {
        LOGE("registerHandler_ is null");
        return;
    }
    StartRegister startRegister = (StartRegister)dlsym(registerHandler_, "StartConnect");
    if (startRegister == nullptr) {
        LOGE("startRegister = NULL, dlerror = %s", dlerror());
        return;
    }
    startRegister(pkgName_);
}

void HdcRegister::StopHdcRegister()
{
    if (!isDebugVersion_) {
        return;
    }
    LOGI("Stop Hdc Register");
    if (registerHandler_ == nullptr) {
        LOGE("registerHandler_ is null");
        return;
    }
    StopRegister stopRegister = (StopRegister)dlsym(registerHandler_, "StopConnect");
    if (stopRegister == nullptr) {
        LOGE("stopRegister = NULL, dlerror = %s", dlerror());
        return;
    }
    stopRegister();
    dlclose(registerHandler_);
    registerHandler_ = nullptr;
}

} // namespace OHOS::Ace
