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
struct AnimationExample {
  @State btn1: boolean = false
  @State duration: number = 100
  @State curve: string = 'Ease'
  @State iteration: number = 1
  @State tempo: number= 1000
  @State playmode: PlayMode = PlayMode.Normal

  onPageShow() {
    console.info('animate page show called');
    var stateChangeEvent = {
      eventId: 50,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack)

    var stateChangeEventa = {
      eventId: 1011,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventa, this.stateChangCallBack)

    var stateChangeEventb = {
      eventId: 1012,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventb, this.stateChangCallBack)

    var stateChangeEventc = {
      eventId: 1013,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventc, this.stateChangCallBack)

    var stateChangeEventd = {
      eventId: 1014,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventd, this.stateChangCallBack)

    var stateChangeEvente = {
      eventId: 1015,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvente, this.stateChangCallBack)

    var stateChangeEventf = {
      eventId: 1016,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventf, this.stateChangCallBack)

    var stateChangeEventg = {
      eventId: 1017,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventg, this.stateChangCallBack)

    var stateChangeEventh = {
      eventId: 1018,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventh, this.stateChangCallBack)

    var stateChangeEventi = {
      eventId: 1019,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventi, this.stateChangCallBack)

    var stateChangeEventj = {
      eventId: 1020,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEventj, this.stateChangCallBack)
  }

  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("animate page state change called:" + JSON.stringify(eventData));
      if (eventData.data.duration != null) {
        this.duration = parseInt(eventData.data.duration);
      }
      if (eventData.data.curve != null) {
        this.curve = eventData.data.curve;
      }
      if (eventData.data.tempo != null) {
        this.tempo = parseInt(eventData.data.tempo);
      }
      if (eventData.data.iteration != null) {
        this.iteration = parseInt(eventData.data.iteration);
      }
      if (eventData.data.playmode != null) {
        this.playmode = eventData.data.playmode;
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, }) {
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button1')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 51,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button1 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button1 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button1 animateTo START")
          console.info("button1 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button1 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button2')
        .onClick(() => {
          try {
            var backDataOne = {
              data: {
                "curve": this.curve,
              }
            }
            var backEventOne = {
              eventId: 52,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button2 start to emit action state")
            events_emitter.emit(backEventOne, backDataOne)
          } catch (err) {
            console.info("button2 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button2 animateTo START")
          console.info("button2 curve value is : " + this.curve)
          animateTo({ curve: this.curve }, () => {
            console.info("button2 curve test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button3')
        .onClick(() => {
          try {
            var backDataTwo = {
              data: {
                "iterations": this.iteration,
              }
            }
            var backEventTwo = {
              eventId: 53,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button3 start to emit action state")
            events_emitter.emit(backEventTwo, backDataTwo)
          } catch (err) {
            console.info("button3 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button3 animateTo START")
          console.info("button3 iteration value is : " + this.iteration)
          animateTo({ iterations: this.iteration }, () => {
            console.info("button3 iterations test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button4')
        .onClick(() => {
          try {
            var backDataThree = {
              data: {
                "tempo": this.tempo
              }
            }
            var backEventThree = {
              eventId: 54,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button4 start to emit action state")
            events_emitter.emit(backEventThree, backDataThree)
          } catch (err) {
            console.info("button4 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button4 animateTo START")
          console.info("button4 tempo value is : " + this.duration)
          animateTo({ tempo: this.tempo }, () => {
            console.info("button4 tempo test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button5')
        .onClick(() => {
          try {
            var backDataFour = {
              data: {
                "playmode": this.playmode,
              }
            }
            var backEventFour = {
              eventId: 55,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button5 start to emit action state")
            events_emitter.emit(backEventFour, backDataFour)
          } catch (err) {
            console.info("button5 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button5 animateTo START")
          console.info("button5 playmode value is : " + this.playmode)
          animateTo({ PlayMode: this.playmode }, () => {
            console.info("button5 playMode test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button6')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1001,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button6 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button6 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button6 animateTo START")
          console.info("button6 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button6 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button7')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1002,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button7 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button7 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button7 animateTo START")
          console.info("button7 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button7 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button8')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1003,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button8 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button8 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button8 animateTo START")
          console.info("button8 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button8 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button9')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1004,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button9 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button9 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button9 animateTo START")
          console.info("button9 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button9 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button10')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1005,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button10 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button10 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button10 animateTo START")
          console.info("button10 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button10 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button11')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1006,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button11 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button11 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button11 animateTo START")
          console.info("button11 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button11 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button12')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1007,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button12 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button12 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button12 animateTo START")
          console.info("button12 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button12 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button13')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1008,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button13 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button13 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button13 animateTo START")
          console.info("button13 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button13 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button14')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1009,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button14 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button14 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button14 animateTo START")
          console.info("button14 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button14 duration test animateTo START")
          })
        })
      Button(this.show)
        .width(80)
        .height(30)
        .backgroundColor(0x317aff)
        .margin({ bottom: 20 })
        .key('button15')
        .onClick(() => {
          try {
            var backData = {
              data: {
                "duration": this.duration,
              }
            }
            var backEvent = {
              eventId: 1010,
              priority: events_emitter.EventPriority.LOW
            }
            console.info("button15 start to emit action state")
            events_emitter.emit(backEvent, backData)
          } catch (err) {
            console.info("button15 emit action state err: " + JSON.stringify(err.message))
          }
          console.info("button15 animateTo START")
          console.info("button15 duration value is : " + this.duration)
          animateTo({ duration: this.duration }, () => {
            console.info("button15 duration test animateTo START")
          })
        })
    }.height(400).width("100%").padding({ top: 100 })
  }
}