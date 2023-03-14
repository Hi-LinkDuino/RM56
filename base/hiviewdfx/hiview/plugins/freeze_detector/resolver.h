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

#ifndef FREEZE_RESOLVER_H
#define FREEZE_RESOLVER_H

#include <list>
#include <memory>
#include <set>
#include <vector>

#include "pipeline.h"
#include "rule_cluster.h"
#include "singleton.h"
#include "watch_point.h"

namespace OHOS {
namespace HiviewDFX {
class FreezeResolver : public Singleton<FreezeResolver> {
    DECLARE_SINGLETON(FreezeResolver);

public:
    static const int MILLISECOND = 1000;

    bool Init();
    std::string GetTimeZone() const;
    std::shared_ptr<PipelineEvent> ProcessEvent(WatchPoint &watchPoint) const;

private:
    static const inline std::string HEADER = "*******************************************";
    static const int DEFAULT_TIME_WINDOW = 30;
    static const int MINUTES_IN_HOUR = 60;

    bool ResolveEvent(WatchPoint& watchPoint, WatchPoint& matchedWatchPoint,
        std::list<WatchPoint>& list, FreezeResult& result) const;

    unsigned long startTime_;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif // FREEZE_DETECTOR_RESOLVER_H
