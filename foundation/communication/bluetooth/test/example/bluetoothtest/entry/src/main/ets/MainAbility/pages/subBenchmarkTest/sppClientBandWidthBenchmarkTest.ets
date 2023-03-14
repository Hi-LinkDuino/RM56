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
 *  Bluetooth SppClient BandWidth Benchmark Test Page Of Bluetooth test
 */

import prompt from '@system.prompt';
import router from '@system.router';
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { TestData } from '../../model/testData'
import * as SppClientInterface from '../../model/sppClientInterface'
import bluetooth from '@ohos.bluetooth';
import { testSetLocalName, testSetBluetoothScanMode } from '../../model/brInterface'

@Entry
@Component
struct SppClientBandWidthBenchmarkTest {
  @State message: string = 'SppClientBenchmarkTest';
  private testItem: TestData = router.getParams().testId;
  @State changeIndex: number = -1
  @StorageLink('dataClientReadMessageBW') dataClientReadMessageBW: string = "00";
  @StorageLink("sppClientBandWidthBenchmarkTestMessage") sppClientBandWidthBenchmarkTestMessage: string = ""
  @State writeClientDataBW: string = '1';
  private clientNumber: number = -1;
  private uuid: string = "00001101-0000-1000-8000-00805f9b34fb";
  @State deviceId: string = "08:FB:EA:19:48:0F"; //08:FB:EA:19:48:0F

  aboutToAppear() {

    AppStorage.SetOrCreate('inputDeviceId', this.deviceId);
    AppStorage.SetOrCreate('inputUuid', this.uuid);
    AppStorage.SetOrCreate("writeClientDataBW", this.writeClientDataBW);
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
              Text("目标设备MAC：").fontSize(17).width(130)
              TextInput({ text: this.deviceId, placeholder: "请输入mac地址" })
                .fontSize("17vp")
                .onChange((str) => {
                  this.deviceId = str;
                  AppStorage.SetOrCreate('inputDeviceId', str)
                })
                .width("80%")
                .borderRadius(1)
                .backgroundColor($r("app.color.white"))
                .height(30)
            }

            .padding(5)
            .justifyContent(FlexAlign.Start)
            .alignItems(VerticalAlign.Center)

