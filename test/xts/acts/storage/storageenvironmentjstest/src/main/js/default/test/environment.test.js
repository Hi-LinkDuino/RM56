/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
  isIntNum,
  describe,
  it,
  expect,
  environment,
  isString,
} from "./Common";

describe("environment", function () {

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_STORAGE_DATA_0000
   * @tc.name environment_test_get_storage_data_async_000
   * @tc.desc Test getStorageDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_storage_data_async_000", 0, async function (done) {
    try {
      let url = await environment.getStorageDataDir();
      expect(isString(url)).assertTrue();
      expect(url == "/data").assertTrue();
      console.log("promise environment_test_get_storage_data_async_000:" + url);
      done();
    } catch (e) {
      console.log("environment_test_get_storage_data_async_000 has failed for " + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_STORAGE_DATA_0010
   * @tc.name environment_test_get_storage_data_async_001
   * @tc.desc Test getStorageDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_storage_data_async_001", 0, async function (done) {
    try {
      environment.getStorageDataDir((err, url) => {
        expect(isString(url)).assertTrue();
        expect(url == "/data").assertTrue();
        console.log("callback environment_test_get_storage_data_async_001:" + url);
        done();
      });
    } catch (e) {
      console.log("environment_test_get_storage_data_async_001 has failed for " + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_STORAGE_DATA_0020
   * @tc.name environment_test_get_storage_data_async_002
   * @tc.desc Test getStorageDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_storage_data_async_002", 0, async function (done) {
    try {
      await environment.getStorageDataDir("123%$*&^");
      expect(null).assertFail();
    } catch (e) {
      console.log("environment_test_get_storage_data_async_002 has failed for " + e);
      expect(!!e).assertTrue();
    }
    done();
  });

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_STORAGE_DATA_0030
   * @tc.name environment_test_get_storage_data_async_003
   * @tc.desc Test getStorageDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_storage_data_async_003", 0, async function (done) {
    try {
      await environment.getStorageDataDir(true);
      expect(null).assertFail();
    } catch (e) {
      console.log("environment_test_get_storage_data_async_003 has failed for " + e);
      expect(!!e).assertTrue();
    }
    done();
  });

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_USER_DATA_0000
   * @tc.name environment_test_get_user_data_async_000
   * @tc.desc Test getUserDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_user_data_async_000", 0, async function (done) {
    try {
      let url = await environment.getUserDataDir();
      expect(isString(url)).assertTrue();
      expect(url == "/storage/media/0/local").assertTrue();
      console.log("promise environment_test_get_user_data_async_000:" + url);
      done();
    } catch (e) {
      console.log("environment_test_get_user_data_async_000 has failed for " + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_USER_DATA_0010
   * @tc.name environment_test_get_user_data_async_001
   * @tc.desc Test getUserDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_user_data_async_001", 0, async function (done) {
    try {
      environment.getUserDataDir((err, url) => {
        expect(isString(url)).assertTrue();
        expect(url == "/storage/media/0/local").assertTrue();
        console.log("callback environment_test_get_user_data_async_001:" + url);
        done();
      });
    } catch (e) {
      console.log("callback environment_test_get_user_data_async_001 has failed for " + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_USER_DATA_0020
   * @tc.name environment_test_get_user_data_async_002
   * @tc.desc Test getUserDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_user_data_async_002", 0, async function (done) {
    try {
      await environment.getUserDataDir("123#$%^&*");
      expect(null).assertFail();
    } catch (e) {
      console.log("environment_test_get_user_data_async_002 has failed for " + e);
      expect(!!e).assertTrue();
    }
    done();
  });

  /**
   * @tc.number SUB_DF_ENVIRONMENT_GET_USER_DATA_0030
   * @tc.name environment_test_get_user_data_async_003
   * @tc.desc Test getUserDataDir() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("environment_test_get_user_data_async_003", 0, async function (done) {
    try {
      await environment.getUserDataDir(true);
      expect(null).assertFail();
    } catch (e) {
      console.log("environment_test_get_user_data_async_003 has failed for " + e);
      expect(!!e).assertTrue();
    }
    done();
  });
});
