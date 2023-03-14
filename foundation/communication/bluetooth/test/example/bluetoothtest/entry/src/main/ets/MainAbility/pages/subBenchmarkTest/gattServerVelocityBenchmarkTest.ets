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
 *  Bluetooth GattServer Velocity Benchmark Test Page Of Bluetooth test
 */

import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import { ScenarioContentTable } from '../../../Component/scenarioContentTable'
import { initBRFoundScenarioData } from '../../model/scenarioTestDataModel'
import * as GattServerInterface from '../../model/gattServerInterface'
import { GattServerBenchmarkTestCase } from '../../model/benchmarkTestDataModels'
import { testSetLocalName, testSetBluetoothScanMode } from '../../model/brInterface'

@Entry
@Component
struct GattServerVelocityBenchmarkTest {
  @State message: string = 'GattServerBenchmarkTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @State gatt_Cycle_index: number = 10
  @StorageLink("gattServerBenchmarkTestMessage") gattServerBenchmarkTestMessage: string = ""

  aboutToAppear() {
    AppStorage.SetOrCreate('txtScanFilterDeviceId', "08:FB:EA:19:48:0F");
    AppStorage.SetOrCreate('txtScanFilterName', "dudu-tiger");
    AppStorage.SetOrCreate('txtScanFilterServiceUuid', "0000180A-0000-1000-8000-00805f9b34fb");
    AppStorage.SetOrCreate('txtScanOptions_interval', "0");
    AppStorage.SetOrCreate('txtScanOptionsDutyMode', '0');
    AppStorage.SetOrCreate('txtScanOptionsMatchMode', '0');
    AppStorage.SetOrCreate('cbxBleScanFilter', false);
    AppStorage.SetOrCreate('cbxBleScanOptions', false);

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
            Text("性能测试:")
              .fontSize("17vp")
              .margin({ top: "10vp", bottom: "10vp", left: "20vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
            Text("Gatt的性能测试是测试Gatt两端收发notify信息变化的速率性能")
              .fontSize("17vp")
              .margin({ left: "15vp" })
              .backgroundColor($r("app.color.spring"))
            Row() {
              Text("gatt_Cycle_index:").fontSize(20).width(180).padding({ left: "15vp" })
              Column() {
                Select([
                  { value: "100" },
                  { value: "100" },
                  { value: "1000" }
                ])
                  .borderRadius(10)
                  .backgroundColor($r("app.color.white"))
                  .selected(0)
                  .value("100")
                  .font({ size: 17 })
                  .margin({ right: 120 })
                  .selectedOptionFont({ size: 17 })
                  .optionFont({ size: 15 })
                  .width("50%")
                  .onSelect((index: number) => {
                    console.info("Select:" + index)
                    AppStorage.SetOrCreate('gatt_Cycle_index', index);
                  })
              }
              .width("90%")

            }
            .padding(3)
            .justifyContent(FlexAlign.Start)
            .alignItems(VerticalAlign.Center)
          }
          .width("100%")
          .height(130)

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
                Text(this.gattServerBenchmarkTestMessage)
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
          .height(330)
        }
        .scrollBarWidth(10)
        .scrollBar(BarState.On)

      }
      .width("100%")
      .backgroundColor($r("sys.color.ohos_id_color_sub_background"))

      Column() {
        Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('速率性能测试').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let gattServerBenchmarkTestMessage = ""
            //创建Gatt服务器
            gattServerBenchmarkTestMessage += "testCreateGattServer 执行:" + testFunc(GattServerInterface.testCreateGattServer) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)
            //添加服务
            gattServerBenchmarkTestMessage += "testAddService 执行:" + testFunc(GattServerInterface.testAddService) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)

            gattServerBenchmarkTestMessage += "testOnConnectStateChange 执行:" + testFunc(GattServerInterface.testOnConnectStateChange) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)

            gattServerBenchmarkTestMessage += "testStartAdvertising 执行:" + testFunc(GattServerInterface.testStartAdvertising) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(20)

            gattServerBenchmarkTestMessage += "testOnCharacteristicRead 执行:" + testFunc(GattServerInterface.testOnCharacteristicRead) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)

            gattServerBenchmarkTestMessage += "testOnCharacteristicWrite 执行:" + testFunc(GattServerInterface.testOnCharacteristicWrite) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)
          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('结束测试').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let gattServerBenchmarkTestMessage = ""
            gattServerBenchmarkTestMessage += "testStopAdvertising 执行：" + testFunc(GattServerInterface.testStopAdvertising) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)

            gattServerBenchmarkTestMessage += "testRemoveService 执行：" + testFunc(GattServerInterface.testRemoveService) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)

            gattServerBenchmarkTestMessage += "testGattServerClose 执行：" + testFunc(GattServerInterface.testGattServerClose) + "\n"
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
            await sleep(2)
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
          .margin({ top: "15vp", right: "40vp" })
          .align(Alignment.Start)
          .onClick(async(event: ClickEvent) => {
            let gattServerBenchmarkTestMessage = ""
            AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
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


//          Button({ type: ButtonType.Normal, stateEffect: true }) {
//            Text('性能测试开始').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
//          }
//          .borderRadius(8)
//          .backgroundColor($r("app.color.blue"))
//          .width(100)
//          .height(66)
//          .margin({ top: "15vp",left:"20vp" })
//          .align(Alignment.Start)
//          .onClick((event: ClickEvent) => {
//            let gattServerBenchmarkTestMessage =""
//
//            //拼装数组
//            var a=[{name:"testSetLocalName",para:["DaYuBlue"]},
//                   {name:"testSetBluetoothScanMode",para:[4,0]}]
//            let Interface = GattServerBenchmarkTestCase.map(i=>{
//              let para = a.some(itm=>itm.name===i.api.name)?a.find(itm=>itm.name===i.api.name).para:[]
//              return {name:i.api.name,para}
//            })
//
//            async function forLoop() {
//              for (let m=0 ; m<Interface.length ; m++){
//                //                gattServerBenchmarkTestMessage += BrBenchmarkTestCase[m]["name"] + ","+ testFun(BrBenchmarkTestCase[m]["api"]) + "\n"
//                gattServerBenchmarkTestMessage += GattServerBenchmarkTestCase[m]["name"] + ","+ testFunc(GattServerBenchmarkTestCase[m]["api"]) + "\n"
//                await sleep(1)
//                AppStorage.SetOrCreate("gattServerBenchmarkTestMessage", gattServerBenchmarkTestMessage)
//              }
//            }
//            forLoop()
//          })