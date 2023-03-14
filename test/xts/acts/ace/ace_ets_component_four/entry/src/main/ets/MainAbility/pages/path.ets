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
struct PathExample {
  @State commandsOne: string = 'M0 0 L900 0'
  @State widthOne: number = 300
  @State heightOne: number = 10


  onPageShow() {
    console.info('[path] page show called ');
    var stateChangeEvent = {
      eventId: 151,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[stateChangCallBack] stateChangCallBack ");
    if (eventData != null) {
      console.info("[stateChangCallBack] state change called: " + JSON.stringify(eventData));
      if (eventData.data.commands != null) {
        this.commandsOne = eventData.data.commands;
      }
      if (eventData.data.width != null) {
        this.widthOne = eventData.data.width;
      }
      if (eventData.data.height != null) {
        this.heightOne = eventData.data.height;
      }

    }
  }

  build() {
    Column({ space: 5 }) {
      Text('Straight line').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Path().width(this.widthOne).height(this.heightOne).commands(this.commandsOne).stroke(Color.Black).key('path1').strokeWidth(3)

      Text('Straight line graph').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Path().width(100).height(100).commands('M150 0 L300 300 L0 300 Z')
        Path().width(100).height(100).commands('M0 0 H300 V300 H0 Z')
        Path().width(100).height(100).commands('M150 0 L0 150 L60 300 L240 300 L300 150 Z')
      }.width('100%')

      Text('Curve graphics').fontSize(9).fontColor(0xCCCCCC).width('90%')
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Path().width(100).height(100).commands("M0 300 S150 0 300 300 Z")
        Path().width(100).height(100).commands('M0 150 C0 150 150 0 300 150 L150 300 Z')
      }
    }.width('100%').margin({ top: 5 })
  }
}