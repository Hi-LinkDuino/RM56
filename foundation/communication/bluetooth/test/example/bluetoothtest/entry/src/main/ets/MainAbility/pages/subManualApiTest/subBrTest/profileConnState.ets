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
// @ts-nocheck
import ConfigData from '../../../../Utils/ConfigData';
import {PageTitle} from '../../../../Component/pageTitle';
import { TitleComponent } from '../../../../Component/titleComponent';

/**
 * about phone
 */
@Entry
@Component
struct profileConnState {
  @State message: string = "请点击选择PROFILE TYPE";
  @State currentClick: number = -1;
  @State profileId: number = -1;
  @State profileType: string = '';
  build() {
    Column() {
      PageTitle({detail: '选择PROFILE ID'})
      Column(){
        List() {
          ListItem() {
            TitleComponent({
              title: "SELECT_PROFILE_ID",
              bgColor: $r("app.color.moon"),
              fontSize: '17vp',
            })
          }
          .height(80)
          .borderRadius(10)

          ListItem() {
            TitleComponent({
              title: "A2DP_SOURCE",
              fontSize: '15vp',
              bgColor: this.currentClick === 0 ? $r('app.color.font_color_007DFF') : $r('app.color.white_bg_color')});
          }
          .padding({top:10})
          .height(80)
          .borderRadius(10)
          .onClick(() => {
            this.currentClick = 0;
            this.profileId = 1;
          })

          ListItem() {
            TitleComponent({
              title: "HANDS_FREE_AUDIO_GATEWAY",
              fontSize: '15vp',
              bgColor: this.currentClick === 1 ? $r('app.color.font_color_007DFF') : $r('app.color.white_bg_color')
            })
          }
          .padding({top:20})
          .height(80)
          .borderRadius(10)
          .onClick(() => {
            this.currentClick = 1;
            this.profileId = 4;
          })

          ListItem() {
            TitleComponent({
              title: "HID_HOST",
              fontSize: '15vp',
              bgColor: this.currentClick === 2 ? $r('app.color.font_color_007DFF') : $r('app.color.white_bg_color')});
          }
          .padding({top:20})
          .height(80)
          .borderRadius(10)
          .onClick(() => {
            this.currentClick = 2;
            this.profileId = 6;
          })
        }
        .height(400)
      }

      Text(this.message)
        .textAlign(TextAlign.Center)
        .fontSize('17fp')
        .height(60)
        .width("100%")
        .backgroundColor($r("app.color.moon"))


      Row(){
        Column(){
          Text("取消")
            .textAlign(TextAlign.Center)
            .fontSize('17fp')
            .height(60)
            .width(100)
            .borderRadius(10)
            .borderColor($r("app.color.moon"))
            .backgroundColor($r("app.color.white"))
        }
        .padding({right: 10})
        Column() {
          Text("确认")
            .textAlign(TextAlign.Center)
            .fontSize('17fp')
            .height(60)
            .width(100)
            .borderRadius(10)
            .borderColor($r("app.color.moon"))
            .backgroundColor($r("app.color.white"))
        }
        .padding({left: 50})

      }
      .width("80%")
    }
    .padding({left: $r('app.float.distance_24'), right: $r('app.float.distance_24')})
    .width("100%")
    .height(ConfigData.WH_100_100)
    .backgroundColor($r("app.color.lead"))
  }
}


@Component
struct ButtonComponent {
  @State isTouched: boolean = false;
  private text;
  private isOnClick = () => {};

  build() {
    Column() {
      Text(this.text)
        .fontSize($r('app.float.font_16'))
        .fontWeight(FontWeight.Medium)
        .lineHeight($r('app.float.wh_value_22'))
        .fontColor($r('app.color.font_color_007DFF'))
        .textAlign(TextAlign.Center)
        .width(100)
        .height($r("app.float.wh_value_40"))
        .borderRadius($r('app.float.radius_20'))
        .backgroundColor(!this.isTouched ? $r("app.color.color_button_grey") : $r("sys.color.ohos_id_color_foreground_contrary"))
        .onTouch((event: TouchEvent) => {
          if (event.type === TouchType.Down) {
            this.isTouched = true;
          }
          if (event.type === TouchType.Up) {
            this.isTouched = false;
          }
        })
        .onClick(() => {
          this.isOnClick();
        })
    }
    .width(ConfigData.WH_100_100)
  }
}
