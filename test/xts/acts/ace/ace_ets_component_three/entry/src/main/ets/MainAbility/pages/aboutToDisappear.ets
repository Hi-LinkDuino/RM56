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
struct ParentComp {
  @State isCountDown: boolean = true

  build() {
    Column() {
      Text(this.isCountDown ? 'Count Down' : 'Stopwatch')
      if (this.isCountDown) {
        Image($rawfile('ic_health_heart.png')).width(50).height(50)
        TimerComponent({ counter: 10, changePerSec: -1, showInColor: Color.Red })
      } else {
        Image($rawfile('ic_health_heart.png')).width(150).height(150)
        TimerComponent({ counter: 0, changePerSec: +1, showInColor: Color.Black })
      }
      Button(this.isCountDown ? 'Swtich to Stopwatch' : 'Switch to Count Down')
        .onClick(() => {
          this.isCountDown = !this.isCountDown
        })
        .key('button1')
    }
  }
}

@Component
struct TimerComponent {
  @State counter: number = 0
  private changePerSec: number = -1
  private showInColor: Color = Color.Black
  private timerId: number = -1
  @State onActionCalled: boolean = false

  build() {
    Text(`${this.counter}sec`)
      .fontColor(this.showInColor)
  }

  aboutToAppear() {
    this.timerId = setInterval(() => {
      this.counter += this.changePerSec
    }, 1000)
  }

  aboutToDisappear() {
    if (this.timerId > 0) {
      clearTimeout(this.timerId)
      console.info('ting--1');
      try {
        var backData = {
          data: {
            "ACTION": this.onActionCalled,
          }
        }
        var backEvent = {
          eventId: 49,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("button1 start to emit action state")
        events_emitter.emit(backEvent, backData)
      } catch (err) {
        console.info("button1 emit action state err: " + JSON.stringify(err.message))
      }
      this.timerId = -1
    }
  }
}