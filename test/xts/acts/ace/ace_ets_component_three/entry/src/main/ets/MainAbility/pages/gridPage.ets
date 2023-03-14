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
struct gridPage {
  @State numbers: string[] = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15']
  @State columnsGap: string= '2px'
  @State rowsGap: string= '2px'
  @State columnsTemplate: string = '1fr 1fr 1fr 1fr 1fr'
  @State rowsTemplate: string = '1fr 1fr 1fr 1fr 1fr'
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("on state change called " + JSON.stringify(eventData))
      if (eventData.data.columnsGap != null) {
        this.columnsGap = eventData.data.columnsGap
      }
      if (eventData.data.rowsGap != null) {
        this.rowsGap = eventData.data.rowsGap
      }
      if (eventData.data.columnsTemplate != null) {
        this.columnsTemplate = eventData.data.columnsTemplate
      }
      if (eventData.data.rowsTemplate != null) {
        this.rowsTemplate = eventData.data.rowsTemplate
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('Hello World')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Column() {
        Grid() {
          GridItem() {
            Text('4')
              .key('grid_item_4')
              .fontSize(16)
              .backgroundColor(0xFAEEE0)
              .width('100%')
              .height('100%')
              .textAlign(TextAlign.Center)
          }.rowStart(1).rowEnd(4)

          ForEach(this.numbers, (item) => {
            GridItem() {
              Text(item)
                .fontSize(16)
                .backgroundColor(0xF9CF93)
                .width('100%')
                .height('100%')
                .textAlign(TextAlign.Center)
            }.forceRebuild(false)
          }, item => item)
          GridItem() {
            Text('5')
              .fontSize(16)
              .backgroundColor(0xDBD0C0)
              .width('100%')
              .height('100%')
              .textAlign(TextAlign.Center)
          }.columnStart(1).columnEnd(5)
        }
        .key('grid')
        .columnsGap(this.columnsGap)
        .rowsGap(this.rowsGap)
        .columnsTemplate(this.columnsTemplate)
        .rowsTemplate(this.rowsTemplate)
        .width('90%')
        .height(300)
      }.width('100%').margin({ top: 5 })
    }
    .width('100%')
    .height('100%')
  }

  onPageShow() {
    console.info('button page show called');
    var stateChangeEvent = {
      eventId: 110,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
    var stateChangeEvent2 = {
      eventId: 111,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent2, this.stateChangCallBack)
    var stateChangeEvent3 = {
      eventId: 112,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent3, this.stateChangCallBack)
    var stateChangeEvent4 = {
      eventId: 113,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent4, this.stateChangCallBack)
  }
}