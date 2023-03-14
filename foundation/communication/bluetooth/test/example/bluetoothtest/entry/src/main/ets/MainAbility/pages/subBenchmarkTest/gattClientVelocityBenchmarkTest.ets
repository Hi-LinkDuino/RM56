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
 *  Bluetooth GattClient Velocity Benchmark Test Page Of Bluetooth test
 */

import router from "@system.router";
import { PageTitle } from "../../../Component/pageTitle";
import { TestImageDisplay } from "../../../Component/testImageDisplay";
import { TestData } from "../../model/testData"
import { ScenarioContentTable } from "../../../Component/scenarioContentTable"
import { initBRFoundScenarioData } from "../../model/scenarioTestDataModel"
import { GattClientBenchmarkTestCase } from "../../model/benchmarkTestDataModels"
import * as GattClientInterface from "../../model/gattClientInterface"
import { testSetLocalName,testSetBluetoothScanMode } from "../../model/brInterface"
import bluetooth from "@ohos.bluetooth";

@Entry
@Component
struct GattClientVelocityBenchmarkTest {
  @State message: string = "GattClientBenchmarkTest";
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @State gatt_Cycle_index: number = 10
  @StorageLink("gattClientBenchmarkTestMessage") gattClientBenchmarkTestMessage: string = ""
  private peripheralDeviceId = "08:FB:EA:19:48:0F"
  @State serviceUUID: string = "00001801-0000-1000-8000-00805f9b34fb";
  @State characteristicUUID: string = "00002b29-0000-1000-8000-00805f9b34fb";
  @State characteristicValue: string = "CccValue";
  @State descriptorUUID: string = "00002902-0000-1000-8000-00805f9b34fb";
  @State descriptorValue: string = "DesValue";

