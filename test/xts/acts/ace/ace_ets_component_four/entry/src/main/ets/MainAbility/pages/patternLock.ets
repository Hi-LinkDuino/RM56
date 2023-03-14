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
struct PatternLockExample {
  @State passwords: Number[] = []
  @State message: string = 'please input password'
  @State sideLength: number = 150;
  @State circleRadius: number = 7;
  @State pathStrokeWidth: number = 17;
  @State backgroundColor: Color = "#FF008000";
  @State autoReset: boolean = true;
  private patternLockController: PatternLockController = new PatternLockController()
  @State onActionCalled :boolean = false

  onPageShow() {
    console.info('[PatternLock] page show called');
    var stateChangeEvent = {
      eventId: 152,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);

  }

  private stateChangCallBack = (eventData) => {
    console.info("[PatternLock] page stateChangCallBack");
    if (eventData != null) {
      console.info("[PatternLock] page state change called:" + JSON.stringify(eventData));
      if(eventData.data.sideLength != null) {
        this.sideLength = eventData.data.sideLength;
      }
      if(eventData.data.circleRadius != null) {
        this.circleRadius = eventData.data.circleRadius;
      }
      if(eventData.data.pathStrokeWidth != null) {
        this.pathStrokeWidth = eventData.data.pathStrokeWidth;
      }
      if(eventData.data.backgroundColor != null) {
        this.backgroundColor = eventData.data.backgroundColor;
      }
      if(eventData.data.autoReset != null) {
        this.autoReset = eventData.data.autoReset;
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(this.message).textAlign(TextAlign.Center)
      PatternLock(this.patternLockController)
        .sideLength(this.sideLength)
        .circleRadius(this.circleRadius)
        .pathStrokeWidth(this.pathStrokeWidth)
        .backgroundColor(`${this.backgroundColor}`)
        .autoReset(this.autoReset)
        .key('PatternLock')
        .onPatternComplete((input: Array<number>) => {
          console.log('PatternLock onPatternComplete')
          if (input == null || input == undefined || input.length < 5) {
            this.message = 'The password length needs to be greater than 5.'
            console.log('The password length needs to be greater than 5')
            return
          }
          if (this.passwords.length > 0) {
            if (this.passwords.toString() == input.toString()) {
              this.passwords = input
              this.message = 'Set password successfully: ' + this.passwords.toString()
              console.log('Set password successfully: ' + this.passwords.toString())

            } else {
              this.message = 'Inconsistent passwords, please enter again.'
              console.log('Inconsistent passwords, please enter again')
            }
          } else {
            this.passwords = input
            this.message = "Please enter again."
            console.log('Please enter again')
          }
        })
      Button('reset button').margin(30).onClick(() => {
        this.patternLockController.reset()
        this.passwords = []
        this.message = 'Please input password'
        this.onActionCalled = true;
        try {
          var backData = {
            data: {
              "ACTION": this.onActionCalled,
            }
          }
          var backEvent = {
            eventId: 153,
            priority: events_emitter.EventPriority.LOW
          }
          console.info("reset button start to emit action state")
          events_emitter.emit(backEvent, backData)
        } catch (err) {
          console.info("reset button emit action state err: " + JSON.stringify(err.message))
        }
      }).key('button1')
    }.width('100%').height('100%')
  }
}