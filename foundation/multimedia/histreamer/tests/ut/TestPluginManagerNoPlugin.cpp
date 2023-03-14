/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#define private public
#define protected public

#include "plugin/core/plugin_manager.h"

#include "plugins/UtSourceTest1.h"
#include "plugins/UtSourceTest2.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace OHOS::Media::Plugin;

TEST(TestPluginManagerNoPlugin, ListPlugins_case1)
{
}
} // namespace Test
} // namespace Media
} // namespace OHOS
