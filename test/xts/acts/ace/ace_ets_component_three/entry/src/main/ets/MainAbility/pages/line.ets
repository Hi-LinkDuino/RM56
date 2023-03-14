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
struct LineExample {
  @State startPoint: Point = [0, 0]
  @State endPoint: Point = [50, 100]

  build() {
    Column() {
      Line({ width: 50, height: 100 }).startPoint(this.startPoint).endPoint(this.endPoint)
        .key('line')
    }.margin({ top: 5 })
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("line page state change called:" + JSON.stringify(eventData));
      if (eventData.data.startPoint != null) {
        this.startPoint[0] = eventData.data.startPoint;
      }
      if (eventData.data.startPointOne != null) {
        this.startPoint[1] = eventData.data.startPointOne;
      }
      if (eventData.data.endPointOne != null) {
        this.endPoint[1] = eventData.data.endPointOne;
      }
      if (eventData.data.endPoint != null) {
        this.endPoint[0] = eventData.data.endPoint;
      }
    }
  }

  onPageShow() {
    console.info('line page show called');
    var stateChangeEvent = {
      eventId: 131,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }
}