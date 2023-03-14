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
 * Classic Bluetooth Found Scenario Test Page Of Bluetooth test
 */
import prompt from '@system.prompt';
import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { BrFoundScenarioTestCase , initBRFoundScenarioData } from '../../model/scenarioTestDataModel'
import * as BrInterface from '../../model/brInterface'
import BluetoothModel from '../../model/BluetoothModel'
import BluetoothDevice from '../../model/BluetoothDevice'

@Entry
@Component
struct BrFoundScenarioTest {
  @State message: string = 'BrFoundScenarioTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink("brFoundScenarioMessage") brFoundScenarioMessage: string = ""

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
              scenarioItems: initBRFoundScenarioData(),
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
                Text(this.brFoundScenarioMessage)
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
            prompt.showToast({ message: '开始Br蓝牙发现场景测试' })

            let self = this
//            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
            let brFoundScenarioMessage = ""

//            // 开启注册状态变化
//            brFoundScenarioMessage += BrInterface.testOnStateChange() + "\n"
//            console.log("testOnStateChange success")
//            self.changeIndex = arr[0]
//            await sleep(1)
//            // 开蓝牙
//            brFoundScenarioMessage += BrInterface.testEnableBluetooth() + "\n"
//            console.log("testEnableBluetooth success")
//            self.changeIndex = arr[1]
//            AppStorage.SetOrCreate("brFoundScenarioMessage", brFoundScenarioMessage)
//            await sleep(2)
//            // 获取状态
//            brFoundScenarioMessage += BrInterface.testGetState() + "\n"
//            console.log("testGetState success")
//            self.changeIndex = arr[2]
//            await sleep(1)
//            // 设置本地名称
//            BrInterface.testSetLocalName("DaYuBlue2") + "\n"
//            console.log("testSetLocalName success")
//            self.changeIndex = arr[3]
//            await sleep(1)
//            // 获取本地名称
//            brFoundScenarioMessage += BrInterface.testGetLocalName() + "\n"
//            console.log("testGetLocalName success")
//            self.changeIndex = arr[4]
//            await sleep(1)
//            // 设置扫描模式
//            BrInterface.testSetBluetoothScanMode(4, 0)
//            console.log("testSetBluetoothScanMode success")
//            self.changeIndex = arr[5]
//            await sleep(1)
//            // 获取扫描模式
//            brFoundScenarioMessage += BrInterface.testGetBluetoothScanMode() + "\n"
//            console.log("testGetBluetoothScanMode success")
//            self.changeIndex = arr[6]
//            await sleep(1)
//
//            // 蓝牙发现监听
//            brFoundScenarioMessage += this.subscribeBluetoothDeviceFind() + "\n"
//            console.log("subscribeBluetoothDeviceFind success")
//            await sleep(1)
//
//            // 开始蓝牙发现
//            brFoundScenarioMessage += "startBluetoothDiscovery 执行：" + BluetoothModel.startBluetoothDiscovery() + "\n";
//            console.log("startBluetoothDiscovery success")
//            self.changeIndex = arr[7]
//            await sleep(1)

            //拼装数组
            var brScenarioArray=[{name:"testSetLocalName",para:["DaYuBlue"]},
                             {name:"testSetBluetoothScanMode",para:[4,0]}]
            let Interface = BrFoundScenarioTestCase.map(i=>{
              let para = brScenarioArray.some(itm=>itm.name===i.api.name)?brScenarioArray.find(itm=>itm.name===i.api.name).para:[]
              return {name:i.api.name,para}
            })

            //循环调用
            async function forLoop() {
              for (let m=0 ; m<Interface.length ; m++){
                brFoundScenarioMessage += BrFoundScenarioTestCase[m]["api"](...Interface[m].para) + "\n"
                self.changeIndex=m
                await sleep(1)
                AppStorage.SetOrCreate("brFoundScenarioMessage", brFoundScenarioMessage)
              }
            }
            forLoop()

//            let scribeMessage = AppStorage.Get("scribeMessage")
//            brFoundScenarioMessage += scribeMessage
//            AppStorage.SetOrCreate("brFoundScenarioMessage", brFoundScenarioMessage)
          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('场景测试结束').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "10vp", right: "10vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let self = this
//            const arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
            let brFoundScenarioMessage = ""
            // 停止蓝牙发现
            brFoundScenarioMessage += BrInterface.testStopBluetoothDiscovery() + "\n"
            console.log("testStopBluetoothDiscovery success")
//            self.changeIndex = arr[8]
            await sleep(1)
            //关闭注册状态变化
            brFoundScenarioMessage += BrInterface.testOnStateChange() + "\n"
            console.log("testOffStateChange success")
            await sleep(1)
            //关蓝牙
            brFoundScenarioMessage += BrInterface.testDisableBluetooth() + "\n"
            console.log("testDisableBluetooth success")
//            self.changeIndex = arr[9]
            await sleep(1)
            AppStorage.SetOrCreate("brFoundScenarioMessage", brFoundScenarioMessage)
          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('清空日志').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.moon"))
          .width(80)
          .height(66)
          .margin({ top: "15vp", right: "20vp" })
          .align(Alignment.Start)
          .onClick(async(event: ClickEvent) => {
            let brFoundScenarioMessage = ""
            AppStorage.SetOrCreate("brFoundScenarioMessage", brFoundScenarioMessage)
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

  subscribeBluetoothDeviceFind(): string {
    let message = "subscribeBluetoothDeviceFind test"
    console.info("subscribeBluetoothDeviceFind")
    BluetoothModel.subscribeBluetoothDeviceFind((deviceIds: Array<string>) => {
      console.log('available bluetooth devices changed.');
      deviceIds?.forEach(deviceId => {
        let device = brFoundDevices.find((availableDevice) => {
          return availableDevice.deviceId === deviceId
        })
        console.log('available bluetooth find [' + deviceId + '] ')
        let message = ('available bluetooth find [' + deviceId + ']  ')
        prompt.showToast({ message: message })
        AppStorage.SetOrCreate('bluetoothAvailableDevices', deviceIds);
      })
    });
    prompt.showToast({ message: message })
    return message
  }
}

function sleep(time) {
  return new Promise((resolve, reject) => {
    setTimeout(() => {
      resolve()
    }, time * 1000)
  })
}




