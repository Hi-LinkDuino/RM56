/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
// @ts-nocheck
/**
 * Profile A2DP Test Page Of Bluetooth test
 */
import prompt from '@system.prompt';
import btProfileModel from './btProfileModel'

export function testProfileConnect() {
  let message="ProfileConnect test"
  let textMessage=""
  let btConnectionState = AppStorage.Get("btConnectionState")
  let deviceId = AppStorage.Get("deviceId")
  let profileType = AppStorage.Get("profileType")
  if (btConnectionState == 1 || btConnectionState == 2) {
    prompt.showToast({ message: btConnectionState == 1 ? "该设备正在连接中" : "该设备已连接" });
    return message;
  }
  let ret = btProfileModel.getProfile(profileType).connect(deviceId);
  if (ret) {
    btConnectionState = 1;
  }
  textMessage = "connect ret：" + ret;
  message = "connect ret：" + ret;
  AppStorage.SetOrCreate("textMessage",textMessage)
  prompt.showToast({message: message});
  return message
}

export function testProfileDisConnect() {
  let message="profileDisConnect test"
  let textMessage=""
  let btConnectionState =AppStorage.Get("btConnectionState")
  let deviceId=AppStorage.Get("deviceId")
  let profileType = AppStorage.Get("profileType")
  if (btConnectionState == 0 || btConnectionState == 3) {
    prompt.showToast({ message: btConnectionState == 0 ? "该设备已断开" : "该设备正在断开连接" });
    return message;
  }
  let ret = btProfileModel.getProfile(profileType).disconnect(deviceId);
  if (ret) {
    btConnectionState = 3;
  }
  textMessage = "disconnect ret：" + ret;
  AppStorage.SetOrCreate("textMessage",textMessage)
  message = "disconnect ret：" + ret;
  prompt.showToast({message: message});
  return message
}

export function testProfileGetPlayingState() {
  let message ="profileGetPlayingState test"
  let textMessage=""
  let deviceId=AppStorage.Get("deviceId")
  let profileType = AppStorage.Get("profileType")
  prompt.showToast({message:profileType})
  console.info("profileType:"+profileType)
  let profile = btProfileModel.getProfile(parseInt(profileType))
  let ret = -1
  if (profile) {
    ret = profile.getPlayingState(deviceId);
  }
  textMessage = "getPlayingState ret: " + ret;
  message = "getPlayingState ret: " + ret;
  AppStorage.SetOrCreate("textMessage",textMessage)
  prompt.showToast({message: message});
  return message
//visibility(this.pageType == 0 ? Visibility.Visible : Visibility.None)
}

export function testProfileGetConnectionDevices() {
  let message ="profileGetConnectionDevices test"
  let textMessage=""
  let profileType = AppStorage.Get("profileType")
  let ret = btProfileModel.getProfile(profileType).getConnectionDevices();
  textMessage = "getConnectionDevices ret: " + ret;
  message = "getConnectionDevices ret: " + ret;
  AppStorage.SetOrCreate("textMessage",textMessage)
  prompt.showToast({message: message});
  return message
}

export function testProfileGetDeviceState() {
  let message ="ProfileGetDeviceState test"
  let textMessage=""
  let deviceId=AppStorage.Get("deviceId")
  let profileType = AppStorage.Get("profileType")
  let ret = btProfileModel.getProfile(profileType).getDeviceState(deviceId);
  textMessage = "getDeviceState ret: " + ret;
  message ="getDeviceState ret: " + ret;
  AppStorage.SetOrCreate("textMessage",textMessage)
  prompt.showToast({message: message});
  return message
}



