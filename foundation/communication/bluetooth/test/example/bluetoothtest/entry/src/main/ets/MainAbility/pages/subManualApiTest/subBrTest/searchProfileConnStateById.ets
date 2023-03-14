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
import { TitleComponent } from '../../../../Component/titleComponent';
import bt from '@ohos.bluetooth';

/**
 * about phone
 */
@Entry
@Component
struct profileConnState2 {
  @State message: string = "";
  @State currentClick: number = -1;
  @State profileId: number = -1;
  private stateProfile: string = ""
  @State deviceId: string = AppStorage.Get('inputDeviceId');

  build() {
    Row() {
      Column() {
        Row() {
          Text("目标设备MAC：").fontSize("20vp")
          TextInput( { text: this.deviceId, placeholder: "请输入mac地址" } )
            .fontSize("20vp")
            .onChange((str) => {
              this.deviceId = str;
              AppStorage.SetOrCreate('inputDeviceId', str)
            })
            .width('70%')
        }
        .backgroundColor($r("app.color.moon"))
        .margin( { top: "20vp", left: "20vp", right: "20vp" } )
        .alignItems(VerticalAlign.Center)

        List() {
          ListItem() {
            TitleComponent({
              title: "A2DP_SOURCE",
              fontSize: '15vp',
              bgColor: this.currentClick === 0 ? $r('app.color.font_color_007DFF') : $r('app.color.white_bg_color')
            });
          }
          .padding({top:20})
          .height(80)
          .borderRadius(10)
          .onClick(() => {
            this.currentClick = 0;
            this.profileId = 1;
            let ret = bt.getProfile(this.profileId).getDeviceState(this.deviceId);
            console.log('A2DP state is -' + ret);
            switch (ret) {
              case 0:
                this.stateProfile = 'STATE_DISCONNECTED';
                break;
              case 1:
                this.stateProfile = 'STATE_CONNECTING';
                break;
              case 2:
                this.stateProfile = 'STATE_CONNECTED';
                break;
              case 3:
                this.stateProfile = 'STATE_DISCONNECTING';
                break;
              case -1:
                this.stateProfile = 'FAIL';
                break;
            }
            this.message = "当前A2DP的状态是：" + this.stateProfile;
          })

          ListItem() {
            TitleComponent({
              title: "HANDS_FREE_AUDIO_GATEWAY",
              fontSize: '15vp',
              bgColor: this.currentClick === 1 ? $r('app.color.font_color_007DFF') : $r('app.color.white_bg_color')
            });
          }
          .padding({top:20})
          .height(80)
          .borderRadius(10)
          .onClick(() => {
            this.currentClick = 1;
            this.profileId = 4;
            let ret = bt.getProfile(this.profileId).getDeviceState(this.deviceId);
            console.log('HFP state is -' + ret);
            switch (ret) {
              case 0:
                this.stateProfile = 'STATE_DISCONNECTED';
                break;
              case 1:
                this.stateProfile = 'STATE_CONNECTING';
                break;
              case 2:
                this.stateProfile = 'STATE_CONNECTED';
                break;
              case 3:
                this.stateProfile = 'STATE_DISCONNECTING';
                break;
              case -1:
                this.stateProfile = 'FAIL';
                break;
            }
            this.message = "当前HFP的状态是：" + this.stateProfile;
          })

          ListItem() {
            TitleComponent({
              title: "HID_HOST",
              fontSize: '15vp',
              bgColor: this.currentClick === 2 ? $r('app.color.font_color_007DFF') : $r('app.color.white_bg_color')
            });
          }
          .padding({top:20})
          .height(80)
          .borderRadius(10)
          .onClick(() => {
            this.currentClick = 2;
            this.profileId = 6;
            let ret = bt.getProfile(this.profileId).getDeviceState(this.deviceId);
            console.log('HID state is -' + ret);
            switch (ret) {
              case 0:
                this.stateProfile = 'STATE_DISCONNECTED';
                break;
              case 1:
                this.stateProfile = 'STATE_CONNECTING';
                break;
              case 2:
                this.stateProfile = 'STATE_CONNECTED';
                break;
              case 3:
                this.stateProfile = 'STATE_DISCONNECTING';
                break;
              case -1:
                this.stateProfile = 'FAIL';
                break;
            }
            this.message = "当前HID的状态是：" + this.stateProfile;
          })
        }
        .height(300)
        .margin(20);

        Text("接口调用结果：" + this.message)
          .textAlign(TextAlign.Start)
          .fontSize('17fp')
          .backgroundColor($r("app.color.white"))
      }.width('100%')
    }
    .height('100%')
    .alignItems(VerticalAlign.Top)
    .backgroundColor($r("app.color.lead"))
  }
  }