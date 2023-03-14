/**
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
 * BT ProfileModel Test Page Of Bluetooth test
 */
import ConfigData from '../../Utils/ConfigData';
import BaseModel from '../../Utils/BaseModel';
import bt from '@ohos.bluetooth';

export class BTProfileModel extends BaseModel {
  private TAG = ConfigData.TAG + 'BTProfileModel';
  private profiles: any[] = new Array();

  public PROFILE_A2DP_SOURCE = 1;

  public PROFILE_HANDS_FREE_AUDIO_GATEWAY = 4;

  public PROFILE_HID_HOST = 6;

  public PROFILE_PAN_NETWORK =7;


  public BLUETOOTH_STATE_OFF = 0;

  constructor() {
    super();
  }
  initProfile() {
    console.log('bt_test BTProfileModel start');

    this.profiles[this.PROFILE_A2DP_SOURCE] = bt.getProfile(this.PROFILE_A2DP_SOURCE);

    this.profiles[this.PROFILE_HANDS_FREE_AUDIO_GATEWAY] = bt.getProfile(this.PROFILE_HANDS_FREE_AUDIO_GATEWAY);

    this.profiles[this.PROFILE_HID_HOST] = bt.getProfile(this.PROFILE_HID_HOST);

    this.profiles[this.PROFILE_PAN_NETWORK] = bt.getProfile(this.PROFILE_PAN_NETWORK);

    console.log('bt_test BTProfileModel end');
  }

  hasProfile(type: number): boolean {
    if (!this.profiles[type]) {
      return false;
    }
    return true;
  }

  getA2DPSourceProfile(): bt.A2dpSourceProfile {
    if (this.profiles[this.PROFILE_A2DP_SOURCE] != null) {
      return this.profiles[this.PROFILE_A2DP_SOURCE]
    }
    return null;
  }

  getHandsFreeAudioGatewayProfile(): bt.HandsFreeAudioGatewayProfile {
    if (this.profiles[this.PROFILE_HANDS_FREE_AUDIO_GATEWAY] != null) {
      return this.profiles[this.PROFILE_HANDS_FREE_AUDIO_GATEWAY];
    }
    return null;
  }


  getHidHostProfile() :bt.HidHostProfile {
    if (this.profiles[this.PROFILE_HID_HOST] != null) {
      return this.profiles[this.PROFILE_HID_HOST];
    }
    return null;
  }

  getPanNetworkProfile() :bt.PanNetworkProfile {
    if (this.profiles[this.PROFILE_PAN_NETWORK] != null) {
      return this.profiles[this.PROFILE_PAN_NETWORK];
    }
    return null;
  }


  getProfile(type: number) {
    if (this.profiles[type] != null) {
      return this.profiles[type];
    }
    return null;
  }

  getBluetoothState() {
    return bt.getState();
  }
}

let btProfileModel = new BTProfileModel();
export default btProfileModel as BTProfileModel;