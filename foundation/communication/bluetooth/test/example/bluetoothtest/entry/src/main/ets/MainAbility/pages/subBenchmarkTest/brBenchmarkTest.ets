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
 * Classic Bluetooth Benchmark Test Page Of Bluetooth test
 */
import prompt from '@system.prompt';
import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { initBRFoundScenarioData } from '../../model/scenarioTestDataModel'
import * as BrInterface from '../../model/brInterface'
import BluetoothModel from '../../model/BluetoothModel'
import BluetoothDevice from '../../model/BluetoothDevice'
import { BrBenchmarkTestCase } from '../../model/benchmarkTestDataModels'
import { testSetLocalName, testSetBluetoothScanMode } from '../../model/brInterface'

@Entry
@Component
struct BrBenchmarkTest {
  @State message: string = 'BrBenchmarkTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink("brBenchmarkTestMessage") brBenchmarkTestMessage: string = ""

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
            Text("性能测试:")
              .fontSize("17vp")
              .margin({ top: "10vp", bottom: "10vp", left: "20vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
            Text("Br的性能测试是测试在发现蓝牙时的时间性能和稳定性能")
              .fontSize("17vp")
              .margin({ left: "10vp" })
          }
          .width("100%")
          .height(100)

        }
        .scrollBarWidth(10)
        .scrollBar(BarState.Auto)

        Stack().height("1vp").backgroundColor("#000000")

