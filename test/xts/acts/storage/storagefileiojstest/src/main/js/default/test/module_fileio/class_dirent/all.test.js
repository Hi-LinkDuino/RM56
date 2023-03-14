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
  fileio, FILE_CONTENT, prepareFile, nextFileName, isBoolean, isString,
  describe, it, expect,
} from '../../Common';

describe('fileio_dirent', function () {

  /**
   * @tc.number SUB_DF_FILEIO_DIRENT_NAME_0010
   * @tc.name fileio_test_dirent_name_001
   * @tc.desc Test dirent interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_name_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dirent_name_001') + 'd';
    let fpath = dpath + '/f2';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      dd.read(function (err, dirent) {
        expect(dirent !== null).assertTrue();
        expect(isString(dirent.name)).assertTrue();
        expect(dd.closeSync() == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        expect(fileio.rmdirSync(dpath) == null).assertTrue();
        done();
      })
    } catch (e) {
      console.log('fileio_test_dirent_name_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_DIRENT_NAME_0000
   * @tc.name fileio_test_dirent_name_000
   * @tc.desc Test dirent interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_name_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dirent_name_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = await dd.read();
      expect(dirent !== null).assertTrue();
      expect(isString(dirent.name)).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();

    } catch (e) {
      console.log('fileio_test_dirent_name_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_0000
   * @tc.name fileio_test_dirent_name_000
   * @tc.desc Test readSync() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_name_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_name_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isString(dirent.name)).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_name_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISBLOCKDEVICE_0000
   * @tc.name fileio_test_dirent_is_block_device_000
   * @tc.desc Test isBlockDevice() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_block_device_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_block_device_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isBoolean(dirent.isBlockDevice())).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_is_block_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISBLOCKDEVICE_0010
   * @tc.name fileio_test_dirent_is_block_device_001
   * @tc.desc Test isBlockDevice() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_block_device_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_block_device_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isBlockDevice(-1);
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISCHARACTERDEVICE_0000
   * @tc.name fileio_test_dirent_is_character_device_000
   * @tc.desc Test isCharacterDevice() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_character_device_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_character_device_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isBoolean(dirent.isCharacterDevice())).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_is_character_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISCHARACTERDEVICE_0010
   * @tc.name fileio_test_dirent_is_character_device_001
   * @tc.desc Test isCharacterDevice() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_character_device_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_character_device_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isCharacterDevice(-1);
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISDIRECTORY_0000
   * @tc.name fileio_test_dirent_is_directory_000
   * @tc.desc Test isDirectory() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_directory_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_directory_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isBoolean(dirent.isDirectory())).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_is_directory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FileIO_Dir_IsDirectory_0010
   * @tc.name fileio_test_dirent_is_directory_001
   * @tc.desc Test isDirectory() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_directory_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_directory_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isDirectory(-1);
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISFIFO_0000
   * @tc.name fileio_test_dirent_is_fifo_000
   * @tc.desc Test isFIFO() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_fifo_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_fifo_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isBoolean(dirent.isFIFO())).assertTrue()
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_is_fifo_000 has failed for ' + e);
      expect(null).assertFail()
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISFIFO_0010
   * @tc.name fileio_test_dirent_is_fifo_001
   * @tc.desc Test isFIFO() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_fifo_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_fifo_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isFIFO(-1);
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISFILE_0000
   * @tc.name fileio_test_dirent_is_file_000
   * @tc.desc Test isFILE() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_file_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_file_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isBoolean(dirent.isFile())).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_is_file_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISFILE_0010
   * @tc.name fileio_test_dirent_is_file_001
   * @tc.desc Test isFILE() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require 
   */
  it('fileio_test_dirent_is_file_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_file_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isFile(-1);
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISSOCKET_0000
   * @tc.name fileio_test_dirent_is_socket_000
   * @tc.desc Test isSocket() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_socket_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_socket_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isBoolean(dirent.isSocket())).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_is_socket_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISSOCKET_0010
   * @tc.name fileio_test_dirent_is_socket_001
   * @tc.desc Test isSocket() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_socket_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_socket_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue()
      dirent.isSocket(-1);
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  })

  /**
   * @tc.number SUB_DF_FileIO_Dir_IsSymbolicLink_0000
   * @tc.name fileio_test_dirent_is_symbolic_link_000
   * @tc.desc Test isSymbolicLink() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dirent_is_symbolic_link_000', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_symbolic_link_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue()
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      expect(isBoolean(dirent.isSymbolicLink())).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dirent_is_symbolic_link_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_DIR_ISSYMBOLICLINK_0010
   * @tc.name fileio_test_dirent_is_symbolic_link_001
   * @tc.desc Test isSymbolicLink() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require 
   */
  it('fileio_test_dirent_is_symbolic_link_001', 0, async function () {
    let dpath = await nextFileName('fileio_test_dirent_is_symbolic_link_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      let dirent = dd.readSync();
      expect(dirent !== null).assertTrue();
      dirent.isSymbolicLink(-1);
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue()
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  });
});