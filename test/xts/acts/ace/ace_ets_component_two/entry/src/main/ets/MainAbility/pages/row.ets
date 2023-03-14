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
struct RowTest {
  @State alignItems: VerticalAlign = VerticalAlign.Center
  @State justifyContent: FlexAlign = FlexAlign.Start

  private stateChangCallBack = (eventData) => {
    console.info("row page stateChangCallBack");
    if (eventData != null) {
      console.info("row page state change called:" + JSON.stringify(eventData));
      if(eventData.data.alignItems != null) {
        this.alignItems = eventData.data.alignItems;
      }
      if(eventData.data.justifyContent != null) {
        this.justifyContent = eventData.data.justifyContent;
      }
    }
  }

  onPageShow() {
    console.info('row page show called');
    var stateChangeEvent = {
      eventId: 182,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  build() {
    Column({ space: 5 }) {
      Text('space').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Row({ space: 5 }) {
        Row().width('30%').height(50).backgroundColor(0xAFEEEE)
        Row().width('30%').height(50).backgroundColor(0x00FFFF)
      }.width('90%').height(107).border({ width: 1 }).key('Row')

      Text('alignItems(Top)').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Row() {
        Row().width('30%').height(50).backgroundColor(0xAFEEEE)
        Row().width('30%').height(50).backgroundColor(0x00FFFF)
      }.alignItems(VerticalAlign.Top).height('15%').border({ width: 1 })

      Text('alignItems(Center)').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Row() {
        Row().width('30%').height(50).backgroundColor(0xAFEEEE)
        Row().width('30%').height(50).backgroundColor(0x00FFFF)
      }.alignItems(VerticalAlign.Center).height('15%').border({ width: 1 })

      Text('justifyContent(End)').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Row() {
        Row().width('30%').height(50).backgroundColor(0xAFEEEE)
        Row().width('30%').height(50).backgroundColor(0x00FFFF)
      }.width('90%').border({ width: 1 }).alignItems(VerticalAlign.Bottom).justifyContent(FlexAlign.End)

      Text('justifyContent(Center)').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Row() {
        Row().width('30%').height(50).backgroundColor(0xAFEEEE)
        Row().width('30%').height(49).backgroundColor(0x00FFFF)
      }.width('90%').border({ width: 1 }).alignItems(VerticalAlign.Center).justifyContent(FlexAlign.Center)
    }.width('100%')
  }
}