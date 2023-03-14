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
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("gauge page state change called:" + JSON.stringify(eventData));
      if (eventData.data.colorValues != null) {
        this.colorValues = JSON.parse(eventData.data.colorValues);
      }
      if (eventData.data.width != null) {
        this.width = eventData.data.width;
      }
      if (eventData.data.height != null) {
        this.height = eventData.data.height;
      }
      if (eventData.data.srcValue != null) {
        this.srcValue = eventData.data.srcValue;
      }
    }
  }
  private stateChangCallBackTwo = (eventData) => {
    if (eventData != null) {
      console.info("Background page state change called:" + JSON.stringify(eventData));
      if (eventData.data.backgroundColor != null) {
        this.backgroundColor = eventData.data.backgroundColor;
      }
      if (eventData.data.backgroundImage != null) {
        this.backgroundImage = eventData.data.backgroundImage;
      }
      if (eventData.data.backgroundImageSize != null) {
        this.backgroundImageSize = eventData.data.backgroundImageSize;
      }
      if (eventData.data.backgroundImagePosition != null) {
        this.backgroundImagePosition = eventData.data.backgroundImagePosition;
      }
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
  }

  build() {
    Column({ space: 5 }) {
      Text('background color').fontSize(9).width('90%').fontColor(0xFF0000).key("fontColor")
      Row()
        .width('90%')
        .height(50)
        .backgroundColor(0xEEEE00)
        .key("backgroundColor")
        .border({ width: 1 })
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
        .width('90%')
        .height(150)
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
    }
    .width('100%').height('100%').padding({ top: 5 })
  }
}