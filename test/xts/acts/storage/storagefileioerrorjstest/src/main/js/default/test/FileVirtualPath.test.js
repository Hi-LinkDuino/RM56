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

import fileio from '@system.fileio';
import file from '@system.file';
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
  nextFileName
}
  from './Common'

describe('fileVirtualPath', function () {

  /**
   * @tc.number SUB_STORAGE_File_Delete_0900
   * @tc.name File_Delete_009
   * @tc.desc Function of API, virtual path, Delete files from outside the package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_Delete_009', 0, async function (done) {
    file.writeText({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_Delete_009',
      text: 'sawyerwang',
      success: function () {
        console.log('File_Delete_009 mkdir success');
        done();
      },
      fail: function (data, code) {
        console.log('File_Delete_009 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.delete({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_Delete_009',
      success: function () {
        console.log('File_Delete_009 call delete success');
        done();
      },
      fail: function (data, code) {
        console.log(code);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_Delete_1000
   * @tc.name File_Delete_010
   * @tc.desc Function of API, virtual path, Delete files from outside the package.
   * Create files and maintain file permissions(755):/data/accounts/account_0/appdata/123456
   */
  it('File_Delete_010', 0, async function (done) {
    file.delete({
      uri: 'internal://app/../../123456',
      success: function () {
        console.log('File_Delete_010 => pass');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('File_Delete_010 => fail ,code: ' + code + ', data: ' + data);
        expect(code == 300).assertTrue();
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_writeText_1200
   * @tc.name File_writeText_012
   * @tc.desc Function of API, virtual path, Write files from outside the package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_writeText_012', 0, async function (done) {
    file.writeText({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_writeText_012',
      text: 'sawyerwang',
      success: function () {
        console.log('File_writeText_012 mkdir success');
        done();
      },
      fail: function (data, code) {
        console.log('File_writeText_012 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.readText({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_writeText_012',
      success: function (data) {
        console.log('File_writeText_012 => call readText pass, ' + data.text);
        done();
      },
      fail: function (data, code) {
        console.log('File_writeText_012 call readText callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.delete({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_writeText_012',
      success: function () {
        console.log('File_writeText_012 delete success');
        done();
      },
      fail: function (data, code) {
        console.log('File_writeText_012  call delete callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_writeArrayBuffer_1100
   * @tc.name File_writeArrayBuffer_011
   * @tc.desc Function of API, virtual path, Write files from outside the package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_writeArrayBuffer_011', 0, async function (done) {
    let buf = new Uint8Array([48, 49, 50, 51, 65, 66, 67, 68, 32, 33]);
    file.writeArrayBuffer({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_writeArrayBuffer_011',
      buffer: buf,
      append: true,
      success: function () {
        console.log('File_writeArrayBuffer_011 call writeArrayBuffer success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_writeArrayBuffer_011 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.readArrayBuffer({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_writeArrayBuffer_011',
      success: function (data) {
        console.log('File_writeArrayBuffer_011 readArrayBuffer: call success' + data.buffer);
        done();
      },
      fail: function (data, code) {
        console.log('File_writeArrayBuffer_011 readArrayBuffer , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.delete({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_writeArrayBuffer_011'
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_readText_0800
   * @tc.name File_readText_008
   * @tc.desc Function of API, virtual path, Read files from outside the package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_readText_008', 0, async function (done) {
    file.writeText({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_readText_008',
      text: 'Text that just for test.',
      success: function () {
        console.log('File_readText_008 call writeText success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_readText_008 call writeText callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.readText({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_readText_008',
      success: function (data) {
        console.log('File_readText_008 call readText success. data.text:' + data.text);
        done();
      },
      fail: function (data, code) {
        console.log('File_readText_008 call readText callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.delete({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_readText_008'
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_readText_0900
   * @tc.name File_readText_009
   * @tc.desc Function of API, virtual path, Read files from outside the package.
   * Create files and maintain file permissions(755):/data/accounts/account_0/appdata/123456
   */
  it('File_readText_009', 0, async function (done) {
    file.readText({
      uri: 'internal://app/../../123456',
      success: function (data) {
        console.log('File_readText_009 => pass, data.text:' + data.text);
        done();
      },
      fail: function (data, code) {
        console.log('File_readText_009 call readText callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_read_array_buffer_1000
   * @tc.name File_read_array_buffer_010
   * @tc.desc Function of API, virtual path, Read files from outside the package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_read_array_buffer_010', 0, async function (done) {
    var buffer = new Uint8Array([48, 49, 50, 51, 65, 66, 67, 68, 32, 33]);
    file.writeArrayBuffer({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_read_array_buffer_010',
      buffer: buffer,
      success: function () {
        console.log('File_read_array_buffer_010 call writeArrayBuffer success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_read_array_buffer_010 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.readArrayBuffer({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_read_array_buffer_010',
      position: 0,
      length: 10,
      success: function (data) {
        console.log('File_read_array_buffer_010 call readArrayBuffer success. data.buffer:' + data.buffer);
        done();
      },
      fail: function (data, code) {
        console.log('File_read_array_buffer_010 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.delete({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_read_array_buffer_010'
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_read_array_buffer_1100
   * @tc.name File_read_array_buffer_011
   * @tc.desc Function of API, virtual path, Read files from outside the package.
   * Create files and maintain file permissions(755):/data/accounts/account_0/appdata/123456
   */
  it('File_read_array_buffer_011', 0, async function (done) {
    file.readArrayBuffer({
      uri: 'internal://cache/../../123456',
      position: 0,
      length: 10,
      success: function (data) {
        console.log('File_read_array_buffer_011 => pass, data.buffer: ' + data.buffer);
        done();
      },
      fail: function (data, code) {
        console.log('File_read_array_buffer_011 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_access_0900
   * @tc.name File_access_009
   * @tc.desc Function of API, virtual path, Determine whether files outside the package exist.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_access_009', 0, async function (done) {
    file.writeText({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_access_009',
      text: 'Text that just for test.',
      success: function () {
        console.log('File_access_009 mkdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_access_009 fail，code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.access({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_access_009',
      success: function () {
        console.log('File_access_009 call access success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_access_009 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.delete({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_access_009',
      success: function () {
        console.log('File_access_009 call delete success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_access_009 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_access_1000
   * @tc.name File_access_010
   * @tc.desc Function of API, virtual path, Determine whether files outside the package exist.
   *  Create files and maintain file permissions(755):
   * /data/accounts/account_0/appdata/123456
   */
  it('File_access_010', 0, async function (done) {
    file.access({
      uri: 'internal://cache/../../123456',
      success: function () {
        console.log('File_access_010 call access success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_access_010 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_mkdir_0900
   * @tc.name File_mkdir_009
   * @tc.desc Function of API, virtual path, Create directory outside package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_mkdir_009', 0, async function (done) {
    file.mkdir({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_mkdir_009',
      success: function () {
        console.log('File_mkdir_009 call mkdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_mkdir_009 call mkdir callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.rmdir({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_mkdir_009',
      success: function () {
        console.log('File_mkdir_009 call rmdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_mkdir_009 call rmdir callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_rmdir_0900
   * @tc.name File_rmdir_009
   * @tc.desc Function of API, virtual path, Delete directory outside package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_rmdir_009', 0, async function (done) {
    file.mkdir({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_rmdir_009',
      success: function () {
        console.log('File_rmdir_009 call mkdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_rmdir_009 call mkdir callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.rmdir({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_rmdir_009',
      success: function () {
        console.log('File_rmdir_009 call rmdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_rmdir_009 call rmdir callback fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_Move_1300
   * @tc.name File_Move_013
   * @tc.desc Function of API, virtual path, Move files out of package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_Move_013', 0, async function (done) {
    let srcFpath = fileName('File_Move_013');
    expect(prepareFile(srcFpath, FILE_CONTENT)).assertTrue();
    file.move({
      srcUri: 'internal://app/../files/File_Move_013',
      dstUri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_Move_013',
      success: function (uri) {
        console.log('File_Move_013 => pass,uri: ' + uri);
        done();
      },
      fail: function (data, code) {
        console.log('File_Move_013 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_Copy_1300
   * @tc.name File_Copy_013
   * @tc.desc Function of API, virtual path, Copy files out of package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_Copy_013', 0, async function (done) {
    let srcFpath = fileName('File_Copy_013');
    expect(prepareFile(srcFpath, 'test')).assertTrue();
    file.copy({
      srcUri: 'internal://app/../files/File_Copy_013',
      dstUri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile/File_Copy_013',
      success: function (uri) {
        console.log('File_Copy_013=> pass uri:' + uri);
        done();
      },
      fail: function (data, code) {
        console.log('File_Copy_013 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_List_1500
   * @tc.name File_List_015
   * @tc.desc Function of API, virtual path, Query the files in the directory outside the package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_List_015', 0, async function (done) {
    file.writeText({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_List_015',
      text: 'Text that just for test.',
      success: function () {
        console.log('File_List_015 mkdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_List_015 fail，code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.writeText({
      uri: 'internal://cache/../../ohos.acts.distributeddatamgr.distributedfile/File_List_015_1',
      text: 'Text that just for test.',
      success: function () {
        console.log('File_List_015_1 mkdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('File_List_015_1 fail，code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.list({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile',
      success: function (data) {
        console.log('File_List_015 => pass' + JSON.stringify(data.fileList));
        done();
      },
      fail: function (data, code) {
        console.log('File_List_015 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_List_1500
   * @tc.name File_List_015
   * @tc.desc Function of API, virtual path, Query the files in the directory outside the package.
   *  create directory and grant 777 permission:
   * /data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile
   */
  it('File_Get_017', 0, async function (done) {
    file.get({
      uri: 'internal://app/../../ohos.acts.distributeddatamgr.distributedfile',
      success: function (data) {
        console.log('File_Get_017 => pass');
        done();
      },
      fail: function (data, code) {
        console.log('File_Get_017 , code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_fileioPerformance_prop_copyFile
   * @tc.name fileioPerformance_prop_copyFile
   * @tc.desc 0.5G Synchronous opening performance test of test files
   * The path needs to be manually placed in advance
   * data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile/cache/p1'的0.5g文件）
   */
  it('fileioPerformance_prop_copyFile', 0, function () {
    console.log('---fileioPerformance_prop_copyFile 0.5G---start---');
    let fpath = nextFileName('p1');
    let fpathTarget = nextFileName('p2');
    for (let i = 0; i < 1000; i++) {
      let start2 = new Date().getTime();
      let copyFileSync = fileio.copyFileSync(fpath, fpathTarget);
      let end2 = new Date().getTime();
      let time2 = end2 - start2
      console.log('fileioPerformance_prop_copyFileSync,copyFileSync:' + copyFileSync + ', time2:' + time2 + ',' + i);
      fileio.unlinkSync(fpathTarget);
    }
    fileio.closeSync(fd);
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_fileioPerformance_prop_openSync
   * @tc.name fileioPerformance_prop_openSync
   * @tc.desc 0.5G Synchronous opening performance test of test files
   * The path needs to be manually placed in advance
   * data/accounts/account_0/appdata/ohos.acts.distributeddatamgr.distributedfile/cache/p1'的0.5g文件）
   */
  it('fileioPerformance_prop_openSync', 0, function () {
    console.log('---fileioPerformance_prop_openSync 0.5G---start---');
    let fpath = nextFileName('p1');
    let fd
    for (let i = 0; i < 1000; i++) {
      let start = new Date().getTime();
      fd = fileio.openSync(fpath, 0o2002);
      let end = new Date().getTime();
      let time = end - start
      console.log('fileioPerformance_prop_openSync,openSync:' + fd + ', time:' + time + ',' + i);
    }
    fileio.closeSync(fd);
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

});
