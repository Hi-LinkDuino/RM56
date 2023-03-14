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
 * GattClient Scenario Test Page Of Bluetooth test
 */

import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { initGattClientScenarioData } from '../../model/scenarioTestDataModel'
import * as GattClientInterface from '../../model/gattClientInterface'
import * as BLEInterface from '../../model/bleInterface'
import * as BrInterface from '../../model/brInterface'

@Entry
@Component
struct GattClientScenarioTest {
  @State message: string = 'GattClientScenarioTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink("gattClientScenarioMessage") gattClientScenarioMessage: string = ""
  @StorageLink("getServices") getServices: string = "";
  @StorageLink("deviceName") deviceName: string = "";
  @StorageLink("Rssi") Rssi: string = "";
  @StorageLink("read_CharacteristicValue") read_CharacteristicValue: string = "";
  private peripheralDeviceId = '08:FB:EA:19:48:0F'
  @State serviceUUID: string = '00001801-0000-1000-8000-00805f9b34fb';
  @State characteristicUUID: string = '00002b29-0000-1000-8000-00805f9b34fb';
  @State characteristicValue: string = 'CccValue';
  @State descriptorUUID: string = '00002902-0000-1000-8000-00805f9b34fb';
  @State descriptorValue: string = 'DesValue';

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
      Stack({ alignContent: Alignment.TopStart }) {
        TestImageDisplay({ testItem: this.testItem })
        PageTitle({ testItem: this.testItem })
      }

