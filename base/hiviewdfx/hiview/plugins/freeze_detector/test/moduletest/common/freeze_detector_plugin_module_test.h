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

#ifndef FREEZE_DETECTOR_PLUGIN_MODULE_TEST_H
#define FREEZE_DETECTOR_PLUGIN_MODULE_TEST_H

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "event.h"
#include "freeze_detector_plugin.h"
#include "pipeline.h"
#include "plugin.h"

namespace OHOS {
namespace HiviewDFX {
class FreezeDetectorPluginModuleTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
    std::shared_ptr<PipelineEvent> CreateFreezeEvent(const std::string& extraInfo, bool fromKernel,
        long happenTime, const std::string& domain, const std::string& stringid) const;
    static std::shared_ptr<FreezeDetectorPlugin> GetFreezeDetectorPlugin();
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif // FREEZE_DETECTOR_PLUGIN_MODULE_TEST_H
