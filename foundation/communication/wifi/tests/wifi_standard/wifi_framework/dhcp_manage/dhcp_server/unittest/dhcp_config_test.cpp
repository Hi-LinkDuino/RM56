/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <string>
#include <securec.h>
#include "dhcp_config.h"

using namespace testing::ext;

class MockConfigFile {
public:
    MockConfigFile() : mFilePath("./unittest_dhcp_config.conf")
    {}

    ~MockConfigFile()
    {
        unlink(mFilePath.c_str());
    }

    void ClearAndWriteFile(const std::string &line)
    {
        FILE *fp = fopen(mFilePath.c_str(), "w");
        if (fp == nullptr) {
            return;
        }
        fprintf(fp, "%s\n", line.c_str());
        fclose(fp);
        return;
    }

    void AppendFile(const std::string &line)
    {
        FILE *fp = fopen(mFilePath.c_str(), "w+");
        if (fp == nullptr) {
            return;
        }
        fprintf(fp, "%s\n", line.c_str());
        fclose(fp);
        return;
    }

    std::string GetConfigFile()
    {
        return mFilePath;
    }

private:
    std::string mFilePath;
};

HWTEST(DhcpConfigTest, LoadConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(LoadConfig(nullptr, nullptr, nullptr) == RET_FAILED);
    std::string configFile;
    EXPECT_TRUE(LoadConfig(configFile.c_str(), nullptr, nullptr) == RET_FAILED);
    std::string ifName;
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), nullptr) == RET_FAILED);
    DhcpConfig config;
    ASSERT_TRUE(memset_s(&config, sizeof(config), 0, sizeof(config)) == EOK);
    MockConfigFile mockConfigFile;
    configFile = mockConfigFile.GetConfigFile();
    ifName = "veryveryveryveryveryveryveryveryveryverylanginterface";
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    std::string content = "# comment \ninterface=" + ifName;
    mockConfigFile.AppendFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    ifName = "wlan0";
    content = "interface=wlan0\ndns=255.255.255.256";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=error_poll_msg";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=a,b";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=192.168.1.1,b";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=192.168.1.10,192.168.1.100";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=192.168.1.1,192.168.1.100\n"
              "server=192.168.1.256";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=192.168.1.10,192.168.1.100\n"
              "server=192.168.1.2\ngateway=192.168.1.1\nnetmask=255.255.255.0\nleaseTime=0";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=192.168.1.10,192.168.1.100\n"
              "server=192.168.1.2\ngateway=192.168.1.1\nnetmask=255.255.255.0\nleaseTime=60\n"
              "renewalTime=10\nrebindingTime=10\ndistribution=3";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_FAILED);
    content = "interface=wlan0\ndns=8.8.8.8,114.114.114.114\npool=192.168.1.10,192.168.1.100\n"
              "server=192.168.1.2\ngateway=192.168.1.1\nnetmask=255.255.255.0\nleaseTime=60\n"
              "renewalTime=10\nrebindingTime=10\ndistribution=0\nbroadcast=1\ninvalid_key=haha";
    mockConfigFile.ClearAndWriteFile(content);
    EXPECT_TRUE(LoadConfig(configFile.c_str(), ifName.c_str(), &config) == RET_SUCCESS);
}
