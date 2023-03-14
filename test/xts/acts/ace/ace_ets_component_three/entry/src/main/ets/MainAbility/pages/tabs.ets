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
struct TabsExample {
  @State barPosition: BarPosition = BarPosition.Start
  @State index: number = 0
  @State vertical: boolean = true
  @State scrollable: boolean = true
  @State barMode: BarMode = BarMode.Fixed
  @State barWidth: number = 70
  @State barHeight: number = 150
  @State animationDuration: number = 400
  @State onActionCalled: boolean = false
  private controller: TabsController = new TabsController()

  build() {
    Column() {
      Tabs({ barPosition: this.barPosition, controller: this.controller, index: this.index }) {
        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Pink)
        }.tabBar('pink').key('tabContent1')
        .onClick(() => {
          this.onActionCalled = true;
          console.info('tabContent1 current action state is: ' + this.onActionCalled);
          try {
            var backData = {
              data: {
                "ACTION": this.onActionCalled,
              }
            }
            var backEvent = {
              eventId: 212,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("tabContent1 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("tabContent1 emit action state err: " + JSON.stringify(err.message))
          }
        })

        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Yellow)
        }.tabBar('yellow')

        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Blue)
        }.tabBar('blue')

        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Green)
        }.tabBar('green')
      }
      .vertical(this.vertical)
      .scrollable(this.scrollable)
      .barMode(this.barMode)
      .barWidth(70)
      .barHeight(150)
      .animationDuration(400)
      .key('tabs')
      .onChange((index: number) => {
        console.info(index.toString())
      })
      .width('90%')
      .backgroundColor(0xF5F5F5)
    }
    .width('100%')
    .height(150)
    .margin({ top: 5 })
  }

  onPageShow() {
    console.info('tabs page show called');
    var stateChangeEvent = {
      eventId: 213,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)

    var stateChangeEventOne = {
      eventId: 214,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventOne, this.stateChangCallBack)

    var stateChangeEventTwo = {
      eventId: 215,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("tabs page state change called:" + JSON.stringify(eventData));
      if (eventData.data.index != null) {
        this.index = parseInt(eventData.data.index);
      }
      if (eventData.data.barPosition != null) {
        this.barPosition = eventData.data.barPosition;
      }
      if (eventData.data.vertical != null) {
        this.vertical = eventData.data.vertical;
      }
      if (eventData.data.scrollable != null) {
        this.scrollable = eventData.data.scrollable;
      }
      if (eventData.data.barMode != null) {
        this.barMode = eventData.data.barMode;
      }
      if (eventData.data.barWidth != null) {
        this.barWidth = parseInt(eventData.data.barWidth);
      }
      if (eventData.data.barHeight != null) {
        this.barHeight = parseInt(eventData.data.barHeight);
      }
      if (eventData.data.animationDuration != null) {
        this.animationDuration = parseInt(eventData.data.animationDuration);
      }
    }
  }
}