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
#include "pageAbilityDemo.h"
#include <fcntl.h>
#include <fstream>
namespace OHOS {
namespace AppExecFwk {
const std::string BUNDLE_DATA_ROOT_PATH = "/data/app/el2/100/base/";
void PageAbilityDemo::OnStart(const Want &want)
{
    APP_LOGI("PageAbilityDemo::onStart");
    Ability::OnStart(want);
    const std::string appName = "com.third.hiworld.example1";
    const std::string testCacheFileNamE1 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/name1.txt";
    const std::string testCacheFileNamE2 = BUNDLE_DATA_ROOT_PATH + appName + "/cache/name2.txt";
    CreateFile(testCacheFileNamE1);
    CreateFile(testCacheFileNamE2);
}

void PageAbilityDemo::OnNewWant(const Want &want)
{
    APP_LOGI("PageAbilityDemo::OnNewWant");
    Ability::OnNewWant(want);
}

void PageAbilityDemo::OnForeground(const Want &want)
{
    APP_LOGI("PageAbilityDemo::OnForeground");
    Ability::OnForeground(want);
}

void PageAbilityDemo::OnStop()
{
    APP_LOGI("PageAbilityDemo::onStop");
    Ability::OnStop();
}

void PageAbilityDemo::OnActive()
{
    APP_LOGI("PageAbilityDemo::OnActive");
    Ability::OnActive();
}

void PageAbilityDemo::OnInactive()
{
    APP_LOGI("PageAbilityDemo::OnInactive");
    Ability::OnInactive();
}

void PageAbilityDemo::OnBackground()
{
    APP_LOGI("PageAbilityDemo::OnBackground");
    Ability::OnBackground();
}

void PageAbilityDemo::CreateFile(const std::string &path) const
{
    std::ofstream file(path);
    file.close();

    if (access(path.c_str(), F_OK) != 0) {
        APP_LOGE("CreateFile-checkFile:%{private}s not exist", path.c_str());
    }
}

REGISTER_AA(PageAbilityDemo)
}  // namespace AppExecFwk
}  // namespace OHOS