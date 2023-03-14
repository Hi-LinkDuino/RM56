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

import featureAbility from '@ohos.ability.featureAbility'
import {describe, beforeEach, afterEach, it, expect} from "deccjsunit/index.ets";
import Utils from './Utils';

export default function ContextJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("context before each called");
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("context after each called");
    });

    /*
     * @tc.number: context_getCacheDir_test_0100
     * @tc.name: getCacheDir : Obtains the Cache directory on internal storage.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it("context_getCacheDir_test_0100", 0, async function (done) {
      console.log("------------------start context_getCacheDir_test_0100-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getCacheDir().then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getCacheDir_test_0100', timeOldStamp, timeNewStamp)
          console.log("context_getCacheDir_test_0100: CacheDir is " + data);
        })
      } catch (error) {
        console.log("logMessage context_getCacheDir_test_0100: error = " + error);
        expect(error).assertEqual("/data/storage/el2/base/haps/entry/cache");
      }
      var data1 = await context.getCacheDir();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("/data/storage/el2/base/haps/entry/cache");
      done();
      console.log("------------------end context_getCacheDir_test_0100-------------------");
    });

    /*
     * @tc.number: context_getCacheDir_test_0200
     * @tc.name: getCacheDir : Obtains the Cache directory on internal storage.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     * @tc.level   0
     */
    it("context_getCacheDir_test_0200", 0, async function (done) {
      console.log("------------------start context_getCacheDir_test_0200-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getCacheDir((err, data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getCacheDir_test_0200', timeOldStamp, timeNewStamp)
          console.log("context_getCacheDir_test_0200: getCacheDir successful, result = " + data);
        });
      } catch (error) {
        console.log("logMessage context_getCacheDir_test_0200: error = " + error);
        expect(error).assertEqual("/data/storage/el2/base/haps/entry/cache");
      }
      var data1 = await context.getCacheDir();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("/data/storage/el2/base/haps/entry/cache");
      done();
      console.log("------------------end context_getCacheDir_test_0200-------------------");
    });

    /*
     * @tc.number: context_getFilesDir_test_0100
     * @tc.name: getFilesDir : Obtains the files directory on internal storage.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it("context_getFilesDir_test_0100", 0, async function (done) {
      console.log("------------------start context_getFilesDir_test_0100-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getFilesDir().then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getFilesDir_test_0100', timeOldStamp, timeNewStamp)
          console.log("context_getFilesDir_test_0100：FilesDir is " + data);
        })
      } catch (error) {
        console.log("logMessage context_getFilesDir_test_0100: error = " + error);
        expect(error).assertEqual("/data/storage/el2/base/haps/entry/files");
      }
      var data1 = await context.getFilesDir();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("/data/storage/el2/base/haps/entry/files");
      done();
      console.log("------------------end context_getFilesDir_test_0100-------------------");
    });

    /*
     * @tc.number: context_getFilesDir_test_0200
     * @tc.name: getFilesDir : Obtains the files directory on internal storage.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     * @tc.level   0
     */
    it("context_getFilesDir_test_0200", 0, async function (done) {
      console.log("------------------start context_getFilesDir_test_0200-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getFilesDir((err, data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getFilesDir_test_0200', timeOldStamp, timeNewStamp)
          console.log("context_getFilesDir_test_0200: getFilesDir successful, result = " + data);
        });
      } catch (error) {
        console.log("logMessage context_getFilesDir_test_0200: error = " + error);
        expect(error).assertEqual("/data/storage/el2/base/haps/entry/files");
      }
      var data1 = await context.getFilesDir();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("/data/storage/el2/base/haps/entry/files");
      done();
      console.log("------------------end context_getFilesDir_test_0200-------------------");
    });


    /*
     * @tc.number: context_getOrCreateDistributedDir_test_0100
     * @tc.name: getOrCreateDistributedDir : Obtains the directory of Distributed files.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it("context_getOrCreateDistributedDir_0100", 0, async function (done) {
      console.log("------------------start context_getOrCreateDistributedDir_0100-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getOrCreateDistributedDir().then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getOrCreateDistributedDir_0100', timeOldStamp, timeNewStamp);
          console.log("context_getOrCreateDistributedDir_0100: DistributedDir is " + data);
        })
      } catch (error) {
        console.log("logMessage context_getOrCreateDistributedDir_0100: error = " + error);
        expect(error).assertEqual("/data/storage/el2/distributedfiles/entry");
      }
      var data1 = await context.getOrCreateDistributedDir();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("/data/storage/el2/distributedfiles/entry");
      done();
      console.log("------------------end context_getOrCreateDistributedDir_0100-------------------");
    });

    /*
     * @tc.number: context_getOrCreateDistributedDir_0200
     * @tc.name: getOrCreateDistributedDir : Obtains the directory of Distributed files.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     * @tc.level   0
     */
    it("context_getOrCreateDistributedDir_0200", 0, async function (done) {
      console.log("------------------start context_getOrCreateDistributedDir_0200-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getOrCreateDistributedDir((err, data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getOrCreateDistributedDir_0200', timeOldStamp, timeNewStamp);
          console.log("context_getOrCreateDistributedDir_0200: getOrCreateDistributedDir successful, result = " + data);
        });
      } catch (error) {
        console.log("logMessage context_getOrCreateDistributedDir_0200: error = " + error);
        expect(error).assertEqual("/data/storage/el2/distributedfiles/entry");
      }
      var data1 = await context.getOrCreateDistributedDir();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("/data/storage/el2/distributedfiles/entry");
      done();
      console.log("------------------end context_getOrCreateDistributedDir_0200-------------------");
    });

    /*
     * @tc.number: context_getAppType_0100
     * @tc.name: getAppType : Obtains the type of current application.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it("context_getAppType_0100", 0, async function (done) {
      console.log("------------------start context_getAppType_0100-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getAppType().then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getAppType_0100_0100', timeOldStamp, timeNewStamp);
          console.log("context_getAppType_0100: AppType is " + data);
        })
      } catch (error) {
        console.log("logMessage context_getAppType_0100: error = " + error);
        expect(error).assertEqual("system");
      }
      var data1 = await context.getAppType();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("system");
      done();
      console.log("------------------end context_getAppType_0100-------------------");
    });

    /*
     * @tc.number: context_getAppType_0200
     * @tc.name: getAppType : Obtains the type of current application.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     * @tc.level   0
     */
    it("context_getAppType_0200", 0, async function (done) {
      console.log("------------------start context_getAppType_0200-------------------");
      try {
        var timeOldStamp = await Utils.getNowTime();
        var context = await featureAbility.getContext();
        context.getAppType((err, data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getAppType_0200_0100', timeOldStamp, timeNewStamp);
          console.log("context_getAppType_0200: getAppType successful, result = " + data);
        });
      } catch (error) {
        console.log("logMessage context_getAppType_0200: error = " + error);
        expect(error).assertEqual("system");
      }
      var data1 = await context.getAppType();
      expect(typeof (data1)).assertEqual("string");
      expect(data1).assertEqual("system");
      done();
      console.log("------------------end context_getAppType_0200-------------------");
    });
  })
}
