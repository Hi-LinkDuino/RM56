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

export default function listItemJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('listItemTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/ListItem',
      }
      try {
        router.clear();
	await sleep(1);
        let pages = router.getState();
        console.info("get ListItem state success " + JSON.stringify(pages));
        if (!("ListItem" == pages.name)) {
          console.info("get ListItem state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push ListItem page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push ListItem page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("ListItem after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0390
     * @tc.name      listItemTest001
     * @tc.desc      aceEtsTest
     */
    it('listItemTest001', 0, async function (done) {
      console.info('++++++++++++++++ ListItem TestCase 1 START ++++++++++++++++');
      await sleep(1)
      let strJson = getInspectorByKey('listitem');
      console.log(JSON.stringify(strJson))
      let obj = JSON.parse(strJson);
      expect(obj.$type).assertEqual('ListItem')
      console.log(JSON.stringify(obj.$attrs.type))
      console.log('ListItem1‘s sticky is ' + JSON.stringify(obj.$attrs.sticky))
      console.log('ListItem1‘s editable is ' + JSON.stringify(obj.$attrs.editable))
      expect(obj.$attrs.sticky).assertEqual('Sticky.None')
      expect(obj.$attrs.editable).assertEqual('true')
      console.info('++++++++++++++++ ListItem TestCase 1 End ++++++++++++++++');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0400
     * @tc.name      listItemTest002
     * @tc.desc      aceEtsTest
     */
    it('listItemTest002', 0, async function (done) {
      console.info('++++++++++++++++ ListItem TestCase 2 START ++++++++++++++++');
      let strJson = getInspectorByKey('listitem2');
      let obj = JSON.parse(strJson);
      console.info("get inspector value is: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('ListItem')
      console.log(JSON.stringify(obj.$attrs.type))
      console.log('ListItem2‘s sticky is ' + JSON.stringify(obj.$attrs.sticky))
      console.log('ListItem2‘s editable is ' + JSON.stringify(obj.$attrs.editable))
      await sleep(1)
      expect(obj.$attrs.sticky).assertEqual('Sticky.Normal')
      expect(obj.$attrs.editable).assertEqual('EditMode.Deletable')
      console.info('++++++++++++++++ ListItem TestCase 2 End ++++++++++++++++');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0410
     * @tc.name      listItemTest003
     * @tc.desc      aceEtsTest
     */
    it('listItemTest003', 0, async function (done) {
      console.info('++++++++++++++++ ListItem TestCase 3 START ++++++++++++++++');
      let strJson = getInspectorByKey('listitem3');
      let obj = JSON.parse(strJson);
      console.info("get inspector value is: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('ListItem')
      console.log(JSON.stringify(obj.$attrs.type))
      console.log('ListItem3‘s sticky is ' + JSON.stringify(obj.$attrs.sticky))
      console.log('ListItem3‘s editable is ' + JSON.stringify(obj.$attrs.editable))
      await sleep(1)
      expect(obj.$attrs.sticky).assertEqual('Sticky.Opacity')
      expect(obj.$attrs.editable).assertEqual('false')
      console.info('++++++++++++++++ ListItem TestCase 3 End ++++++++++++++++');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0420
     * @tc.name      listItemTest004
     * @tc.desc      aceEtsTest
     */
    it('listItemTest004', 0, async function (done) {
      console.info('++++++++++++++++ ListItem TestCase 4 START ++++++++++++++++');
      let strJson = getInspectorByKey('listitem4');
      let obj = JSON.parse(strJson);
      console.info("get inspector value is: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('ListItem')
      console.log(JSON.stringify(obj.$attrs.type))
      console.log('ListItem4‘s sticky is ' + JSON.stringify(obj.$attrs.sticky))
      console.log('ListItem4‘s editable is ' + JSON.stringify(obj.$attrs.editable))
      await sleep(1)
      expect(obj.$attrs.sticky).assertEqual('Sticky.Opacity')
      expect(obj.$attrs.editable).assertEqual('EditMode.Movable')
      console.info('++++++++++++++++ ListItem TestCase 4 End ++++++++++++++++');
      done();
    });
  })
}
