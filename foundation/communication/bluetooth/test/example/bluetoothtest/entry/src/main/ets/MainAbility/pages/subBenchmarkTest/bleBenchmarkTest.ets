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
 * BLE Benchmark Test Page Of Bluetooth test
 */
import prompt from '@system.prompt';
import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { initBRFoundScenarioData } from '../../model/scenarioTestDataModel'
import * as BleInterface from '../../model/bleInterface'
import * as BrInterface from '../../model/brInterface'
import BluetoothModel from '../../model/BluetoothModel'
import { BleBenchmarkTestCase } from '../../model/benchmarkTestDataModels'
import { testGetConnectedBLEDevices, testStartBLEScan, testStopBLEScan, testOnBLEDeviceFind
} from '../../model/bleInterface'
import BluetoothDevice from '../../model/BluetoothDevice'
import { testSetLocalName, testSetBluetoothScanMode } from '../../model/brInterface'
import bluetooth from '@ohos.bluetooth';

@Entry
@Component
struct BleBenchmarkTest {
  @State message: string = 'BleBenchmarkTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink("bleBenchmarkTestMessage") bleBenchmarkTestMessage: string = ""

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
            Text("Ble的性能测试是测试在发现BLE蓝牙时的时间性能和稳定性能")
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
                Text(this.bleBenchmarkTestMessage)
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
            let bleBenchmarkTestMessage = ""

            //拼装数组
            var bleBenchmarkArray = [{ name: "testSetLocalName", para: ["DaYuBlue"] },
                     { name: "testSetBluetoothScanMode", para: [4, 0] }]
            let Interface = BleBenchmarkTestCase.map(i => {
              let para = bleBenchmarkArray.some(itm => itm.name === i.api.name) ? bleBenchmarkArray.find(itm => itm.name === i.api.name).para : []
              return { name: i.api.name, para }
            })

            async function forLoop() {
              for (let m = 0; m < Interface.length; m++) {
                bleBenchmarkTestMessage += BleBenchmarkTestCase[m]["name"] + "," + testFunc(BleBenchmarkTestCase[m]["api"]) + "\n"
                AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
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
            let bleBenchmarkTestMessage = ""
            bleBenchmarkTestMessage += "subscribeBLEDeviceFind:" + testFunc(subscribeBLEDeviceFind) + "\n"
            AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
            await sleep(1)
            bleBenchmarkTestMessage += "startBleScan" + testFunc(BluetoothModel.startBLEScan) + "\n"
            AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
//            await sleep(1)
            //            BleInterface.testOnBLEDeviceFind() +"\n"
            //            await sleep(5)
//            subscribeBLEDeviceFind() + "\n"
            await sleep(10)
            bleBenchmarkTestMessage += "testStopBLEScan 执行：" + testFunc(BleInterface.testStopBLEScan) + "\n"
            AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
            await sleep(1)
            bleBenchmarkTestMessage += "testOffStateChange:" + testFunc(BrInterface.testOnStateChange) + "\n"
            AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
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
            let bleBenchmarkTestMessage = ""
            let i = 0
            for (i; i <10 ; i++) {
              bleBenchmarkTestMessage = "第"+(i+1)+"次稳定性测试：" + "\n"
              AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
              await sleep(1)

              bleBenchmarkTestMessage += "subscribeBLEDeviceFind:" + testFunc(subscribeBLEDeviceFind) + "\n"
              AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
              await sleep(1)

              bleBenchmarkTestMessage += "startBleScan" + testFunc(BluetoothModel.startBLEScan) + "\n"
              AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
              await sleep(10)

              bleBenchmarkTestMessage += "testStopBLEScan 执行：" + testFunc(testStopBLEScan) + "\n"
              AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
              await sleep(1)

              bleBenchmarkTestMessage += "unsubscribeBLEDeviceFind 执行：" + testFunc(BluetoothModel.unsubscribeBLEDeviceFind) + "\n"
              AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
              await sleep(1)

              bleBenchmarkTestMessage = ""
              AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
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
          .onClick(async (event: ClickEvent) => {
            let bleBenchmarkTestMessage = ""
            AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleBenchmarkTestMessage)
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


let arr = []

export function testFunc(func) {
  let start = new Date().getTime()
  //  for (let index = 0; index < 10000; index++) {
  //    arr.push(index)
  //    // ...Do b
  //  }
  switch (func) {
    case BluetoothModel.startBLEScan: {
      func(null)
    }
      break;
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

function subscribeBLEDeviceFind() {
  let start2 = new Date().getTime()
  let bleRunMessage = "scan:"
  let message = "subscribeBLEDeviceFind test"
  console.info("subscribeBLEDeviceFind test")
  BluetoothModel.subscribeBLEDeviceFind((deviceIds: Array<string>) => {
    let end2 = new Date().getTime()
    console.log('available ble devices changed.');
    deviceIds?.forEach(async (deviceId) => {
      let {data, ...obj2} = deviceId
      //      let arr = deviceId.map((item) => {
      //        return Object.assign({}, { "deviceId": item.deviceId, rssi: item.rssi})
      //      })
      //      console.log('available bluetooth find [' + deviceId + '] ' );
      //        console.log('available bluetooth find [' + deviceId + '] = ' + JSON.stringify(device));
      console.log('available ble find [' + JSON.stringify(obj2) + '] ')
      let message = ('available ble find [' + JSON.stringify(obj2) + ']  ')
      prompt.showToast({ message: message })
      bleRunMessage += "\n" + "花费:" + Number(end2 - start2) + "ms;" + "Mac为：[" + JSON.stringify(obj2) + "]" + "\n"
      bleRunMessage += "开始:" + Number(start2) + "ms; " + "结束:" + Number(end2) + "ms" + "\n"

      console.log("花费:" + Number(end2 - start2) + "ms;" + "Mac为：[" + JSON.stringify(obj2) + "]" + "\n")
      console.log("开始:" + Number(start2) + "ms; " + "结束:" + Number(end2) + "ms" + "\n")
      await sleep(1)
      AppStorage.SetOrCreate('bleRunMessage', bleRunMessage);
      await sleep(1)
      //      AppStorage.Set('bluetoothAvailableDevices', deviceIds);
      //      await sleep(1/5)
      AppStorage.SetOrCreate("bleBenchmarkTestMessage", bleRunMessage)
      await sleep(1)
      prompt.showToast({ message: bleRunMessage })
      return bleRunMessage
    })
  });
  //  bleRunMessage = AppStorage.Get('bleRunMessage');
  //  prompt.showToast({ message: bleRunMessage })
  //  return bleRunMessage
}