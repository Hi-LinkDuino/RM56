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
struct FlexWrapExample {
  @State width: string = '20%'
  @State height: number = 40
  @State backgroundColor: Color = 0xD2B48C

  build() {
    Column() {
      Column({ space: 5 }) {
        Text('WrapReverse').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({ wrap: FlexWrap.WrapReverse, direction: FlexDirection.Row }) {
          Text('1').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('2').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('3').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
            .key('WrapReverse')
        }
        .width('90%')
        .height(120)
        .padding(10)
        .backgroundColor(0xAFEEEE)
      }.width('100%').margin({ top: 5 })
    }.width('100%')
  }
  onPageShow() {
    console.info('flexDirection page show called');
    var stateChangeEvent = {
      eventId: 100,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("swiper page state change called:" + JSON.stringify(eventData));
      if (eventData.data.width != null) {
        this.width = eventData.data.width;
      }
      if (eventData.data.height != null) {
        this.height = eventData.data.height;
      }
      if (eventData.data.backgroundColor != null) {
        this.backgroundColor = eventData.data.backgroundColor;
      }
    }
  }
}