/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <sys/stat.h>
namespace OHOS {
namespace AppExecFwk {
const std::string BUNDLE_DATA_ROOT_PATH = "/data/app/el2/100/base/";
const int32_t FOLDER_LAYERS = 7;
void PageAbilityDemo::OnStart(const Want &want)
{
    APP_LOGI("PageAbilityDemo::onStart");
    Ability::OnStart(want);
    const std::string appName = "com.third.hiworld.example1";
    std::string path = BUNDLE_DATA_ROOT_PATH + appName + "/cache/";
    APP_LOGI("PageAbilityDemo::CreateDir");
    for (int i = 1; i < FOLDER_LAYERS; i++) {
        path += "dir" + std::to_string(i) + "/";
        APP_LOGI("PageAbilityDemo::CreateDir %{private}s", path.c_str());
        CreateDir(path);
    }
    const std::string testCacheFileName =
        BUNDLE_DATA_ROOT_PATH + appName + "/cache/dir1/dir2/dir3/dir4/dir5/dir6/name.txt";
    CreateFile(testCacheFileName);
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

void PageAbilityDemo::CreateDir(const std::string &path) const
{
    if (access(path.c_str(), F_OK) != 0) {
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            APP_LOGE("CreateDir:%{private}s error", path.c_str());
        }
    }
}

REGISTER_AA(PageAbilityDemo)
}  // namespace AppExecFwk
}  // namespace OHOS