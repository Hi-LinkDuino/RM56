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
struct GaugeExample {
  @State gaugeValue: string = 50;
  @State strokeWidthValue: string = 20;
  @State startAngleValue: string = 210;
  @State endAngleValue: string = 150;
  @State colorValues: Array = [[0x317AF7, 1], [0x5BA854, 1], [0xE08C3A, 1], [0x9C554B, 1], [0xD94838, 1]];
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("gauge page state change called:" + JSON.stringify(eventData));
      if (eventData.data.gaugeValue != null) {
        this.gaugeValue = eventData.data.gaugeValue;
      }
      if (eventData.data.strokeWidthValue != null) {
        this.strokeWidthValue = eventData.data.strokeWidthValue;
      }
      if (eventData.data.startAngleValue != null) {
        this.startAngleValue = eventData.data.startAngleValue;
      }
      if (eventData.data.endAngleValue != null) {
        this.endAngleValue = eventData.data.endAngleValue;
      }
      if (eventData.data.colorValues != null) {
        this.colorValues = JSON.parse(eventData.data.colorValues);
      }
    }
  }

  onPageShow() {
    console.info('gauge page show called');
    var stateChangeEvent = {
      eventId: 102,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

    var stateChangeEventTwo = {
      eventId: 103,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventTwo, this.stateChangCallBack);

    var stateChangeEvent3 = {
      eventId: 904,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent3, this.stateChangCallBack);

    var stateChangeEvent4 = {
      eventId: 905,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent4, this.stateChangCallBack);

    var stateChangeEvent5 = {
      eventId: 906,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent5, this.stateChangCallBack);
  }

  build() {
    Column() {
      Gauge({ value: 0, min: 0, max: 100 })
        .key("gauge")
        .value(`${this.gaugeValue}`)
        .startAngle(`${this.startAngleValue}`)
        .endAngle(`${this.endAngleValue}`)
        .strokeWidth(`${this.strokeWidthValue}`)
        .width(200)
        .height(200)
    }.width('100%').margin({ top: 300 })
  }
}

