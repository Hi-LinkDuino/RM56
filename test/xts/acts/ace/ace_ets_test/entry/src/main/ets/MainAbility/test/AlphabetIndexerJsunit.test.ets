// @ts-nocheck
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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';

export default function alphabetIndexerJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('alphabetIndexerTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/AlphabetIndexer',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get AlphabetIndexer state success " + JSON.stringify(pages));
        if (!("AlphabetIndexer" == pages.name)) {
          console.info("get AlphabetIndexer state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push AlphabetIndexer page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push AlphabetIndexer page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("AlphabetIndexer after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0100
     * @tc.name      alphabetIndexerTest001
     * @tc.desc      aceEtsTest
     */
    it('alphabetIndexerTest001', 0, async function (done) {
      console.info('AlphabetIndexerTest001 START');
      await sleep(1)
      let strJson = getInspectorByKey('alphabetIndexer');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('AlphabetIndexer');
      console.info('AlphabetIndexerTest001 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0110
     * @tc.name      alphabetIndexerTest002
     * @tc.desc      aceEtsTest
     */
    it('alphabetIndexerTest002', 0, async function (done) {
      console.info('AlphabetIndexerTest002 START');
      let strJson = getInspectorByKey('alphabetIndexer1');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.info('AlphabetIndexerTest002 END');
      done();
    });
  })
}
