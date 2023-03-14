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
struct GestureGroupExample {
  @State count: number = 0
  @State offsetX: number = 0
  @State offsetY: number = 0
  @State borderStyle: BorderStyle = BorderStyle.Solid
  @State mode: GestureMode = GestureMode.Sequence

  build() {
    Flex({ direction: FlexDirection.Column, mode:this.mode, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Text('sequence gesture\n' + 'LongPress onAction:' + this.count + '\nPanGesture offset:\nX: ' + this.offsetX + '\n' + 'Y: ' + this.offsetY)
    }
    .key('gestureMode')
    .translate({ x: this.offsetX, y: this.offsetY, z: 5 })
    .height(100).width(200).padding(10).margin(80).border({ width: 1, style: this.borderStyle })
    .gesture(
    GestureGroup(GestureMode.Sequence,
    LongPressGesture({ repeat: true })
      .onAction((event: GestureEvent) => {
        if (event.repeat) {this.count++}
        console.log('LongPress onAction')
      })
      .onActionEnd(() => {
        console.log('LongPress end')
      }),
    PanGesture({})
      .onActionStart(() => {
        this.borderStyle = BorderStyle.Dashed
        console.log('pan start')
      })
      .onActionUpdate((event: GestureEvent) => {
        this.offsetX = event.offsetX
        this.offsetY = event.offsetY
        console.log('pan update')
      })
    )
      .onCancel(() => {
        console.log('sequence gesture canceled')
      })
    )
  }
}