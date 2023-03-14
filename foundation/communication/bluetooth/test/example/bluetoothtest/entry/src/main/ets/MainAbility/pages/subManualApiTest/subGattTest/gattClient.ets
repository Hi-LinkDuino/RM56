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
 * Gatt Client Test Page Of Bluetooth test
 */
import { TestImageDisplay } from '../../../../Component/testImageDisplay';
import Router from '@system.router';
import ConfigData from '../../../../Utils/ConfigData';
import { initGattClientApiData } from '../../../model/testDataModels'
import {PageTitle} from '../../../../Component/pageTitle';
import { ContentTable } from '../../../../Component/contentTable';
const G_MAC = '08:FB:EA:19:48:0F'

@Component
struct GattClientSetting {
  private gattClientInstance=null;
  private peripheralDeviceId = G_MAC;// '08:FB:EA:19:48:0F';DaYu200//
  private gattClientExist = false;
  @State isOnBLECharacteristicChangeClick: boolean = false;
  @State bleCharacteristicChangeInfo:string = '';
  @State serviceUUID: string = '00001801-0000-1000-8000-00805f9b34fb';

  @State characteristicUUID:  string = '00002b29-0000-1000-8000-00805f9b34fb';
  @State characteristicValue: string = 'CccValue';
  @State descriptorUUID:      string = '00002902-0000-1000-8000-00805f9b34fb';
  @State descriptorValue:     string = 'DesValue';

  aboutToAppear() {
    AppStorage.SetOrCreate('peripheralDeviceId', this.peripheralDeviceId);
    AppStorage.SetOrCreate('serviceUUID', this.serviceUUID);
    AppStorage.SetOrCreate('characteristicUUID', this.characteristicUUID);
    AppStorage.SetOrCreate('characteristicValue', this.characteristicValue);
    AppStorage.SetOrCreate('descriptorUUID', this.descriptorUUID);
    AppStorage.SetOrCreate('descriptorValue', this.descriptorValue);

  }

  build() {
    Column() {
      Row() {
        Text("外围设备MAC:").fontSize(16).width(70)
        TextInput({ text: this.peripheralDeviceId, placeholder: "input peripheral deviceId." })
          .fontSize("15vp")
          .onChange((strInput) => {
            this.peripheralDeviceId = strInput;
            //判断合法性
            if (strInput.length >= 17) {
              AppStorage.SetOrCreate('peripheralDeviceId', strInput);
            }
          })
          .width("80%")
          .borderRadius(1)
      }
      .backgroundColor($r("app.color.moon"))
      .padding(5)
      .justifyContent(FlexAlign.Start)
      .alignItems(VerticalAlign.Center)

      Column() {
        Stack().height("0.25vp").backgroundColor("#000000");
        Column() {
          Row() {
            Text("服务UUID:").fontSize(15).width(60);
            TextInput({ text: this.serviceUUID, placeholder: "input Service UUID" })
              .fontSize("15vp")
              .onChange((strInput) => {
                this.serviceUUID = strInput;
                if (strInput.length >= 36) {
                  AppStorage.SetOrCreate('serviceUUID', strInput);
                }
              })
              .width("80%")
              .borderRadius(1)
          }
          .padding(5)
          .justifyContent(FlexAlign.Start)
          .alignItems(VerticalAlign.Center)
          .backgroundColor($r("app.color.spring"))

          Row() {
            Text("特征值UUID:").fontSize(15).width(60);
            TextInput({ text: this.characteristicUUID, placeholder: "input Characteristic UUID" })
              .fontSize("15vp")
              .onChange((strInput) => {
                this.characteristicUUID = strInput;
                if (strInput.length >= 36) {
                  AppStorage.SetOrCreate('characteristicUUID', strInput);
                }
              })
              .width("80%")
              .borderRadius(1)
          }
          .backgroundColor($r("app.color.spring"))
          .padding(5)
          .justifyContent(FlexAlign.Start)
          .alignItems(VerticalAlign.Center)

          Row() {
            Text("特征值Value:").fontSize(15).width(60);
            TextInput({ text: this.characteristicUUID, placeholder: "Characteristic Value" })
              .fontSize("15vp")
              .onChange((strInput) => {
                this.characteristicValue = strInput;
                if (strInput.length >= 1) {
                  AppStorage.SetOrCreate('characteristicValue', strInput);
                }
              })
              .width("80%")
              .borderRadius(1)
          }
          .backgroundColor($r("app.color.spring"))
          .padding(5)
          .justifyContent(FlexAlign.Start)
          .alignItems(VerticalAlign.Center)

          Row() {
            Text("描述符UUID:").fontSize(15).width(60);
            TextInput({ text: this.descriptorUUID, placeholder: "input descriptor UUID" })
              .fontSize("15vp")
              .onChange((strInput) => {
                this.descriptorUUID = strInput;
                if (strInput.length >= 36) {
                  AppStorage.SetOrCreate('descriptorUUID', strInput);
                }
              })
              .width("80%")
              .borderRadius(1)
          }
          .backgroundColor($r("app.color.spring"))
          .padding(5)
          .justifyContent(FlexAlign.Start)
          .alignItems(VerticalAlign.Center)

          Row() {
            Text("描述符Value:").fontSize(15).width(60);
            TextInput({ text: this.descriptorUUID, placeholder: "descriptor Value" })
              .fontSize("15vp")
              .onChange((strInput) => {
                this.descriptorValue = strInput;
                if (strInput.length >= 1) {
                  AppStorage.SetOrCreate('descriptorValue', strInput);
                }
              })
              .width("80%")
              .borderRadius(1)
          }
          .backgroundColor($r("app.color.spring"))
          .padding(5)
          .justifyContent(FlexAlign.Start)
          .alignItems(VerticalAlign.Center)
        }
        Stack().height("0.25vp").backgroundColor("#000000");
      }
    }
  }
}

