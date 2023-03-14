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
 * BLE Test Page Of Bluetooth test
 */

import BluetoothModel from '../../model/BluetoothModel';
import Router from '@system.router';
import LogUtil from '../../../Utils/LogUtil';
import bluetooth from '@ohos.bluetooth';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { ContentTable } from '../../../Component/contentTable';
import { TestData } from '../../model/testData'
import { initBleApiData } from '../../model/testDataModels'
import { BleFilterTable } from '../../../Component/bleFilterTable';

@Entry
@Component
struct BleTest {
  private testItem: TestData = Router.getParams().testId;
  @State showList: boolean = true;
  @State message: string = 'BleTest Result';
  @State peripheralDeviceId: string  = '08:FB:EA:19:48:0F'; // 88:36:CF:09:C1:90
  @State currentClick: number = -1;
  @State btOnBLEDeviceFind: string = 'on("BLEDeviceFind"): void';
  @State isBLEDeviceFindClick: boolean = false;


  // input ble scan parameters:
  /*ScanFilter*/
  @State cbxBleScanFilter: boolean = false;
  @State txtScanFilter_deviceId: string = "00:1A:7D:DA:71:15";

  @State txtScanFilter_name: string = "dudu-tiger"; 

  @State txtScanFilter_serviceUuid: string = "0000180A-0000-1000-8000-00805f9b34fb"; 
  @State curScanFilters: Array<bluetooth.ScanFilter> = [];

  /*ScanOptions*/
  @State cbxBleScanOptions: boolean = false;
  @State txtScanOptions_interval: string = "0"; 

  @State txtScanOptions_dutyMode: number = 0;//bluetooth.ScanDuty.SCAN_MODE_LOW_POWER; 
  @State rd3ScanOptions_dutyModeChecked:boolean = false;
  @State rd2ScanOptions_dutyModeChecked:boolean = false;
  @State rd1ScanOptions_dutyModeChecked:boolean = true;

  @State txtScanOptions_matchMode: number = 1;
  @State rd1ScanOptions_matchModeChecked:boolean = true;
  @State rd2ScanOptions_matchModeChecked:boolean = false;

  @State curScanOptions: bluetooth.ScanOptions = {
    interval: 0,
    dutyMode: 0,
    matchMode: 1
  }




  @StorageLink('bleAvailableDevices') availBleDeviceIds: string[] = [];
  @StorageLink('OnBLEDeviceFind') On_off_BLEDeviceFind: boolean = false;
  private gattServerInstance = null;
  private gattClientDeviceInstance = null;

  aboutToAppear() {
    AppStorage.SetOrCreate("peripheralDeviceId", this.peripheralDeviceId)
    AppStorage.SetOrCreate('txtScanFilterDeviceId', this.txtScanFilter_deviceId);
    AppStorage.SetOrCreate('txtScanFilterName', this.txtScanFilter_name);
    AppStorage.SetOrCreate('txtScanFilterServiceUuid', this.txtScanFilter_serviceUuid);
    AppStorage.SetOrCreate('txtScanOptions_interval', this.txtScanOptions_interval);
    AppStorage.SetOrCreate('txtScanOptionsDutyMode', '0');
    AppStorage.SetOrCreate('txtScanOptionsMatchMode', '0');

  }

  build() {
    Column() {
      Stack({ alignContent: Alignment.TopStart }) {
        TestImageDisplay({ testItem: this.testItem })
        PageTitle({testItem: this.testItem})
      }
      Stack().height("0.5vp").backgroundColor("#000000");
      Column() {
        Row() {
          Text("外设MAC:")
            .fontSize("18vp")
            .height(60)
            .width("20%")
            .padding({left: 10})
          TextInput({ text: this.peripheralDeviceId, placeholder: "input peripheral deviceId." })
            .fontSize("18vp")
            .onChange((strInput) => {
              this.peripheralDeviceId = strInput;
              AppStorage.SetOrCreate("peripheralDeviceId", strInput)
            })
            .width("70%")
          Image($r('app.media.Switch'))
            .height(40)
            .width(40)
            .padding({ top: 10, bottom: 10, left: 2 })
            .onClick(() => {
                this.showList = !this.showList;
            })
        }
        .margin({ top: "10vp", left: "20vp", right: "20vp" })
        .alignItems(VerticalAlign.Center)
        .backgroundColor($r("app.color.white"))
        .width("100%")
        .height(60)
      }
      .align(Alignment.TopStart)
      if (this.showList) {
        ContentTable({ testItem: this.testItem, apiItems: initBleApiData() })
      } else {
        BleFilterTable({ testItem: this.testItem,  })
      }

    }
    .alignItems(HorizontalAlign.Center)
    .backgroundColor($r("app.color.lead"))
    .height("100%")
  }

