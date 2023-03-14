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
import BluetoothModel, { BondState, DeviceState } from '../model/BluetoothModel';
import ConfigData from '../../Utils/ConfigData';
import LogUtil from '../../Utils/LogUtil'
import BluetoothDevice from '../model/BluetoothDevice'
import BaseSettingsController from '../../Component/controller/BaseSettingsController';
import ISettingsController from '../../Component/controller/ISettingsController';

const DISCOVERY_DURING_TIME: number = 30000;    // 30'
const DISCOVERY_INTERVAL_TIME: number = 3000;   // 3'

export default class BluetoothDeviceController extends BaseSettingsController {
  private TAG = ConfigData.TAG + 'BluetoothDeviceController '
  //state
  private isOn: boolean = false;
  private enabled: boolean = false;
  // paired devices
  private pairedDevices: BluetoothDevice[] = [];
  // available devices
  private isDeviceDiscovering: boolean = false;
  private availableDevices: BluetoothDevice[] = [];
  private pairPinCode: string = '';
  private discoveryStartTimeoutId: number;
  private discoveryStopTimeoutId: number;

  initData(): ISettingsController {
    super.initData();
    let isOn = BluetoothModel.isStateOn();
    LogUtil.log(this.TAG + 'initData bluetooth state isOn ' + isOn + ', typeof isOn = ' + typeof(isOn))
    if (isOn) {
      this.refreshPairedDevices();
    }
    LogUtil.log(this.TAG + 'initData save value to app storage. ')
    this.isOn = new Boolean(isOn).valueOf()
    this.enabled = true
    AppStorage.SetOrCreate('bluetoothIsOn', this.isOn);
    AppStorage.SetOrCreate('bluetoothToggleEnabled', this.enabled);
    AppStorage.SetOrCreate('bluetoothAvailableDevices', this.availableDevices);
    return this;
  }

  subscribe(): ISettingsController {
    LogUtil.log(this.TAG + 'subscribe bluetooth state isOn ' + this.isOn)
    this.subscribeStateChange();
    this.subscribeBluetoothDeviceFind();
    this.subscribeBondStateChange();
    this.subscribeDeviceConnectStateChange();
    if (this.isOn) {
      this.startBluetoothDiscovery();
    }
    return this;
  }

  unsubscribe(): ISettingsController {
    this.stopBluetoothDiscovery();
    if (this.discoveryStartTimeoutId) {
      clearTimeout(this.discoveryStartTimeoutId);
    }
    if (this.discoveryStopTimeoutId) {
      clearTimeout(this.discoveryStopTimeoutId);
    }
    BluetoothModel.unsubscribeStateChange();
    BluetoothModel.unsubscribeBluetoothDeviceFind();
    BluetoothModel.unsubscribeBondStateChange();
    BluetoothModel.unsubscribeDeviceStateChange();
    return this;
  }

  subscribePinRequired(success?: (deviceId: string, pinCode: string) => void): void {
    // start listening pinCode
    BluetoothModel.subscribePinRequired((pinRequiredParam: {
      deviceId: string;
      pinCode: string;
    }) => {
      LogUtil.log(this.TAG + 'bluetooth subscribePinRequired callback. pinRequiredParam = ' + JSON.stringify(pinRequiredParam));
      this.pairPinCode = pinRequiredParam.pinCode;
      if (success) {
        success(pinRequiredParam.deviceId, pinRequiredParam.pinCode);
      }
    })
  }

  /**
   * Set toggle value
   */
  toggleValue(isOn: boolean) {
    this.enabled = false
    AppStorage.SetOrCreate('bluetoothToggleEnabled', this.enabled);
    LogUtil.log(this.TAG + 'afterCurrentValueChanged bluetooth state isOn = ' + this.isOn)
    if (isOn) {
      BluetoothModel.enableBluetooth();
    } else {
      BluetoothModel.disableBluetooth()
    }
  }

  /**
   * Get Local Name
   */
  getLocalName() {
    AppStorage.SetOrCreate('bluetoothLocalName', BluetoothModel.getLocalName());
  }

  /**
   * Pair device.
   *
   * @param deviceId device id
   * @param success success callback
   * @param error error callback
   */
  pair(deviceId: string, success?: (pinCode: string) => void, error?: () => void): void {
    const device: BluetoothDevice = this.getAvailableDevice(deviceId);
    if (device && device.connectionState === DeviceState.STATE_PAIRING) {
      LogUtil.log(this.TAG + `bluetooth no Aavailable device or device(${deviceId}) is already pairing.`)
      return;
    }


    // start pairing
    const result = BluetoothModel.pairDevice(deviceId);
    LogUtil.log(this.TAG + 'bluetooth pairDevice result = ' + result);
    if (!result) {
      AppStorage.SetOrCreate('pairedDeviceId', '');
      BluetoothModel.unsubscribePinRequired(() => LogUtil.log(this.TAG + 'available pinRequired unsubscribed.'));
      if (error) {
        error();
      }
    }
  }

