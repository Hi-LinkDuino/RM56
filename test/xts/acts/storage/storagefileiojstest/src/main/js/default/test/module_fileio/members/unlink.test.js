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
  fileio, FILE_CONTENT, prepareFile, nextFileName,
  describe, it, expect,
} from '../../Common';

describe('fileio_unlink', function () {

  /**
   * @tc.number SUB_DF_FILEIO_UNLINK_SYNC_0000
   * @tc.name fileio_test_unlink_sync_000
   * @tc.desc Test unlinkSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_unlink_sync_000', 0, function () {
    try {
      fileio.unlinkSync();
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_unlink_sync_000 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_UNLINK_SYNC_0010
   * @tc.name fileio_test_unlink_sync_001
   * @tc.desc Test unlinkSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_unlink_sync_001', 0, async function () {
    let fpath = await nextFileName('fileIOTest');

    try {
      fileio.unlinkSync(fpath);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_unlink_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_UNLINK_SYNC_0020
   * @tc.name fileio_test_unlink_sync_002
   * @tc.desc Test unlinkSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_unlink_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_unlink_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(fileio.accessSync(fpath) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_unlink_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_UnlinkAync_0000
   * @tc.name fileio_test_unlink_async_000
   * @tc.desc Test unlinkAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_unlink_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileIOTest');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(await fileio.unlink(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_unlink_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_UnlinkAync_0010
   * @tc.name fileio_test_unlink_async_001
   * @tc.desc Test unlinkAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_unlink_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileIOTest');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.unlink(fpath, function (error) {
        done();
      });
    } catch (e) {
      console.log('fileio_test_unlink_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_UNLINK_SYNC_0020
   * @tc.name fileio_test_unlink_sync_002
   * @tc.desc Test unlink() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_unlink_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_unlink_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.unlink(fpath, function (err) {
        done();
      });
    } catch (e) {
      console.log('fileio_test_unlink_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_UNLINK_SYNC_0030
   * @tc.name fileio_test_unlink_sync_003
   * @tc.desc Test unlink() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_unlink_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_unlink_async_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.unlink(fpath).then(function (err) {
        done();
      });
    } catch (e) {
      console.log('fileio_test_unlink_async_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
