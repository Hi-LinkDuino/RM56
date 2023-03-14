/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import ConfigData from '../Utils/ConfigData';

/**
 * Sub-Page Entry Component
 */
@Component
export struct TitleComponent {
  private title:string | Resource;
  private fontSize:string = '30vp';
  private stateChangeFlag: boolean = false;
  private pinRequiredFlag: boolean = false;
  private bondStateChangeFlag: boolean = false;
  @State bgColor: Resource = $r('app.color.font_color_007DFF');
  @State isTouched:boolean = false;
  @StorageLink('on_stateChange') state: string = 'on:stateChange';
  @StorageLink('on_pinRequired') pin: string = 'on:pinRequired';
  @StorageLink('on_bondStateChange') bondState: string = 'on:bondStateChange';

  build() {
    Column(){
      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems:ItemAlign.Center }) {
        Text(this.stateChangeFlag ? this.state : (this.pinRequiredFlag ? this.pin : (this.bondStateChangeFlag ? this.bondState : this.title)))
          .textAlign(TextAlign.Center)
          .width("100%")
          .height(70)
          .fontSize(this.fontSize)
          .fontColor($r('app.color.font_color_182431'))
          .fontWeight(FontWeight.Medium)
      }
      .height(80)
      .width(ConfigData.WH_100_100)
      .backgroundColor(this.bgColor)
      .linearGradient(this.isTouched ? {
         angle: 90,
         direction: GradientDirection.Right,
         colors: [[$r("app.color.DCEAF9"), 0.0], [$r("app.color.FAFAFA"), 1.0]]
      } : {
         angle: 90,
         direction: GradientDirection.Right,
         colors: [[$r("sys.color.ohos_id_color_foreground_contrary"), 1], [$r("sys.color.ohos_id_color_foreground_contrary"), 1]]
      })
      .onTouch((event: TouchEvent) => {
        if (event.type === TouchType.Down) {
          this.isTouched = true;
        }
        if (event.type === TouchType.Up) {
          this.isTouched = false;
        }
      })
    }
    .padding($r('app.float.distance_4'))
    .height("100vp")
    .borderRadius($r('app.float.radius_12'))
    .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"));
  }
}