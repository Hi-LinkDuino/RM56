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

describe('fileio_posix_fallocate', function () {

  /**
   * @tc.number SUB_DF_FILEIO_POSIX_FALLOCATE_SYNC_0000
   * @tc.name fileio_test_posix_fallocate_sync_000
   * @tc.desc Test fsyncSync() interface.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_posix_fallocate_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_posix_fallocate_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      fileio.posixFallocateSync(fd, 100, 999);
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_posix_fallocate_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_POSIX_FALLOCATE_ASYNC_0000
   * @tc.name fileio_test_posix_fallocate_async_000
   * @tc.desc Test fsyncASync() interface.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_posix_fallocate_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_posix_fallocate_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      await fileio.posixFallocate(fd, 100, 999, function (err) {
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_posix_fallocate_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_POSIX_FALLOCATE_ASYNC_0010
   * @tc.name fileio_test_posix_fallocate_async_001
   * @tc.desc Test lseekAsync()interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_posix_fallocate_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_posix_fallocate_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      await fileio.posixFallocate(fd, 100, 999);
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_posix_fallocate_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
