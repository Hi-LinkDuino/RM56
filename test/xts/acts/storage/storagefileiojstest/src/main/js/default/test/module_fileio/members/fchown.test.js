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

describe('fileio_fchown', async function () {

  /**
   * @tc.number SUB_DF_FILEIO_FCHOWN_ASYNC_0000
   * @tc.name fileio_test_fchown_async_000
   * @tc.desc Test fchown() and open() and statSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchown_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fchown_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(isIntNum(stat.uid)).assertTrue();
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      fileio.fchown(fd, stat.uid, stat.gid, function (err) {
        fileio.closeSync(fd);
        fileio.unlinkSync(fpath);
        done();
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHOWN_ASYNC_0010
   * @tc.name fileio_test_fchown_async_001
   * @tc.desc Test fchown() and open() and statSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchown_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fchown_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(isIntNum(stat.uid)).assertTrue();
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      fileio.fchown(fd, stat.uid, stat.gid);
      expect(fileio.closeSync(fd) == null).assertTrue();
      fileio.unlinkSync(fpath);
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHOWN_ASYNC_0020
   * @tc.name fileio_test_fchown_async_002
   * @tc.desc Test fchown() and open() and statSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it('fileio_test_fchown_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fchown_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(isIntNum(stat.uid)).assertTrue();
      fileio.fchown(-1, stat.uid, stat.gid);
      expect(null).assertFail();
      done();
    } catch (e) {
      console.log('fileio_test_fchown_003 has failed for ' + e);
      expect(!!e).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHOWN_SYNC_0000
   * @tc.name fileio_test_fchown_sync_000
   * @tc.desc Test fchownSync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchown_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchown_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(isIntNum(stat.uid)).assertTrue();
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      fileio.fchownSync(fd, stat.uid, stat.gid);
      expect(fileio.closeSync(fd) == null).assertTrue();
      fileio.unlinkSync(fpath);
    } catch (e) {
      expect(null).assertFail();
    }
  });
});
