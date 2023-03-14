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

import Router from '@system.router';
import prompt from '@system.prompt';

const PAGE_URI_GET_A2DP_SOURCE_PROFILE = 'pages/subManualApiTest/subProfileTest/a2dpSourceProfileTest';
const PAGE_URI_GET_HANDS_FREE_AUDIO_GATEWAY_PROFILE = 'pages/subManualApiTest/subProfileTest/handsFreeAudioGatewayProfileTest';
const PAGE_URI_GET_HID_HOST_PROFILE = 'pages/subManualApiTest/subProfileTest/hidHostProfileTest';
const PAGE_URI_GET_Pan_NETWORK_PROFILE = 'pages/subManualApiTest/subProfileTest/panNetworkProfileTest';


export function testA2dpSourceProfile():string{
  Router.push({ uri: PAGE_URI_GET_A2DP_SOURCE_PROFILE })
  let message = "跳转成功"
  prompt.showToast({message: message})
  return message
}

export function testHandsFreeAudioGatewayProfile():string{
  Router.push({ uri: PAGE_URI_GET_HANDS_FREE_AUDIO_GATEWAY_PROFILE })
  let message = "跳转成功"
  prompt.showToast({message: message})
  return message
}

export function testHidHostProfile():string{
  Router.push({ uri: PAGE_URI_GET_HID_HOST_PROFILE })
  let message = "跳转成功"
  prompt.showToast({message: message})
  return message
}

export function testPanNetworkProfile():string{
  Router.push({ uri: PAGE_URI_GET_Pan_NETWORK_PROFILE })
  let message = "跳转成功"
  prompt.showToast({message: message})
  return message
}
