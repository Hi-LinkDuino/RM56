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
struct AreaExample {
  @State value: string = 'Text';
  @State size: string = '';
  @State onActionCalled: boolean = false;

  build() {
    Column() {
      Text(this.value)
        .key('text1')
        .backgroundColor(Color.Green)
        .margin(30)
        .fontSize(20)
        .onClick(() => {
          this.value = this.value + 'Text'
          try {
            var backData = {
              data: {
                "value": this.value,
              }
            }
            var backEvent = {
              eventId: 61,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("onClick start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("onClick emit action state err: " + JSON.stringify(err.message))
          }
        })
        .onAreaChange((oldValue: Area, newValue: Area) => {
          console.info(`Ace: on area change, oldValue is ${JSON.stringify(oldValue)} newvalue is ${JSON.stringify(newValue)}`)
          this.size = JSON.stringify(newValue)
          this.onActionCalled = true;
          console.info('onAreaChange current action state is: ' + this.onActionCalled);
          try {
            var backData = {
              data: {
                "ACTION": this.onActionCalled,
              }
            }
            var backEvent = {
              eventId: 62,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("onAreaChange start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("onAreaChange emit action state err: " + JSON.stringify(err.message))
          }
        })
      Text('new area is: \n' + this.size)
        .key('text2')
        .margin({ right: 30, left: 30 })
    }
    .width('100%').height('100%').margin({ top: 30 })
  }
}