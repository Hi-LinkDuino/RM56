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
struct DragExample {
  @State numbers: string[] = ['one', 'two', 'three', 'four', 'five', 'six']
  @State text: string = ''
  @State bool: boolean = false
  @State bool1: boolean = false
  @State size: string = ''
  @State appleVisible: Visibility = Visibility.Visible
  @State orangeVisible: Visibility = Visibility.Visible
  @State bananaVisible: Visibility = Visibility.Visible
  @State mangoVisible: Visibility = Visibility.Visible
  @State strawberryVisible: Visibility = Visibility.Visible
  @State select: number = 0
  @State currentIndex: number = 0

  @Builder pixelMapBuilder() {
    Column() {
      Text(this.text)
        .width('50%').height(60).fontSize(16).borderRadius(10)
        .textAlign(TextAlign.Center).backgroundColor(Color.Yellow)
    }
  }

  build() {
    Column() {
      Text('There are three Text elements here')
        .fontSize(12).fontColor(0xCCCCCC).width('90%')
        .textAlign(TextAlign.Start).margin(5)
      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
        Text('apple').width('25%').height(35).fontSize(16).key('apple')
          .textAlign(TextAlign.Center).backgroundColor(0xAFEEEE)
          .visibility(this.appleVisible)
          .onDragStart(() => {
            this.bool = true
            this.text = 'apple'
            this.appleVisible = Visibility.Hidden
            console.log('apple onDragStart onAction')
            try {
              var backData = {
                data: {
                  "ACTION": 'onDragStart onAction'
                }
              }
              var backEvent = {
                eventId: 86,
                priority: events_emitter.EventPriority.LOW
              }
              console.info("apple start to emit action state")
              events_emitter.emit(backEvent, backData)
            } catch (err) {
              console.info("apple emit action state err: " + JSON.stringify(err.message))
            }
            return this.pixelMapBuilder
          })
        Text('orange').width('25%').height(35).fontSize(16).key('orange')
          .textAlign(TextAlign.Center).backgroundColor(0xAFEEEE)
          .visibility(this.orangeVisible)
          .onDragStart(() => {
            this.bool = true
            this.text = 'orange'
            this.orangeVisible = Visibility.Hidden
            return this.pixelMapBuilder
          })
        Text('banana').width('25%').height(35).fontSize(16).key('banana')
          .textAlign(TextAlign.Center).backgroundColor(0xAFEEEE)
          .visibility(this.bananaVisible)
          .onDragStart((event: DragEvent, extraParams: string) => {
            console.log('Text onDragStarts, ' + extraParams)
            this.bool = true
            this.text = 'banana'
            this.bananaVisible = Visibility.Hidden
            return this.pixelMapBuilder
          })
        Text('mango').width('25%').height(35).fontSize(16).key('mango')
          .textAlign(TextAlign.Center).backgroundColor(0xAFEEEE)
          .visibility(this.mangoVisible)
          .onDragStart(() => {
            this.bool = true
            this.text = 'mango'
            this.mangoVisible = Visibility.Hidden
            return this.pixelMapBuilder
          })
        Text('strawberry').width('25%').height(35).fontSize(16).key('strawberry')
          .textAlign(TextAlign.Center).backgroundColor(0xAFEEEE)
          .visibility(this.strawberryVisible)
          .onDragStart(() => {
            this.bool = true
            this.text = 'strawberry'
            this.strawberryVisible = Visibility.Hidden
            return this.pixelMapBuilder
          })
      }.border({ width: 1 }).width('90%').padding({ top: 10, bottom: 10 }).margin(10)

      Text('This is a List element').fontSize(12)
        .fontColor(0xCCCCCC).width('90%')
        .textAlign(TextAlign.Start).margin(15)
      List({ space: 20, initialIndex: 0 }) {
        ForEach(this.numbers, (item) => {
          ListItem() {
            Text('' + item)
              .width('100%').height(80).fontSize(16).borderRadius(10)
              .textAlign(TextAlign.Center).backgroundColor(0xAFEEEE)
          }.key('listItem')
          .onDragStart((event: DragEvent, extraParams: string) => {
            console.log('ListItem onDragStarts, ' + extraParams)
            var jsonString = JSON.parse(extraParams)
            this.bool1 = true
            this.text = this.numbers[jsonString.selectedIndex]
            this.select = jsonString.selectedIndex
            return this.pixelMapBuilder
          })
        }, item => item)
      }
      .editMode(true)
      .height('50%').width('90%').border({ width: 1 })
      .divider({ strokeWidth: 2, color: 0xFFFFFF, startMargin: 20, endMargin: 20 })
      .onDragEnter((event: DragEvent, extraParams: string) => {
        console.log('List onDragEnter, ' + extraParams + ',' + event.getX() + ',' + event.getY())
        try {
          var backData = {
            data: {
              "pointX": event.getX(),
              "pointY": event.getY()
            }
          }
          var backEvent = {
            eventId: 87,
            priority: events_emitter.EventPriority.LOW
          }
          console.info("onDragEnter start to emit action state")
          events_emitter.emit(backEvent, backData)
        } catch (err) {
          console.info("onDragEnter emit action state err: " + JSON.stringify(err.message))
        }
      })
      .onDragMove((event: DragEvent, extraParams: string) => {
        console.log('List onDragMove, ' + extraParams)
        try {
          var backData = {
            data: {
              "ACTION": 'onDragMove onAction'
            }
          }
          var backEvent = {
            eventId: 88,
            priority: events_emitter.EventPriority.LOW
          }
          console.info("onDragMove start to emit action state")
          events_emitter.emit(backEvent, backData)
        } catch (err) {
          console.info("onDragMove emit action state err: " + JSON.stringify(err.message))
        }
      })
      .onDragLeave((event: DragEvent, extraParams: string) => {
        console.log('List onDragLeave, ' + extraParams)
        try {
          var backData = {
            data: {
              "ACTION": 'onDragLeave onAction'
            }
          }
          var backEvent = {
            eventId: 89,
            priority: events_emitter.EventPriority.LOW
          }
          console.info("onDragLeave start to emit action state")
          events_emitter.emit(backEvent, backData)
        } catch (err) {
          console.info("onDragLeave emit action state err: " + JSON.stringify(err.message))
        }
      })
      .onDrop((event: DragEvent, extraParams: string) => {
        console.log('onDrop onAction')
        try {
          var backData = {
            data: {
              "ACTION": 'onDrop onAction'
            }
          }
          var backEvent = {
            eventId: 90,
            priority: events_emitter.EventPriority.LOW
          }
          console.info("onDrop start to emit action state")
          events_emitter.emit(backEvent, backData)
        } catch (err) {
          console.info("onDrop emit action state err: " + JSON.stringify(err.message))
        }
        var jsonString = JSON.parse(extraParams)
        if (this.bool) {
          this.numbers.splice(jsonString.insertIndex, 0, this.text)
          this.bool = false
        } else if (this.bool1) {
          this.numbers.splice(jsonString.selectedIndex, 1)
          this.numbers.splice(jsonString.insertIndex, 0, this.text)
          this.bool = false
          this.bool1 = false
        }
      })
    }.width('100%').height('100%').padding({ top: 20 }).margin({ top: 20 })
  }
}