  /**
   * Confirm pairing.
   *
   * @param deviceId device id
   * @param accept accept or not
   * @param success success callback
   * @param error error callback
   */
  confirmPairing(deviceId: string, accept: boolean): void {
    if (accept) {
      this.getAvailableDevice(deviceId).connectionState = DeviceState.STATE_PAIRING;
      this.forceRefresh(this.availableDevices);
      AppStorage.SetOrCreate('bluetoothAvailableDevices', this.availableDevices);
    }
    // set paring confirmation
    BluetoothModel.setDevicePairingConfirmation(deviceId, accept);
  }

  /**
   * Connect device.
   * @param deviceId device id
   */
  connect(deviceId: string): Array<{
    profileId: number;
    connectRet: boolean;
  }> {
    return BluetoothModel.connectDevice(deviceId);
  }

  /**
   * disconnect device.
   * @param deviceId device id
   */
  disconnect(deviceId: string): Array<{
    profileId: number;
    disconnectRet: boolean;
  }> {
    return BluetoothModel.disconnectDevice(deviceId);
  }

  /**
   * Unpair device.
   * @param deviceId device id
   */
  unpair(deviceId: string): boolean {
    const result = BluetoothModel.unpairDevice(deviceId);
    LogUtil.log(this.TAG + 'bluetooth paired device unpair. result = ' + result)
    this.refreshPairedDevices()
    return result;
  }

  /**
   * Refresh paired devices.
   */
  refreshPairedDevices() {
    let deviceIds: string[] = BluetoothModel.getPairedDeviceIds();
    let list: BluetoothDevice[] = []
    deviceIds.forEach(deviceId => {
      list.push(this.getDevice(deviceId));
    });
    this.pairedDevices = list;
    AppStorage.SetOrCreate('bluetoothPairedDevices', this.pairedDevices);
    LogUtil.log(this.TAG + 'bluetooth paired devices. list = ' + JSON.stringify(list))
  }


  //---------------------- subscribe ----------------------
  /**
   * Subscribe bluetooth state change
   */
  private subscribeStateChange() {
    console.info("subscribeStateChange")
    BluetoothModel.subscribeStateChange((isOn: boolean) => {
      LogUtil.log(this.TAG + 'bluetooth state changed. isOn = ' + isOn)
      this.isOn = new Boolean(isOn).valueOf();
      this.enabled = true;
      LogUtil.log(this.TAG + 'bluetooth state changed. save value.')
      this.getLocalName()
      AppStorage.SetOrCreate('bluetoothIsOn', this.isOn);
      AppStorage.SetOrCreate('bluetoothToggleEnabled', this.enabled);
      if (isOn) {
        LogUtil.log(this.TAG + 'bluetooth state changed. unsubscribe')
        this.startBluetoothDiscovery();
      } else {
        LogUtil.log(this.TAG + 'bluetooth state changed. subscribe')
        this.stopBluetoothDiscovery();
      }
    });
  }

  /**
   * Subscribe device find
   */
  private subscribeBluetoothDeviceFind() {
    console.info("subscribeBluetoothDeviceFind")
    BluetoothModel.subscribeBluetoothDeviceFind((deviceIds: Array<string>) => {
      LogUtil.log(ConfigData.TAG + 'available bluetooth devices changed.');
      deviceIds?.forEach(deviceId => {
        let device = this.availableDevices.find((availableDevice) => {
          return availableDevice.deviceId === deviceId
        })
        LogUtil.log(this.TAG + 'available bluetooth find [' + deviceId + '] = ' + JSON.stringify(device));
        if (!device) {
          let newDevice = this.getDevice(deviceId);
          LogUtil.log(this.TAG + 'available bluetooth new device : ' + JSON.stringify(newDevice));
          this.availableDevices.push(newDevice);
          LogUtil.log(this.TAG + 'available bluetooth new device pushed. availableDevices length = ' + this.availableDevices.length);
        }
      })
      AppStorage.SetOrCreate('bluetoothAvailableDevices', this.availableDevices);
    });
  }

  /**
   * Subscribe bond state change
   */
  private subscribeBondStateChange() {
    console.info("subscribeBondStateChange")
    BluetoothModel.subscribeBondStateChange((data: {
      deviceId: string;
      bondState: number;
    }) => {
      LogUtil.log(this.TAG + 'bluetooth subscribeBondStateChange callback.');
      //paired devices
      if (data.bondState !== BondState.BOND_STATE_BONDING) {
        this.refreshPairedDevices();
      }
      //available devices
      if (data.bondState == BondState.BOND_STATE_BONDING) {
        // case bonding
        // do nothing and still listening
        LogUtil.log(this.TAG + 'bluetooth continue listening bondStateChange.');
      } else if (data.bondState == BondState.BOND_STATE_INVALID) {
        // case failed
        this.getAvailableDevice(data.deviceId).connectionState = DeviceState.STATE_DISCONNECTED;
        this.forceRefresh(this.availableDevices);
        AppStorage.SetOrCreate('bluetoothAvailableDevices', this.availableDevices);
        this.showConnectFailedDialog();
      } else if (data.bondState == BondState.BOND_STATE_BONDED) {
        // case success
        LogUtil.log(this.TAG + 'bluetooth bonded : remove device.');
        this.removeAvailableDevice(data.deviceId);
        BluetoothModel.connectDevice(data.deviceId);
      }
    });
  }

