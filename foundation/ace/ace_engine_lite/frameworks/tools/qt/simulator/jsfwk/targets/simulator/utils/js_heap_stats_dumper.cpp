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

#include "js_heap_stats_dumper.h"
#include <QDir>
#include "ace_log.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
JSHeapStatsDumper::JSHeapStatsDumper()
    : stats_(), writer_(QDir::currentPath() + "/dump_heap.txt")
{
    if (memset_s(&stats_, sizeof(stats_), 0, sizeof(stats_)) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to memset.");
    }
}

JSHeapStatsDumper::~JSHeapStatsDumper() {}

void JSHeapStatsDumper::Dump()
{
    JSHeapStatus stats;
    if (!JSI::GetJSHeapStatus(stats)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to get JS heap stats.");
        return;
    }
    if (HasChanged(stats)) {
        HandleChange(stats);
    }
}

bool JSHeapStatsDumper::HasChanged(const JSHeapStatus &stats) const
{
    return stats.allocBytes != stats_.allocBytes || stats.peakAllocBytes != stats_.peakAllocBytes;
}

void JSHeapStatsDumper::HandleChange(const JSHeapStatus &stats)
{
    UpdateStats(stats);
    LogStats(stats);
    SaveStats(stats);
}

void JSHeapStatsDumper::UpdateStats(const JSHeapStatus &stats)
{
    stats_.allocBytes = stats.allocBytes;
    stats_.peakAllocBytes = stats.peakAllocBytes;
    stats_.totalBytes = stats.totalBytes;
}

void JSHeapStatsDumper::LogStats(const JSHeapStatus &stats) const
{
    HILOG_DEBUG(HILOG_MODULE_ACE,
                "Current Allocated Bytes: %{public}d, Peak Allocated Bytes: %{public}d, Total Bytes: %{public}d",
                stats.allocBytes, stats.peakAllocBytes, stats.totalBytes);
}

void JSHeapStatsDumper::SaveStats(const JSHeapStatus &stats)
{
    char buffer[255] = {0};
    if (sprintf_s(buffer, sizeof(buffer), "%d/%d ", stats.allocBytes, stats.peakAllocBytes) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to format dump message.");
        return;
    }
    writer_.Write(buffer);
}
} // namespace ACELite
} // namespace OHOS
