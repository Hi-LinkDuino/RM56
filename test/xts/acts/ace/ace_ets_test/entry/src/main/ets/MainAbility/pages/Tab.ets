/*
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
 
@Entry
@Component
struct TabsExample {
  private controller: TabsController = new TabsController()

  build() {
    Column() {
      Button('changeIndex')
        .onClick(() => {
          this.controller.changeIndex(0)
          console.log('change Index ')
        })

      Tabs({ barPosition: BarPosition.Start, index: 1, controller: this.controller }) {
        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Pink)
        }.tabBar('pink')

        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Yellow)
        }.tabBar('yellow')
        .key('tabcontent')

        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Blue)
        }.tabBar('blue')

        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Green)
        }.tabBar('green')
      }
      .key('tab')
      .vertical(true)
      .scrollable(true)
      .barMode(BarMode.Fixed)
      .barWidth(70)
      .barHeight(150)
      .animationDuration(400)
      .onChange((index: number) => {
        console.info(index.toString())
      })
      .width('90%')
      .backgroundColor(0xF5F5F5)

      Tabs({ barPosition: BarPosition.End, index: 1, controller: this.controller }) {
        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Pink)
        }.tabBar('pink')

        TabContent() {
          Column().width('100%').height('100%').backgroundColor(Color.Green)
        }.tabBar('green')
      }
      .key('tab2')
      .vertical(false)
      .scrollable(false)
      .barMode(BarMode.Scrollable)
      .barWidth(70)
      .barHeight(150)
      .animationDuration(400)
      .onChange((index: number) => {
        console.info(index.toString())
      })
      .width('90%')
      .backgroundColor(0xF5F5F5)
    }.width('100%').height(150).margin({ top: 5 })
  }
}