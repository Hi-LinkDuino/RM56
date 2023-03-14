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
#include "wifi_supplicant_hal_interface_test.h"
#include "wifi_supplicant_hal_interface.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiSupplicantHalInterfaceTest, StartSupplicantTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().StartSupplicant();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, ConnectSupplicantTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().ConnectSupplicant();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, RequestToSupplicantTest, TestSize.Level1)
{
    std::string req = "abcd";
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().RequestToSupplicant(req);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, RegisterSupplicantEventCallbackTest, TestSize.Level1)
{
    SupplicantEventCallback cbk;
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().RegisterSupplicantEventCallback(cbk);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, UnRegisterSupplicantEventCallbackTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().UnRegisterSupplicantEventCallback();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, SetPowerSaveTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().SetPowerSave(true);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, WpaSetCountryCodeTest, TestSize.Level1)
{
    std::string code;
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().WpaSetCountryCode(code);
    EXPECT_TRUE(ret != WIFI_IDL_OPT_OK);
    code = "CN";
    ret = WifiSupplicantHalInterface::GetInstance().WpaSetCountryCode(code);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, WpaGetCountryCodeTest, TestSize.Level1)
{
    std::string code;
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().WpaGetCountryCode(code);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, DisconnectSupplicantTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().DisconnectSupplicant();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiSupplicantHalInterfaceTest, StopSupplicantTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiSupplicantHalInterface::GetInstance().StopSupplicant();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}
}  // namespace Wifi
}  // namespace OHOS