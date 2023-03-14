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

describe('fileio_symlink', function () {

  /**
   * @tc.number SUB_DF_FILEIO_SYMLINK_SYNC_0000
   * @tc.name fileio_test_symlink_sync_000
   * @tc.desc Test SymlinkSync interfaces.
   * This interface shall not treat a normal file as a directory.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_symlink_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_symlink_sync_000');

    try {
      fileio.symlinkSync(fpath, fpath + 'aaaa');
      expect(fileio.accessSync(fpath + 'aaaa') == null).assertTrue();
      fileio.unlinkSync(fpath + 'aaaa');
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_symlink_sync_000 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_SYMLINK_ASYNC_0000
   * @tc.name fileio_test_symlink_async_000
   * @tc.desc Test SymlinkAsync interfaces.
   * This interface shall not treat a normal file as a directory.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_symlink_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_symlink_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio
        .symlink(fpath, fpath + '1067')
        .then(function (err) {
          expect(fileio.accessSync(fpath + '1067') == null).assertTrue();
          fileio.unlinkSync(fpath + '1067');
          expect(err == null).assertTrue();
        })
        .catch(function (e) {
          expect(e == null).assertTrue();
        });
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });
  
  /**
   * @tc.number SUB_DF_FILEIO_SYMLINK_ASYNC_0010
   * @tc.name fileio_test_symlink_async_001
   * @tc.desc Test SymlinkAsync interfaces.
   * This interface shall not treat a normal file as a directory.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_symlink_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_symlink_async_010');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.symlink(fpath, fpath + 'pass2', function (err) {
        expect(fileio.accessSync(fpath + 'pass2') == null).assertTrue();
        fileio.unlinkSync(fpath + 'pass2');
        done();
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });
});
