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
#include "wifi_internal_event_dispatcher_test.h"
#include "wifi_internal_event_dispatcher.h"
#include "permission_def.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiInternalEventDispatcherTest, ThreadTest, TestSize.Level1)
{
    WifiEventCallbackMsg msg;
    msg.msgCode = WIFI_CBK_MSG_STATE_CHANGE;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_CONNECTION_CHANGE;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_SCAN_STATE_CHANGE;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_RSSI_CHANGE;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_CHANGE;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_JOIN;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_LEAVE;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_STREAM_DIRECTION;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = WIFI_CBK_MSG_WPS_STATE_CHANGE;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(1);
    msg.msgCode = 0xffff;
    EXPECT_EQ(0, WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(msg));
    sleep(2);
}
}  // namespace Wifi
}  // namespace OHOS