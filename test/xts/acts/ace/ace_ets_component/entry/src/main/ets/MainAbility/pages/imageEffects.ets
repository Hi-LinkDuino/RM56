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

import events_emitter from '@ohos.emitter'

@Entry
@Component
struct ImageEffectsExample {
  @State blurValue: string = 100;
  @State backdropBlueValue: string= 30;
  @State shadowValue: object= { radius: 10, color: Color.Gray, offsetX: 5, offsetY: 5 }
  @State grayscaleValue: string= 10;
  @State brightnessValue: string= 20;
  @State saturateValue: string= 50;
  @State contrastValue: string= 20;
  @State invertValue: string= 10;
  @State hueRotateValue: string= 10
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("imageEffects page state change called:" + JSON.stringify(eventData));
      if (eventData.data.blurValue != null) {
        this.blurValue = eventData.data.blurValue;
      }
      if (eventData.data.shadowValue != null) {
        this.shadowValue = JSON.parse(eventData.data.shadowValue);
      }
      if (eventData.data.grayscaleValue != null) {
        this.grayscaleValue = eventData.data.grayscaleValue;
      }
      if (eventData.data.brightnessValue != null) {
        this.brightnessValue = eventData.data.brightnessValue;
      }
      if (eventData.data.saturateValue != null) {
        this.saturateValue = eventData.data.saturateValue;
      }
      if (eventData.data.contrastValue != null) {
        this.contrastValue = eventData.data.contrastValue;
      }
      if (eventData.data.invertValue != null) {
        this.invertValue = eventData.data.invertValue;
      }
      if (eventData.data.hueRotateValue != null) {
        this.hueRotateValue = eventData.data.hueRotateValue;
      }
    }
  }

  onPageShow() {
    console.info('imageEffects page show called');
    var stateChangeEvent = {
      eventId: 12,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEvent2 = {
      eventId: 13,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent2, this.stateChangCallBack);

    var stateChangeEvent3 = {
      eventId: 14,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent3, this.stateChangCallBack);

    var stateChangeEvent4 = {
      eventId: 15,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent4, this.stateChangCallBack);

    var stateChangeEvent5 = {
      eventId: 16,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent5, this.stateChangCallBack);

    var stateChangeEvent6 = {
      eventId: 17,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent6, this.stateChangCallBack);

    var stateChangeEvent7 = {
      eventId: 18,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent7, this.stateChangCallBack);

    var stateChangeEvent8 = {
      eventId: 19,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent8, this.stateChangCallBack);
  }

  build() {
    Column({ space: 10 }) {
      Text('font blur').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Text('text')
        .blur(this.blurValue)
        .width('90%')
        .height(40)
        .fontSize(16)
        .backgroundColor(0xF9CF93)
        .padding({ left: 5 })
        .key('centerRow')

      Text('backdropBlur').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Text()
        .width('90%')
        .height(40)
        .fontSize(16)
        .backdropBlur(this.backdropBlueValue)
        .backgroundImage('/images/download.png')
        .backgroundImageSize({ width: 1200, height: 160 })
        .key('centerRow2')

      Text('shadow').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Image('/images/download.png')
        .width('90%')
        .height(40)
        .shadow(this.shadowValue)
        .key('centerRow3')

      Text('grayscale').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Image('/images/download.png')
        .width('90%')
        .height(40)
        .grayscale(`${this.grayscaleValue}`)
        .key('centerRow4')

      Text('brightness').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Image('/images/download.png').width('90%').height(40).brightness(this.brightnessValue).key('centerRow5')

      Text('saturate').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Image('/images/download.png').width('90%').height(40).saturate(this.saturateValue).key('centerRow6')

      Text('contrast').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Image('/images/download.png').width('90%').height(40).contrast(this.contrastValue).key('centerRow7')

      Text('invert').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Image('/images/download.png').width('90%').height(40).invert(`${this.invertValue}`).key('centerRow8')

      Text('hueRotate').fontSize(15).fontColor(0xCCCCCC).width('90%')
      Image('/images/download.png').width('90%').height(40).hueRotate(`${this.hueRotateValue}`).key('centerRow9')
    }.width('100%').margin({ top: 5 })
  }
}
