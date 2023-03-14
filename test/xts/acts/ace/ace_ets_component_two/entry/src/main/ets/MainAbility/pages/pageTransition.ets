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
struct PageTransitionExample1 {
  @State scale: number = 1
  @State opacity: number = 1
  @State active: boolean = false
  @State type: RouteType	 = NavigationType.Push

  build() {
    Column() {
      Navigator({ target: 'pages/pageTransition2', type: this.type }) {
        Image($rawfile('ic_health_heart.png'))
          .width("100%")
          .height("100%")
      }
      .key('Navigator')
      .onClick(() => {
        this.active = true
        console.info('pageTransition current action state is: ' + this.active);
        try {
          var backData = {
            data: {
              "scale" : this.scale,
              "opacity" : this.opacity,
              "type": this.type
            }
          }
          var backEvent = {
            eventId: 144,
            priority: events_emitter.EventPriority.LOW
          }
          console.info("pageTransition start to emit action state")
          events_emitter.emit(backEvent, backData)
        } catch (err) {
          console.info("pageTransition emit action state err: " + JSON.stringify(err.message))
        }
      })
    }.scale({ x: this.scale }).opacity(this.opacity)
  }

  pageTransition() {
    PageTransitionEnter({ duration: 1200, curve: Curve.Linear })
      .onEnter((type: RouteType, progress: number) => {
        this.scale = 1
        this.opacity = progress
      })
    PageTransitionExit({ duration: 1500, curve: Curve.Ease })
      .onExit((type: RouteType, progress: number) => {
        this.scale = 1 - progress
        this.opacity = 1
      })
  }

  onPageShow() {
    console.info('active page show called');
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)

    var stateChangeEventOne = {
      eventId: 145,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventOne, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("active page state change called:" + JSON.stringify(eventData));
      if (eventData.data.scale != null) {
       this.scale = parseInt(eventData.data.scale);
      }
      if (eventData.data.opacity != null) {
        this.opacity = parseInt(eventData.data.opacity);
      }
    }
  }
}