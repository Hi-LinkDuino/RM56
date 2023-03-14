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
struct EllipseExample {
  @State width: string = "200px";
  @State height: string = "200px";

  onPageShow() {
    console.info('[ellipse] page show called');
    var stateChangeEvent = {
      eventId: 92,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEventTwo = {
      eventId: 907,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBack);

    var stateChangeEventThree = {
      eventId: 908,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventThree, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[ellipse] page stateChangCallBack");
    if (eventData != null) {
      console.info("[ellipse] page state change called:" + JSON.stringify(eventData));
      if (eventData.data.width != null) {
        this.width = eventData.data.width;
      }
      if (eventData.data.height != null) {
        this.height = eventData.data.height;
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Ellipse().width(`${this.width}`).height(`${this.height}`).key('ellipse')
    }.width('100%').height('100%')
  }
}