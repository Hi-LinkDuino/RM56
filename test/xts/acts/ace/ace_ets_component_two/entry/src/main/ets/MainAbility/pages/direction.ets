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
struct Index {
  @State direction:FlexDirection = FlexDirection.Column
  build() {
    Flex({ direction: this.direction, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('Hello World')
        .fontSize(60)
        .fontWeight(500)
      Button('Next')
        .fontSize(40)
        .fontWeight(500)
        .width(280)
        .height(60)
    }
    .key('direction')
    .width('100%')
    .height('100%')
  }

  onPageShow() {
    console.info('direction page show called');
    var stateChangeEvent = {
      eventId: 84,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("direction page state change called:" + JSON.stringify(eventData));
      if (eventData.data.direction != null) {
        this.direction = eventData.data.direction;
      }
    }
  }
}