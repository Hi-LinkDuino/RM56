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
  describe, it, expect, isIntNum, statfs
} from './Common';

describe('statfs', function () {

  /**
   * @tc.number SUB_DF_Statfs_getFreeBytes_0000
   * @tc.name statfs_test_get_free_bytes_async_000
   * @tc.desc Test getFreeBytes() interfaces.
   */
  it('statfs_test_get_free_bytes_async_000', 0, async function (done) {
    try {
      statfs.getFreeBytes('/', function (err, number) {
        expect(isIntNum(number)).assertTrue();
        done();
      })
    } catch (e) {
      console.log('statfs_test_get_free_bytes_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getFreeBytes_0010
   * @tc.name statfs_test_get_free_bytes_async_001
   * @tc.desc Test getFreeBytes() interfaces.
   */
  it('statfs_test_get_free_bytes_async_001', 0, async function (done) {
    try {
      let number = await statfs.getFreeBytes("/data");
      expect(isIntNum(number)).assertTrue();
      done();
    } catch (e) {
      console.log('statfs_test_get_free_bytes_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getFreeBytes_0020
   * @tc.name statfs_test_get_free_bytes_async_002
   * @tc.desc Test getFreeBytes() interfaces.
   */
  it('statfs_test_get_free_bytes_async_002', 0, async function (done) {
    try {
      let number = await statfs.getFreeBytes("/dev");
      expect(isIntNum(number)).assertTrue();
      done();
    } catch (e) {
      console.log('statfs_test_get_free_bytes_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getFreeBytes_0030
   * @tc.name statfs_test_get_free_bytes_async_003
   * @tc.desc Test getFreeBytes() interfaces.
   */
  it('statfs_test_get_free_bytes_async_003', 0, async function (done) {
    try {
      await statfs.getFreeBytes("/", '');
      expect(null).assertFail();
      done();
    } catch (e) {
      console.log('statfs_test_get_free_bytes_async_003 has failed for ' + e);
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getFreeBytes_0040
   * @tc.name statfs_test_get_free_bytes_async_004
   * @tc.desc Test getFreeBytes() interfaces.
   */
  it('statfs_test_get_free_bytes_async_004', 0, async function (done) {
    try {
      statfs.getFreeBytes(function (err, number) {
        done();
      })
    } catch (e) {
      console.log('statfs_test_get_free_bytes_async_004 has failed for ' + e);
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getFreeBytes_0050
   * @tc.name statfs_test_get_free_bytes_async_005
   * @tc.desc Test getFreeBytes() interfaces.
   */
  it('statfs_test_get_free_bytes_async_005', 0, async function (done) {
    try {
      await statfs.getFreeBytes("");
      expect(null).assertFail();
      done();
    } catch (e) {
      console.log('statfs_test_get_free_bytes_async_005 has failed for ' + e);
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getTotalBytes_0000
   * @tc.name statfs_test_get_total_bytes_async_000
   * @tc.desc Test getTotalBytes() interfaces.
   */
  it('statfs_test_get_total_bytes_async_000', 0, async function (done) {
    try {
      statfs.getTotalBytes('/', function (err, number) {
        expect(isIntNum(number)).assertTrue();
        done();
      })
    } catch (e) {
      console.log('statfs_test_get_total_bytes_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getTotalBytes_0010
   * @tc.name statfs_test_get_total_bytes_async_001
   * @tc.desc Test getTotalBytes() interfaces.
   */
  it('statfs_test_get_total_bytes_async_001', 0, async function (done) {
    try {
      let number = await statfs.getTotalBytes("/data");
      expect(isIntNum(number)).assertTrue();
      done();
    } catch (e) {
      console.log('statfs_test_get_total_bytes_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getTotalBytes_0020
   * @tc.name statfs_test_get_total_bytes_async_002
   * @tc.desc Test getTotalBytes() interfaces.
   */
  it('statfs_test_get_total_bytes_async_002', 0, async function (done) {
    try {
      let number = await statfs.getTotalBytes("/dev");
      expect(isIntNum(number)).assertTrue();
      done();
    } catch (e) {
      console.log('statfs_test_get_total_bytes_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getTotalBytes_0030
   * @tc.name statfs_test_get_total_bytes_async_003
   * @tc.desc Test getTotalBytes() interfaces.
   */
  it('statfs_test_get_total_bytes_async_003', 0, async function (done) {
    try {
      await statfs.getTotalBytes("/", '');
      expect(null).assertFail();
      done();
    } catch (e) {
      console.log('statfs_test_get_total_bytes_async_003 has failed for ' + e);
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getTotalBytes_0040
   * @tc.name statfs_test_get_total_bytes_async_004
   * @tc.desc Test getTotalBytes() interfaces.
   */
  it('statfs_test_get_total_bytes_async_004', 0, async function (done) {
    try {
      statfs.getTotalBytes(function (err, number) {
        done();
      })
    } catch (e) {
      console.log('statfs_test_get_total_bytes_async_004 has failed for ' + e);
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_Statfs_getTotalBytes_0050
   * @tc.name statfs_test_get_total_bytes_async_005
   * @tc.desc Test getTotalBytes() interfaces.
   */
  it('statfs_test_get_total_bytes_async_005', 0, async function (done) {
    try {
      await statfs.getFreeBytes("");
      expect(null).assertFail();
      done();
    } catch (e) {
      console.log('statfs_test_get_total_bytes_async_005 has failed for ' + e);
      expect(!!e).assertTrue();
      done();
    }
  });
});
