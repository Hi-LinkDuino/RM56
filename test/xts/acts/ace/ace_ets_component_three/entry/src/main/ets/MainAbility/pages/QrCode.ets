// @ts-nocheck
/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import events_emitter from '@ohos.emitter';

@Entry
@Component
struct QrCode {
  @State qrCodeBackgroundColor: Color = "#FF008000";
  @State qrCodeColor: Color = "#FF0000FF";
  @State qrCodeValue: string = 'qrCodeValue';
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("qrCode page state change called:" + JSON.stringify(eventData));
      var colorValue = eventData.data.color;
      if (colorValue != null && colorValue.length != 0) {
        this.qrCodeColor = colorValue;
        console.info("qrCodeColor:" + qrCodeColor);
      } else {
        console.info("qrCodeColor is null or empty " + qrCodeColor);
      }
    } else {
      console.info("qrCode page color not change called:" + JSON.stringify(eventData));
    }
  }
  private stateChangCallBackTwo = (eventData) => {
    if (eventData != null) {
      console.info("qrCode page state change called:" + JSON.stringify(eventData));
      var backgroundColorValue = eventData.data.backgroundColor;
      if (backgroundColorValue != null && backgroundColorValue.length != 0) {
        this.qrCodeBackgroundColor = backgroundColorValue;
        console.info("qrCodeBackgroundColor:" + qrCodeBackgroundColor);
      } else {
        console.info("backgroundColorValue is null or empty " + qrCodeColor);
      }
    } else {
      console.info("qrCode page backgroundColorValue not change called:" + JSON.stringify(eventData));
    }
  }

  onPageShow() {
    console.info('qrCode page show called');
    var stateChangeEvent = {
      eventId: 170,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEvent = {
      eventId: 171,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBackTwo);

    var stateChangeEvent = {
      eventId: 1023,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEvent = {
      eventId: 1024,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEvent = {
      eventId: 1025,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBackTwo);

    var stateChangeEvent = {
      eventId: 1026,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBackTwo);
  }

  build() {
    Column({ space: 5 }) {
      Text('QRCodeColor')
        .fontSize(40)
        .fontWeight(FontWeight.Bold)
        .width('90%')
        .fontColor(0xCCCCCC)
      QRCode(`${this.qrCodeValue}`)
        .backgroundColor(`${this.qrCodeBackgroundColor}`)
        .color(`${this.qrCodeColor}`)
        .width(198)
        .height(198)
        .key("QrCodeColor")
      Text('QRCodeBackgroundColor')
        .fontSize(40)
        .fontWeight(FontWeight.Bold)
        .width('90%')
        .fontColor(0xCCCCCC)
      QRCode(`${this.qrCodeValue}`)
        .backgroundColor(`${this.qrCodeBackgroundColor}`)
        .color(`${this.qrCodeColor}`)
        .width(197)
        .height(197)
        .key("QrCodeBackgroundColor")
    }
    .width('100%')
    .height('100%')
    .margin({ top: 5 })
  }
}