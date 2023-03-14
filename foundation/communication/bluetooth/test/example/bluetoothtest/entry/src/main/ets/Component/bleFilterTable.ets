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
import { TestData, TestApi } from '../MainAbility/model/testData'
import { initBRApiData } from '../MainAbility/model/testDataModels'

/**
  * BleFilterTable Of Bluetooth test
  */

@Component
export struct BleFilterTable {
  private testItem: TestData
  @State apiItems: TestApi[] = initBRApiData()
  @State changeIndex: number = -1
  // input ble scan parameters:
  /*ScanFilter*/
  @State cbxBleScanFilter: boolean = false;
  @State txtScanFilter_deviceId: string = "08:FB:EA:1B:38:AB";//00:1A:7D:DA:71:15
  @State txtScanFilter_name: string = "dudu-tiger";
  @State txtScanFilter_serviceUuid: string = "0000180A-0000-1000-8000-00805f9b34fb";
  /*ScanOptions*/
  @State cbxBleScanOptions: boolean = false;
  @State txtScanOptions_interval: string = "0";

  getCurrentState(index: number) {
    //    console.log("currentState:" + index.toString() + this.apiItems[index].result)
    return this.apiItems[index].result
  }

  aboutToAppear() {
    AppStorage.SetOrCreate('txtScanFilterDeviceId', this.txtScanFilter_deviceId);
    AppStorage.SetOrCreate('txtScanFilterName', this.txtScanFilter_name);
    AppStorage.SetOrCreate('txtScanFilterServiceUuid', this.txtScanFilter_serviceUuid);
    AppStorage.SetOrCreate('txtScanOptions_interval', this.txtScanOptions_interval);
    AppStorage.SetOrCreate('txtScanOptionsDutyMode', '0');
    AppStorage.SetOrCreate('txtScanOptionsMatchMode', '0');
    AppStorage.SetOrCreate('cbxBleScanFilter', this.cbxBleScanFilter);
    AppStorage.SetOrCreate('cbxBleScanOptions', this.cbxBleScanOptions);
  }
  build() {
    Scroll() {
      Column() {
        Row() {
          Text("BLE扫描过滤参数:")
            .fontSize("17vp")
            .fontColor("#4444444")
            .width("80%")
            .padding({left: 10})
          Checkbox({ name: 'bleScanFilter', group: 'checkboxGroup99' })
            .select(this.cbxBleScanFilter)
            .selectedColor(0x39a2db)
            .onChange((value) => {
              if (value) {
                this.cbxBleScanFilter = true;
              } else {
                this.cbxBleScanFilter = false;
              }
              AppStorage.SetOrCreate('cbxBleScanFilter', value);
            })
            .width(15)
        }
        .height(30)
        .width("100%")
        .borderWidth(1)
        .borderColor($r("app.color.lead"))

        Column() {
          Column() {
            Column() {
              Row(){
                Text("mac").fontSize("17vp").width(60);
                TextInput({ text: this.txtScanFilter_deviceId, placeholder: "input deviceId" })
                  .fontSize("15vp")
                  .onChange((strInput) => {
                    this.txtScanFilter_deviceId = strInput;
                    AppStorage.SetOrCreate('txtScanFilterDeviceId', strInput);
                  })
                  .width("80%")
                  .borderRadius(1)
              }
              .padding(5)
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)

              Row(){
                Text("name").fontSize(17).width(60);
                TextInput({ text: this.txtScanFilter_name, placeholder: "input name" })
                  .fontSize("15vp")
                  .onChange((strInput) => {
                    this.txtScanFilter_name = strInput;
                    AppStorage.SetOrCreate('txtScanFilterName', strInput);
                  })
                  .width("80%")
                  .borderRadius(1)
              }
              .padding(5)
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)

              Row() {
                Text("serviceUuid").fontSize(17).width(60);
                TextInput({ text: this.txtScanFilter_serviceUuid, placeholder: "input serviceUuid" })
                  .fontSize("15vp")
                  .onChange((strInput) => {
                    this.txtScanFilter_serviceUuid = strInput;
                    AppStorage.SetOrCreate('txtScanFilterServiceUuid', strInput);
                  })
                  .width("80%")
                  .borderRadius(1)
              }
              .padding(5)
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)
            }
            .height(180)
            .width('100%')
            .padding(5)
          }
          .borderColor($r("app.color.lead"))
          .borderWidth(1)

          Column() {
            Row() {
              Text("BLE扫描配置:")
                .padding("5vp")
                .fontSize("17vp")
                .fontColor("#4444444")
                .width("80%")
              Checkbox({ name: 'bleScanOptions', group: 'checkboxGroup95' })
                .select(this.cbxBleScanOptions)
                .selectedColor(0x39a2db)
                .onChange((value) => {
                  if (value) {
                    this.cbxBleScanOptions = true;
                    this.cbxBleScanFilter = true;
                    AppStorage.SetOrCreate('cbxBleScanOptions', this.cbxBleScanOptions);
                    AppStorage.SetOrCreate('cbxBleScanFilter', this.cbxBleScanFilter);
                  } else {
                    this.cbxBleScanOptions = false;
                    AppStorage.SetOrCreate('cbxBleScanOptions', this.cbxBleScanOptions);
                  }
                })
                .width("15vp")
                .margin({ left: "1vp" })
            }
            .height(40)
            .width('100%')
            .padding(5);
            Column() {
              Row() {
                Text("interval").fontSize(17).width(80)
                TextInput({ text: this.txtScanOptions_interval, placeholder: "input interval" })
                  .fontSize("17vp")
                  .onChange((strInput) => {
                    this.txtScanOptions_interval = strInput;
                    AppStorage.SetOrCreate('txtScanOptionsInterval', strInput);
                  })
                  .width("80%")
              }
              .padding(5)
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)

              Row() {
                Text("dutyMode").fontSize(17).width(80)
                Column() {
                  Select([
                    {value:'LOW_POWER'},
                    {value:'BALANCED'},
                    {value:'LOW_LATENCY'}
                  ])
                    .selected(2)
                    .value('LOW_POWER')
                    .font({size: 17})
                    .selectedOptionFont({size: 17})
                    .optionFont({size: 15})
                    .width("80%")
                    .onSelect((index:number)=>{
                      console.info("Select:" + index)
                      AppStorage.SetOrCreate('txtScanOptionsDutyMode', index);
                    })
                }
                .width("80%")
              }
              .padding(5)
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)

              Row() {
                Text("matchMode").fontSize(17).width(80)
                Column() {
                  Select([
                    {value:'MATCH_MODE_AGGRESSIVE'},
                    {value:'MATCH_MODE_STICKY'}
                  ])
                    .value('MATCH_MODE_AGGRESSIVE')
                    .font({size: 17})
                    .selectedOptionFont({size: 17})
                    .optionFont({size: 15})
                    .width("80%")
                    .onSelect((index:number)=>{
                      console.info("Select:" + index)
                      AppStorage.SetOrCreate('txtScanOptionsMatchMode', index);
                    })
                }.width("80%")
              }
              .padding(5)
              .justifyContent(FlexAlign.Start)
              .alignItems(VerticalAlign.Center)
            }
            .width('100%')
            .padding(5)
          }
          .height(240)
          .borderWidth(1)
          .borderColor($r("app.color.lead"))
        }
      }
      .height("100%")
      .backgroundColor($r("app.color.white"))
    }
    .scrollBarWidth(20)
    .padding({ bottom: 10})
  }
}