            Row() {
              Text("写入目标设备数据：").fontSize(17).width(160).padding({ left: "5vp" });
              TextInput({ text: this.writeClientDataBW, placeholder: "请输入写入的数据" })
                .align(Alignment.Center)
                .fontSize("15vp")
                .backgroundColor($r("app.color.white"))
                .onChange((str) => {
                  this.writeClientDataBW = str;
                  AppStorage.SetOrCreate("writeClientDataBW", str);
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
                      Text(this.dataClientReadMessageBW)
                        .align(Alignment.TopStart)
                        .fontSize("14vp")
                        .width("90%")
                        .height(150)
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
              .height(150)
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
              .margin({ top: "8vp", bottom: "8vp", left: "10vp" })
              .textAlign(TextAlign.Start)
              .width("100%")
              .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
            List() {
              ListItem() {
                Text(this.sppClientBandWidthBenchmarkTestMessage)
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
          //            Text('性能测试准备').fontSize("24vp").fontColor(0xffffff).margin({ left: 5, right: 5 })
          //          }
          //          .borderRadius(8)
          //          .backgroundColor($r("app.color.blue"))
          //          .width(100)
          //          .height(66)
          //          .margin({ top: "15vp", left: "5vp" })
          //          .align(Alignment.Start)
          //          .onClick((event: ClickEvent) => {
          //            let sppClientBandWidthBenchmarkTestMessage = ""
          //
          //            //拼装数组
          //            var a = [{ name: "testSetLocalName", para: ["DaYuBlue"] },
          //                     { name: "testSetBluetoothScanMode", para: [4, 0] }]
          //            let Interface = SppClientBenchmarkTestCase.map(i => {
          //              let para = a.some(itm => itm.name === i.api.name) ? a.find(itm => itm.name === i.api.name).para : []
          //              return { name: i.api.name, para }
          //            })
          //
          //            async function forLoop() {
          //              for (let m = 0; m < Interface.length; m++) {
          //                //              sppClientBandWidthBenchmarkTestMessage += BrBenchmarkTestCase[m]["name"] + ","+ testFun(BrBenchmarkTestCase[m]["api"]) + "\n"
          //                sppClientBandWidthBenchmarkTestMessage += SppClientBenchmarkTestCase[m]["name"] + "," + testFunc(SppClientBenchmarkTestCase[m]["api"]) + "\n"
          //                AppStorage.SetOrCreate("sppClientBandWidthBenchmarkTestMessage", sppClientBandWidthBenchmarkTestMessage)
          //                await sleep(1.5)
          //              }
          //            }
          //
          //            forLoop()
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
            let sppClientBandWidthBenchmarkTestMessage = ""
            let writeClientDataBW = loopWriteDate()
            AppStorage.SetOrCreate("writeClientDataBW", writeClientDataBW)
            await sleep(2)
            sppClientBandWidthBenchmarkTestMessage += "sppClientConnectSafe" + testFunc(sppClientConnectSafe)
            AppStorage.SetOrCreate("sppClientBandWidthBenchmarkTestMessage", sppClientBandWidthBenchmarkTestMessage)
            await sleep(5)

            sppClientBandWidthBenchmarkTestMessage += "sppClientWrite" + testFunc(sppClientWrite)
            AppStorage.SetOrCreate("sppClientBandWidthBenchmarkTestMessage", sppClientBandWidthBenchmarkTestMessage)
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
            let sppClientBandWidthBenchmarkTestMessage = ""

            sppClientBandWidthBenchmarkTestMessage += "testSppClientReadOff 执行：" + testFunc(SppClientInterface.testSppClientReadOff) + "\n"
            AppStorage.SetOrCreate("sppClientBandWidthBenchmarkTestMessage", sppClientBandWidthBenchmarkTestMessage)
            await sleep(1)

            sppClientBandWidthBenchmarkTestMessage += "testSppClientClose 执行：" + testFunc(SppClientInterface.testSppClientClose) + "\n"
            AppStorage.SetOrCreate("sppClientBandWidthBenchmarkTestMessage", sppClientBandWidthBenchmarkTestMessage)
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
            let sppClientBandWidthBenchmarkTestMessage = ""
            AppStorage.SetOrCreate("sppClientBandWidthBenchmarkTestMessage", sppClientBandWidthBenchmarkTestMessage)
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
      this.dataClientReadMessageBW = "读取成功" + JSON.stringify(data);
    } else {
      this.dataClientReadMessageBW = "读取失败";
    }
  }

  sppClientClose() {
    LogUtil.info(MODEL + 'sppCloseClientSocket clientNumber: ' + JSON.stringify(this.clientNumber))
    bluetooth.sppCloseClientSocket(this.clientNumber);
    this.message = '关闭成功 clientNumber' + this.clientNumber;
  }
}

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
  //  console.log("运行了" + JSON.stringify(func))
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


let clientNumber = -1

function sppClientConnectSafe(): String {
  let message = "sppClientConnectSafe test"
  console.info('testSppClientConnectSafe create! ');
  let uuid = AppStorage.Get('inputUuid')
  let deviceId = AppStorage.Get('inputDeviceId')
  console.info('bluetoothModel sppConnect inputDeviceId: ' + deviceId);
  bluetooth.sppConnect(deviceId, {
    uuid: uuid,
    secure: true,
    type: 0
  }, (code, socketID) => {
    console.log("spp client code:" + code.code)
    if (code.code != 0) {
      message = "客户端连接失败!";
      console.info('bluetoothModel sppConnect callback socketNumber(safe mode): ' + code.code);
      prompt.showToast({ message: message })
      return message;
    }
    acceptClientSocket(socketID);
    console.info('bluetoothModel sppConnect callback socketNumber(safe mode): ' + socketID);
    message = "客户端连接成功!";
    prompt.showToast({ message: message })
    return message
  })
  prompt.showToast({ message: message })
  return message
}

function acceptClientSocket(number) {
  console.info('spp clientSocket socketID: ' + number);
  // 获取的clientNumber用作服务端后续读/写操作socket的id。
  clientNumber = number;
  AppStorage.SetOrCreate("clientNumber", clientNumber)
}

function sppClientWrite(): String {
  let message = "sppClientWrite test"
  let clientNumber = AppStorage.Get("clientNumber")
  console.info('bluetoothModel sppClientWrite sppWrite clientNumber: ' + JSON.stringify(clientNumber));
  let writeClientDataBW = AppStorage.Get("writeClientDataBW")
  let writeArrayBuffer = strToArrayBuffer(writeClientDataBW)
  let writeStartTimeBW = new Date().getTime()
  const sppWriteSuccess = bluetooth.sppWrite(clientNumber, writeArrayBuffer);
  if (sppWriteSuccess) {
    console.info('sppClientWrite successfully');
    message = "sppClientWrite写入成功 ";
    AppStorage.SetOrCreate("writeStartTimeBW", writeStartTimeBW)
    sppClientReadOn()
  } else {
    console.info('sppClientWrite failed');
    message = "sppClientWrite写入失败";
  }
  prompt.showToast({ message: message })
  return message
}


function sppClientReadOn(): String {
  let message = "sppClientReadOn test"
  let clientReadMessageBW = ""
  console.info("sppClientReadOn");
  let clientNumber = AppStorage.Get("clientNumber")
  bluetooth.on('sppRead', clientNumber, (dataBuffer: ArrayBuffer) => {
    const data = new Uint8Array(dataBuffer);
    console.info('spp bluetooth read is: ' + data);
    if (data != null) {
      clientReadMessageBW += "读取成功！" + "\n"
      console.info("读取成功：" + JSON.stringify(data));
      AppStorage.SetOrCreate("dataClientReadMessageBW", clientReadMessageBW)
      prompt.showToast({ message: clientReadMessageBW })

      let readEndTimeBW = new Date().getTime()
      let writeStartTimeBW = AppStorage.Get("writeStartTimeBW")
      let sppRunTime = ""
      sppRunTime += "sppRunTime,花费:" + Number(readEndTimeBW - writeStartTimeBW) + "ms;" + "\n"
      sppRunTime += "开始:" + Number(writeStartTimeBW) + "ms; " + "结束:" + Number(readEndTimeBW) + "ms" + "\n"
      AppStorage.SetOrCreate("sppClientBandWidthBenchmarkTestMessage", sppRunTime)
      console.log("花费:" + Number(readEndTimeBW - writeStartTimeBW) + "ms;" + "\n")
      console.log("开始:" + Number(writeStartTimeBW) + "ms; " + "结束:" + Number(readEndTimeBW) + "ms" + "\n")
      sppClientReadOff()
    } else {
      message = "读取失败";
    }
    //    AppStorage.SetOrCreate("dataClientReadMessageBW",message)
    prompt.showToast({ message: message })
    return message
  });
  prompt.showToast({ message: message })
  return message
}


function sppClientReadOff(): String {
  let message = "sppClientReadOff test"
  let clientNumber = AppStorage.Get("clientNumber")
  console.info("sppClientReadOff");
  bluetooth.off('sppRead', clientNumber, (dataBuffer: ArrayBuffer) => {
    const data = new Uint8Array(dataBuffer);
    console.info('spp bluetooth read is: ' + data);
    if (data != null) {
      message = "读取成功" + JSON.stringify(data);
    } else {
      message = "读取失败";
    }
    AppStorage.SetOrCreate("dataClientReadMessageBW", message)
    prompt.showToast({ message: message })
    return message
  });
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

function loopWriteDate() {
  let a = "1";
  for (let i = 0;i < 100; i++) {
    a += "1";
  }
  return a
}

