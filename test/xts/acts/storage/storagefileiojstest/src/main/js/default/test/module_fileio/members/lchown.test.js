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

describe('fileio_lchown', function () {

  /**
   * @tc.number SUB_DF_FILEIO_LSEEK_SYNC_000
   * @tc.name fileio_test_lchown_sync_000
   * @tc.desc Test lchownSync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_lchown_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_lchown_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(fileio.lchownSync(fpath, stat.uid, stat.gid) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();

    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_LSEEK_SYNC_001
   * @tc.name fileio_test_lchown_sync_001
   * @tc.desc Test lchownSync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it('fileio_test_lchown_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_lchown_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(isIntNum(stat.uid)).assertTrue();
      expect(fileio.lchownSync('/', stat.uid, stat.gid));
      expect(null).assertFail();
    } catch (e) {
      expect(!!e).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_LCHOWN_ASYNC_000
   * @tc.name fileio_test_lchown_async_00
   * @tc.desc Test lchownASync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_lchown_Async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_lchown_Async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(await fileio.lchown(fpath, stat.uid, stat.gid) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_lchown_async_000 has failed for ' + e);
      expect(null).assertFail();
    }

  });

  /**
   * @tc.number SUB_DF_FILEIO_LCHOWN_ASYNC_001
   * @tc.name fileio_test_lchown_async_00
   * @tc.desc Test lchownASync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_lchown_Async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_lchown_Async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      fileio.lchown(fpath, stat.uid, stat.gid, function (error) {
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_lchown_Async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

});
