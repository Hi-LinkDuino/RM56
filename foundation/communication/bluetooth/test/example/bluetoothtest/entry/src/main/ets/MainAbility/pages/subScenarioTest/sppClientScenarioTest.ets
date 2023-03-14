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
 * SppClient Scenario Test Page Of Bluetooth test
 */

import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { initSppClientScenarioData } from '../../model/scenarioTestDataModel'
import * as BLEInterface from '../../model/bleInterface'
import * as BrInterface from '../../model/brInterface'
import * as SPPClientInterface from '../../model/sppClientInterface'

@Entry
@Component
struct SppClientScenarioTest {
  @State message: string = 'SppClientScenarioTest';
  @State changeIndex: number = -1;
  @State writeData: string = '11';
  private uuid: string = "00001101-0000-1000-8000-00805f9b34fb";
  @State deviceId: string = "08:FB:EA:19:48:0F"; //08:FB:EA:19:48:0F
  private testItem: TestData = router.getParams().testId;
  @StorageLink("sppClientScenarioMessage") sppClientScenarioMessage: string = ""
  @StorageLink('dataReadMessage') dataReadMessage: string = "";

  aboutToAppear() {

    AppStorage.SetOrCreate('txtScanFilterDeviceId', "08:FB:EA:19:48:0F");
    AppStorage.SetOrCreate('txtScanFilterName', "dudu-tiger");
    AppStorage.SetOrCreate('txtScanFilterServiceUuid', "0000180A-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('txtScanOptions_interval', "0");
    AppStorage.SetOrCreate('txtScanOptionsDutyMode', '0');
    AppStorage.SetOrCreate('txtScanOptionsMatchMode', '0');
    AppStorage.SetOrCreate('cbxBleScanFilter', true);
    AppStorage.SetOrCreate('cbxBleScanOptions', true);

    AppStorage.SetOrCreate('inputDeviceId', this.deviceId);
    AppStorage.SetOrCreate('inputUuid', this.uuid);
    AppStorage.SetOrCreate('writeData', this.writeData);
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
              scenarioItems: initSppClientScenarioData(),
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
                Text(this.sppClientScenarioMessage)
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
            //  AlertDialog.show({ message: '开始SppClient场景测试' })

            let self = this
            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
            let sppClientScenarioMessage = ""
            // 注册状态变化
            sppClientScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOnStateChange success")
            self.changeIndex = arr[0]
            await sleep(1)

            // 开蓝牙
            sppClientScenarioMessage += BrInterface.testEnableBluetooth() + "\n";
            console.log("testEnableBluetooth success")
            self.changeIndex = arr[1]
            await sleep(2)
            AppStorage.SetOrCreate("sppClientScenarioMessage", sppClientScenarioMessage)

            // 获取状态
            sppClientScenarioMessage += BrInterface.testGetState() + "\n";
            console.log("testGetState success")
            self.changeIndex = arr[2]
            await sleep(1)

            // 设置扫描模式
            sppClientScenarioMessage += BrInterface.testSetBluetoothScanMode(4, 0) + "\n"
            console.log("testSetBluetoothScanMode success")
            self.changeIndex = arr[3]
            await sleep(1)

            // 获取扫描模式
            sppClientScenarioMessage += BrInterface.testGetBluetoothScanMode() + "\n"
            console.log("testGetBluetoothScanMode success")
            self.changeIndex = arr[4]
            await sleep(1)

            // Spp客户端连接
            sppClientScenarioMessage += SPPClientInterface.testSppClientConnectSafe() + "\n"
            console.log("testSppClientConnectSafe success")
            self.changeIndex = arr[5]
            AppStorage.SetOrCreate("sppClientScenarioMessage", sppClientScenarioMessage)
            await sleep(10)

            //获取当前连接状态
            sppClientScenarioMessage += BrInterface.testGetBtConnectionState() + "\n"
            console.log("testGetBtConnectionState success")
            self.changeIndex = arr[6]
            AppStorage.SetOrCreate("sppClientScenarioMessage", sppClientScenarioMessage)
            await sleep(2)

            //spp客户端读取打开
            sppClientScenarioMessage += SPPClientInterface.testSppClientReadOn() + "\n"
            console.log("testSppClientReadOn success")
            self.changeIndex = arr[7]
            sppClientScenarioMessage += AppStorage.Get("dataReadMessage") + "\n"
            AppStorage.SetOrCreate("sppClientScenarioMessage", sppClientScenarioMessage)
            await sleep(5)

            // SPP客户端写入
            sppClientScenarioMessage += SPPClientInterface.testSppClientWrite() + "\n"
            console.log("testSppClientWrite success")
            self.changeIndex = arr[8]
            AppStorage.SetOrCreate("sppClientScenarioMessage", sppClientScenarioMessage)
            await sleep(2)
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
            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
            let sppClientScenarioMessage = ""
            // spp客户端读取关闭
            sppClientScenarioMessage += SPPClientInterface.testSppClientReadOff() + "\n"
            console.log("testSppClientReadOff success")
            self.changeIndex = arr[9]
            await sleep(1)

            // spp客户端关闭
            sppClientScenarioMessage += SPPClientInterface.testSppClientClose() + "\n"
            console.log("testSppClientClose success")
            self.changeIndex = arr[10]
            await sleep(1)

            //关闭注册状态变化
            sppClientScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOffStateChange success")
            await sleep(1)

            //关蓝牙
            sppClientScenarioMessage += BrInterface.testDisableBluetooth() + "\n"
            console.log("testDisableBluetooth success")
            self.changeIndex = arr[11]
            await sleep(1)
            AppStorage.SetOrCreate("sppClientScenarioMessage", sppClientScenarioMessage)
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
            let sppClientScenarioMessage = ""
            AppStorage.SetOrCreate("sppClientScenarioMessage", sppClientScenarioMessage)
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

  dataRead(dataBuffer) {
    const data = new Uint8Array(dataBuffer);
    console.log('spp bluetooth read is: ' + data);
    if (data != null) {
      this.dataReadMessage = "读取成功" + JSON.stringify(data);
    } else {
      this.dataReadMessage = "读取失败";
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

let clientNumber = -1

export function acceptClientSocket(number) {
  console.info('spp clientSocket socketID: ' + number);
  // 获取的clientNumber用作服务端后续读/写操作socket的id。
  clientNumber = number;
}

export function sppClientClose() {
  console.info("bluetoothModel:" + 'sppCloseClientSocket clientNumber: ' + JSON.stringify(clientNumber))
  bluetooth.sppCloseClientSocket(clientNumber);
  let message = '关闭成功 clientNumber' + clientNumber;
  prompt.showToast({ message: message })
  return message
}

export function strToArrayBuffer(str) {
  let array = new Uint8Array(str.length);
  for (var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}