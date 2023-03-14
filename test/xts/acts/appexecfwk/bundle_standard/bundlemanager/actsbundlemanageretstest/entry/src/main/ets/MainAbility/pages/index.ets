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

import router from '@system.router';
import file from '@system.file';
import {Core, ExpectExtend, InstrumentLog, ReportExtend} from "deccjsunit/index.ets";
import testsuite from "../test/List.test.ets";
import featureAbility from "@ohos.ability.featureAbility";

async function routePage() {
  let options = {
    uri: 'pages/second'
  }
  try {
    await router.push(options)
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`)
  }
}

@Entry
@Component
struct Index {

  aboutToAppear() {
    console.info("start run testcase!!!!")
    featureAbility.getWant()
      .then((Want) => {
        const core = Core.getInstance();
        const expectExtend = new ExpectExtend({
          'id': 'extend'
        });
        core.addService('expect', expectExtend);
        const reportExtend = new ReportExtend(file);
        core.addService('report', reportExtend);
        core.init();
        core.subscribeEvent('task', reportExtend);
        const configService = core.getDefaultService('config');
        console.info('parameters---->' + JSON.stringify(Want.parameters));
        configService.setConfig(Want.parameters);
        testsuite();
        core.execute();
        console.info('Operation successful. Data: ' + JSON.stringify(Want));
      })
      .catch((error) => {
        console.error('Operation failed. Cause: ' + JSON.stringify(error));
      })
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('Hello World')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Button() {
        Text('ACE ETS TEST 包管理 2 ')
          .fontSize(25)
          .fontWeight(FontWeight.Bold)
      }.type(ButtonType.Capsule)
      .margin({
        top: 20
      })
      .backgroundColor('#0D9FFB')
      .onClick(() => {
        routePage()
      })
    }
    .width('100%')
    .height('100%')
  }
}