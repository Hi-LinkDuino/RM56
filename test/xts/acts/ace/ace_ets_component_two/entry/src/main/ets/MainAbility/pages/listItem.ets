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
struct ListItemExample {
  private arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
  @State editFlag: boolean = false;
  @State sticky: Sticky = Sticky.Normal;
  @State editMode: boolean = true;

  onPageShow() {
    console.info('[ListItem] page show called');
    var stateChangeEvent = {
      eventId: 122,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEventOne = {
      eventId: 123,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventOne, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[ListItem] page stateChangCallBack");
    if (eventData != null) {
      console.info("[ListItem] page state change called:" + JSON.stringify(eventData));
      if(eventData.data.sticky != null) {
        this.sticky = eventData.data.sticky;
      }
    }
    if (eventData != null) {
      console.info("[ListItem] page state change called:" + JSON.stringify(eventData));
      if(eventData.data.editMode != null) {
        this.editMode = eventData.data.editMode;
      }
    }
  }

  build() {
    Column() {
      List({ space: 20, initialIndex: 0 }) {
        ListItem() {
          Text('sticky:Normal , click me edit list')
            .width('100%').height(40).fontSize(12).fontColor(0xFFFFFF)
            .textAlign(TextAlign.Center).backgroundColor(0x696969)
            .onClick(() => {
              this.editFlag = !this.editFlag
            })
        }.sticky(this.sticky).key('ListItem')

        ForEach(this.arr, (item) => {
          ListItem() {
            Text('' + item)
              .width('100%').height(100).fontSize(16)
              .textAlign(TextAlign.Center).borderRadius(10).backgroundColor(0xFFFFFF)
          }.editable(this.editFlag)
        }, item => item)
      }
      .key('List')
      .editMode(this.editMode)
      .onItemDelete((index: number) => {
        console.info(this.arr[index - 1] + 'Delete')
        this.arr.splice(index - 1,1)
        this.editFlag = false
        return true
      })
      .width('90%')
    }.width('100%').height('100%').backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}