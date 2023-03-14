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
import app from '@system.app';

export default function systemAppJsunit() {
  describe('appInfoTest', function () {

    it('systemAppTest_0100', 0, async function (done) {
      console.info('systemAppTest_0100 START');
      var a = app.getInfo();
      console.log('systemAppTest_0100 END' + JSON.stringify(a));
      expect(a.appID == "com.open.harmony.acetestfour").assertTrue();
      console.info('systemAppTest_0100 END');
      done();
    });

    it('systemAppTest_0200', 0, async function (done) {
      console.info('systemAppTest_0200 START');
      var a = app.getInfo();
      console.log('systemAppTest_0200 END' + JSON.stringify(a));
      expect(a.appName == "$string:entry_MainAbility").assertTrue();
      console.info('systemAppTest_0200 END');
      done();
    });

    it('systemAppTest_0300', 0, async function (done) {
      console.info('systemAppTest_0300 START');
      var a = app.getInfo();
      console.log('systemAppTest_0300 END' + JSON.stringify(a));
      expect(a.versionName == "1.0.0").assertTrue();
      console.info('systemAppTest_0300 END');
      done();
    });

    it('systemAppTest_0400', 0, async function (done) {
      console.info('systemAppTest_0400 START');
      try {
        var a = 2000;
        app.requestFullWindow(a);
      } catch (err) {
        console.log("systemAppTest_0400 error: " + err);
      }
      console.info('systemAppTest_0400 END');
      done();
    });

    it('systemAppTest_0500', 0, async function (done) {
      console.info('systemAppTest_0500 START');
      try {
        var visible = true;
        app.screenOnVisible(visible);
      } catch (err) {
        console.log("systemAppTest_0500 error: " + err);
      }
      console.info('systemAppTest_0500 END');
      done();
    });

    it('systemAppTest_0600', 0, async function (done) {
      console.info('systemAppTest_0600 START');
      var a = 100;
      app.setImageCacheCount(a);
      console.info('systemAppTest_0600 END');
      done();
    });

    it('systemAppTest_0700', 0, async function (done) {
      console.info('systemAppTest_0700 START');
      var a = 104857600;
      app.setImageRawDataCacheSize(a);
      console.info('systemAppTest_0700 END');
      done();
    });

    it('systemAppTest_0800', 0, async function (done) {
      console.info('systemAppTest_0800 START');
      var a = 1048;
      app.setImageFileCacheSize(a);
      console.info('systemAppTest_0800 END');
      done();
    });

    it('systemAppTest_0900', 0, async function (done) {
      console.info('systemAppTest_0900 START');
      app.terminate();
      console.info('systemAppTest_0900 END');
      done();
    });
  })
}