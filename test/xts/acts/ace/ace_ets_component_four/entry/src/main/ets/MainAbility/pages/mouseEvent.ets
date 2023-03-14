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

@Entry
@Component
struct MouseEventExample {
  @State hoverText: string = 'no hover'
  @State mouseText: string = 'MouseText'
  @State color: Color = Color.Blue
  @State eventButton: string = ''
  @State eventAction: string = ''

  build() {
    Column({ space:20 }) {
      Button(this.hoverText)
        .onHover((isHover: boolean) => {
          if (isHover) {
            this.hoverText = 'on hover'
            this.color = Color.Pink
          } else {
            this.hoverText = 'no hover'
            this.color = Color.Blue
          }
        })
        .backgroundColor(this.color)
      Button('onMouse')
        .onMouse((event: MouseEvent) => {
          if (event.button === MouseButton.Back) {
            this.eventButton = 'Back'
          }
          if (event.button === MouseButton.None) {
            this.eventButton = 'None'
          }
          if (event.event.action === MouseAction.Release) {
            this.eventAction = 'Release'
          }
          if (event.event.action === MouseAction.Hover) {
            this.eventAction = 'Hover'
          }
          if (event.event.action === MouseAction.None) {
            this.eventAction = 'None'
          }
          console.log(this.mouseText = 'onMouse:\nButton = ' + event.button + '\nAction = ' + event.action + '\nlocalXY=(' + event.x + ',' + event.y + ')' + '\nscreenXY=(' + event.screenX + ',' + event.screenY + ')')
          console.log(this.mouseText = 'onMouse:\nButton = ' + this.eventAction + '\nAction = ' + this.eventAction + '\nlocalXY=(' + event.x + ',' + event.y + ')' + '\nscreenXY=(' + event.screenX + ',' + event.screenY + ')')
          stopPropagation: () => {
            console.info('stopPropagation')
          }
        })
      Text(this.mouseText)
    }.padding({ top: 20 }).width('100%')
  }
}