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

import fileio from '@system.fileio'
import {
  describe,
  it,
  expect
}
  from 'deccjsunit/index'
import {
  FILE_CONTENT,
  prepareFile,
  fileName,
  nextFileName,
  appName,
  randomString,
}
  from './Common'

describe('FileIOError', function () {

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0000
   * @tc.name FileIo_test_error_000
   * @tc.desc Function of API, Copy files when disk space is full
   *  Create the file (fileio_test_error) in advance and give 777 permission,
   *  path:/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/cache/fileio_test_error
   */
  it('FileIo_test_error_000', 0, function () {
    let fpath = nextFileName('fileio_test_error');
    let fpathTarget = appName('fileio_test_error');
    try {
      expect(fileio.copyFileSync(fpath, fpathTarget) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_000 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0100
   * @tc.name FileIo_test_error_001
   * @tc.desc Function of API, mkdir files when disk space is full
   */
  it('FileIo_test_error_001', 0, function () {
    let dpath = nextFileName('fileio_test_error_001d');
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0200
   * @tc.name FileIo_test_error_002
   * @tc.desc Function of API, When inode is full, verify the folder creation function
   */
  it('FileIo_test_error_002', 0, function () {
    let dpath = fileName('fileio_test_error_002d');
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0300
   * @tc.name FileIo_test_error_003
   * @tc.desc Function of API, When the disk space is full, verify the synchronous write file data function
   */
  it('FileIo_test_error_003', 0, function () {
    let fpath = nextFileName('fileio_test_error_003');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o777);
      expect(fd !== null).assertTrue();
      expect(fileio.writeSync(fd, FILE_CONTENT) !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0400
   * @tc.name FileIo_test_error_004
   * @tc.desc Function of API, When the disk space is full,
   * verify synchronization and write the buffer data back to the disk for data synchronization
   */
  it('FileIo_test_error_004', 0, function () {
    let fpath = nextFileName('fileio_test_error_004');
    try {
      let fd = fileio.openSync(fpath, 0o102, 0o777);
      expect(fd !== null).assertTrue();
      expect(fileio.fsyncSync(fd) !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_004 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0500
   * @tc.name FileIo_test_error_005
   * @tc.desc Function of API, Open 200m file function when there is 100m left in RAM
   *  Create a 200m file (fileio_test_error_005) and grant 777 permissions,
   *  path:/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/cache/fileio_test_error_005
   */
  it('FileIo_test_error_005', 0, function () {
    let fpath = nextFileName('fileio_test_error_005');
    try {
      let fd = fileio.openSync(fpath);
      expect(fd !== null).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_005 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0600
   * @tc.name FileIo_test_error_006
   * @tc.desc Function of API, Read 200m file function when there is 100m left in RAM
   *  Create a 200m file (fileio_test_error_005) and grant 777 permissions,
   *  path:/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/cache/fileio_test_error_005
   */
  it('FileIo_test_error_006', 0, function () {
    let fpath = nextFileName('fileio_test_error_005');
    try {
      let fd = fileio.openSync(fpath);
      expect(fd !== null).assertTrue();
      let rlen = fileio.readSync(fd, new ArrayBuffer(209715200));
      expect(rlen == 209715200).assertTrue();
      expect(fileio.closeSync(fd) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_006 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0700
   * @tc.name FileIo_test_error_007
   * @tc.desc Function of API, Verify the function of obtaining
   * 200m file stream synchronously when 100m ram is left
   *  Create a 200m file (fileio_test_error_005) and grant 777 permissions,
   *  path:/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/cache/fileio_test_error_005
   */
  it('FileIo_test_error_007', 0, function () {
    let fpath = nextFileName('fileio_test_error_005');
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0800
   * @tc.name FileIo_test_error_008
   * @tc.desc Function of API, Read 200m file function when there is 100m left in RAM
   *  Create a 200m file (fileio_test_error_005) and grant 777 permissions,
   *  path:/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/cache/fileio_test_error_005
   */
  it('FileIo_test_error_008', 0, function () {
    let fpath = nextFileName('fileio_test_error_005');
    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let rlen = ss.readSync(new ArrayBuffer(209715200));
      expect(rlen == 209715200).assertTrue();
      expect(ss.closeSync() !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_008 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0900
   * @tc.name FileIo_test_error_009
   * @tc.desc Function of API, Delete directories with files
   */
  it('FileIo_test_error_009', 0, function () {
    let dpath = fileName('fileio_test_error_009d');
    let fpath = dpath + '/fileio_test_error_009f';
    fileio.mkdirSync(dpath);
    expect(prepareFile(fpath, 'hello')).assertTrue();
    try {
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_009 has failed for ' + e);
      fileio.unlinkSync(fpath);
      fileio.rmdirSync(dpath);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error1000
   * @tc.name FileIo_test_error_010
   * @tc.desc Function of API, delete file
   */
  it('FileIo_test_error_010', 0, function () {
    let fpath = fileName('fileio_test_error_010f');
    expect(prepareFile(fpath, 'hello')).assertTrue();
    try {
      expect(fileio.rmdirSync(fpath) !== null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_error_010 has failed for ' + e);
      fileio.unlinkSync(fpath);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_1100
   * @tc.name FileIo_test_error_011
   * @tc.desc Function of API, Pass in a
   * path that exceeds 4096 bytes to copy the file
   */
  it('FileIo_test_error_011', 0, function () {
    let fpath = nextFileName('fileio_test_error_011');
    fileio.openSync(fpath, 0o102, 0o777);
    let dpath = nextFileName('fileio_error_011d');
    fileio.mkdirSync(dpath);
    try {
      for (let i = 0; i < 16; i++) {
        if (i == 15) {
          let fpathTarget = dpath + '/f' + randomString(248);
          expect(fileio.copyFileSync(fpath, fpathTarget) !== null).assertTrue();
          console.log((i + 1) + ' copy success');
          expect(null).assertFail();
        } else {
          dpath = dpath + '/d' + randomString(248);
          fileio.mkdirSync(dpath);
          console.log((i + 1) + ' mkdir success');
        }
      }
    } 
    catch (e) {
      console.log('fileio_test_error_011 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIo_test_error_0120
   * @tc.name FileIo_test_error_012
   * @tc.desc Function of API, flags=0o102. The test file is exist.
   */
  it('FileIo_test_error_012', 0, function () {
    let fpath = nextFileName('FileIo_test_error_012');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.openSync(fpath, 0o102);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_012 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0130
   * @tc.name FileIo_test_error_013
   * @tc.desc Function of API, flags=0o102
   */
  it('FileIo_test_error_013', 0, function () {
    let fpath = nextFileName('FileIo_test_error_013');
    try {
      fileio.openSync(fpath, 0o102);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_013 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0140
   * @tc.name FileIo_test_error_014
   * @tc.desc Function of API,  flags=0o202. The test file is exist.
   */
  it('FileIo_test_error_014', 0, function () {
    let fpath = nextFileName('FileIo_test_error_014');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.openSync(fpath, 0o202);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_014 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0150
   * @tc.name FileIo_test_error_015
   * @tc.desc Function of API,  flags=0o302. The test file is exist.
   */
  it('FileIo_test_error_015', 0, function () {
    let fpath = nextFileName('FileIo_test_error_015');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.openSync(fpath, 0o302);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_015 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0160
   * @tc.name FileIo_test_error_016
   * @tc.desc Function of API,  flags=0o100002. The test file is exist.
   */
  it('FileIo_test_error_016', 0, function () {
    let fpath = nextFileName('FileIo_test_error_016');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      fileio.openSync(fpath, 0o100002);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_016 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0170
   * @tc.name FileIo_test_error_017
   * @tc.desc Function of API, flags=0o40002 The test file is exist.
   */
  it('FileIo_test_error_017', 0, function () {
    let dpath = nextFileName('FileIo_test_error_017');
    expect(fileio.mkdirSync(dpath) !== null).assertTrue();
    try {
      fileio.openSync(dpath, 0o40002);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_017 has failed for ' + e);
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0180
   * @tc.name FileIo_test_error_018
   * @tc.desc Function of API, flags=0o20040002. The test file is exist.
   */
  it('FileIo_test_error_018', 0, function () {
    let fpath = nextFileName('FileIo_test_error_018');
    try {
      fileio.openSync(fpath, 0o20040002);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_018 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0190
   * @tc.name FileIo_test_error_019
   * @tc.desc Function of API, flags=0o400002. The test file is exist.
   */
  it('FileIo_test_error_019', 0, function () {
    let fpath = nextFileName('FileIo_test_error_019');
    let txt = 'h'
    expect(prepareFile(fpath, txt)).assertTrue();
    try {
      fileio.openSync(fpath, 0o400002);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_019 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0200
   * @tc.name FileIo_test_error_020
   * @tc.desc Function of API, flags=0o400002. The test file is exist.
   */
  it('FileIo_test_error_020', 0, function () {
    let fpath = nextFileName('FileIo_test_error_020');
    let txt = randomString(5000);
    expect(prepareFile(fpath, txt)).assertTrue();
    try {
      fileio.openSync(fpath, 0o400002);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_020 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0210
   * @tc.name FileIo_test_error_021
   * @tc.desc Function of API, flags=0o10000102. The test file is exist.
   */
  it('FileIo_test_error_021', 0, function () {
    let fpath = nextFileName('FileIo_test_error_021');
    try {
      fileio.openSync(fpath, 0o10000102);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_021 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_OpenSync_0220
   * @tc.name FileIo_test_error_022
   * @tc.desc Function of API,mode=1 The test file is exist.
   */
  it('FileIo_test_error_022', 0, function () {
    let fpath = nextFileName('FileIo_test_error_022');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let fpathTarget = fpath + 'f1';
    try {
      fileio.copyFileSync(fpath, fpathTarget, 1);
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('FileIo_test_error_021 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

});
