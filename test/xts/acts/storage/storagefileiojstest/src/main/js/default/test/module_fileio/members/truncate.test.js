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
  fileio, FILE_CONTENT, prepareFile, nextFileName, isIntNum,
  describe, it, expect,
} from '../../Common';

describe('fileio_truncate', function () {

  /**
   * @tc.number SUB_DF_FILEIO_TRUNCATE_ASYNC_0000
   * @tc.name fileio_truncate_async_000
   * @tc.desc Test truncateAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_truncate_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_truncate_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let truncateLen = 5;

    try {
      fileio.truncate(fpath, truncateLen, function (err) {
        let fd = fileio.openSync(fpath, 0o2);
        expect(isIntNum(fd)).assertTrue();
        let len = fileio.readSync(fd, new ArrayBuffer(4096));
        expect(len == truncateLen).assertTrue();
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_truncate_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });
  
  /**
   * @tc.number SUB_DF_FILEIO_TRUNCATE_ASYNC_0010
   * @tc.name fileio_truncate_async_001
   * @tc.desc Test truncateAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_truncate_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_truncate_async_001');
    expect(prepareFile(fpath, 'truncate')).assertTrue();
    let truncateLen = 2;

    try {
      fileio
        .truncate(fpath, truncateLen)
        .then(function (err) {
          expect(err == null).assertTrue();
          let fd = fileio.openSync(fpath, 0o2);
          expect(isIntNum(fd)).assertTrue();
          let len = fileio.readSync(fd, new ArrayBuffer(4096));
          expect(len == truncateLen).assertTrue();
          expect(fileio.closeSync(fd) == null).assertTrue();
          expect(fileio.unlinkSync(fpath) == null).assertTrue();
          done();
        })
        .catch(function (e) {
          console.log('catch ' + e);
        });
    } catch (e) {
      console.log('fileio_truncate_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_TRUNCATE_ASYNC_0020
   * @tc.name fileio_truncate_async_002
   * @tc.desc Test truncateAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_truncate_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_truncate_async_002');
    let truncateLen = 2;

    try {
      fileio.truncate(fpath, truncateLen, function (err) {
        done();
      });
    } catch (e) {
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_TRUNCATE_SYNC_0000
   * @tc.name fileio_test_truncate_sync_000
   * @tc.desc Test truncateSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_truncate_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_truncate_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(fileio.truncateSync(fpath) == null).assertTrue();
      expect(fileio.statSync(fpath).size == 0).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_truncate_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_TRUNCATE_SYNC_0010
   * @tc.name fileio_test_truncate_sync_001
   * @tc.desc Test truncateSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_truncate_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_truncate_sync_001');

    try {
      fileio.truncateSync(fpath);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_truncate_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_TRUNCATE_SYNC_0020
   * @tc.name fileio_test_truncate_sync_002
   * @tc.desc Test truncateSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_truncate_sync_002', 0, function () {
    try {
      fileio.truncateSync();
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_truncate_sync_002 has failed for ' + e);
    }
  });
});
