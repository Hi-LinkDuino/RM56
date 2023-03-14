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

#include "base/log/frame_report.h"

#include <cstdio>
#include <dlfcn.h>
#include <unistd.h>

#include "base/log/log_wrapper.h"

namespace OHOS::Ace {
namespace {
    const std::string FRAME_AWARE_SO_PATH = "/system/lib/libframe_ui_intf.z.so";
}
FrameReport& FrameReport::GetInstance()
{
    static FrameReport instance;
    return instance;
}

FrameReport::FrameReport() {}

FrameReport::~FrameReport()
{
    CloseLibrary();
}

bool FrameReport::LoadLibrary()
{
    if (!frameSchedSoLoaded_) {
        frameSchedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
        if (frameSchedHandle_ == nullptr) {
            LOGE("frame-ace:[LoadLibrary]dlopen libframe_ui_intf.so failed! error = %s\n", dlerror());
            return false;
        }
        frameSchedSoLoaded_ = true;
    }
    LOGI("frame-ace:[LoadLibrary]dlopen libframe_ui_intf.so success");
    return true;
}

void FrameReport::CloseLibrary()
{
    if (dlclose(frameSchedHandle_) != 0) {
        LOGE("frame-ace:[CloseLibrary]libframe_ui_intf.so failed!\n");
        return;
    }
    frameSchedHandle_ = nullptr;
    frameSchedSoLoaded_ = false;
    LOGI("frame-ace:[CloseLibrary]libframe_ui_intf.so close success!\n");
}

void *FrameReport::LoadSymbol(const char *symName)
{
    if (!frameSchedSoLoaded_) {
        LOGE("frame-ace:[loadSymbol]libframe_ui_intf.so not loaded.\n");
        return nullptr;
    }

    void *funcSym = dlsym(frameSchedHandle_, symName);
    if (funcSym == nullptr) {
        LOGE("frame-ace:[loadSymbol]Get %{public}s symbol failed: %{public}s\n", symName, dlerror());
        return nullptr;
    }
    return funcSym;
}

void FrameReport::Init()
{
    LoadLibrary();
    frameInitFunc_ = (FrameInitFunc)LoadSymbol("Init");
    if (frameInitFunc_ != nullptr) {
        frameInitFunc_();
    } else {
        LOGE("frame-ace:[Init]load init function failed!");
    }
}

int FrameReport::GetEnable()
{
    if (!frameSchedSoLoaded_) {
        return 0;
    }
    frameGetEnableFunc_ = (FrameGetEnableFunc)LoadSymbol("GetSenseSchedEnable");
    if (frameGetEnableFunc_ != nullptr) {
        return frameGetEnableFunc_();
    } else {
        LOGE("frame-ace:[GetEnable]load GetSenseSchedEnable function failed!");
        return 0;
    }
}

void FrameReport::BeginFlushAnimation()
{
    beginFlushAnimationFunc_ = (BeginFlushAnimationFunc)LoadSymbol("BeginFlushAnimation");
    if (beginFlushAnimationFunc_ != nullptr) {
        beginFlushAnimationFunc_();
    } else {
        LOGE("frame-ace:[BeginFlushAnimation]load BeginFlushAnimation function failed!");
    }
}

void FrameReport::EndFlushAnimation()
{
    endFlushAnimationFunc_ = (EndFlushAnimationFunc)LoadSymbol("EndFlushAnimation");
    if (endFlushAnimationFunc_ != nullptr) {
        endFlushAnimationFunc_();
    } else {
        LOGE("frame-ace:[EndFlushAnimation]load EndFlushAnimation function failed!");
    }
}

void FrameReport::BeginFlushBuild()
{
    beginFlushBuildFunc_ = (BeginFlushBuildFunc)LoadSymbol("BeginFlushBuild");
    if (beginFlushBuildFunc_ != nullptr) {
        beginFlushBuildFunc_();
    } else {
        LOGE("frame-ace:[BeginFlushBuild]load BeginFlushBuild function failed!");
    }
}

void FrameReport::EndFlushBuild()
{
    endFlushBuildFunc_ = (EndFlushBuildFunc)LoadSymbol("EndFlushBuild");
    if (endFlushBuildFunc_ != nullptr) {
        endFlushBuildFunc_();
    } else {
        LOGE("frame-ace:[EndFlushBuild]load EndFlushBuild function failed!");
    }
}

void FrameReport::BeginFlushLayout()
{
    beginFlushLayoutFunc_ = (BeginFlushLayoutFunc)LoadSymbol("BeginFlushLayout");
    if (beginFlushLayoutFunc_ != nullptr) {
        beginFlushLayoutFunc_();
    } else {
        LOGE("frame-ace:[BeginFlushLayout]load BeginFlushLayout function failed!");
    }
}

void FrameReport::EndFlushLayout()
{
    endFlushLayoutFunc_ = (EndFlushLayoutFunc)LoadSymbol("EndFlushLayout");
    if (endFlushLayoutFunc_ != nullptr) {
        endFlushLayoutFunc_();
    } else {
        LOGE("frame-ace:[EndFlushLayout]load EndFlushLayout function failed!");
    }
}

void FrameReport::BeginFlushRender()
{
    beginFlushRenderFunc_ = (BeginFlushRenderFunc)LoadSymbol("BeginFlushRender");
    if (beginFlushRenderFunc_ != nullptr) {
        beginFlushRenderFunc_();
    } else {
        LOGE("frame-ace:[BeginFlushRender]load BeginFlushRender function failed!");
    }
}

void FrameReport::EndFlushRender()
{
    endFlushRenderFunc_ = (EndFlushRenderFunc)LoadSymbol("EndFlushRender");
    if (endFlushRenderFunc_ != nullptr) {
        endFlushRenderFunc_();
    } else {
        LOGE("frame-ace:[EndFlushRender]load EndFlushRender function failed!");
    }
}

void FrameReport::BeginFlushRenderFinish()
{
    beginFlushRenderFinishFunc_ = (BeginFlushRenderFinishFunc)LoadSymbol("BeginFlushRenderFinish");
    if (beginFlushRenderFinishFunc_ != nullptr) {
        beginFlushRenderFinishFunc_();
    } else {
        LOGE("frame-ace:[BeginFlushRenderFinish]load EndFlushRenderFinish function failed!");
    }
}

void FrameReport::EndFlushRenderFinish()
{
    endFlushRenderFinishFunc_ = (EndFlushRenderFinishFunc)LoadSymbol("EndFlushRenderFinish");
    if (endFlushRenderFinishFunc_ != nullptr) {
        endFlushRenderFinishFunc_();
    } else {
        LOGE("frame-ace:[EndFlushRenderFinish]load EndFlushRenderFinish function failed!");
    }
}

void FrameReport::BeginProcessPostFlush()
{
    beginProcessPostFunc_ = (BeginProcessPostFlushFunc)LoadSymbol("BeginProcessPostFlush");
    if (beginProcessPostFunc_ != nullptr) {
        beginProcessPostFunc_();
    } else {
        LOGE("frame-ace:[BeginProcessPostFlush]load BeginProcessPostFlush function failed!");
    }
}

void FrameReport::BeginListFling()
{
    beginListFlingFunc_ = (BeginListFlingFunc)LoadSymbol("BeginListFling");
    if (beginListFlingFunc_ != nullptr) {
        beginListFlingFunc_();
    } else {
        LOGE("frame-ace:[BeginListFling]load BeginListFling function failed!");
    }
}

void FrameReport::EndListFling()
{
    endListFlingFunc_ = (EndListFlingFunc)LoadSymbol("EndListFling");
    if (endListFlingFunc_ != nullptr) {
        endListFlingFunc_();
    } else {
        LOGE("frame-ace:[EndListFling]load EndListFling function failed!");
    }
}
} // namespace OHOS::Ace
