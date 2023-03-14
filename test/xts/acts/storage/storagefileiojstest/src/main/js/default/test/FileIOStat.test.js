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

describe('fileIOTestStat', function () {

  /**
   * @tc.number SUB_STORAGE_FileIO_statSync_0000
   * @tc.name fileio_test_stat_stat_sync_000
   * @tc.desc Function of API, statSync. Gets information about the file in the specified path.
   */
  it('fileio_test_stat_stat_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_stat_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(stat !== null).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_stat_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_statSync_0100
   * @tc.name fileio_test_stat_stat_sync_001
   * @tc.desc Function of API, statSync. The specified file does not exist.
   */
  it('fileio_test_stat_stat_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_stat_sync_001');
    try {
      fileio.statSync(fpath);
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stat_stat_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_statSync_0200
   * @tc.name fileio_test_stat_stat_sync_002
   * @tc.desc Function of API, statSync. The parameter is the directory path.
   */
  it('fileio_test_stat_stat_sync_002', 0, async function () {
    let dpath = await nextFileName('fileio_test_stat_stat_sync_002') + 'd';
    expect(fileio.mkdirSync(dpath) !== null).assertTrue();
    try {
      let stat = fileio.statSync(dpath);
      expect(stat !== null).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_stat_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_statSync_0300
   * @tc.name fileio_test_stat_stat_sync_003
   * @tc.desc Function of API, statSync. The file path is greater than 4096.
   */
  it('fileio_test_stat_stat_sync_003', 0, async function () {
    let dpath = await nextFileName('fileio_stat1');
    try {
      for (let i = 0; i < 16; i++) {
        if (i == 15) {
          let fpath = dpath + '/f' + randomString(252);
          fileio.statSync(fpath);
        } else {
          dpath = dpath + '/d' + randomString(252);
        }
      }
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stat_stat_sync_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_dev_0000
   * @tc.name fileio_test_stat_dev_000
   * @tc.desc Function of API. Get the device number used by the file.
   */
  it('fileio_test_stat_dev_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_dev_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.dev) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_dev_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_ino_0000
   * @tc.name fileio_test_stat_ino_000
   * @tc.desc Function of API. Get inode number.
   */
  it('fileio_test_stat_ino_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_ino_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.ino) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_ino_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_mode_0000
   * @tc.name fileio_test_stat_mode_000
   * @tc.desc Function of API. Get file type and access rights.
   */
  it('fileio_test_stat_mode_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_mode_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.mode) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_mode_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_nlink_0000
   * @tc.name fileio_test_stat_nlink_000
   * @tc.desc Function of API. Gets the number of hard connections to the file.
   */
  it('fileio_test_stat_nlink_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_nlink_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.nlink) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_nlink_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_uid_0000
   * @tc.name fileio_test_stat_uid_000
   * @tc.desc Function of API. Get file owner user ID.
   */
  it('fileio_test_stat_uid_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_uid_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.uid) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_uid_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_gid_0000
   * @tc.name fileio_test_stat_gid_000
   * @tc.desc Function of API. Get group ID.
   */
  it('fileio_test_stat_gid_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_gid_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.gid) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_gid_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_rdev_0000
   * @tc.name fileio_test_stat_rdev_000
   * @tc.desc Function of API. Get the device number of the device file.
   */
  it('fileio_test_stat_rdev_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_rdev_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.rdev) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_rdev_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stat_size_0000
   * @tc.name fileio_test_stat_size_000
   * @tc.desc Function of API. Gets the file capacity in bytes.
   */
  it('fileio_test_stat_size_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_size_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.size) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_size_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_blksize_0000
   * @tc.name fileio_test_stat_blksize_000
   * @tc.desc Function of API. Gets the size of the disk block containing the file.
   */
  it('fileio_test_stat_blksize_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_blksize_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.blksize) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_blksize_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_blocks_0000
   * @tc.name fileio_test_stat_blocks_000
   * @tc.desc Function of API. Gets the disk block occupied by the file.
   */
  it('fileio_test_stat_blocks_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_blocks_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.blocks) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_blocks_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stat_atime_0000
   * @tc.name fileio_test_stat_atime_000
   * @tc.desc Function of API. Gets the time when a file was last accessed.
   */
  it('fileio_test_stat_atime_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_atime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.atime) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_atime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_mtime_0000
   * @tc.name fileio_test_stat_mtime_000
   * @tc.desc Function of API. Gets the time when a file was last modified
   */
  it('fileio_test_stat_mtime_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_mtime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.mtime) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_mtime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_ctime_0000
   * @tc.name fileio_test_stat_ctime_000
   * @tc.desc Function of API. Gets the time when a file last changed state
   */
  it('fileio_test_stat_ctime_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_ctime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.ctime) == 'number').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_ctime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isBlockDevice_0000
   * @tc.name fileio_test_stat_is_block_device_000
   * @tc.desc Function of API, isBlockDevice. Determine whether it is a block device. The return value is Boolean.
   */
  it('fileio_test_stat_is_block_device_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_block_device_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.isBlockDevice()) == 'boolean').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_block_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isBlockDevice_0100
   * @tc.name fileio_test_stat_is_block_device_001
   * @tc.desc Function of API, isBlockDevice. Determine whether it is a block device. The return value is false.
   */
  it('fileio_test_stat_is_block_device_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_block_device_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isBlockDevice()).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_block_device_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isBlockDevice_0200
   * @tc.name fileio_test_stat_is_block_device_002
   * @tc.desc Function of API, isBlockDevice. The parameter is - 1
   */
  it('fileio_test_stat_is_block_device_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_block_device_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isBlockDevice(-1)).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('fileio_test_stat_is_block_device_002 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stat_isCharacterDevice_0000
   * @tc.name fileio_test_stat_is_character_device_000
   * @tc.desc Function of API, isCharacterDevice. Determine whether it is a character device. The return value is Boolean.
   */
  it('fileio_test_stat_is_character_device_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_character_device_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.isCharacterDevice()) == 'boolean').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_character_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isCharacterDevice_0100
   * @tc.name fileio_test_stat_is_character_device_001
   * @tc.desc Function of API, isCharacterDevice. Determine whether it is a character device. The return value is false.
   */
  it('fileio_test_stat_is_character_device_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_character_device_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isCharacterDevice()).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_character_device_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isCharacterDevice_0200
   * @tc.name fileio_test_stat_is_character_device_002
   * @tc.desc Function of API, isCharacterDevice. The parameter is - 1
   */
  it('fileio_test_stat_is_character_device_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_character_device_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isCharacterDevice(-1)).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_character_device_002 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isDirectory_0000
   * @tc.name fileio_test_stat_is_directory_000
   * @tc.desc Function of API, isDirectory. Determine whether it is a directory. The return value is Boolean.
   */
  it('fileio_test_stat_is_directory_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_directory_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.isDirectory()) == 'boolean').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_directory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isDirectory_0100
   * @tc.name fileio_test_stat_is_directory_001
   * @tc.desc Function of API, isDirectory. Determine whether it is a directory. The return value is false.
   */
  it('fileio_test_stat_is_directory_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_directory_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isDirectory()).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_directory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isDirectory_0200
   * @tc.name fileio_test_stat_is_directory_002
   * @tc.desc Function of API, isDirectory. Determine whether it is a directory. The return value is true.
   */
  it('fileio_test_stat_is_directory_002', 0, async function () {
    let dpath = await nextFileName('fileio_test_stat_is_directory_002') + 'd'
      try {
        expect(fileio.mkdirSync(dpath) !== null).assertTrue();
        let stat = fileio.statSync(dpath);
        expect(stat.isDirectory()).assertTrue();
        expect(fileio.rmdirSync(dpath) !== null).assertTrue();
      } 
      catch (e) {
        console.log('fileio_test_stat_is_directory_002 has failed for ' + e);
        expect(null).assertFail();
      }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isDirectory_0300
   * @tc.name fileio_test_stat_is_directory_003
   * @tc.desc Function of API, isDirectory. The parameter is - 1
   */
  it('fileio_test_stat_is_directory_003', 0, async function () {
    let dpath = await nextFileName('fileio_test_stat_is_directory_003') + 'd'
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let stat = fileio.statSync(dpath);
      expect(stat.isDirectory(-1) == null).assertTrue();
      expect(null).assertFail();
    }
    catch (e) {
      console.log('fileio_test_stat_is_directory_003 has failed for ' + e);
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_stat_isFifo_0000
   * @tc.name fileio_test_stat_is_fifo_000
   * @tc.desc Function of API, isFIFO. Determine whether it is a fifo. The return value is Boolean.
   */
  it('fileio_test_stat_is_fifo_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_fifo_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.isFIFO()) == 'boolean').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_fifo_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isFifo_0100
   * @tc.name fileio_test_stat_is_fifo_001
   * @tc.desc Function of API, isFIFO. Determine whether it is a fifo. The return value is false.
   */
  it('fileio_test_stat_is_fifo_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_fifo_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isFIFO()).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_fifo_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isFifo_0200
   * @tc.name fileio_test_stat_is_fifo_002
   * @tc.desc Function of API, isFIFO. The parameter is - 1
   */
  it('fileio_test_stat_is_fifo_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_fifo_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(stat.isFIFO(-1) == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_fifo_002 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isFile_0000
   * @tc.name fileio_test_stat_is_file_000
   * @tc.desc Function of API, isFile. Determine whether it is a file. The return value is Boolean.
   */
  it('fileio_test_stat_is_file_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_file_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.isFile()) == 'boolean').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_file_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isFile_0100
   * @tc.name fileio_test_stat_is_file_001
   * @tc.desc Function of API, isFile. Determine whether it is a file. The return value is true.
   */
  it('fileio_test_stat_is_file_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_file_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(stat.isFile()).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
      console.log('---fileio_test_stat_is_file_001 is passed!');
    } 
    catch (e) {
      console.log('fileio_test_stat_is_file_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isFile_0200
   * @tc.name fileio_test_stat_is_file_002
   * @tc.desc Function of API, isFile. Determine whether it is a file. The return value is false.
   */
  it('fileio_test_stat_is_file_002', 0, async function () {
    let dpath = await nextFileName('fileio_test_stat_is_file_002');
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let stat = fileio.statSync(dpath);
      expect(!stat.isFile()).assertTrue();
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
    }
    catch (e) {
      console.log('fileio_test_stat_is_file_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isFile_0300
   * @tc.name fileio_test_stat_is_file_003
   * @tc.desc Function of API, isFile. The parameter is - 1
   */
  it('fileio_test_stat_is_file_003', 0, async function () {
    let dpath = await nextFileName('fileio_test_stat_is_file_003');
    try {
      expect(fileio.mkdirSync(dpath) !== null).assertTrue();
      let stat = fileio.statSync(dpath);
      expect(stat.isFile(-1) == null).assertTrue();
      expect(null).assertFail();
    }
    catch (e) {
      expect(fileio.rmdirSync(dpath) !== null).assertTrue();
      console.log('fileio_test_stat_is_file_003 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isSocket_0000
   * @tc.name fileio_test_stat_is_socket_000
   * @tc.desc Function of API, isSocket. Determine whether it is a socket. The return value is Boolean.
   */
  it('fileio_test_stat_is_socket_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_socket_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.isSocket()) == 'boolean').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_socket_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isSocket_0100
   * @tc.name fileio_test_stat_is_socket_001
   * @tc.desc Function of API, isSocket. Determine whether it is a socket. The return value is false.
   */
  it('fileio_test_stat_is_socket_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_socket_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isSocket()).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_socket_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isSocket_0200
   * @tc.name fileio_test_stat_is_socket_002
   * @tc.desc Function of API, isSocket. The parameter is - 1
   */
  it('fileio_test_stat_is_socket_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_socket_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(stat.isSocket(-1) == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_socket_002 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isSymbolicLink_0000
   * @tc.name fileio_test_stat_is_symbolic_link_000
   * @tc.desc Function of API, isSymbolicLink. Determine whether it is a symbolic link. The return value is Boolean.
   */
  it('fileio_test_stat_is_symbolic_link_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_symbolic_link_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(typeof(stat.isSymbolicLink()) == 'boolean').assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_symbolic_link_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isSymbolicLink_0100
   * @tc.name fileio_test_stat_is_symbolic_link_001
   * @tc.desc Function of API, isSymbolicLink. Determine whether it is a symbolic link. The return value is false.
   */
  it('fileio_test_stat_is_symbolic_link_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_symbolic_link_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(!stat.isSymbolicLink()).assertTrue();
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_symbolic_link_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_STORAGE_FileIO_Stat_isSymbolicLink_0200
   * @tc.name fileio_test_stat_is_symbolic_link_002
   * @tc.desc Function of API, isSymbolicLink. The parameter is - 1
   */
  it('fileio_test_stat_is_symbolic_link_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stat_is_symbolic_link_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let stat = fileio.statSync(fpath);
      expect(stat.isSymbolicLink(-1) == null).assertTrue();
      expect(null).assertFail();
    } 
    catch (e) {
      console.log('fileio_test_stat_is_symbolic_link_002 has failed for ' + e);
      expect(fileio.unlinkSync(fpath) !== null).assertTrue();
    }
  });

});
