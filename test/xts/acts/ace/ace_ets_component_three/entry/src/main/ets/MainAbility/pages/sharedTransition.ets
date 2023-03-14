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
struct SharedTransitionExample {
  @State scale: number = 1
  @State opacity: number = 1
  @State active: boolean = false
  @State duration: number = 1000
  @State delay: number = 0
  @State type: SharedTransitionEffectType = SharedTransitionEffectType.Static

  build() {
    List() {
      ListItem() {
        Row() {
          Navigator({ target: 'pages/sharedTransition2', type: NavigationType.Push }) {
            Image($rawfile('ic_health_heart.png')).width(50).height(50)
              .sharedTransition('sharedImage1', { duration: this.duration, delay: this.delay, type: this.type })
          }
          .key('Navigator')
          .padding({ left: 10 })
          .onClick(() => {
            this.active = true
            console.info('SharedTransition current action state is: ' + this.active);
            try {
              var backData = {
                data: {
                  "duration": this.duration,
                  "delay": this.delay,
                  "type": this.type
                }
              }
              var backEvent = {
                eventId: 193,
                priority: events_emitter.EventPriority.LOW
              }
              console.info("SharedTransition start to emit action state")
              events_emitter.emit(backEvent, backData)
            } catch (err) {
              console.info("SharedTransition emit action state err: " + JSON.stringify(err.message))
            }
          })

          Text('SharedTransition').width(80).height(80).textAlign(TextAlign.Center)
        }
      }
    }
  }

  onPageShow() {
    console.info('sharedTransition page show called');
    var stateChangeEvent = {
      eventId: 194,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("sharedTransition page state change called:" + JSON.stringify(eventData));
      if (eventData.data.duration != null) {
        this.duration = eventData.data.duration;
      }
      if (eventData.data.delay != null) {
        this.delay = eventData.data.delay;
      }
      if (eventData.data.type != null) {
        this.type = eventData.data.type;
      }
    }
  }
}