@Entry
@Component
struct GattClient {
  @State message: string = 'GattClient'
  @State currentClick: number = -1;
  @State showList: boolean = false;

  @State btOnBLEConnectionStateChange: string = 'on:BLEConnectionStateChange';
  @State isOnBLEConnectionStateChangeClick: boolean = false;
  @StorageLink('bleConnectionStateInfo') bleConnectionStateInfo: string = 'Disconnected';
  @StorageLink('bleCharacteristicChangeInfo') bleCharacteristicChangeInfo: string = 'bleCharacteristicChangeInfo';
  @State bleConnectionState: number = 0;

  @State btOnBLECharacteristicChange: string = 'on:BLECharacteristicChange';

  @State isOnBLECharacteristicChangeClick: boolean = false;

  @State serviceUUID: string = '00001801-0000-1000-8000-00805f9b34fb';

  @State characteristicUUID:  string = '00002b29-0000-1000-8000-00805f9b34fb';
  @State characteristicValue: string = 'CccValue';
  @State descriptorUUID:      string = '00002902-0000-1000-8000-00805f9b34fb';
  @State descriptorValue:     string = 'DesValue';
  @StorageLink("getServices") getServices: string = "";
  @StorageLink("deviceName") deviceName: string = "";
  @StorageLink("Rssi") Rssi: string = "";
  @StorageLink("read_CharacteristicValue") read_CharacteristicValue: string = "";
  @State mtuSize:number = 128;

  //------------@State->private------------------------------
  // @State  gattClientInstance:bluetooth.GattClientDevice = null;
  private gattClientInstance=null;
  private peripheralDeviceId = G_MAC;// '00:1A:7D:DA:71:15';PC//
  private gattClientExist = false;
  private testItem: TestData = Router.getParams().testId;
  aboutToAppear() {
    AppStorage.SetOrCreate('peripheralDeviceId', this.peripheralDeviceId);
    AppStorage.SetOrCreate('serviceUUID', this.serviceUUID);
    AppStorage.SetOrCreate('characteristicUUID', this.characteristicUUID);
    AppStorage.SetOrCreate('characteristicValue', this.characteristicValue);
    AppStorage.SetOrCreate('descriptorUUID', this.descriptorUUID);
    AppStorage.SetOrCreate('descriptorValue', this.descriptorValue);
  }

  build() {
    Column() {
//      PageTitle({detail: 'Gatt_Client测试'})
      Stack({ alignContent: Alignment.TopStart }) {
        TestImageDisplay({ testItem: this.testItem })
        PageTitle({testItem: this.testItem})
      }
      Stack().height("0.5vp").backgroundColor("#000000");
      Row() {
        Text("GattClientSetting")
          .fontSize("20vp")
          .height(60)
          .width("80%")
          .padding({left: "15vp"})
        Image($r('app.media.Switch'))
          .height(40)
          .width(40)
          .padding({ top: 10, bottom: 10, left: 10 })
          .onClick(() => {
            this.showList = !this.showList;
          })
      }.width("100%")
      Column(){
        if (this.showList) {
          GattClientSetting()
        }else {
          Column() {
            Scroll(){
              Column(){
                Text("结果显示:")
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "10vp"})
                  .textAlign(TextAlign.Start)
                  .width("100%")
                Text("当前配置:" + this.message)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "15vp" })
                  .textAlign(TextAlign.Start)
                  .width("90%")
                Text("BLE连接状态:" + this.bleConnectionStateInfo)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "15vp" })
                  .textAlign(TextAlign.Start)
                  .width("90%")
                Text("BLE特征变化:" + this.bleCharacteristicChangeInfo)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "15vp" })
                  .textAlign(TextAlign.Start)
                  .width("90%")
                Text("getService:" + this.getServices)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "15vp"  })
                  .textAlign(TextAlign.Start)
                  .width("90%")
                Text("deviceName:" + this.deviceName)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "15vp" })
                  .textAlign(TextAlign.Start)
                  .width("90%")
                Text("Rssi:" + this.Rssi)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "15vp"  })
                  .textAlign(TextAlign.Start)
                  .width("90%")
                Text("readCharacteristicValue:" + this.read_CharacteristicValue)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "15vp" })
                  .textAlign(TextAlign.Start)
                  .width("90%")
              }
              .width("100%")
            }
            .scrollBarWidth(10)
            .scrollBar(BarState.Auto)
          }
          .height(110)
          .width("95%")
          .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"))

          Stack().height("0.5vp").backgroundColor("#000000");
          Column() {
            Text("功能测试:")
              .margin({ top: "5vp",left: "10vp" })
              .padding("5vp")
              .fontSize("18vp")
              .width("100%")
            Scroll() {
              ContentTable({ apiItems: initGattClientApiData() })
            }
            .width(ConfigData.WH_100_100)
            .align(Alignment.TopStart)
            .layoutWeight(1)
            .margin({ top: $r('sys.float.ohos_id_card_margin_middle') })
            .height("100%")
          }
          .height("80%")
        }
      }
    .visibility(Visibility.Visible);
    }
    .backgroundColor($r("app.color.lead"))
    .height('100%')
  }
}

/**
* string to ArrayBuffer
* @param {*} str, the string object to    
* @return:  ArrayBuffer
*/
function string2ArrayBuffer (str) {
  let array = new Uint8Array(str.length);
  for(var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}

// example: sleep(500).then(()=>{/*TODO after sleeping*/});
function sleep (time) {
  return new Promise((resolve) => setTimeout(resolve, time));
}
