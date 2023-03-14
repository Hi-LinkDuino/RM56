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

#ifndef FREEZE_DB_HELPER_H
#define FREEZE_DB_HELPER_H

#include <list>
#include <string>

#include "watch_point.h"

namespace OHOS {
namespace HiviewDFX {
class DBHelper {
public:
    static void SelectEventFromDB(bool all, unsigned long long start, unsigned long long end, std::list<WatchPoint>& list);
    static void UpdateEventIntoDB(const WatchPoint& watchPoint, int id);
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
