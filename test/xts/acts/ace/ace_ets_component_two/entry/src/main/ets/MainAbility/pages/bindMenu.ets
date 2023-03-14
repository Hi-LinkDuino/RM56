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
struct MenuExample {
  @State onActionCalled: boolean = true
  @Builder MenuBuilder() {
    Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
      Text('text1')
        .fontSize(20)
        .width(100)
        .height(50)
        .textAlign(TextAlign.Center)
      Divider().height(10)
      Text('text2')
        .fontSize(20)
        .width(100)
        .height(50)
        .textAlign(TextAlign.Center)
      Divider().height(10)
      Button('button')
        .key('button')
        .fontSize(20)
        .width(100)
        .height(50)
        .onClick(() => {
          console.info("bindMenu button START")
          try {
            var backData = {
              data: {
                "ACTION": this.onActionCalled,
              }
            }
            var backEvent = {
              eventId: 64,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("bindMenuTest_0100 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("bindMenuTest_0100 emit action state err: " + JSON.stringify(err.message))
          }
        })
    }.width(100)
  }

  build() {
    Column() {
      Text('click for menu')
        .key('text')
    }
    .width('100%')
    .margin({ top: 5 })
    .bindMenu(this.MenuBuilder)
  }
}