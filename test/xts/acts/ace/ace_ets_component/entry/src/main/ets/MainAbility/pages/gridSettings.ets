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
struct GridSetting {
  @State smSpan: number = 2;
  @State smOffset: number = 1;
  @State gridSpan: number = 1;
  @State gridOffset: number = 0;

  build() {
    GridContainer({ sizeType: SizeType.SM }) {
      Row({}) {
        Row() {
          Text('Left').fontSize(25)
        }
        .gridOffset(this.gridOffset)
        .height("100%")
        .backgroundColor(0x66bbb2cb)
        .key('gridOffset')

        Row() {
          Text('Center').fontSize(25)
        }
        .useSizeType({
          xs: { span: 1, offset: 0 }, sm: { span: this.smSpan, offset: this.smOffset },
          md: { span: 5, offset: 1 }, lg: { span: 7, offset: 2 }
        })
        .height("100%")
        .backgroundColor(0x66b6c5d1)
        .key('gridRow')

        Row() {
          Text('Right').fontSize(25)
        }
        .gridSpan(this.gridSpan)
        .height("100%")
        .backgroundColor(0x66bbb2cb)
        .key('gridSpan')
      }
      .height(200)
    }
    .backgroundColor(0xf1f3f5)
    .margin({ top: 10 })
    .key('gridContainer')
  }

  onPageShow() {
    let smChangeEvent = {
      eventId: 9,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(smChangeEvent, this.stateChangCallBack);
    let offsetChangeEvent = {
      eventId: 10,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(offsetChangeEvent, this.stateChangCallBack);
    let spanChangeEvent = {
      eventId: 11,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(spanChangeEvent, this.stateChangCallBack);
  }

  stateChangCallBack = (eventData) => {
    console.info("GridSetting page stateChangCallBack" + JSON.stringify(eventData));
    if (eventData && eventData.data) {
      if (eventData.data.span) {
        this.smSpan = eventData.data.span;
        this.smOffset = eventData.data.offset;
      } else if (eventData.data.gridOffset) {
        this.gridOffset = eventData.data.gridOffset;
      } else if (eventData.data.gridSpan) {
        this.gridSpan = eventData.data.gridSpan;
      }
    }
  }
}