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
import ConfigData from '../../../../Utils/ConfigData';
import Router from '@system.router'
import { TitleComponent } from '../../../../Component/titleComponent';
import {PageTitle} from '../../../../Component/pageTitle';
/**
 * about phone
 */
@Entry
@Component
struct pairedDevices {
  @State message: string = "";
  private duration: string = '0';
  private deviceInfo: any = '';
  private aboutDeviceList: any[] = [];
  private scanMode: number = 0;
  @StorageLink('pairedDevices') pairedDevices: string[] = [];


  build() {
    Column(){
      GridContainer({columns:12, sizeType: SizeType.Auto, gutter: vp2px(1) === 2 ? '12vp' : '0vp', margin: vp2px(1) === 2 ? '24vp' : '0vp'}) {
        Row({}) {
          Column() {
          }
          .width(ConfigData.WH_100_100)
          .height(ConfigData.WH_100_100)
          .useSizeType({
            xs: { span: 0, offset: 0 }, sm: { span: 0, offset: 0 },
            md: { span: 0, offset: 0 }, lg: { span: 2, offset: 0 }
          });

          Column() {
            PageTitle({detail: '已配对设备'})


            Divider()
              .padding({left: $r('sys.float.ohos_id_card_margin_start'), right: $r('sys.float.ohos_id_card_margin_end')})

            Column(){
              List() {
                ForEach(this.pairedDevices, (item: string) => {
                  ListItem() {
                    TitleComponent({title: item});
                  }.padding({ top: $r('app.float.distance_2'), bottom: $r('app.float.distance_2') })
                }, item => JSON.stringify(item));
              }
              Blank()
            }

            Flex({direction: FlexDirection.Row, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }){
              Row(){
                ButtonComponent({text: $r("app.string.cancel"), isOnClick: () => {
                  Router.back();
                }})
              }
              .width($r("app.float.deviceName_button_width"))
              .margin({right: $r("app.float.distance_12")})
              Row(){
                ButtonComponent({text: $r("app.string.confirm"), isOnClick: () => {
                  Router.back();
                }})
              }.width($r("app.float.deviceName_button_width"))
            }
          }
          .padding({left: $r('app.float.distance_24'), right: $r('app.float.distance_24')})
          .width(ConfigData.WH_100_100)
          .height(ConfigData.WH_100_100)
          .useSizeType({
            xs: { span: 12, offset: 0 }, sm: { span: 12, offset: 0 },
            md: { span: 12, offset: 0 }, lg: { span: 8, offset: 2 }
          });
        }
        .width(ConfigData.WH_100_100)
        .height(ConfigData.WH_100_100);
      }
      .width(ConfigData.WH_100_100)
      .height(ConfigData.WH_100_100);
    }
    .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
    .width(ConfigData.WH_100_100)
    .height(ConfigData.WH_100_100);
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
        .width($r('app.float.deviceName_button_width'))
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
    .align(Alignment.Center);
  }
}
