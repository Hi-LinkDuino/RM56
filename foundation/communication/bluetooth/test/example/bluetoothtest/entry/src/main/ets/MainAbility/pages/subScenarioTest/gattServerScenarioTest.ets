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
 * GattServer Scenario Test Page Of Bluetooth test
 */

import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { initGattServerScenarioData } from '../../model/scenarioTestDataModel'
import * as GattServerInterface from '../../model/gattServerInterface'
import * as BLEInterface from '../../model/bleInterface'
import * as BrInterface from '../../model/brInterface'

@Entry
@Component
struct GattServerScenarioTest {
  @State message: string = 'GattServerScenarioTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink("gattServerScenarioMessage") gattServerScenarioMessage: string = ""
  @StorageLink("characteristicReadMsg") characteristicReadMsg: string = "00002BE0-0000-1000-8000-00805F9B34FB";
  @StorageLink("characteristicWriteMsg") characteristicWriteMsg: string = "initial";
  @StorageLink("descriptorReadMsg") descriptorReadMsg: string = "00002902-0000-1000-8000-00805F9B34FB";
  @StorageLink("descriptorWriteMsg") descriptorWriteMsg: string = "initial";

  aboutToAppear() {
    AppStorage.SetOrCreate('txtScanFilterDeviceId', "08:FB:EA:19:48:0F");
    AppStorage.SetOrCreate('txtScanFilterName', "dudu-tiger");
    AppStorage.SetOrCreate('txtScanFilterServiceUuid', "0000180A-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('txtScanOptions_interval', "0");
    AppStorage.SetOrCreate('txtScanOptionsDutyMode', '0');
    AppStorage.SetOrCreate('txtScanOptionsMatchMode', '0');
    AppStorage.SetOrCreate('cbxBleScanFilter', true);
    AppStorage.SetOrCreate('cbxBleScanOptions', true);

    AppStorage.SetOrCreate('curServiceUUID', '00001877-0000-1000-8000-00805F9B34FB');
    AppStorage.SetOrCreate('curCharacteristicUUID', '00002BE0-0000-1000-8000-00805F9B34FB');
    AppStorage.SetOrCreate('curDescriptorUUID', '00002902-0000-1000-8000-00805F9B34FB');
    AppStorage.SetOrCreate('curDescriptorValue', 'Descriptor');
    AppStorage.SetOrCreate('curCharacteristicValue', 'DaYuBlue2');
    AppStorage.SetOrCreate('isPrimary', true);
    AppStorage.SetOrCreate('advSettings_interval', "1600");
    AppStorage.SetOrCreate('advSettings_txPower', "-7");
    AppStorage.SetOrCreate('advSettings_connectable', true);
    AppStorage.SetOrCreate('advDataServiceUuids_serviceUuid', "000054F6-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('advDataManufactureData_manufactureId', "7456");
    AppStorage.SetOrCreate('advDataManufactureData_manufactureValue', "CHN");
    AppStorage.SetOrCreate('advDataServiceData_serviceUuid', "000054F6-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('advDataServiceData_serviceValue', "AdvValue");
    AppStorage.SetOrCreate('advResponseServiceUuids_serviceUuid', "000054F6-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('advResponseManufactureData_manufactureId', "6547");
    AppStorage.SetOrCreate('advResponseManufactureData_manufactureValue', "CHN");
    AppStorage.SetOrCreate('advResponseServiceData_serviceUuid', "000054F6-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('advResponseServiceData_serviceValue', "AdvValue");
    AppStorage.SetOrCreate('cbxAdvSettings', true);
    AppStorage.SetOrCreate('cbxInterval', true);
    AppStorage.SetOrCreate('cbxTxPower', true);
    AppStorage.SetOrCreate('cbxAdvResponse', true);
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
              scenarioItems: initGattServerScenarioData(),
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
                Text(this.gattServerScenarioMessage)
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
            Row() {
              Text('场景测试开始').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "10vp", right: "10vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
//            AlertDialog.show({ message: '开始GattServer场景测试' })
            let self = this
            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,13,14]

            let gattServerScenarioMessage = ""
            // 注册状态变化
            gattServerScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOnStateChange success")
            self.changeIndex = arr[0]
            await sleep(1)

            // 开蓝牙
            gattServerScenarioMessage += BrInterface.testEnableBluetooth() + "\n";
            console.log("testEnableBluetooth success")
            self.changeIndex = arr[1]
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
            await sleep(2)

            // 获取状态
            gattServerScenarioMessage += BrInterface.testGetState() + "\n";
            console.log("testGetState success")
            self.changeIndex = arr[2]
            await sleep(1)

            //创建Gatt服务器
            gattServerScenarioMessage += GattServerInterface.testCreateGattServer() + "\n"
            console.log("testCreateGattServer success")
            self.changeIndex = arr[3]
            await sleep(1)

            // 添加服务
            gattServerScenarioMessage += GattServerInterface.testAddService() + "\n"
            console.log("testAddService success")
            self.changeIndex = arr[4]

            // 开启连接状态变化
            gattServerScenarioMessage += GattServerInterface.testOnConnectStateChange() + "\n"
            console.log("testOnConnectStateChange success")
            self.changeIndex = arr[5]
            await sleep(1)

            // 开始广播
            gattServerScenarioMessage += GattServerInterface.testStartAdvertising() + "\n"
            console.log("testStartAdvertising success")
            self.changeIndex = arr[6]
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
            await sleep(10)

            // 读取特征值
            gattServerScenarioMessage += GattServerInterface.testOnCharacteristicRead() + "\n"
            console.log("testOnCharacteristicRead success")
            self.changeIndex = arr[7]
            gattServerScenarioMessage += AppStorage.Get("characteristicReadMsg") + "\n"
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
            await sleep(3)

            // 写入特征值
            gattServerScenarioMessage += GattServerInterface.testOnCharacteristicWrite() + "\n"
            console.log("testOnCharacteristicWrite success")
            self.changeIndex = arr[8]
            gattServerScenarioMessage += AppStorage.Get("characteristicWriteMsg") + "\n"
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
            await sleep(3)

            // 读取描述符值
            gattServerScenarioMessage += GattServerInterface.testOnDescriptorRead() + "\n"
            console.log("testOnDescriptorRead success")
            self.changeIndex = arr[9]
            gattServerScenarioMessage += AppStorage.Get("descriptorReadMsg") + "\n"
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
            await sleep(3)

            // 写入描述符值
            gattServerScenarioMessage += GattServerInterface.testOnDescriptorWrite() + "\n"
            console.log("testOnDescriptorWrite success")
            self.changeIndex = arr[10]
            gattServerScenarioMessage += AppStorage.Get("descriptorWriteMsg") + "\n"
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
            await sleep(3)
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
            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
            let gattServerScenarioMessage = ""
            // 停止广播
            gattServerScenarioMessage += GattServerInterface.testStopAdvertising() + "\n"
            console.log("testStopAdvertising success")
            self.changeIndex = arr[11]
            await sleep(1)
            // 关闭连接状态变化
            gattServerScenarioMessage += GattServerInterface.testOnConnectStateChange() + "\n"
            console.log("testOffConnectStateChange success")
            await sleep(1)
            // 删除服务
            gattServerScenarioMessage += GattServerInterface.testRemoveService() + "\n"
            console.log("testRemoveService success")
            self.changeIndex = arr[12]
            await sleep(1)
            // 关闭服务端
            gattServerScenarioMessage += GattServerInterface.testGattServerClose() + "\n"
            console.log("testGattServerClose success")
            self.changeIndex = arr[13]
            await sleep(1)
            // 关闭注册状态变化
            gattServerScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOffStateChange success")
            await sleep(1)
            // 关蓝牙
            gattServerScenarioMessage += BrInterface.testDisableBluetooth() + "\n"
            console.log("testDisableBluetooth success")
            self.changeIndex = arr[14]
            await sleep(1)
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
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
            let gattServerScenarioMessage = ""
            AppStorage.SetOrCreate("gattServerScenarioMessage", gattServerScenarioMessage)
          })
        }
        .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
        .width("100%")
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
//export function sleep (time) {
//  return new Promise((resolve) => setTimeout(resolve, time));
//}

export function string2ArrayBuffer(str) {
  let array = new Uint8Array(str.length);
  for (var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}