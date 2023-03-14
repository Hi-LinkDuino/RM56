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
struct RatingExample {
  @State rating: number = 1
  @State indicator: boolean = false
  private stateChangCallBack = (eventData) => {
    console.info("rating page stateChangCallBack");
    if (eventData != null) {
      console.info("rating page state change called:" + JSON.stringify(eventData.data.rating));
      if (eventData.data.rating != null) {
        this.rating = eventData.data.rating;
      }
    }
  }

  onPageShow() {
    console.info('rating page show called');
    var stateChangeEvent = {
      eventId: 173,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)

    var stateChangeEventTwo = {
      eventId: 174,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBack)

    var stateChangeEventThree = {
      eventId: 175,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventThree, this.stateChangCallBack)

    var stateChangeEventFour = {
      eventId: 176,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventFour, this.stateChangCallBack)

    var stateChangeEventSix = {
      eventId: 177,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventSix, this.stateChangCallBack)

    var stateChangeEventSeven = {
      eventId: 178,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventSeven, this.stateChangCallBack)

    var stateChangeEventSeven = {
      eventId: 1027,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventSeven, this.stateChangCallBack)

    var stateChangeEventSeven = {
      eventId: 1028,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventSeven, this.stateChangCallBack)
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Text('current score is ' + this.rating).fontSize(20)
      Rating({ rating: this.rating, indicator: this.indicator })
        .stars(5).key('Rating')
        .stepSize(0.5)
        .onChange((value: number) => {
          this.rating = value
        })
    }.width(350).height(200).padding(35)
  }
}
