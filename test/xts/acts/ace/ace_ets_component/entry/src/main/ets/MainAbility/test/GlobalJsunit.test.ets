// @ts-nocheck
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';
import Utils from './Utils.ets';

export default function globalJsunit() {
  describe('globalJsunit', function () {
    beforeEach(async function (done) {
      console.info("global beforeEach start");
      let options = {
        uri: 'pages/global',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get global state pages:" + JSON.stringify(pages));
        if (!("global" == pages.name)) {
          console.info("get global state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push global page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push global page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
    });

    /**
     * @tc.number ACE_global_setTimeout
     * @tc.name setTimeout()
     * @tc.desc Test setTimeout
     */
    it('ACE_global_setTimeout', 0, function (done) {
      let caseName: string = "ACE_global_setTimeout"
      console.info(`${caseName} START`);
      setTimeout(() => {
        expect(true).assertTrue();
      }, 100)
      console.info(`${caseName} END`);
      done();
    });

    /**
     * @tc.number ACE_global_canIUse
     * @tc.name canIUse()
     * @tc.desc Test canIUse
     */
    it('ACE_global_canIUse', 0, function (done) {
      let caseName: string = "ACE_global_canIUse"
      console.info(`${caseName} START`);
      let syscapName: string = "";
      var result = canIUse(syscapName);
      expect(result === false).assertTrue();
      console.info(`${caseName} END`);
      done();
    });

    /**
     * @tc.number ACE_global_getApp
     * @tc.name getApp()
     * @tc.desc Test getApp
     */
    it('ACE_global_getApp', 0, function (done) {
      let caseName: string = "ACE_global_getApp"
      console.info(`${caseName} START`);
      try {
        var info = getApp();
      } catch (err) {
      }
      console.info(`${caseName} END`);
      done();
    });

    /**
     * @tc.number ACE_global_createLocalParticleAbility
     * @tc.name createLocalParticleAbility()
     * @tc.desc Test createLocalParticleAbility
     */
    it('ACE_global_createLocalParticleAbility', 0, function (done) {
      let caseName: string = "ACE_global_createLocalParticleAbility"
      console.info(`${caseName} START`);
      try {
        var info = createLocalParticleAbility(100);
      } catch (err) {
      }
      console.info(`${caseName} END`);
      done();
    });


    /**
     * @tc.number ACE_global_console_log_0100
     * @tc.name console::log
     * @tc.desc Test console log printing
     */
    it("ACE_global_console_log_0100", 0, async function (done) {
      var casename = "ACE_global_001";
      console.log("-----------------------ACE_global_console_log Test is starting-----------------------");
      try {
        console.log("ACE_global_console_log test ok");
        expect(true).assertTrue();
      } catch (error) {
        console.log(casename + "errorcode: " + casename);
        expect().assertFail();
        done();
      }
      console.log("-----------------------ACE_global_console_log Test end-----------------------");
      done();
    });

    /**
     * @tc.number ACE_global_console_info_0100
     * @tc.name console::info
     * @tc.desc Test console info printing
     */
    it("ACE_global_console_info_0100", 0, async function (done) {
      var casename = "ACE_global_002";
      console.log("-----------------------ACE_global_console_info Test is starting-----------------------");
      try {
        console.info("ACE_global_console_info test ok");
        expect(true).assertTrue();
      } catch (error) {
        console.log(casename + "errorcode: " + casename);
        expect().assertFail();
        done();
      }
      console.log("-----------------------ACE_global_console_info Test end-----------------------");
      done();
    });

    /**
     * @tc.number ACE_global_console_warn_0100
     * @tc.name console::warn
     * @tc.desc Test console warn printing
     */
    it("ACE_global_console_warn_0100", 0, async function (done) {
      var casename = "ACE_global_003";
      console.log("-----------------------ACE_global_console_warn Test is starting-----------------------");
      try {
        console.warn("ACE_global_console_warn test ok");
        expect(true).assertTrue();
      } catch (error) {
        console.log(casename + "errorcode: " + casename);
        expect().assertFail();
        done();
      }
      console.log("-----------------------ACE_global_console_warn Test end-----------------------");
      done();
    });


  })
}
