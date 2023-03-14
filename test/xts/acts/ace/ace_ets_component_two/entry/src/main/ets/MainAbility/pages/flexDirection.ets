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
struct flexDirectionExample {
  @State width: string = '20%'
  @State height: number = 40
  @State backgroundColor: Color = 0xD2B48C

  build() {
    Column() {
      Column({ space: 5 }) {
        Text('direction:Row').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({ direction:FlexDirection.Row }) {
          Text('1').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('2').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('3').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('4').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          .key('Row')
        }.height(70).width('90%').padding(10).backgroundColor(0xAFEEEE)

        Text('direction:Column').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({ direction: FlexDirection.Column }) {
          Text('1').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('2').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('3').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('4').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          .key('Column')
        }
        .height(160)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)

        Text('direction:ColumnReverse').fontSize(9).fontColor(0xCCCCCC).width('90%')
        Flex({ direction: FlexDirection.ColumnReverse }) {
          Text('1').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('2').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('3').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          Text('4').width(this.width).height(this.height).backgroundColor(this.backgroundColor)
          .key('ColumnReverse')
        }
        .height(160)
        .width('90%')
        .padding(10)
        .backgroundColor(0xAFEEEE)
      }.width('100%').margin({ top: 5 })
    }.width('100%')
  }
  onPageShow() {
    console.info('flexDirection page show called');
    var stateChangeEvent1 = {
      eventId: 97,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent1, this.stateChangCallBack)
    var stateChangeEvent2 = {
      eventId: 98,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent2, this.stateChangCallBack)
    var stateChangeEvent3 = {
      eventId: 99,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent3, this.stateChangCallBack)
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