/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdlib>
#include <string>
#include <iostream>
#include "wifi_log.h"

#define SIZE_ARRAY(arr) sizeof((arr)) / sizeof((arr)[0])

const std::string ApTestMember[] = {
    "WifiHotspotTest",
    "WifiApNatManagerTest",
};

int main()
{
    const size_t num = 11;
    for (size_t i = 0; i < SIZE_ARRAY(ApTestMember); i++) {
        std::string cmd = "./";
        const std::string &cmdTest = ApTestMember[i];

        cmd += cmdTest;
        int ret = system(cmd.c_str());
        if (ret == num) {
            std::cout << "====================Error: " << cmdTest
                      << ", Segmentation fault!====================" << std::endl;
        }
    }

    return 0;
}