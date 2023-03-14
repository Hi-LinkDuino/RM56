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

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include "adapter/preview/entrance/ace_ability.h"
#include "adapter/preview/entrance/ace_run_args.h"

namespace {

constexpr int32_t GET_INSPECTOR_TREE_TIMES = 12;
constexpr int32_t GET_INSPECTOR_TREE_INTERVAL = 5000;
constexpr char FILE_NAME[] = "InspectorTree.txt";
constexpr char ACE_VERSION_2[] = "2.0";
constexpr char MODEL_STAGE[] = "stage";
constexpr char MAX_ARGS_COUNT = 2;

auto&& renderCallback = [](const void*, const size_t bufferSize, const int32_t width, const int32_t height) -> bool {
    return true;
};

} // namespace

int main(int argc, const char* argv[])
{
#ifdef MAC_PLATFORM
    std::string assetPathJs = "/Volumes/SSD2T/daily-test/preview/js/default";
    std::string assetPathEts = "/Volumes/SSD2T/daily-test/preview/js/default_2.0";
    std::string assetPathEtsStage = "/Volumes/SSD2T/daily-test/preview/js/default_stage/ets";
    std::string appResourcesPath = "/Volumes/SSD2T/daily-test/preview/js/AppResources";
    std::string appResourcesPathStage = "/Volumes/SSD2T/daily-test/preview/js/default_stage";
    std::string systemResourcesPath = "/Volumes/SSD2T/daily-test/preview/js/SystemResources";
    std::string fontBasePath = "/Volumes/SSD2T/daily-test/preview/js/fonts";
#else
    std::string assetPathJs = "D:\\Workspace\\preview\\js\\default";
    std::string assetPathEts = "D:\\Workspace\\preview\\js\\default_2.0";
    std::string assetPathEtsStage = "D:\\Workspace\\preview\\js\\default_stage\\ets";
    std::string appResourcesPath = "D:\\Workspace\\preview\\js\\AppResources\\assets\\entry";
    std::string appResourcesPathStage = "D:\\Workspace\\preview\\js\\default_stage";
    std::string systemResourcesPath = "D:\\Workspace\\preview\\js\\SystemResources\\assets\\entry";
    std::string fontBasePath = "D:\\Workspace\\preview\\js\\fonts";
#endif
    OHOS::Ace::Platform::AceRunArgs args = {
        .assetPath = assetPathJs,
        .systemResourcesPath = systemResourcesPath,
        .appResourcesPath = appResourcesPath,
        .fontBasePath = fontBasePath,
        .deviceConfig.orientation = OHOS::Ace::DeviceOrientation::LANDSCAPE,
        .deviceConfig.density = 1,
        .deviceConfig.deviceType = OHOS::Ace::DeviceType::CAR,
        .windowTitle = "ACE Car",
        .deviceWidth = 1280,
        .deviceHeight = 800,
        .onRender = std::move(renderCallback),
    };

    if (argc == MAX_ARGS_COUNT) {
        if (!std::strcmp(argv[1], ACE_VERSION_2)) {
            args.assetPath = assetPathEts;
            args.aceVersion = OHOS::Ace::Platform::AceVersion::ACE_2_0;
        } else if (!std::strcmp(argv[1], MODEL_STAGE)) {
            args.assetPath = assetPathEtsStage;
            args.aceVersion = OHOS::Ace::Platform::AceVersion::ACE_2_0;
            args.appResourcesPath = appResourcesPathStage;
            args.projectModel = OHOS::Ace::Platform::ProjectModel::STAGE;
        }
    }

    auto ability = OHOS::Ace::Platform::AceAbility::CreateInstance(args);
    if (!ability) {
        std::cerr << "Could not create AceAbility!" << std::endl;
        return -1;
    }

    std::thread timer([&ability]() {
        int32_t getJSONTreeTimes = GET_INSPECTOR_TREE_TIMES;
        while (getJSONTreeTimes--) {
            std::this_thread::sleep_for(std::chrono::milliseconds(GET_INSPECTOR_TREE_INTERVAL));
            std::string jsonTreeStr = ability->GetJSONTree();
            // clear all information
            std::ofstream fileCleaner(FILE_NAME, std::ios_base::out);
            std::ofstream fileWriter(FILE_NAME, std::ofstream::app);
            fileWriter << jsonTreeStr;
            fileWriter << std::endl;
            fileWriter.close();
        }
    });
    ability->InitEnv();
    std::cout << "Ace initialize done. run loop now" << std::endl;
    ability->Start();

    return 0;
}