      Stack().height("1vp").backgroundColor("#000000");
      Column() {
        Scroll() {
          Column() {
            Text("场景流程验证:")
              .fontSize("17vp")
              .margin({ top: "10vp", bottom: "10vp", left: "20vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
            ScenarioContentTable({
              testItem: this.testItem,
              scenarioItems: initGattClientScenarioData(),
              //            testMessage: this.testMessage,  优化时修改
              changeIndex: this.changeIndex
            })
          }
          .width("100%")
          .height(280)
        }
        .scrollBarWidth(10)
        .scrollBar(BarState.Auto)

        Stack().height("1vp").backgroundColor("#000000");

        Scroll() {
          Column() {
            Text("流程日志:")
              .fontSize("17vp")
              .margin({ top: "8vp", bottom: "8vp", left: "20vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
            List() {
              ListItem() {
                Text(this.gattClientScenarioMessage)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "30vp", right: "10vp" })
                  .textAlign(TextAlign.Start)
                  .width("90%")
              }
            }
            .height("80%")
            .width("90%")
            .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"))
          }
          .width("100%")
          .height(260)
        }
        .scrollBarWidth(10)
        .scrollBar(BarState.On)
      }
      .height(500)
      .width("100%")
      .backgroundColor($r("sys.color.ohos_id_color_sub_background"))


      Stack().height("1vp").backgroundColor("#000000");
      Column() {
        Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Text('场景测试开始').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "20vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let self = this
            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
//            AlertDialog.show({ message: '开始GattClient场景测试' })
            let gattClientScenarioMessage = ""

            // 开启注册状态变化
            gattClientScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOnStateChange success")
            self.changeIndex = arr[0]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 开蓝牙
            gattClientScenarioMessage += BrInterface.testEnableBluetooth() + "\n";
            console.log("testEnableBluetooth success")
            self.changeIndex = arr[1]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(2)

            // 获取状态
            gattClientScenarioMessage += BrInterface.testGetState() + "\n";
            console.log("testGetState success")
            self.changeIndex = arr[2]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 创建Gatt客户端
            gattClientScenarioMessage += GattClientInterface.testCreateGattClient() + "\n"
            console.log("testCreateGattClient success")
            self.changeIndex = arr[3]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(2)

            //开启Ble连接状态变化
            gattClientScenarioMessage += GattClientInterface.testOnBLEConnectionStateChange() + "\n"
            console.log("testOnBLEConnectionStateChange success")
            self.changeIndex = arr[4]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 连接
            gattClientScenarioMessage += GattClientInterface.testConnect() + "\n"
            console.log("testConnect success")
            self.changeIndex = arr[5]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(5)

            // 设置通知特性更改
            gattClientScenarioMessage += GattClientInterface.testSetNotifyCharacteristicChanged() + "\n"
            console.log("testSetNotifyCharacteristicChanged success")
            self.changeIndex = arr[6]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 开启BLE特征值变化
            gattClientScenarioMessage += GattClientInterface.testOnBLECharacteristicChange() + "\n"
            console.log("testOnBLECharacteristicChange success")
            self.changeIndex = arr[7]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 获取服务callback
            gattClientScenarioMessage += GattClientInterface.testGetServicesCallback() + "\n"
            console.log("testGetServicesCallback success")
            self.changeIndex = arr[8]
            gattClientScenarioMessage += AppStorage.Get("getServices")+ "\n"
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 读取特征值callback
            gattClientScenarioMessage += GattClientInterface.testReadCharacteristicValueCallback() + "\n"
            console.log("testReadCharacteristicValueCallback success")
            self.changeIndex = arr[9]
            gattClientScenarioMessage += AppStorage.Get("read_CharacteristicValue") + "\n"
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 读取描述符值callback
            gattClientScenarioMessage += GattClientInterface.testReadDescriptorValueCallback() + "\n"
            console.log("testReadDescriptorValueCallback success")
            self.changeIndex = arr[10]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 获取设备名称callback
            gattClientScenarioMessage += GattClientInterface.testGetDeviceNameCallback() + "\n"
            console.log("testGetDeviceNameCallback success")
            self.changeIndex = arr[11]
            gattClientScenarioMessage += AppStorage.Get("deviceName") + "\n"
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 获取Rssi值callback
            gattClientScenarioMessage += GattClientInterface.testGetRssiValueCallback() + "\n"
            console.log("testGetRssiValueCallback success")
            self.changeIndex = arr[12]
            gattClientScenarioMessage += AppStorage.Get("Rssi")  + "\n"
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('场景测试结束').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Start)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "10vp", right: "10vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let self = this
            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
            let gattClientScenarioMessage = ""
            // 关闭BLE特征值变化
            gattClientScenarioMessage += GattClientInterface.testOnBLECharacteristicChange() + "\n"
            console.log("testOffBLECharacteristicChange success")
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 取消连接
            gattClientScenarioMessage += GattClientInterface.testDisconnect() + "\n"
            console.log("testDisconnect success")
            self.changeIndex = arr[13]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            //关闭Ble连接状态变化
            gattClientScenarioMessage += GattClientInterface.testOnBLEConnectionStateChange() + "\n"
            console.log("testOffBLEConnectionStateChange success")
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            //关闭客户端
            gattClientScenarioMessage += GattClientInterface.testGattClientClose() + "\n"
            console.log("testGattClientClose success")
            self.changeIndex = arr[14]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            // 关闭注册状态变化
            gattClientScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOffStateChange success")
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

            //关蓝牙
            gattClientScenarioMessage += BrInterface.testDisableBluetooth() + "\n"
            console.log("testDisableBluetooth success")
            self.changeIndex = arr[15]
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
            await sleep(1)

          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('清空日志').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Start)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.moon"))
          .width(80)
          .height(66)
          .margin({ top: "15vp", right: "20vp" })
          .align(Alignment.Start)
          .onClick(async(event: ClickEvent) => {
            let gattClientScenarioMessage = ""
            AppStorage.SetOrCreate("gattClientScenarioMessage", gattClientScenarioMessage)
          })
        }
      }
      .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
      .width("100%")
      .height("100%");
    }
  }
}

function sleep(time) {
  return new Promise((resolve, reject) => {
    setTimeout(() => {
      resolve()
    }, time * 1000)
  })
}

export function string2ArrayBuffer(str) {
  let array = new Uint8Array(str.length);
  for (var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}