  /**
   * Subscribe device connect state change
   */
  private subscribeDeviceConnectStateChange() {
    console.info("subscribeDeviceConnectStateChange")
    BluetoothModel.subscribeDeviceStateChange((data: {
      profileId: number;
      deviceId: string;
      profileConnectionState: number;
    }) => {
      LogUtil.log(this.TAG + 'device connection state changed. data = ' + JSON.stringify(data))
      for (let device of this.pairedDevices) {
        if (device.deviceId === data.deviceId) {
          device.setProfile(data);
          this.forceRefresh(this.pairedDevices);
          AppStorage.SetOrCreate('bluetoothPairedDevices', this.pairedDevices);
          break;
        }
      };
      LogUtil.log(this.TAG + 'device connection state changed. pairedDevices = ' + JSON.stringify(this.pairedDevices))
      LogUtil.log(this.TAG + 'device connection state changed. availableDevices = ' + JSON.stringify(this.availableDevices))
      this.removeAvailableDevice(data.deviceId);
    });
  }

  //---------------------- private ----------------------
  /**
   * Get device by device id.
   * @param deviceId device id
   */
  protected getDevice(deviceId: string): BluetoothDevice {
    let device = new BluetoothDevice();
    device.deviceId = deviceId;
    device.deviceName = BluetoothModel.getDeviceName(deviceId);
    device.deviceType = BluetoothModel.getDeviceType(deviceId);
    device.setProfiles(BluetoothModel.getDeviceState(deviceId));
    console.info("getDevice")
    return device;
  }

/**
   * Force refresh array.
   * Note: the purpose of this function is just trying to fix page (ets) level's bug below,
   *   and should be useless if fixed by the future sdk.
   * Bug Details:
   *   @State is not supported well for Array<CustomClass> type.
   *   In the case that the array item's field value changed, while not its length,
   *   the build method on page will not be triggered!
   */
  protected forceRefresh(arr: BluetoothDevice[]): void {
    arr.push(new BluetoothDevice())
    arr.pop();
  }

  /**
   * Start bluetooth discovery.
   */
  public startBluetoothDiscovery() {
    this.isDeviceDiscovering = true;
    BluetoothModel.startBluetoothDiscovery();
    console.info("startBluetoothDiscovery")
    this.discoveryStopTimeoutId = setTimeout(() => {
      this.stopBluetoothDiscovery();
    }, DISCOVERY_DURING_TIME);
  }

  /**
   * Stop bluetooth discovery.
   */
  private stopBluetoothDiscovery() {
    console.info("stopBluetoothDiscovery")
    this.isDeviceDiscovering = false;
    BluetoothModel.stopBluetoothDiscovery();
    this.discoveryStartTimeoutId = setTimeout(() => {
      this.startBluetoothDiscovery();
    }, DISCOVERY_INTERVAL_TIME);
  }

  /**
   * Get available device.
   *
   * @param deviceId device id
   */
  private getAvailableDevice(deviceId: string): BluetoothDevice {
    console.info("getAvailableDevice:", this.availableDevices.length)
    for (let i=0; i<this.availableDevices.length; i++) {
      if (this.availableDevices[i].deviceId === deviceId) {
        return this.availableDevices[i];
      }
    }
    return null
//    this.availableDevices.forEach(device => {
//      console.info("device.deviceId: "+device.deviceId)
//      console.info("input.deviceId:  "+deviceId)
//      console.info("input.localeCompare:  " + device.deviceId.localeCompare(deviceId).toString())
//      if (device.deviceId.localeCompare(deviceId) == 0) {
//        return device;
//      }
//    })
  }

  /**
   * Remove available device.
   *
   * @param deviceId device id
   */
  private removeAvailableDevice(deviceId: string): void {
    console.info("removeAvailableDevice")
    LogUtil.log(this.TAG + 'removeAvailableDevice : before : availableDevices length = ' + this.availableDevices.length);
    this.availableDevices = this.availableDevices.filter((device) => device.deviceId !== deviceId)
    AppStorage.SetOrCreate('bluetoothAvailableDevices', this.availableDevices);
    LogUtil.log(this.TAG + 'removeAvailableDevice : after : availableDevices length = ' + this.availableDevices.length);
  }

  /**
   * Connect Failed Dialog
   */
  private showConnectFailedDialog() {
    AlertDialog.show({
      title: $r("app.string.bluetooth_connect_failed"),
      message: $r("app.string.bluetooth_connect_failed_msg"),
      confirm: {
        value: $r("app.string.bluetooth_know_button"),
        action: () => {
          LogUtil.info('Button-clicking callback')
        }
      },
      cancel: () => {
        LogUtil.info('Closed callbacks')
      },
      alignment: DialogAlignment.Bottom
    })
  }
}