  /**
  * subscribeBLEDeviceFind
  */
  private subscribeBLEDeviceFind() {
    //回调函数里的动作是过滤，已经过滤好了就不用再考虑了。
    BluetoothModel.subscribeBLEDeviceFind((deviceItems: Array<bluetooth.ScanResult>) => {
      LogUtil.log("BLE Test:" + 'available ble devices changed.');
      deviceItems?.forEach(deviceItem => {
        let device = this.availBleDeviceIds.find((availableBleDevice) => {
          return availableBleDevice === deviceItem.deviceId
        })
        if (!device) {
          let newDevice = deviceItem.deviceId;
          LogUtil.log("BLE Test:" + 'available bluetooth new device : ' + newDevice);
          this.availBleDeviceIds.push(newDevice);
          LogUtil.log("BLE Test:" + 'available bluetooth new device pushed. availableDevices length = ' + this.availBleDeviceIds.length);
        }
      })
      AppStorage.SetOrCreate('bluetoothAvailableDevices', this.availBleDeviceIds);
    });
  }

  private buildBleScanFilters() {
    this.curScanFilters = [];
    if (this.cbxBleScanFilter) {
      let bleScanFilter = {}

      if (this.txtScanFilter_deviceId.length > 0 && this.txtScanFilter_name.length > 0 && this.txtScanFilter_serviceUuid.length > 0) {
        bleScanFilter = {
          deviceId: this.txtScanFilter_deviceId,
          name: this.txtScanFilter_name,
          serviceUuid: this.txtScanFilter_serviceUuid
        }
        this.curScanFilters.push(bleScanFilter);
      }
      if (this.txtScanFilter_deviceId === '' && this.txtScanFilter_name === '' && this.txtScanFilter_serviceUuid.length > 0) {
        bleScanFilter = {
          serviceUuid: this.txtScanFilter_serviceUuid
        }
        this.curScanFilters.push(bleScanFilter);
      }
      if (this.txtScanFilter_deviceId === '' && this.txtScanFilter_name.length > 0 && this.txtScanFilter_serviceUuid === '') {
        bleScanFilter = {
          name: this.txtScanFilter_name
        }
        this.curScanFilters.push(bleScanFilter);
      }
      if (this.txtScanFilter_deviceId.length > 0 && this.txtScanFilter_name === '' && this.txtScanFilter_serviceUuid === '') {
        bleScanFilter = {
          deviceId: this.txtScanFilter_deviceId
        }
        this.curScanFilters.push(bleScanFilter);
      }
      if (this.txtScanFilter_deviceId === '' && this.txtScanFilter_name.length > 0 && this.txtScanFilter_serviceUuid.length > 0) {
        bleScanFilter = {
          name: this.txtScanFilter_name,
          serviceUuid: this.txtScanFilter_serviceUuid
        }
        this.curScanFilters.push(bleScanFilter);
      }
      if (this.txtScanFilter_deviceId.length > 0 && this.txtScanFilter_name === '' && this.txtScanFilter_serviceUuid.length > 0) {
        bleScanFilter = {
          deviceId: this.txtScanFilter_deviceId,
          serviceUuid: this.txtScanFilter_serviceUuid
        }
        this.curScanFilters.push(bleScanFilter);
      }
      if (this.txtScanFilter_deviceId.length > 0 && this.txtScanFilter_name.length > 0 && this.txtScanFilter_serviceUuid === '') {
        bleScanFilter = {
          deviceId: this.txtScanFilter_deviceId,
          name: this.txtScanFilter_name
        }
        this.curScanFilters.push(bleScanFilter);
      }
    }
  }

  private buildBleScanOption() {
    if (this.cbxBleScanOptions) {
      this.curScanOptions = {
        interval:  parseInt(this.txtScanOptions_interval),
        dutyMode:  this.txtScanOptions_dutyMode,
        matchMode: this.txtScanOptions_matchMode}
    }
  }
}