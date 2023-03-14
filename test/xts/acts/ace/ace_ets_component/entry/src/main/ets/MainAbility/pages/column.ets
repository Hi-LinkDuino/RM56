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
struct ColumnExample {
  @State alignItems: HorizontalAlign = HorizontalAlign.Start;

  onPageShow() {
    console.info('[column] page show called');
    var stateChangeEvent = {
      eventId: 192,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[column] page stateChangCallBack");
    if (eventData != null) {
      console.info("[column] page state change called:" + JSON.stringify(eventData));
      if(eventData.data.alignItems != null) {
        this.alignItems = eventData.data.alignItems;
      }
    }
  }

  build() {
    Column() {
      Text('alignItems').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Column() {
        Column().width('50%').height(30).backgroundColor(0xAFEEEE)
        Column().width('50%').height(30).backgroundColor(0x00FFFF)
      }
      .key("alignItemsColumn")
      .width('90%')
      .border({ width: 1 })
      .alignItems(this.alignItems)
    }.width('100%').padding({ top: 5 })
  }
}