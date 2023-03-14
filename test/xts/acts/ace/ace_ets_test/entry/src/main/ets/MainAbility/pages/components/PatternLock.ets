// @ts-nocheck
// @ts-nocheck
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
struct PatternLockTest {
  @State passwords: Number[] = [];
  @State length: number = 200;
  @State redius: number = 10;
  @State color1: string = '#DB7093';
  @State color2: string = '#7B68EE';
  @State color3: string = '#20B2AA';
  @State color4: string = '#BDB76B';
  @State color5: string = '#FF8C00';
  @State stroke: number= 30;
  @State isAutoReset: boolean= true;
  private patternLockController: PatternLockController = new PatternLockController();
  onPageShow() {
    console.info('patternLock page show called');
  }

  onBuildDone() {
    console.info('patternLock page build done called');
  }
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      PatternLock(this.patternLockController)
        .sideLength(this.length)
        .circleRadius(this.redius)
        .backgroundColor(this.color1)
        .regularColor(this.color2)
        .selectedColor(this.color3)
        .activeColor(this.color4)
        .pathColor(this.color5)
        .key('patternLock')
        .pathStrokeWidth(this.stroke)
        .autoReset(this.isAutoReset)
        .onPatternComplete((input) => {
          if (!input) {
            return;
          }
          console.info(input.toString());
          if (input.Length < 4) {
            this.patternLockController.reset()
            return
          }
          this.passwords = input;
        })
      Text(this.passwords.toString()).fontSize(18).fontColor(0x123A42).margin(15)
      Flex({ wrap: FlexWrap.Wrap, justifyContent: FlexAlign.SpaceAround, alignItems: ItemAlign.Center, }) {
        Button('Reset').onClick(() => {
          this.patternLockController.reset()
        }).margin(8)
        Button('sideLength' + this.length).onClick(() => {
          if (this.length > 500) {
            this.length = -50
          } else {
            this.length += 50
          }
        }).margin(8)
        Button('circleRadius' + this.redius).onClick(() => {
          if (this.redius > 30) {
            this.redius = -5
          } else {
            this.redius += 5
          }
        }).margin(8)
        Button('pathStrokeWidth' + this.stroke).onClick(() => {
          if (this.stroke > 30) {
            this.stroke = -5
          } else {
            this.stroke += 5
          }
        }).margin(8)
        Button('autoReset' + this.isAutoReset).onClick(() => {
          this.isAutoReset = !this.isAutoReset
        }).margin(8)
        Button('backgroundColor').onClick(() => {
          if (this.color1 == '#DB7093') {
            this.color1 = '#4B0082'
          } else {
            this.color1 = '#DB7093'
          }
        }).margin(8)
        Button('regularColor').onClick(() => {
          if (this.color2 == '#7B68EE') {
            this.color2 = '#4169E1'
          } else {
            this.color2 = '#7B68EE'
          }
        }).margin(8)
        Button('selectedColor').onClick(() => {
          if (this.color3 == '#20B2AA') {
            this.color3 = '#32CD32'
          } else {
            this.color3 = '#20B2AA'
          }
        }).margin(8)
        Button('activeColor').onClick(() => {
          if (this.color4 == '#BDB76B') {
            this.color4 = '#808000'
          } else {
            this.color4 = '#BDB76B'
          }
        }).margin(8)
        Button('pathColor').onClick(() => {
          if (this.color5 == '#FF8C00') {
            this.color5 = '#FF6347'
          } else {
            this.color5 = '#FF8C00'
          }
        }).margin(8)
      }
    }.width('100%').height('100%')
  }
}