        Scroll() {
          Column() {
            Text("日志:")
              .fontSize("17vp")
              .margin({ top: "8vp", bottom: "8vp", left: "10vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
            List() {
              ListItem() {
                Text(this.brBenchmarkTestMessage)
                  .fontSize("17vp")
                  .margin({ top: "5vp", left: "10vp", right: "10vp" })
                  .textAlign(TextAlign.Start)
                  .width("90%")
              }
            }
            .height("90%")
            .width("90%")
            .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"))
          }
          .width("100%")
          .height(400)
        }
        .scrollBarWidth(10)
        .scrollBar(BarState.On)

      }
      .width("100%")
      .backgroundColor($r("sys.color.ohos_id_color_sub_background"))

      Column() {
        Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Text('性能测试准备').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let brBenchmarkTestMessage = ""
            //拼装数组
            var brBenchmarkArray = [{ name: "testSetLocalName", para: ["DaYuBlue"] },
                                    { name: "testSetBluetoothScanMode", para: [4, 0] }]
            let Interface = BrBenchmarkTestCase.map(i => {
              let para = brBenchmarkArray.some(itm => itm.name === i.api.name) ? brBenchmarkArray.find(itm => itm.name === i.api.name).para : []
              return { name: i.api.name, para }
            })

            async function forLoop() {
              for (let m = 0; m < Interface.length; m++) {
                brBenchmarkTestMessage += BrBenchmarkTestCase[m]["name"] + "," + testFunc(BrBenchmarkTestCase[m]["api"]) + "\n"
                AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
                await sleep(1)
              }
            }

            forLoop()

          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('时间性能测试').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let brBenchmarkTestMessage = ""
            brBenchmarkTestMessage += "subscribeBluetoothDeviceFind:" + testFunc(subscribeBluetoothDeviceFind) + "\n"
            AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
            await sleep(1)
            brBenchmarkTestMessage += "startBluetoothDiscovery 执行：" + testFunc(BluetoothModel.startBluetoothDiscovery) + "\n"
            AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
            //            await sleep(1)
            //            subscribeBluetoothDeviceFind() + "\n"
            await sleep(10)
            brBenchmarkTestMessage += "stopBluetoothDiscovery 执行：" + testFunc(BrInterface.testStopBluetoothDiscovery) + "\n"
            AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
            await sleep(1)
            brBenchmarkTestMessage += "testOffStateChange:" + testFunc(BrInterface.testOnStateChange) + "\n"
            AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
            await sleep(1)
          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('稳定性能测试').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let brBenchmarkTestMessage = ""
            let i = 0
            for (i; i < 10; i++) {
              let m =i+1
              brBenchmarkTestMessage = "第"+(i+1)+"次稳定性测试：" + "\n"
              AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
              await sleep(1)
              brBenchmarkTestMessage += "subscribeBluetoothDeviceFind:" + testFunc(subscribeBluetoothDeviceFind) + "\n"
              AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
              await sleep(1)
              brBenchmarkTestMessage += "startBluetoothDiscovery 执行：" + testFunc(BluetoothModel.startBluetoothDiscovery) + "\n"
              AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
              await sleep(10)
              brBenchmarkTestMessage += "stopBluetoothDiscovery 执行：" + testFunc(BrInterface.testStopBluetoothDiscovery) + "\n"
              AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
              await sleep(1)
              brBenchmarkTestMessage += "unsubscribeBluetoothDeviceFind 执行：" + testFunc(BluetoothModel.unsubscribeBluetoothDeviceFind) + "\n"
              AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
              await sleep(1)
              brBenchmarkTestMessage = ""
              AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
              await sleep(1)
            }
          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Image($r('app.media.ic_public_delete_filled')).width(30).height(40).margin({ left: 5 })
              //              Text('清空').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 10 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.moon"))
          .width(50)
          .height(66)
          .margin({ top: "15vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async(event: ClickEvent) => {
            let brBenchmarkTestMessage = ""
            AppStorage.SetOrCreate("brBenchmarkTestMessage", brBenchmarkTestMessage)
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


export function test(func) {
  var start = new Date().getTime(); //起始时间
  func(); //执行待测函数
  var end = new Date().getTime(); //接受时间
  return (end - start) + "ms"; //返回函数执行需要时间
}


//let arr = []
export function testFunc(func) {
  let start = new Date().getTime()
  //  for (let index = 0; index < 10000; index++) {
  //    arr.push(index)
  //    // ...Do something
  //  }
  switch (func) {
    case testSetLocalName: {
      func("DaYuBlue")
    }
      break;
    case testSetBluetoothScanMode: {
      func(4, 0)
    }
      break;
    default: {
      func()
    }
      break;
  }
  let end = new Date().getTime()
  console.log("开始:" + Number(start) + "ms")
  console.log("结束:" + Number(end) + "ms")

  console.log("花费:" + Number(end - start) + "ms")
  let message = ""
  message += "花费:" + Number(end - start) + "ms" + "\n"
  message += "开始:" + Number(start) + "ms; " + "结束:" + Number(end) + "ms" + "\n"
  return message
}


function sleep(time) {
  return new Promise((resolve, reject) => {
    setTimeout(() => {
      resolve()
    }, time * 1000)
  })
}

function subscribeBluetoothDeviceFind() {
  let start1 = new Date().getTime()
  let brRunMessage = "scan:"
  //  let brFoundDevices: BluetoothDevice[] = [];
  //  let message = "subscribeBluetoothDeviceFind test"
  console.info("subscribeBluetoothDeviceFind test")
  BluetoothModel.subscribeBluetoothDeviceFind((deviceInfo: Array<string>) => {
    let end1 = new Date().getTime()
    console.log('available bluetooth devices changed.');
    deviceInfo?.forEach(async (deviceItemInfo) => {
      //      let device = brFoundDevices.find((availableDevice) => {
      //        return availableDevice.deviceId === deviceId
      //      })
      //      console.log('available bluetooth find [' + deviceId + '] ' );
      //        console.log('available bluetooth find [' + deviceId + '] = ' + JSON.stringify(device));
      console.log('available bluetooth find [' + deviceItemInfo + '] ')
      let message = ('available bluetooth find [' + deviceItemInfo + ']  ')
      prompt.showToast({ message: message })
      brRunMessage += "\n" + "花费:" + Number(end1 - start1) + "ms;" + "Mac为：[" + deviceItemInfo + "]" + "\n"
      brRunMessage += "开始:" + Number(start1) + "ms; " + "结束:" + Number(end1) + "ms" + "\n"

      console.log("花费:" + Number(end1 - start1) + "ms;" + "Mac为：[" + deviceItemInfo + "]" + "\n")
      console.log("开始:" + Number(start1) + "ms; " + "结束:" + Number(end1) + "ms" + "\n")

      AppStorage.SetOrCreate('brRunMessage', brRunMessage);
      await sleep(1)

      AppStorage.SetOrCreate("brBenchmarkTestMessage", brRunMessage)
      await sleep(1)
      return brRunMessage
    })
  });
}
