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
 * BLE found Scenario Test Page Of Bluetooth test
 */
import prompt from '@system.prompt';
import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { BleFoundScenarioTestCase,initBleFoundScenarioData } from '../../model/scenarioTestDataModel'
import * as BLEInterface from '../../model/bleInterface'
import * as BrInterface from '../../model/brInterface'

@Entry
@Component
struct BleFoundScenarioTest {
  @State message: string = 'BleFoundScenarioTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink("bleFoundScenarioMessage") bleFoundScenarioMessage: string = ""

  aboutToAppear() {
    AppStorage.SetOrCreate('txtScanFilterDeviceId', "08:FB:EA:19:48:0F"); // 00:1A:7D:DA:71:15
    AppStorage.SetOrCreate('txtScanFilterName', "dudu-tiger");
    AppStorage.SetOrCreate('txtScanFilterServiceUuid', "0000180A-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('txtScanOptions_interval', "0");
    AppStorage.SetOrCreate('txtScanOptionsDutyMode', '0');
    AppStorage.SetOrCreate('txtScanOptionsMatchMode', '0');
    AppStorage.SetOrCreate('cbxBleScanFilter', true);
    AppStorage.SetOrCreate('cbxBleScanOptions', true);
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
              scenarioItems: initBleFoundScenarioData(),
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
                Text(this.bleFoundScenarioMessage)
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
            prompt.showToast({ message: '开始BleFound场景测试' })
            let self = this
//            const arr = [0, 1, 2, 3, 4, 5, 6, 7]
            let bleFoundScenarioMessage = ""
            // 注册状态变化
//            bleFoundScenarioMessage += BrInterface.testOnStateChange() + "\n"
//            console.log("testOnStateChange success")
//            self.changeIndex = arr[0]
//            await sleep(1)
//            // 开蓝牙
//            bleFoundScenarioMessage += BrInterface.testEnableBluetooth() + "\n";
//            console.log("testEnableBluetooth success")
//            self.changeIndex = arr[1]
//            AppStorage.SetOrCreate("bleFoundScenarioMessage", bleFoundScenarioMessage)
//            await sleep(2)
//            // 获取状态
//            bleFoundScenarioMessage += BrInterface.testGetState() + "\n";
//            AppStorage.SetOrCreate('cbxBleScanOptions', true);
//            AppStorage.SetOrCreate('cbxBleScanFilter', true);
//            self.changeIndex = arr[2]
//            await sleep(1)
//            // 开启注册BLE查找状态变化
//            bleFoundScenarioMessage += BLEInterface.testOnBLEDeviceFind() + "\n"
//            console.log("testOnBLEDeviceFind success")
//            self.changeIndex = arr[3]
//            await sleep(1)
//            // 开始BLE扫描
//            bleFoundScenarioMessage += BLEInterface.testStartBLEScan() + "\n"
//            console.log("testStartBLEScan success")
//            self.changeIndex = arr[4]
//            await sleep(1)
//            // 获取连接的BLE设备
//            bleFoundScenarioMessage += BLEInterface.testGetConnectedBLEDevices() + "\n"
//            console.log("testGetConnectedBLEDevices success")
//            self.changeIndex = arr[5]
//            await sleep(1)
//            AppStorage.SetOrCreate("bleFoundScenarioMessage", bleFoundScenarioMessage)

            //拼装数组
            var bleScenarioArray=[{name:"testSetLocalName",para:["DaYuBlue"]},
                                 {name:"testSetBluetoothScanMode",para:[4,0]}]
            let Interface = BleFoundScenarioTestCase.map(i=>{
              let para = bleScenarioArray.some(itm=>itm.name===i.api.name)?bleScenarioArray.find(itm=>itm.name===i.api.name).para:[]
              return {name:i.api.name,para}
            })

            //循环调用
            async function forLoop() {
              for (let m=0 ; m<Interface.length ; m++){
                bleFoundScenarioMessage += BleFoundScenarioTestCase[m]["api"](...Interface[m].para) + "\n"
                self.changeIndex=m
                await sleep(1)
                AppStorage.SetOrCreate("bleFoundScenarioMessage", bleFoundScenarioMessage)
              }
            }
            forLoop()

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
            const arr = [0, 1, 2, 3, 4, 5, 6, 7]
            let bleFoundScenarioMessage = ""
            // 停止BLE扫描
            bleFoundScenarioMessage += BLEInterface.testStopBLEScan() + "\n"
            console.log("testStopBLEScan success")
//            self.changeIndex = arr[6]
            await sleep(1)
            // 关闭注册BLE查找状态变化
            bleFoundScenarioMessage += BLEInterface.testOnBLEDeviceFind() + "\n"
            console.log("testOffBLEDeviceFind success")
            await sleep(1)
            //关闭注册状态变化
            bleFoundScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOffStateChange success")
            await sleep(1)
            //关蓝牙
            bleFoundScenarioMessage += BrInterface.testDisableBluetooth() + "\n"
            console.log("testDisableBluetooth success")
//            self.changeIndex = arr[7]
            await sleep(1)
            AppStorage.SetOrCreate("bleFoundScenarioMessage", bleFoundScenarioMessage)
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
            let bleFoundScenarioMessage = ""
            AppStorage.SetOrCreate("bleFoundScenarioMessage", bleFoundScenarioMessage)
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