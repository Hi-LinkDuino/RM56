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

// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';

export default function panelJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('panelTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Panel',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Panel state success " + JSON.stringify(pages));
        if (!("Panel" == pages.name)) {
          console.info("get Panel state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Panel page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error(JSON.stringify(result));
      }
      await sleep(1)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Panel after each called");
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0930
     * @tc.name      testPanelGetInspectorByKey010
     * @tc.desc      aceEtsTest
     */
    it('testPanelGetInspectorByKey010', 0, async function (done) {
      console.info('++++++++++++++++ Panel TestCase 1 START ++++++++++++++++');
      let strJson = getInspectorByKey('panel');
      let obj = JSON.parse(strJson);
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('Panel')
      console.log('Panel‘s type is ' + JSON.stringify(obj.$attrs.type))
      console.log('Panel‘s mode is ' + JSON.stringify(obj.$attrs.mode))
      console.log('Panel‘s dragBar is ' + JSON.stringify(obj.$attrs.dragBar))
      console.log('Panel‘s fullHeight is ' + JSON.stringify(obj.$attrs.fullHeight))
      console.log('Panel‘s halfHeight is ' + JSON.stringify(obj.$attrs.halfHeight))
      console.log('Panel‘s miniHeight is ' + JSON.stringify(obj.$attrs.miniHeight))
      expect(obj.$attrs.type).assertEqual('PanelType.Foldable')
      expect(obj.$attrs.mode).assertEqual('PanelMode.Half')
      expect(obj.$attrs.dragBar).assertEqual('true')
      expect(obj.$attrs.fullHeight).assertEqual('500.000000vp')
      expect(obj.$attrs.halfHeight).assertEqual('250.000000vp')
      expect(obj.$attrs.miniHeight).assertEqual('100.000000vp')
      console.info('++++++++++++++++ Panel TestCase 1 End ++++++++++++++++');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0940
     * @tc.name      testPanelGetInspectorByKey020
     * @tc.desc      aceEtsTest
     */
    it('testPanelGetInspectorByKey020', 0, async function (done) {
      console.info('++++++++++++++++ Panel TestCase 2 START ++++++++++++++++');
      let strJson = getInspectorByKey('panel2');
      let obj = JSON.parse(strJson);
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('Panel')
      console.log('Panel‘s type is ' + JSON.stringify(obj.$attrs.type))
      console.log('Panel‘s mode is ' + JSON.stringify(obj.$attrs.mode))
      console.log('Panel‘s show is ' + JSON.stringify(obj.$attrs.show))
      expect(obj.$attrs.type).assertEqual('PanelType.Minibar')
      expect(obj.$attrs.mode).assertEqual('PanelMode.Mini')
      expect(obj.$attrs.show).assertEqual('true')
      console.info('++++++++++++++++ Panel TestCase 2 End ++++++++++++++++');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0950
     * @tc.name      testPanelGetInspectorByKey030
     * @tc.desc      aceEtsTest
     */
    it('testPanelGetInspectorByKey030', 0, async function (done) {
      console.info('++++++++++++++++ Panel TestCase 3 START ++++++++++++++++');
      let strJson = getInspectorByKey('panel3');
      let obj = JSON.parse(strJson);
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('Panel')
      console.log('Panel‘s type is ' + JSON.stringify(obj.$attrs.type))
      console.log('Panel‘s mode is ' + JSON.stringify(obj.$attrs.mode))
      console.log('Panel‘s show is ' + JSON.stringify(obj.$attrs.show))
      expect(obj.$attrs.type).assertEqual('PanelType.Temporary')
      expect(obj.$attrs.mode).assertEqual('PanelMode.Full')
      expect(obj.$attrs.show).assertEqual('false')
      console.info('++++++++++++++++ Panel TestCase 3 End ++++++++++++++++');
      done();
    });
  })
}
