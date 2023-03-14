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
struct BackgroundExample {
  @State width: string = "200px";
  @State height: string = "200px";
  @State color: Array = [[0x080808], [0xEEEE00], [0xFF0000]];
  @State width: string = 80;
  @State height: string = 80;
  @State padding: string = 20;
  @State margin: string = 20;
  @State sizeWidth: string = '100%';
  @State sizeHeight: string = '100%';
  @State layoutWeight1: string = 1;
  @State layoutWeight2: string = 2;

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("gauge page state change called:" + JSON.stringify(eventData));
      if(eventData.data.colorValues != null) {
        this.colorValues = JSON.parse(eventData.data.colorValues);
      }
      if(eventData.data.width != null) {
        this.width = eventData.data.width;
      }
      if(eventData.data.height != null) {
        this.height = eventData.data.height;
      }
      if(eventData.data.srcValue != null) {
        this.srcValue = eventData.data.srcValue;
      }
    }
  }

  private stateChangCallBackTwo = (eventData) => {
    if (eventData != null) {
      console.info("Background page state change called:" + JSON.stringify(eventData));
      if(eventData.data.backgroundColor != null) {
        this.backgroundColor = eventData.data.backgroundColor;
      }
      if(eventData.data.backgroundImage != null) {
        this.backgroundImage = eventData.data.backgroundImage;
      }
      if(eventData.data.backgroundImageSize != null) {
        this.backgroundImageSize = eventData.data.backgroundImageSize;
      }
      if(eventData.data.backgroundImagePosition != null) {
        this.backgroundImagePosition = eventData.data.backgroundImagePosition;
      }
    }
  }

  private stateChangCallBackOne = (eventData) => {
    console.info("size page stateChangCallBack" + JSON.stringify(eventData));
    if (eventData) {
      this.width = eventData.data.width;
      this.height = eventData.data.height;
      this.padding = eventData.data.padding;
      this.margin = eventData.data.margin;
    }
  }

  private sizeChangCallBack = (eventData) => {
    console.info("size page sizeChangCallBack" + JSON.stringify(eventData));
    if (eventData) {
      this.sizeWidth = eventData.data.width;
      this.sizeHeight = eventData.data.height;
    }
  }

  private weightChangCallBack = (eventData) => {
    console.info("size page weightChangCallBack" + JSON.stringify(eventData));
    if (eventData) {
      this.layoutWeight1 = eventData.data.weight;
    }
  }

  onPageShow() {
    console.info('background page show called');
    var stateChangeEvent = {
      eventId: 1,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEventTwo = {
      eventId: 2,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBackTwo);

    let stateChangeEventOne = {
      eventId: 34,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventOne, this.stateChangCallBackOne)

    let sizeChangeEvent = {
      eventId: 35,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(sizeChangeEvent, this.sizeChangCallBack)

    let weightChangeEvent = {
      eventId: 36,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(weightChangeEvent, this.weightChangCallBack)
  }

  build() {
    Column({ space: 5 }) {
      Text('background color').fontSize(9).width('90%').fontColor(0xFF0000).key("fontColor")
      Row().width('90%').height(50).backgroundColor(0xEEEE00).key("backgroundColor").border({ width: 1 })
      Text('background image repeat along X').fontSize(9).width('90%').fontColor(0xCCCCCC)
      Row()
        .backgroundImage('/images/bg.jpg', ImageRepeat.X)
        .backgroundImageSize({ width: '250px', height: '140px' })
        .width('90%')
        .height(70)
        .border({ width: 1 })
      Text('background image repeat along Y').fontSize(9).width('90%').fontColor(0xFF0000)
      Row()
        .backgroundImage('/images/bg.jpg', ImageRepeat.Y)
        .backgroundImageSize({ width: '500px', height: '120px' })
        .width('90%')
        .height(100)
        .border({ width: 1 })
      Text('background image size').fontSize(9).width('90%').fontColor(0xFF0000)
      Row()
        .width('90%').height(150)
        .backgroundImage('/images/bg.jpg', ImageRepeat.NoRepeat)
        .backgroundImageSize({ width: 1000, height: 500 })
        .border({ width: 1 })
      Text('background fill the box(Cover)').fontSize(9).width('90%').fontColor(0xFF0000)
      Row()
        .width(200)
        .height(50)
        .backgroundImage('/images/bg.jpg', ImageRepeat.NoRepeat)
        .backgroundImageSize(ImageSize.Cover)
        .border({ width: 1 })
      Text('background fill the box(Contain)').fontSize(9).width('90%').fontColor(0xFF0000)
      Row()
        .width(200)
        .height(50)
        .backgroundImage('/images/bg.jpg', ImageRepeat.NoRepeat)
        .backgroundImageSize(ImageSize.Contain)
        .border({ width: 1 })
      Text('background image position').fontSize(9).width('90%').fontColor(0xFF0000)
      Row()
        .width(100)
        .height(50)
        .backgroundImage('/images/bg.jpg', ImageRepeat.NoRepeat)
        .backgroundImageSize({ width: 1000, height: 560 })
        .backgroundImagePosition({ x: -500, y: -300 })
        .border({ width: 1 })

      Text('margin and padding:').fontSize(26).fontColor(Color.Black).width('90%')
      Row() {
        Row() {
          Row()
            .size({ width: this.sizeWidth, height: this.sizeHeight })
            .backgroundColor(0xAFEEEE)
            .key('innerRow')
        }
        .key('centerRow')
        .width(this.width)
        .height(this.height)
        .padding(this.padding)
        .margin(this.margin)
        .backgroundColor(0xFDF5E6)
      }.backgroundColor(0xFFA500)

      Text('layoutWeight')
        .fontSize(26)
        .fontColor(Color.Black)
        .width('90%')
      Row() {
        Text('layoutWeight(1)')
          .size({ width: '30%', height: 110 })
          .backgroundColor(0xFFEFD5)
          .textAlign(TextAlign.Center)
          .layoutWeight(this.layoutWeight1)
          .fontSize(20)
          .key('layoutWeight1')
        Text('layoutWeight(2)')
          .size({ width: '30%', height: 110 })
          .backgroundColor(0xF5DEB3)
          .textAlign(TextAlign.Center)
          .layoutWeight(this.layoutWeight2)
          .fontSize(20)
          .key('layoutWeight2')
        Text('no layoutWeight')
          .size({ width: '30%', height: 110 })
          .backgroundColor(0xD2B48C)
          .textAlign(TextAlign.Center)
          .fontSize(20)
      }.size({ width: '90%', height: 140 })
      .backgroundColor(0xAFEEEE)

      Text('constraintSize')
        .fontSize(26)
        .fontColor(Color.Black)
        .width('90%')
      Row() {
        Row()
          .constraintSize({minWidth: '10%', maxWidth: '50%', height: 70})
          .backgroundColor(0xFFA500)
        Row()
          .size({width: '30%', height:70})
          .backgroundColor(0xAFEEEE)
      }.size({ width: '90%', height: 140 })
      .backgroundColor(0xAFEEEE)
    }.width('100%').height('100%').padding({ top: 5 })
  }
}