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
  from 'deccjsunit/index';
import {
  FILE_CONTENT,
  prepareFile,
  nextFileName
}
  from './Common';

describe('fileIOTestDirent', function () {

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_name_0000
   * @tc.name fileio_test_dirent_name_000
   * @tc.desc Function of API, Get file name.
   */
  it('fileio_test_dirent_name_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_name_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent.name !== null).assertTrue();
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_dirent_name_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isBlockDevice_0000
   * @tc.name fileio_test_dirent_is_block_device_000
   * @tc.desc Function of API, isBlockDevice. Determine whether it is a block device.
   */
  it('fileio_test_dirent_is_block_device_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_block_device_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(dirent.isBlockDevice() !== null);
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      return
    } 
    catch (e) {
      console.log('fileio_test_dirent_is_block_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isBlockDevice_0100
   * @tc.name fileio_test_dirent_is_block_device_001
   * @tc.desc Function of API, isBlockDevice. The parameter is - 1.
   */
  it('fileio_test_dirent_is_block_device_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_block_device_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isBlockDevice(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      console.log('fileio_test_dirent_is_block_device_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isCharacterDevice_0000
   * @tc.name fileio_test_dirent_is_character_device_000
   * @tc.desc Function of API, isCharacterDevice. Determine whether it is a character device.
   */
  it('fileio_test_dirent_is_character_device_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_character_device_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(dirent.isCharacterDevice() !== null);
      expect(dd.closeSync() !== null !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_dirent_is_character_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isCharacterDevice_0100
   * @tc.name fileio_test_dirent_is_character_device_001
   * @tc.desc Function of API, isCharacterDevice. The parameter is - 1.
   */
  it('fileio_test_dirent_is_character_device_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_character_device_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isCharacterDevice(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      console.log('fileio_test_dirent_is_character_device_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isDirectory_0000
   * @tc.name fileio_test_dirent_is_directory_000
   * @tc.desc Function of API, isDirectory. Determine whether it is a directory.
   */
  it('fileio_test_dirent_is_directory_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_directory_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(dirent.isDirectory() !== null);
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_dirent_is_directory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isDirectory_0100
   * @tc.name fileio_test_dirent_is_directory_001
   * @tc.desc Function of API, isDirectory. The parameter is - 1
   */
  it('fileio_test_dirent_is_directory_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_directory_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isDirectory(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      console.log('fileio_test_dirent_is_directory_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isFifo_0000
   * @tc.name fileio_test_dirent_is_fifo_000
   * @tc.desc Function of API, isFIFO. Determine whether it is a fifo.
   */
  it('fileio_test_dirent_is_fifo_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_fifo_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(dirent.isFIFO() !== null);
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_dirent_is_fifo_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isFifo_0100
   * @tc.name fileio_test_dirent_is_fifo_001
   * @tc.desc Function of API, isFIFO. The parameter is - 1
   */
  it('fileio_test_dirent_is_fifo_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_fifo_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isFIFO(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      console.log('fileio_test_dirent_is_fifo_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isFile_0000
   * @tc.name fileio_test_dirent_is_file_000
   * @tc.desc Function of API, isFile. Determine whether it is a file.
   */
  it('fileio_test_dirent_is_file_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_file_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(dirent.isFile() !== null);
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_dirent_is_file_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isFile_0100
   * @tc.name fileio_test_dirent_is_file_001
   * @tc.desc Function of API, isFile. The parameter is - 1
   */
  it('fileio_test_dirent_is_file_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_file_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isFile(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      console.log('fileio_test_dirent_is_file_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isSocket_0000
   * @tc.name fileio_test_dirent_is_socket_000
   * @tc.desc Function of API, isSocket. Determine whether it is a socket.
   */
  it('fileio_test_dirent_is_socket_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_socket_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(dirent.isSocket() !== null);
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_dirent_is_socket_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isSocket_0100
   * @tc.name fileio_test_dirent_is_socket_001
   * @tc.desc Function of API, isSocket. The parameter is - 1
   */
  it('fileio_test_dirent_is_socket_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_socket_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isSocket(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      console.log('fileio_test_dirent_is_socket_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isSymbolicLlink_0000
   * @tc.name fileio_test_dirent_is_symbolic_link_000
   * @tc.desc Function of API, isSymbolicLink. Determine whether it is a symbolic link.
   */
  it('fileio_test_dirent_is_symbolic_link_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_symbolic_link_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(dirent.isSymbolicLink() !== null);
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
    } 
    catch (e) {
      console.log('fileio_test_dirent_is_symbolic_link_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_dirent_isSymbolicLlink_0100
   * @tc.name fileio_test_dirent_is_symbolic_link_001
   * @tc.desc Function of API, isSymbolicLink. The parameter is - 1
   */
  it('fileio_test_dirent_is_symbolic_link_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_symbolic_link_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;
    try {
      expect(fileio.mkdirSync(dpath) !== null);
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isSymbolicLink(-1);
      expect(null).assertFail();
    } 
    catch (e) {
      expect(dd.closeSync() !== null);
      expect(fileio.unlinkSync(fpath) !== null);
      expect(fileio.rmdirSync(dpath) !== null);
      console.log('fileio_test_dirent_is_symbolic_link_001 has failed for ' + e);
    }
  });

});