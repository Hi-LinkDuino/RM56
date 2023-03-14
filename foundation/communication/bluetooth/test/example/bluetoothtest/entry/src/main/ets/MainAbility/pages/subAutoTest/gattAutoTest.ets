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
// @ts-nocheck
import ConfigData from '../../../Utils/ConfigData';
import router from '@system.router';
import { TestData } from '../../model/testData'
import { PageTitle } from '../../../Component/pageTitle';
import { TestImageDisplay } from '../../../Component/testImageDisplay';
import { initGattAutoTestData } from '../../model/testDataModels'
import { TestListItem } from '../../../Component/testListItems';

/**
 *  GattAutoTest Page Of Bluetooth test
 */
@Entry
@Component
struct GattAutoTest {
  @State message: string = 'GattAutoTest';
  @State textMessage: string = 'null';
  private gattAutoTestItems: TestData[]=initGattAutoTestData();
  private testItem: TestData = router.getParams().testId;
  @StorageLink ("autoTestMessage") autoTestMessage:string =""
  build() {
    Column() {
      Stack({ alignContent: Alignment.TopStart }) {
        TestImageDisplay({ testItem: this.testItem })
        PageTitle({ testItem: this.testItem })
      }
      List(){
        ForEach(this.gattAutoTestItems,item =>{
          ListItem() {
            TestListItem({
              testItem: item
            })
          }
        },item=> item.id.toString())
      }
      .height('93%')
      .backgroundColor($r("app.color.lead"))
    }
    .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
    .width(ConfigData.WH_100_100)
    .height(ConfigData.WH_100_100);
  }
}