  aboutToAppear() {
    AppStorage.SetOrCreate("peripheralDeviceId", this.peripheralDeviceId);
    AppStorage.SetOrCreate("serviceUUID", this.serviceUUID);
    AppStorage.SetOrCreate("characteristicUUID", this.characteristicUUID);
    AppStorage.SetOrCreate("characteristicValue", this.characteristicValue);
    AppStorage.SetOrCreate("descriptorUUID", this.descriptorUUID);
    AppStorage.SetOrCreate("descriptorValue", this.descriptorValue);
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
                  { value: "1000" },
                  { value: "10000" }
                ])
                  .borderRadius(10)
                  .backgroundColor($r("app.color.white"))
                  .selected(0)
                  .value( "100")
                  .font({ size: 17 })
                  .margin({ right: 80 })
                  .selectedOptionFont({ size: 17 })
                  .optionFont({ size: 15 })
                  .width("50%")
                  .onSelect((index: number) => {
                    console.info("Select:" + index)
                    AppStorage.SetOrCreate("gatt_Cycle_index", index);
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
                Text(this.gattClientBenchmarkTestMessage)
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
              Text("速率性能测试").fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let gattClientBenchmarkTestMessage = ""
            //创建Gatt客户端
            gattClientBenchmarkTestMessage += "testCreateGattClient 执行：" + testFunc(GattClientInterface.testCreateGattClient) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(5)

            //BLE连接状态
            gattClientBenchmarkTestMessage += "testOnBLEConnectionStateChange 执行：" + testFunc(GattClientInterface.testOnBLEConnectionStateChange) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(2)

            //连接
            gattClientBenchmarkTestMessage += "testConnect 执行：" + testFunc(GattClientInterface.testConnect) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(5)

            //设置通知特性更改
            gattClientBenchmarkTestMessage += "testSetNotifyCharacteristicChanged 执行：" + testFunc(GattClientInterface.testSetNotifyCharacteristicChanged) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(3)

            //BLE特征值变化
            gattClientBenchmarkTestMessage += "testOnBLECharacteristicChange 执行：" + testFunc(testOnBLECharacteristicChange) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(2)

            //获取server
            gattClientBenchmarkTestMessage += "testGetServicesCallback 执行：" + testFunc(GattClientInterface.testGetServicesCallback) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(2)

            //获取特征值
            gattClientBenchmarkTestMessage += "testReadCharacteristicValueCallback 执行：" + testFunc(GattClientInterface.testReadCharacteristicValueCallback) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(2)

            //写入特征值
            gattClientBenchmarkTestMessage += "testWriteCharacteristicValue 执行：" + testFunc(writeCharacteristicValue) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(2)
          })


          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text("结束测试").fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let gattClientBenchmarkTestMessage = ""

            gattClientBenchmarkTestMessage += "testDisconnect 执行：" + testFunc(GattClientInterface.testDisconnect) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(2)

            gattClientBenchmarkTestMessage += "testGattClientClose 执行：" + testFunc(GattClientInterface.testGattClientClose) + "\n"
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
            await sleep(2)
          })


          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Image($r("app.media.ic_public_delete_filled")).width(30).height(40).margin({ left: 5 })
              //              Text("清空").fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 10 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.moon"))
          .width(50)
          .height(66)
          .margin({ top: "15vp", right: "40vp" })
          .align(Alignment.Start)
          .onClick(async(event: ClickEvent) => {
            let gattClientBenchmarkTestMessage = ""
            AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
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
  console.log("开始:"+ Number(start)+ "ms")
  console.log("结束:"+Number(end)+ "ms")
  console.log("花费:"+ Number(end - start)+ "ms")
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

let i=1
function writeCharacteristicValue():String{
  let serviceUUID = AppStorage.Get("serviceUUID");
  let characteristicUUID = AppStorage.Get("characteristicUUID");
  let characteristicValue = AppStorage.Get("characteristicValue");
  let message = "writeCharacteristicValue test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  if(i < 101){
    let bufferCCC = string2ArrayBuffer(characteristicValue);
    let characteristic = {
      serviceUuid: serviceUUID,
      characteristicUuid: characteristicUUID,
      characteristicValue: bufferCCC
    };
    if (gattClientInstance.writeCharacteristicValue(characteristic)) {
      message = "writeCharacteristicValue succeeds.";
      readCharacteristicValueCallback()
    } else {
      message = "writeCharacteristicValue failed!";
    }
  }
  prompt.showToast({message: message})
  return message
}

function readCharacteristicValueCallback():String{
  let serviceUUID = AppStorage.Get('serviceUUID');
  let characteristicUUID = AppStorage.Get('characteristicUUID');
  let message = "readCharacteristicValueCallback test"
  let btState = bluetooth.getState();
  if (btState === bluetooth.BluetoothState.STATE_OFF) {
    message = "BT is not enabled!";
    prompt.showToast({message: message})
    return message;
  }
  if (!gattClientInstance) {
    message = "NO_GATT_CLIENT_OBJECT";
    prompt.showToast({message: message})
    return message;
  }
  let bleCharacteristicDataIn = {
    serviceUuid: serviceUUID,
    characteristicUuid: characteristicUUID
  };
  gattClientInstance.readCharacteristicValue(bleCharacteristicDataIn, (err, bleCharacteristicDataOut) => {
    if (err.code != 0) {
      message = "readCharacteristicValue error code:" + err.code + ",id:"+serviceUUID;
      console.info(message)
      prompt.showToast({message: message})
      return message;
    }
    message = "readCharacteristicValue callback:";
    message += 'bluetooth characteristic uuid:' + bleCharacteristicDataOut.characteristicUuid + "\n";
    let value = new Uint8Array(bleCharacteristicDataOut.characteristicValue);
    for (let i = 0; i < bleCharacteristicDataOut.characteristicValue.byteLength; i++) {
      message += 'bluetooth characteristic value: ' + value[i];
    }
    AppStorage.SetOrCreate("read_CharacteristicValue",message)
    prompt.showToast({message: message})
    i++
    writeCharacteristicValue()
  });
  message = 'readCharacteristicValue(callback):invoke succeeds!';
  prompt.showToast({message: message})
  return message
}


function string2ArrayBuffer (str) {
  let array = new Uint8Array(str.length);
  for(var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}


//    let gatt_Cycle_index = AppStorage.Get("gatt_Cycle_index");
//    for (let i = 0;i < gatt_Cycle_index; i++) {
//
//      gattClientBenchmarkTestMessage += "testGetServicesCallback 执行:" + testFunc(GattClientInterface.testGetServicesCallback) + "\n"
//      AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
//      await sleep(2)
//
//      gattClientBenchmarkTestMessage += "testReadCharacteristicValueCallback 执行：" + testFunc(GattClientInterface.testReadCharacteristicValueCallback) + "\n"
//      AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
//      await sleep(2)
//
//      gattClientBenchmarkTestMessage += "testWriteCharacteristicValue 执行：" + testFunc(GattClientInterface.testWriteCharacteristicValue) + "\n"
//      AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattClientBenchmarkTestMessage)
//      await sleep(10)
//
//
//    }
//
//    if () {
//      let endTime = new Date().getTime()
//      console.log("开始:", Number(startTime), "ms")
//      console.log("结束:", Number(endTime - 2000), "ms")
//      console.log("花费:", Number(endTime - startTime - 2000), "ms")
//      let gattVelocityTime = ""
//      gattVelocityTime += "花费:" + Number(endTime - startTime - 2000) + "ms" + "\n"
//      gattVelocityTime += "开始:" + Number(startTime) + "ms; " + "结束:" + Number(endTime - 2000) + "ms" + "\n"
//      AppStorage.SetOrCreate("gattClientBenchmarkTestMessage", gattVelocityTime)
//      await sleep(2)
//    } else {
//      alert("dataReadMessage error")
//      prompt.showToast({ message: "dataReadMessage error" })
//      return false
//    }