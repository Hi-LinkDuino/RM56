/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_GT_BUNDLE_EXTRACTOR_H
#define OHOS_GT_BUNDLE_EXTRACTOR_H

#include "bundle_common.h"

namespace OHOS {
class GtBundleExtractor {
public:
    static uint8_t ExtractHap(const char *codePath, const char *bundleName, int32_t fp, uint32_t totalFileSize,
        uint8_t bundleStyle);
    static char *ExtractHapProfile(int32_t fp, uint32_t totalFileSize);
    static uint8_t ExtractBundleParam(const char *path, int32_t &fpStart, char **bundleName);
    static uint8_t ExtractInstallMsg(const char *path, char **bundleName, char **label, char **smallIconPath,
        char **bigIconPath);
private:
    static uint8_t ExtractFileDataPos(int32_t fp, uint64_t &filePos);
    static bool ExtractResourceFile(const char *path, int32_t fp, uint32_t totalFileSize);
};
} // namespace OHOS
#endif // OHOS_GT_BUNDLE_EXTRACTOR_H