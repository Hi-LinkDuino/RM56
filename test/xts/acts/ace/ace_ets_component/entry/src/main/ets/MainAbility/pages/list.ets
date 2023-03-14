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
struct ListExample {
  private arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
  @State listDirection: Axis = Axis.Vertical;
  @State editMode: boolean = false;
  @State edgeEffect: EdgeEffect = EdgeEffect.None;
  @State chainAnimation: boolean = false;
  @State strokeWidth: number = 2.000000;
  @State color: string = 0xFFFFFF;
  @State startMargin: number = 20.000000;
  @State endMargin: number = 20.000000;

  onPageShow() {
    console.info('[list] page show called');
    var stateChangeEvent = {
      eventId: 124,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEventTwo = {
      eventId: 125,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBackTwo);

    var stateChangeEvent3 = {
      eventId: 901,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent3, this.stateChangCallBackTwo);

    var stateChangeEvent4 = {
      eventId: 902,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent4, this.stateChangCallBackTwo);

    var stateChangeEvent5 = {
      eventId: 903,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent5, this.stateChangCallBackTwo);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[stateChangCallBack] stateChangCallBack");
    if (eventData != null) {
      console.info("[stateChangCallBack] state change called:" + JSON.stringify(eventData));
      if (eventData.data.listDirection != null) {
        this.listDirection = eventData.data.listDirection;
      }
      if (eventData.data.editMode != null) {
        this.editMode = eventData.data.editMode;
      }
      if (eventData.data.edgeEffect != null) {
        this.edgeEffect = eventData.data.edgeEffect;
      }
      if (eventData.data.chainAnimation != null) {
        this.chainAnimation = eventData.data.chainAnimation;
      }
    }
  }
  private stateChangCallBackTwo = (eventData) => {
    console.info("[stateChangCallBackTwo] stateChangCallBack");
    if (eventData != null) {
      console.info("[stateChangCallBackTwo] state change called:" + JSON.stringify(eventData));
      if (eventData.data.strokeWidth != null) {
        this.strokeWidth = eventData.data.strokeWidth;
      }
      if (eventData.data.color != null) {
        this.color = eventData.data.color;
      }
      if (eventData.data.startMargin != null) {
        this.startMargin = eventData.data.startMargin;
      }
      if (eventData.data.endMargin != null) {
        this.endMargin = eventData.data.endMargin;
      }
    }
  }

  build() {
    Stack({ alignContent: Alignment.TopStart }) {
      Column() {
        List({ space: 20, initialIndex: 0 }) {
          ForEach(this.arr, (item) => {
            ListItem() {
              Text('' + item)
                .width('100%')
                .height(100)
                .fontSize(16)
                .textAlign(TextAlign.Center)
                .borderRadius(10)
                .backgroundColor(0xFFFFFF)
            }.editable(true)
          }, item => item)
        }
        .key('list')
        .listDirection(this.listDirection)
        .divider({
          strokeWidth: this.strokeWidth,
          color: this.color,
          startMargin: this.startMargin,
          endMargin: this.endMargin
        })
        .edgeEffect(this.edgeEffect)
        .chainAnimation(this.chainAnimation)
        .multiSelectable(true)
        .onScrollIndex((firstIndex: number, lastIndex: number) => {
          console.info('first' + firstIndex)
          console.info('last' + lastIndex)
        })
        .onScroll((scrollOffset: number, scrollState: ScrollState) => {
          console.info('scrollOffset:' + scrollOffset)
          console.info('scrollState:' + scrollState)
        })
        .editMode(this.editMode)
        .onItemDelete((index: number) => {
          console.info(this.arr[index] + 'Delete')
          this.arr.splice(index, 1)
          console.info(JSON.stringify(this.arr))
          this.editFlag = false
          return true
        })
        .width('90%')
      }.width('100%')

      Button('edit list')
        .onClick(() => {
          this.editMode = !this.editMode
        }).margin({ top: 5, left: 20 })
    }.width('100%').height('100%').backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}