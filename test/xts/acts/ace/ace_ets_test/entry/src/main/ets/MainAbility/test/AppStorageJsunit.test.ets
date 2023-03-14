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

export default function appStorageJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('appStorageTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/AppStorage',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get AppStorage state success " + JSON.stringify(pages));
        if (!("AppStorage" == pages.name)) {
          console.info("get AppStorage state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push AppStorage page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push AppStorage page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("AppStorage after each called");
    });


    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0130
     * @tc.name      appStorageTest001
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest001', 0, async function (done) {
      console.info('AppStorageTest001 START');
      await sleep(1)
      let strJson = getInspectorByKey('Button0_1');
      let obj = JSON.parse(strJson);
      expect(obj.$type).assertEqual('Button');
      console.log(JSON.stringify(obj.$type))
      console.info("yr AppStorage0 type: " + JSON.stringify(obj.$type));
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('20');
      console.log("yr AppStorage0 label-'47'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorage0 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0140
     * @tc.name      appStorageTest002
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest002', 0, async function (done) {
      console.info('AppStorageTest002 START');
      let strJson = getInspectorByKey('Button0_2');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('20');
      console.log("yr AppStorage1 label-'47'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest002 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0150
     * @tc.name      appStorageTest003
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest003', 0, async function (done) {
      console.info('AppStorageTest003 START');
      let strJson = getInspectorByKey('Button0_3');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('50');
      console.log("yr AppStorage2 label-'50'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest003 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0160
     * @tc.name      appStorageTest004
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest004', 0, async function (done) {
      console.info('AppStorageTest004 START');
      let strJson = getInspectorByKey('Button0_4');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('47');
      console.log("yr AppStorage3 label-'47'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest004 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0170
     * @tc.name      appStorageTest005
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest005', 0, async function (done) {
      console.info('AppStorageTest005 START');
      let strJson = getInspectorByKey('Button0_5');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('true');
      console.log("yr AppStorage4 label-' '" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest005 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0180
     * @tc.name      appStorageTest006
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest006', 0, async function (done) {
      console.info('AppStorageTest006 START');
      let strJson = getInspectorByKey('Button0_6');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('PropB');
      console.log("yr AppStorage5 label-'PropA'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest006 END');
      done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0190
     * @tc.name      appStorageTest007
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest007', 0, async function (done) {
      console.info('AppStorageTest007 START');
      let strJson = getInspectorByKey('Button0_7');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('20');
      console.log("yr AppStorage6 label-'47'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest007 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0100
     * @tc.name      appStorageTest008
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest008', 0, async function (done) {
      console.info('AppStorageTest008 START');
      let strJson = getInspectorByKey('Button0_8');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('true');
      console.log("yr AppStorage7 label-'20'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest008 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0210
     * @tc.name      appStorageTest009
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest008', 0, async function (done) {
      console.info('AppStorageTest009 START');
      let strJson = getInspectorByKey('Button0_9');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('1');
      console.log("yr AppStorage8 label-'1'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest009 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0220
     * @tc.name      appStorageTest010
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest010', 0, async function (done) {
      console.info('AppStorageTest010 START');
      let strJson = getInspectorByKey('Button0_10');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.label).assertEqual('false');
      console.log("yr AppStorage9 label-' '" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest010 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0230
     * @tc.name      appStorageTest011
     * @tc.desc      aceEtsTest
     */
    it('appStorageTest011', 0, async function (done) {
      console.info('AppStorageTest011 START');
      let strJson = getInspectorByKey('Button0_12');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.content).assertEqual('true');
      console.log("yr AppStorage11 label-'50'" + JSON.stringify(obj.$attrs.label))
      console.info('AppStorageTest011 END');
      done();
    });
  })
}
