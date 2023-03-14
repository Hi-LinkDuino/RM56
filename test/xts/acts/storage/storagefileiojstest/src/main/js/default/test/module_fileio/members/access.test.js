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

describe('fileio_access', function () {

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSSYNC_0000
   * @tc.name fileio_test_access_sync_000
   * @tc.desc Test accessSync() interface.
   * This interface shall work properly in normal case
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(fileio.accessSync(fpath) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_access_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSSYNC_0010
   * @tc.name fileio_test_access_sync_001
   * @tc.desc Test accessSync() interface.
   * This interface shall throw an exception when the fpath is not given
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(fileio.accessSync(fpath, 0x2) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_access_sync_001' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSSYNC_0020
   * @tc.name fileio_test_access_sync_002
   * @tc.desc Test accessSync() interface.
   * This interface shall throw an exception when the fpath is not given
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(fileio.accessSync(fpath, 0x4) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_access_sync_002' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSSYNC_0030
   * @tc.name fileio_test_access_sync_003
   * @tc.desc Test accessSync() interface.
   * This interface shall throw an exception when the fpath is not given.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_access_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(fileio.accessSync(fpath, 0x2 | 0x4) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_access_sync_003' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSSYNC_0040
   * @tc.name fileio_test_access_sync_004
   * @tc.desc Test accessSync() interface.
   * This interface shall throw an exception when the fpath is not given
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_sync_004', 0, async function () {
    let fpath = await nextFileName('fileIOTest');

    try {
      fileio.accessSync(fpath);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_access_sync_004' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSASYNC_0000
   * @tc.name fileio_test_access_async_000
   * @tc.desc Test accessAsync() interface.
   * This interface shall throw an exception when the fpath is not given
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_access_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.access(fpath);
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (error) {
      console.log('fileio_test_access_async_000 has failed for ' + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSASYNC_0010
   * @tc.name fileio_test_access_async_001
   * @tc.desc Test accessAsync() interface.
   * This interface shall throw an exception when the fpath is not given
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_access_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      expect(await fileio.access(fpath, 4) == null).assertTrue();
      let fd = fileio.openSync(fpath);
      expect(isIntNum(fd)).assertTrue();
      expect(
        (await fileio.read(fd, new ArrayBuffer(4096))) != null
      ).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (error) {
      console.log('fileio_test_access_async_001 has failed for ' + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSASYNC_0020
   * @tc.name fileio_test_access_async_002
   * @tc.desc Test accessAsync() interface.
   * This interface shall throw an exception when the fpath is not given
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_access_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.access(fpath, function (error) {
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (error) {
      console.log('fileio_test_access_async_002 has failed for ' + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSASYNC_0030
   * @tc.name fileio_test_access_async_003
   * @tc.desc Test accessAsync() interface.
   * This interface shall throw an exception when the fpath is not given
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_access_async_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.access(fpath, 4, async function (error) {
        let fd = fileio.openSync(fpath);
        expect(isIntNum(fd)).assertTrue();
        expect(
          (await fileio.read(fd, new ArrayBuffer(4096))) != null
        ).assertTrue();
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
      });
      done();
    } catch (error) {
      console.log('fileio_test_access_async_003 has failed for ' + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_ACCESSASYNC_0040
   * @tc.name fileio_test_access_async_004
   * @tc.desc Test accessAsync() interface.
   * This interface shall throw an exception when the fpath is not given.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_access_async_004', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_access_async_004');

    try {
      fileio
        .access(fpath)
        .then(function (err) {
          expect(null).assertFail();
          done();
        })
        .catch(function (e) {
          expect(!!e).assertTrue();
          done();
        });
    } catch (e) {
      console.log('fileio_test_access_async_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
