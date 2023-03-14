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
struct PanelExample {
  @State show: boolean = true

  onPageShow() {
    console.info('Panel page show called');
  }

  onBuildDone() {
    console.info('Panel page build done called');
  }

  build() {
    Column() {
      Text('2021-09-30    Today Calendar: 1.afternoon......Click for details')
        .width('90%')
        .height(50)
        .borderRadius(10)
        .backgroundColor(0xFFFFFF)
        .padding({ left: 20 })
        .onClick(() => {
          this.show = !this.show
        })
      Panel(this.show) {
        Column() {
          Text('Today Calendar')
          Divider()
          Text('1. afternoon 4:00 The project meeting')
        }
      }
      .key('panel')
      .type(PanelType.Foldable)
      .mode(PanelMode.Half)
      .dragBar(true)
      .fullHeight(500)
      .halfHeight(250)
      .miniHeight(100)
      .onChange((value: any) => {
        console.info(`width:${value.width},height:${value.height},mode:${value.mode}`)
      })

      Panel(this.show) {
        Column() {
          Text('Today Calendar')
          Divider()
          Text('1. afternoon 4:00 The project meeting')
        }
      }
      .key('panel2')
      .type(PanelType.Minibar)
      .mode(PanelMode.Mini)
      .dragBar(true)
      .fullHeight(500)
      .halfHeight(250)
      .miniHeight(100)
      .onChange((value: any) => {
        console.info(`width:${value.width},height:${value.height},mode:${value.mode}`)
      })

      Panel(false) {
        Column() {
          Text('Today Calendar')
          Divider()
          Text('1. afternoon 4:00 The project meeting')
        }
      }
      .key('panel3')
      .type(PanelType.Temporary)
      .mode(PanelMode.Full)
      .dragBar(true)
      .fullHeight(500)
      .halfHeight(250)
      .miniHeight(100)
      .onChange((value: any) => {
        console.info(`width:${value.width},height:${value.height},mode:${value.mode}`)
      })

    }.width('100%').height('50%').backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}