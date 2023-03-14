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
import eventsEmitter from '@ohos.emitter';

@Entry
@Component
struct RichTextCreateComponent {
  @State data: string = "test data"

  onPageShow() {
    console.info('[list] page show called');
    var stateChangeEvent = {
      eventId: 181,
      priority: eventsEmitter.EventPriority.LOW
    }
    eventsEmitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  private stateChangCallBack = (eventData) => {
    console.info("[stateChangCallBack] stateChangCallBack");
    if (eventData != null) {
      console.info("[stateChangCallBack] state change called:" + JSON.stringify(eventData));
      if (eventData.data.data != null) {
        this.data = eventData.data.data;
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center }) {
      Column() {
        RichText(this.data)
          .onStart(() => {
            console.info("RichText onStart")
          })
          .onComplete(() => {
            console.info("RichText onComplete")
          })
      }

    }.padding({ top: 30 })
  }
}