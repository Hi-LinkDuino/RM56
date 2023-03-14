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
 *  Bluetooth SppServer BandWidth Benchmark Test Page Of Bluetooth test
 */

import prompt from '@system.prompt';
import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import * as SppServerInterface from '../../model/sppServerInterface'
import bluetooth from '@ohos.bluetooth';
import { testSetLocalName, testSetBluetoothScanMode } from '../../model/brInterface'

@Entry
@Component
struct SppServerBandWidthBenchmarkTest {
  @State message: string = 'SppServerBenchmarkTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink("sppServerBandWidthBenchmarkTestMessage") sppServerBandWidthBenchmarkTestMessage: string = ""
  private uuid: string = "00001101-0000-1000-8000-00805f9b34fb";
  @StorageLink('dataServerReadMessageBW') dataServerReadMessageBW: string = "00";
  @State writeServerDataBW: string = '1';

  aboutToAppear() {
    AppStorage.SetOrCreate('inputUuid', this.uuid);
    AppStorage.SetOrCreate("writeServerDataBW", this.writeServerDataBW);
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
              .margin({ top: "5vp", bottom: "5vp", left: "20vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
            Text("Spp的性能测试是测试Spp两端收发信息的速率和带宽性能")
              .fontSize("17vp")
              .margin({ left: "15vp" })
              .backgroundColor($r("app.color.spring"))
            Stack().height("1vp").backgroundColor("#000000")

            Row() {
              Text("写入目标设备数据：").fontSize(17).width(160)
              TextInput({ text: this.writeServerDataBW, placeholder: "请输入写入的数据" })
                .fontSize("14vp")
                .backgroundColor($r("app.color.white"))
                .onChange((str) => {
                  this.writeServerDataBW = str;
                  AppStorage.SetOrCreate("writeServerDataBW", str);
                })
                .width("50%")
                .borderRadius(10)
                .height(32)

            }
            .padding(2)
            .justifyContent(FlexAlign.Start)
            .alignItems(VerticalAlign.Center)

            Scroll() {
              Column() {
                Text("读取的数据为：")
                  .fontSize("17vp")
                  .margin({ left: "10vp" })
                  .textAlign(TextAlign.Start)
                  .width("100%")
                Scroll() {
                  List() {
                    ListItem() {
                      Text(this.dataServerReadMessageBW)
                        .align(Alignment.TopStart)
                        .fontSize("14vp")
                        .width("90%")
                        .height(180)
                        .borderRadius(10)
                        .backgroundColor($r("app.color.white"))
                    }
                  }
                  .height("95%")
                  .width("90%")
                  .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"))
                }
                .scrollBarWidth(10)
                .scrollBar(BarState.On)
              }
              .width("100%")
              .height(180)
            }
            .scrollBarWidth(10)
            .scrollBar(BarState.On)
          }
          .width("100%")
          .height(360)
          .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
        }
        .scrollBarWidth(10)
        .scrollBar(BarState.Auto)

        Stack().height("1vp").backgroundColor("#000000")

        Scroll() {
          Column() {
            Text("日志:")
              .fontSize("17vp")
              .margin({ top: "5vp", bottom: "5vp", left: "10vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
              .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
            List() {
              ListItem() {
                Text(this.sppServerBandWidthBenchmarkTestMessage)
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
          .height(150)
        }
        .scrollBarWidth(10)
        .scrollBar(BarState.On)

      }
      .width("100%")
      .backgroundColor($r("sys.color.ohos_id_color_sub_background"))

      Column() {
        Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
          //          Button({ type: ButtonType.Normal, stateEffect: true }) {
          //            Text('性能测试开始').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
          //          }
          //          .borderRadius(8)
          //          .backgroundColor($r("app.color.blue"))
          //          .width(100)
          //          .height(66)
          //          .margin({ top: "15vp", left: "20vp" })
          //          .align(Alignment.Start)
          //          .onClick(async (event: ClickEvent) => {
          //            let sppServerBandWidthBenchmarkTestMessage = ""
          //
          //            //拼装数组
          //            var a = [{ name: "testSetLocalName", para: ["DaYuBlue"] },
          //                     { name: "testSetBluetoothScanMode", para: [4, 0] }]
          //            let Interface = SppServerBenchmarkTestCase.map(i => {
          //              let para = a.some(itm => itm.name === i.api.name) ? a.find(itm => itm.name === i.api.name).para : []
          //              return { name: i.api.name, para }
          //            })
          //
          //            async function forLoop() {
          //              for (let m = 0; m < Interface.length; m++) {
          //                //              sppServerBandWidthBenchmarkTestMessage += BrBenchmarkTestCase[m]["name"] + ","+ testFun(BrBenchmarkTestCase[m]["api"]) + "\n"
          //                sppServerBandWidthBenchmarkTestMessage += SppServerBenchmarkTestCase[m]["name"] + "," + testFunc(SppServerBenchmarkTestCase[m]["api"]) + "\n"
          //                AppStorage.SetOrCreate("sppServerBandWidthBenchmarkTestMessage", sppServerBandWidthBenchmarkTestMessage)
          //                await sleep(1.5)
          //              }
          //            }
          //            forLoop()
          //
          //          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('带宽性能测试').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(100)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let sppServerBandWidthBenchmarkTestMessage = ""

            let writeServerDataBW = loopWriteDate()
            AppStorage.SetOrCreate("writeServerDataBW", writeServerDataBW)
            await sleep(2)

            sppServerBandWidthBenchmarkTestMessage += "sppServerCreateSafe" + testFunc(sppServerCreateSafe)
            AppStorage.SetOrCreate("sppServerBandWidthBenchmarkTestMessage", sppServerBandWidthBenchmarkTestMessage)
            await sleep(8)

            sppServerBandWidthBenchmarkTestMessage += "sppServerAccept" + testFunc(sppServerAccept)
            AppStorage.SetOrCreate("sppServerBandWidthBenchmarkTestMessage", sppServerBandWidthBenchmarkTestMessage)
            await sleep(5)

          })

          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Row() {
              Text('结束测试').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
            }.alignItems(VerticalAlign.Center)
          }
          .borderRadius(8)
          .backgroundColor($r("app.color.blue"))
          .width(80)
          .height(66)
          .margin({ top: "15vp", left: "5vp", right: "5vp" })
          .align(Alignment.Start)
          .onClick(async (event: ClickEvent) => {
            let sppServerBandWidthBenchmarkTestMessage = ""

            sppServerBandWidthBenchmarkTestMessage += "testSppServerReadOff 执行：" + testFunc(SppServerInterface.testSppServerReadOff) + "\n"
            AppStorage.SetOrCreate("sppServerBandWidthBenchmarkTestMessage", sppServerBandWidthBenchmarkTestMessage)
            await sleep(1)

            sppServerBandWidthBenchmarkTestMessage += "testSppCloseServerSocket 执行：" + testFunc(SppServerInterface.testSppCloseServerSocket) + "\n"
            AppStorage.SetOrCreate("sppServerBandWidthBenchmarkTestMessage", sppServerBandWidthBenchmarkTestMessage)
            await sleep(1)

            sppServerBandWidthBenchmarkTestMessage += "sppCloseClientSocket 执行：" + testFunc(SppServerInterface.testSppCloseClientSocket) + "\n"
            AppStorage.SetOrCreate("sppServerBandWidthBenchmarkTestMessage", sppServerBandWidthBenchmarkTestMessage)
            await sleep(1)
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
          .onClick(async (event: ClickEvent) => {
            let sppServerBandWidthBenchmarkTestMessage = ""
            AppStorage.SetOrCreate("sppServerBandWidthBenchmarkTestMessage", sppServerBandWidthBenchmarkTestMessage)
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
      this.dataServerReadMessageBW = "读取成功" + JSON.stringify(data);
    } else {
      this.dataServerReadMessageBW = "读取失败";
    }
  }
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


let serverNumber: number = -1;
let clientNumber: number = -1;

function sppServerCreateSafe(): String {
  let message = "sppServerCreateSafe test"
  console.info('spp server socketId(safe mode) create! ');
  let uuid = AppStorage.Get('inputUuid')
  if (uuid == '') {
    message = 'uuid为空';
    prompt.showToast({ message: message })
    return message
  }
  // 创建蓝牙服务器侦听套接字, 返回服务端ID
  bluetooth.sppListen('BluetoothDevice', { uuid: uuid, secure: true, type: 0 }, (code, serverSocketID) => {
    console.info('bluetooth error code: ' + code.code);
    if (code.code == 0) {
      console.info('spp server: serverSocketID(safe mode) - ' + serverSocketID);
      serverNumber = serverSocketID;
      message = "创建server成功";
      prompt.showToast({ message: message })
      //      AppStorage.SetOrCreate("serverNumber", serverNumber)
      //      serverNumber = AppStorage.Get("serverNumber")
      console.info('testSppServerAccept test')
      console.info('testSppServerAccept serverNumber ' + serverNumber)
      return message
    }
  });
  prompt.showToast({ message: message })
  return message
}


function sppServerAccept(): String {
  let message = "testSppServerAccept test"
  let serverReadMessageBW = ""
  //  serverNumber = AppStorage.Get("serverNumber")
  console.info('testSppServerAccept test')
  console.info('testSppServerAccept serverNumber ' + serverNumber)

  bluetooth.sppAccept(serverNumber, (code, clientSocketID) => {
    if (code.code == 0) {
      console.info('spp server: clientSocketID - ' + clientSocketID);
      console.info('test serverNumber ' + serverNumber);
      clientNumber = clientSocketID;
      AppStorage.SetOrCreate("clientNumber", clientNumber)
      message = "接受客户端成功"
      prompt.showToast({ message: message })
      bluetooth.on('sppRead', clientNumber, (dataBuffer: ArrayBuffer) => {
        const data = new Uint8Array(dataBuffer);
        console.info('spp bluetooth read is: ' + data);
        if (data != null) {
          serverReadMessageBW += "读取成功！" + "\n"
          console.info("读取成功：" + JSON.stringify(data));
          AppStorage.SetOrCreate("dataServerReadMessageBW", serverReadMessageBW)
          prompt.showToast({ message: serverReadMessageBW })
          //          sppServerWrite();
          console.info('sppServerWrite sppWrite clientNumber: ' + clientNumber);
          //            let writeServerDataBW = AppStorage.Get("writeServerDataBW") 值为1
          let writeServerDataBW = AppStorage.Get("writeServerDataBW")
          let writeArrayBuffer = strToArrayBuffer(writeServerDataBW)
          console.info('sppServerWrite  writeServerDataBW: ' + writeServerDataBW);
          const sppWriteSuccess = bluetooth.sppWrite(clientNumber, writeArrayBuffer);
          if (sppWriteSuccess) {
            console.info("写入成功");
            message = "写入成功 ";

          } else {
            console.info("写入失败");
            message = "写入失败";
            return message
          }
          prompt.showToast({ message: message })
        } else {
          message = "读取失败";
        }
        //        AppStorage.SetOrCreate("dataServerReadMessageBW", message)
        prompt.showToast({ message: message })
        return message
      })
      prompt.showToast({ message: message })
      return message
    } else {
      message = "接受客户端失败";
      console.log("code.code:" + code.code)
      prompt.showToast({ message: message })
    }
  })
  prompt.showToast({ message: message })
  return message
}


function sppServerWrite(): String {
  let message = "sppServerWrite test"
  clientNumber = AppStorage.Get("clientNumber")
  console.info('sppServerWrite sppWrite clientNumber: ' + clientNumber);
  let writeServerDataBW = AppStorage.Get("writeServerDataBW")
  let writeArrayBuffer = strToArrayBuffer(writeServerDataBW)
  const sppWriteSuccess = bluetooth.sppWrite(clientNumber, writeArrayBuffer);
  if (sppWriteSuccess) {
    console.info('sppWrite successfully');
    message = "写入成功 ";
  } else {
    console.info('sppWrite failed');
    message = "写入失败";
  }
  prompt.showToast({ message: message })
  return message
}

function sppServerReadOff(): String {
  let message = "sppServerReadOff test"
  console.info('sppServerReadOff');
  let clientNumber: number = 0;
  clientNumber = AppStorage.Get("clientNumber")
  bluetooth.off('sppRead', clientNumber,
    (dataBuffer: ArrayBuffer) => {
      const data = new Uint8Array(dataBuffer);
      console.info('spp bluetooth read is: ' + data);
      if (data != null) {
        message = "读取成功" + JSON.stringify(data);
      } else {
        message = "读取失败";
      }
      AppStorage.SetOrCreate("dataServerReadMessageBW", message)
      prompt.showToast({ message: message })
      return message
    }
  );
  prompt.showToast({ message: message })
  return message
}

function strToArrayBuffer(str) {
  let array = new Uint8Array(str.length);
  for (var i = 0; i < str.length; i++) {
    array[i] = str.charCodeAt(i);
  }
  return array.buffer
}

function loopWriteDate() {
  let a = "1";
  for (let i = 0;i < 100; i++) {
    a += "1";
  }
  return a
}
