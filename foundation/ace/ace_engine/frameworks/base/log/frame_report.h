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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_FRAME_REPORT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_FRAME_REPORT_H

#include <string>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {
using FrameInitFunc = void (*)();
using FrameGetEnableFunc = int (*)();
using BeginFlushAnimationFunc = void(*)();
using EndFlushAnimationFunc = void(*)();
using BeginFlushBuildFunc = void(*)();
using EndFlushBuildFunc = void(*)();
using BeginFlushLayoutFunc = void(*)();
using EndFlushLayoutFunc = void(*)();
using BeginFlushRenderFunc = void(*)();
using EndFlushRenderFunc = void(*)();
using BeginFlushRenderFinishFunc = void(*)();
using EndFlushRenderFinishFunc = void(*)();
using BeginProcessPostFlushFunc = void(*)();
using BeginListFlingFunc = void(*)();
using EndListFlingFunc = void(*)();

class ACE_EXPORT FrameReport final {
public:
    static FrameReport& GetInstance();
    void Init();
    int GetEnable();
    void BeginFlushAnimation();
    void EndFlushAnimation();

    void BeginFlushBuild();
    void EndFlushBuild();
    void BeginFlushLayout();
    void EndFlushLayout();
    void BeginFlushRender();
    void EndFlushRender();
    void BeginFlushRenderFinish();
    void EndFlushRenderFinish();
    void BeginProcessPostFlush();
    void BeginListFling();
    void EndListFling();

private:
    FrameReport();
    ~FrameReport();
    bool LoadLibrary();
    void CloseLibrary();
    void *LoadSymbol(const char *symName);
    void *frameSchedHandle_ = nullptr;
    bool frameSchedSoLoaded_ = false;

    ACE_EXPORT FrameInitFunc frameInitFunc_ = nullptr;
    ACE_EXPORT FrameGetEnableFunc frameGetEnableFunc_ = nullptr;
    ACE_EXPORT BeginFlushAnimationFunc beginFlushAnimationFunc_ = nullptr;
    ACE_EXPORT EndFlushAnimationFunc endFlushAnimationFunc_ = nullptr;
    ACE_EXPORT BeginFlushBuildFunc beginFlushBuildFunc_ = nullptr;
    ACE_EXPORT EndFlushBuildFunc endFlushBuildFunc_ = nullptr;
    ACE_EXPORT BeginFlushLayoutFunc beginFlushLayoutFunc_ = nullptr;
    ACE_EXPORT EndFlushLayoutFunc endFlushLayoutFunc_ = nullptr;
    ACE_EXPORT BeginFlushRenderFunc beginFlushRenderFunc_ = nullptr;
    ACE_EXPORT EndFlushRenderFunc endFlushRenderFunc_ = nullptr;
    ACE_EXPORT BeginFlushRenderFinishFunc beginFlushRenderFinishFunc_ = nullptr;
    ACE_EXPORT EndFlushRenderFinishFunc endFlushRenderFinishFunc_ = nullptr;
    ACE_EXPORT BeginProcessPostFlushFunc beginProcessPostFunc_ = nullptr;
    ACE_EXPORT BeginListFlingFunc beginListFlingFunc_ = nullptr;
    ACE_EXPORT EndListFlingFunc endListFlingFunc_ = nullptr;
};
}
#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_FRAME_REPORT_H
