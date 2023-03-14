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
  fileio,
  describe, it, expect, nextFileName, prepareFile, FILE_CONTENT
} from '../../Common';

describe('fileio_close', function () {

  /**
   * @tc.number SUB_DF_FILEIO_CLOSESYNC_0000
   * @tc.name fileio_test_close_sync_000
   * @tc.desc Test closeSync() interfaces
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_close_sync_000', 0, function () {
    try {
      fileio.closeSync();
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_close_sync_000 has failed for ' + e);
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_CLOSESYNC_0010
   * @tc.name fileio_test_close_sync_001
   * @tc.desc Test closeSync() interfaces
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_close_sync_001', 0, function () {
    try {
      fileio.closeSync(-1);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_close_sync_001 has failed for ' + e);
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_CLOSE_ASYNC_0000
   * @tc.name fileio_test_close_async_000
   * @tc.desc Test close() interfaces
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_close_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_close_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      fileio.close(fd, function (err) {
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_close_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_CLOSE_ASYNC_0010
   * @tc.name fileio_test_close_async_001
   * @tc.desc Test close() interfaces
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_close_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_close_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(await fileio.close(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_close_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_CLOSE_ASYNC_0020
   * @tc.name fileio_test_close_async_002
   * @tc.desc Test close() interfaces
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_close_async_002', 0, async function (done) {
    try {
      await fileio.close(-1);
      expect(null).assertFail();
      done();
    } catch (e) {
      expect(!!e).assertTrue();
      console.log('fileio_test_close_async_002 has failed for ' + e);
      done();
    }
  })
})
