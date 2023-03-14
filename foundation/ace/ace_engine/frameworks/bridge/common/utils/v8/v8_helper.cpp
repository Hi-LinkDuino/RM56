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

#include "frameworks/bridge/common/utils/v8/v8_helper.h"

extern const char _binary_v8_startup_snapshot_bin_start[];
extern const char _binary_v8_startup_snapshot_bin_end[];

namespace OHOS::Ace::Framework {

V8Helper::V8Helper()
{
    static v8::StartupData snapshotBlob = {
        .data = _binary_v8_startup_snapshot_bin_start,
        .raw_size = _binary_v8_startup_snapshot_bin_end - _binary_v8_startup_snapshot_bin_start,
    };
    v8::V8::SetSnapshotDataBlob(&snapshotBlob);
    platform_ = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform_.get());
    v8::V8::Initialize();
}

V8Helper::~V8Helper()
{
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
}

std::unique_ptr<v8::Platform>& V8Helper::GetPlatform()
{
    static V8Helper v8Helper;
    return v8Helper.platform_;
}

} // namespace OHOS::Ace::Framework
