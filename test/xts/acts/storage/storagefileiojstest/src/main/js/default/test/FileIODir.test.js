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

import fileio from '@ohos.fileio';
import {
  describe,
  it,
  expect
}
  from 'deccjsunit/index'
import {
  FILE_CONTENT,
  prepareFile,
  nextFileName,
  randomString
}
  from './Common'

describe('fileIOTestDir', function () {

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_OpenSync_0000
   * @tc.name fileio_test_dir_open_sync_000
   * @tc.desc Function of API, opendirSync.
   */
  it('fileio_test_dir_open_sync_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_open_sync_000') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_dir_open_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_OpenSync_0100
   * @tc.name fileio_test_dir_open_sync_001
   * @tc.desc Function of API, not set parameter.
   */
  it('fileio_test_dir_open_sync_001', 0, function () {
    try {
      fileio.opendirSync();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_dir_open_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_OpenSync_0200
   * @tc.name fileio_test_dir_open_sync_002
   * @tc.desc Function of API, dpath value is not exist.
   */
  it('fileio_test_dir_open_sync_002', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_open_sync_003') + 'd'
    try {
      fileio.opendirSync(dpath);
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_dir_open_sync_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_OpenSync_0300
   * @tc.name fileio_test_dir_open_sync_003
   * @tc.desc Function of API, dpath too long.
   */
  it('fileio_test_dir_open_sync_003', 0, async function () {
    let dpath = await nextFileName('fileio_dir11');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 16; i++) {
        console.log('time' + i);
        dpath = dpath + '/f' + randomString(248);
        fileio.mkdirSync(dpath);
      }
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_dir_open_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_OpenSync_0400
   * @tc.name fileio_test_dir_open_sync_004
   * @tc.desc Function of API, filename too long.
   */
  it('fileio_test_dir_open_sync_004', 0, async function () {
    let dpath = await nextFileName(randomString(256));
    try {
      fileio.mkdirSync(dpath);
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_dir_open_sync_004 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_OpenSync_0500
   * @tc.name fileio_test_dir_open_sync_005
   * @tc.desc Function of API, uri dir too many layers.
   */
  it('fileio_test_dir_open_sync_005', 0, async function () {
    let dpath = await nextFileName('dir');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 1024; i++) {
        console.log('time' + i);
        dpath = dpath + '/' + i
        fileio.mkdirSync(dpath);
      }
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_dir_open_sync_005 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_OpenSync_0600
   * @tc.name fileio_test_dir_open_sync_006
   * @tc.desc Function of API, file name contain special character.
   */
  it('fileio_test_dir_open_sync_006', 0, async function () {
    let dpath = await nextFileName('?*:<>/|');
    try {
      fileio.mkdirSync(dpath);
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_dir_open_sync_006 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_ReadSync_0000
   * @tc.name fileio_test_dir_read_sync_000
   * @tc.desc Function of API, readSync. The dir contains 1 file.
   */
  it('fileio_test_dir_read_sync_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_read_sync_000') + 'd'
    let fpath = dpath + '/f0'
    try {
      expect(typeof(fileio.mkdirSync(dpath)) == 'undefined').assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.readSync() !== null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_dir_read_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_ReadSync_0100
   * @tc.name fileio_test_dir_read_sync_001
   * @tc.desc Function of API, readSync. The dir contains more than 1 files.
   */
  it('fileio_test_dir_read_sync_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_read_sync_001') + 'd'
    let fpathArray = new Array(dpath + '/f1', dpath + '/f2', dpath + '/d3');
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      for (let i = 0; i < 3; i++) {
        if (i == 2) {
          expect(fileio.mkdirSync(fpathArray[i]) !== null).assertTrue();
        } else {
          expect(prepareFile(fpathArray[i], FILE_CONTENT)).assertTrue();
        }
      }
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      for (let i = 0; i < 3; i++) {
        if (i == 2) {
          let bool = typeof(dd.readSync());
          expect(bool == 'object').assertTrue();
          console.log('---bool=' + bool);
        } else {
          expect(dd.readSync() !== null).assertTrue();
        }
      }
      expect(dd.closeSync() == null).assertTrue();
      for (let i = 0; i < 3; i++) {
        if (i == 2) {
          expect(fileio.rmdirSync(fpathArray[i]) !== null).assertTrue();
        } else {
          expect(fileio.unlinkSync(fpathArray[i]) !== null).assertTrue();
        }
      }
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_dir_read_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_ReadSync_0200
   * @tc.name fileio_test_dir_read_sync_002
   * @tc.desc Function of API, repeat read. The dir contains more than 1 files.
   */
  it('fileio_test_dir_read_sync_002', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_read_sync_002') + 'd'
    let fpathArray = new Array(dpath + '/f1', dpath + '/f2', dpath + '/d3');
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      for (let i = 0; i < 3; i++) {
        if (i == 2) {
          expect(fileio.mkdirSync(fpathArray[i]) !== null).assertTrue();
        } else {
          expect(prepareFile(fpathArray[i], FILE_CONTENT)).assertTrue();
        }
      }
      var dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      for (let i = 0; i < 3; i++) {
        expect(dd.readSync() !== null).assertTrue();
      }
      expect(dd.readSync() == null).assertTrue();
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_dir_read_sync_002 has failed for ' + e);
      expect(dd.closeSync() == null).assertTrue();
      for (let i = 0; i < 3; i++) {
        if (i == 2) {
          expect(fileio.rmdirSync(fpathArray[i]) !== null).assertTrue();
        } else {
          expect(fileio.unlinkSync(fpathArray[i]) !== null).assertTrue();
        }
      }
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
      }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_ReadSync_0300
   * @tc.name fileio_test_dir_read_sync_003
   * @tc.desc Function of API, readSync. The dir no any files.
   */
  it('fileio_test_dir_read_sync_003', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_read_sync_003') + 'd'
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.readSync() == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
      console.log('fileio_test_dir_read_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_ReadSync_0400
   * @tc.name fileio_test_dir_read_sync_004
   * @tc.desc Function of API, error parameter.
   */
  it('fileio_test_dir_read_sync_004', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_read_sync_004') + 'd'
    let fpath = dpath + '/f1'
    let dd
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.readSync(-1) == null).assertTrue();
      expect(null).assertFail();
    }
    catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
      console.log('fileio_test_dir_read_sync_004 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_ReadSync_0500
   * @tc.name fileio_test_dir_read_sync_005
   * @tc.desc Function of API, excess files.
   */
  it('fileio_test_dir_read_sync_005', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_read_sync_005') + 'd'
    let fpath = dpath + '/f'
    try {
      expect(typeof(fileio.mkdirSync(dpath)) == 'undefined').assertTrue();
      for (let i = 0; i < 10001; i++) {
      expect(prepareFile(fpath + i, 'test_text')).assertTrue();
      }
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.readSync() !== null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      for (let i = 0; i < 10001; i++) {
        fileio.unlinkSync(fpath + i);
      }
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_dir_read_sync_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dir_CloseSync_0000
   * @tc.name fileio_test_dir_close_sync_000
   * @tc.desc Function of API, closeSync.
   */
  it('fileio_test_dir_close_sync_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dir_close_sync_000') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_dir_close_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
