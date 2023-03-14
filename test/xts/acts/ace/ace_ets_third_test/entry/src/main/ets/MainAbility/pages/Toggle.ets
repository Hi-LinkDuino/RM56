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
struct ToggleExample {
  @State text: string = ''
  @State textNew: string = ''

  onPageShow() {
    console.info('toggle page show called');
  }

  onBuildDone() {
    console.info('toggle page build done called');
  }

  build() {
    Column({ space: 10 }) {
      Text('type:Switch').fontSize(30).fontColor(Color.Blue).width('90%')
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Toggle({ type: ToggleType.Switch, isOn: false })
          .selectedColor('#FFFF0000')
          .size({ width: 50, height: 50 })
          .switchPointColor('#FF808080')
          .key('toggle1')
          .onChange((isOn: boolean) => {
            console.info("status changed " + isOn)
          })
        Toggle({ type: ToggleType.Switch, isOn: true })
          .selectedColor('#FFFF0000')
          .size({ width: 50, height: 50 })
          .switchPointColor('#FF808080')
          .key('toggle2')
          .onChange((isOn: boolean) => {
            console.info("status changed " + isOn)
          })
      }

      Text('type:CheckBox').fontSize(30).fontColor(Color.Blue).width('90%')
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Toggle({ type: ToggleType.Checkbox, isOn: false })
          .size({ width: 50, height: 50 })
          .selectedColor('#FFFF0000')
          .key('toggle3')
          .onChange((isOn: boolean) => {
            console.info("status changed " + isOn)
          })
        Toggle({ type: ToggleType.Checkbox, isOn: true })
          .selectedColor('#FFFF0000')
          .size({ width: 50, height: 50 })
          .key('toggle4')
          .onChange((isOn: boolean) => {
            console.info("status changed " + isOn)
          })
      }

      Text('type:Button').fontSize(30).fontColor(Color.Blue).width('90%')
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Toggle({ type: ToggleType.Button, isOn: false }) {
          Text('status button').padding({ left: 12, right: 12 })
        }.key('toggle5')
        .size({ width: 100, height: 50 })
        .switchPointColor('#FF808080')
        .onChange((isOn: boolean) => {
          console.info("status changed " + isOn)
        })

        Toggle({ type: ToggleType.Button, isOn: true }) {
          Text('status button').padding({ left: 12, right: 12 })
        }.key('toggle6')
        .size({ width: 100, height: 50 })
        .switchPointColor('#FF808080')
        .onChange((isOn: boolean) => {
          console.info("status changed " + isOn)
        })
      }
    }
    .width('100%').padding(24)
  }
}
