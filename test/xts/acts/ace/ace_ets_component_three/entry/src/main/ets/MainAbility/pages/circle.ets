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
struct CircleExample {
  @State width: string = "450px";
  @State height: string = "450px";

  onPageShow() {
    console.info('[circle] page show called');
    var stateChangeEvent = {
      eventId: 77,
      priority: events_emitter.EventPriority.LOW
    }
    console.info('[circle] page show calledA');
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[circle] page stateChangCallBack");
    if (eventData != null) {
      console.info("[circle] page state change called:" + JSON.stringify(eventData));
      if (eventData.data.height != null) {
        this.height = eventData.data.height;
      }
      if (eventData.data.width != null) {
        this.width = eventData.data.width;
      }
    }
  }

  build() {
    Flex({ justifyContent: FlexAlign.SpaceAround }) {
      Circle({ width: 150, height: 150 })
      Circle()
        .width(`${this.width}`)
        .height(`${this.height}`)
        .key('circle')
    }.width('100%').height('100%').margin({ top: 5 })
  }
}
