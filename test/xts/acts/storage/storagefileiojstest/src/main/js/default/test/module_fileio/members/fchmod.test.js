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

describe('fileio_fchmod', function () {

  /**
   * @tc.number SUB_DF_FILEIO_FCHMOD_ASYNC_0000
   * @tc.name fileio_test_fchmod_async_000
   * @tc.desc Test fchmodAsync()interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchmod_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fchmod_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio
        .open(fpath, 0o1, 0o0200)
        .then(function (fd) {
          fileio.fchmod(fd, 1002, function (err) {
            fileio.statSync(fpath);
            fileio.closeSync(fd);
            fileio.unlinkSync(fpath);
            done();
          });
        })
        .catch(function (err) {
          expect(err == null).assertTrue();
        });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHMOD_ASYNC_0010
   * @tc.name fileio_test_fchmod_async_001
   * @tc.desc Test fchmodAsync()interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchmod_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fchmod_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      await fileio
        .open(fpath, 0o0, 0o0200)
        .then(function (fd) {
          fileio
            .fchmod(fd, 1003)
            .then(function (err) {
              fileio.statSync(fpath);
              fileio.closeSync(fd);
              fileio.unlinkSync(fpath);
            })
            .catch(function (err) {
              expect(err == null).assertTrue();
            });
        })
        .catch(function (err) {
          expect(err == null).assertTrue();
        });
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHMOD_SYNC_0000
   * @tc.name fileio_test_fchmod_sync_000
   * @tc.desc Test fchmodSync()interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchmod_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_fchmod_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(fileio.fchmodSync(fd, 0o660) == null).assertTrue();
      expect((fileio.statSync(fpath).mode & 0o777) == 0o660).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_fchmod_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHMOD_SYNC_0010
   * @tc.name fileio_test_fchmod_sync_001
   * @tc.desc Test fchmodSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchmod_sync_001', 0, function () {
    try {
      expect(fileio.fchmodSync(-1, 0o660) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_fchmod_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHMOD_ASYNC_0020
   * @tc.name fileio_test_fchmod_async_002
   * @tc.desc Test fchmod() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchmod_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fchmod_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      await fileio.open(fpath, 0o1, 0o020).then(function (fd) {
        fileio.fchmod(fd, 0o100, function (err) {
          expect(fileio.closeSync(fd) == null).assertTrue();
          expect(fileio.unlinkSync(fpath) == null).assertTrue();
          done();
        });
      });
    } catch (e) {
      console.log('fileio_test_fchmod_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FCHMOD_ASYNC_0030
   * @tc.name fileio_test_fchmod_async_003
   * @tc.desc Test fchmod() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fchmod_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fchmod_async_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = await fileio.open(fpath, 0o1, 0o020);

      expect((await fileio.fchmod(fd, 0o400)) == null).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_fchmod_async_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

});
