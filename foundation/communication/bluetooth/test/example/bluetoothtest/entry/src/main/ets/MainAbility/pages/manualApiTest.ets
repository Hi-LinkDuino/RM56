/**
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
import ConfigData from '../../Utils/ConfigData';
import HeadComponent from '../../component/headcomponent';
import {SubEntryComponent} from '../../component/subentrycomponent';
/**
 * Manual API Test Page Of Bluetooth test
 */
@Entry
@Component
struct ManualApiTest {
  @State message: string = 'ManualApiTest'

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
            HeadComponent({headName: $r('app.string.api_test_title'), isActive: true})
            Column({space: '12vp'}){
              List() {
                ListItem() {
                  SubEntryComponent({targetPage: 'pages/subManualApiTest/BrInterfaceTest', title: $r("app.string.br_test_title")})
                }
                .padding({top: $r('app.float.distance_2'), bottom: $r('app.float.distance_2')})
                ListItem() {
                  SubEntryComponent({targetPage: "pages/subManualApiTest/bleTest", title:$r("app.string.ble_test_title")})
                }
                .padding({top: $r('app.float.distance_2'), bottom: $r('app.float.distance_2')})
                ListItem() {
                  SubEntryComponent({targetPage: "pages/subManualApiTest/gattTest", title: $r("app.string.gatt_test_title")})
                }
                .padding({top: $r('app.float.distance_2'), bottom: $r('app.float.distance_2')})
                ListItem() {
                  SubEntryComponent({targetPage: "pages/subManualApiTest/sppTest", title: $r("app.string.spp_test_title")})
                }
                .padding({top: $r('app.float.distance_2'), bottom: $r('app.float.distance_2')})
                ListItem() {
                  SubEntryComponent({targetPage: "pages/subManualApiTest/profileTest", title: $r("app.string.profile_test_title")})
                }
                .padding({top: $r('app.float.distance_2'), bottom: $r('app.float.distance_2')})
              }
            }
            .width(ConfigData.WH_100_100)
          }
          .padding({left: $r('app.float.distance_24'), right: $r('app.float.distance_24')})
          .width(ConfigData.WH_100_100)
          .height(ConfigData.WH_100_100)
          .useSizeType({
            xs: { span: 12, offset: 0 }, sm: { span: 12, offset: 0 },
            md: { span: 12, offset: 0 }, lg: { span: 8, offset: 2 }
          });

          Column() {
          }
          .width(ConfigData.WH_100_100)
          .height(ConfigData.WH_100_100)
          .useSizeType({
            xs: { span: 0, offset: 12 }, sm: { span: 0, offset: 12 },
            md: { span: 0, offset: 12 }, lg: { span: 2, offset: 10 }
          })
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