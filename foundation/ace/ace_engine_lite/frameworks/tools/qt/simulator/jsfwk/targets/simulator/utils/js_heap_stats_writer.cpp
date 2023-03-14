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

#include "js_heap_stats_writer.h"
#include "ace_log.h"
namespace OHOS {
namespace ACELite {
JSHeapStatsWriter::JSHeapStatsWriter(const QString filename) : file_(filename)
{
}
JSHeapStatsWriter::~JSHeapStatsWriter()
{
    file_.flush();
    file_.close();
}

void JSHeapStatsWriter::Write(const char *text)
{
    if (!file_.isOpen()) {
        if (!file_.open(QIODevice::WriteOnly | QIODevice::Text)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "failed to open file.");
            return;
        }
    }
    file_.write(text);
    file_.flush();
}
} // namespace ACELite
} // namespace OHOS
