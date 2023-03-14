/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import filemanager from '@ohos.filemanager';
import {describe,it,expect,beforeAll}from 'deccjsunit/index';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import bundle from '@ohos.bundle';

// getRoot() interface, when the parameter is "local", the returned data and the data contrast,  return the same data is correct
const ROOTFILE = [
  {"name":"image_album","path":"dataability:///album","type":"album","size":0,"added_time":0,"modified_time":0},
  {"name":"video_album","path":"dataability:///album","type":"album","size":0,"added_time":0,"modified_time":0},
  {"name":"audio_album","path":"dataability:///album","type":"album","size":0,"added_time":0,"modified_time":0},
  {"name":"file_folder","path":"dataability:///album","type":"album","size":0,"added_time":0,"modified_time":0}
];

let IMAGE_ROOT = "";
let VIDEO_ROOT = "";
let FILE_ROOT = "";
let AUDIO_ROOT = "";
let IMAGE_ALBUM = "";
let LOG_ = "FMS_XTS_TEXT: ";
let LENGTH = 0;
let tokenID = null;
describe("filemanager_test", function () {

  beforeAll(async function (){
    let appInfo = await bundle.getApplicationInfo('ohos.acts.storage.filemanager', 0, 100);
    tokenID = appInfo.accessTokenId;
    let atManager = abilityAccessCtrl.createAtManager();
    let result = await atManager.grantUserGrantedPermission(tokenID, "ohos.permission.READ_MEDIA",1);
    console.log("tokenID:" + tokenID + "-result:" + result);
    let result1 = await atManager.verifyAccessToken(tokenID, "ohos.permission.READ_MEDIA");
    console.log("tokenID:" + tokenID + "-result:" + result1);
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_GET_ROOT_0000
   * @tc.name filemanager_test_get_root_async_000
   * @tc.desc Test getRoot() interfaces, the first layer directory album information, promise way back.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_get_root_async_000", 0, async function (done) {
    try {
      let fileInfos = await filemanager.getRoot();
      expect(Array.isArray(fileInfos)).assertTrue();
      expect(JSON.stringify(fileInfos) == JSON.stringify(ROOTFILE)).assertTrue();
      for (let i = 0; i < fileInfos.length; i++) {
        if (fileInfos[i].name == "image_album") {
          console.log(LOG_ +JSON.stringify(fileInfos[i]))
          IMAGE_ROOT = fileInfos[i].path;
          console.log(LOG_ + IMAGE_ROOT)
        } else if (fileInfos[i].name == "audio_album") {
          console.log(LOG_ + JSON.stringify(fileInfos[i]))
          AUDIO_ROOT = fileInfos[i].path;
          console.log(LOG_ + AUDIO_ROOT)
        } else if (fileInfos[i].name == "video_album") {
          console.log(LOG_ + JSON.stringify(fileInfos[i]))
          VIDEO_ROOT = fileInfos[i].path;
          console.log(LOG_ + VIDEO_ROOT)
        } else if (fileInfos[i].name == "file_folder") {
          console.log(LOG_ + JSON.stringify(fileInfos[i]))
          FILE_ROOT = fileInfos[i].path;
          console.log(LOG_ + FILE_ROOT)
        }
      }
      done();
    } catch (error) {
      console.log("filemanager_test_get_root_async_000 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_GET_ROOT_0010
   * @tc.name filemanager_test_get_root_async_001
   * @tc.desc Test getRoot() interfaces, the first layer directory album information, callback way back.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_get_root_async_001", 0, async function (done) {
    try {
      filemanager.getRoot(function(error, fileInfos){
        expect(Array.isArray(fileInfos)).assertTrue();
        expect(JSON.stringify(fileInfos) == JSON.stringify(ROOTFILE)).assertTrue();
        done();
      });
    } catch (error) {
      console.log("filemanager_test_get_root_async_001 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_GET_ROOT_0020
   * @tc.name filemanager_test_get_root_async_002
   * @tc.desc Test getRoot() interfaces, the first layer directory album information, callback way back.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_get_root_async_002", 0, async function (done) {
    try {
      let options = {
        dev:{
          "name":"local"
        }
      }
      filemanager.getRoot(options,function(error, fileInfos) {
        expect(Array.isArray(fileInfos)).assertTrue();
        expect(JSON.stringify(fileInfos) == JSON.stringify(ROOTFILE)).assertTrue();
        done();
      });
    } catch (error) {
      console.log("filemanager_test_get_root_async_002 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_GET_ROOT_0030
   * @tc.name filemanager_test_get_root_async_003
   * @tc.desc Test FileInfo property 
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it("filemanager_test_get_root_async_003", 0, async function (done) {
    try {
      let fileInfos = await filemanager.getRoot();
      expect(Array.isArray(fileInfos)).assertTrue();
      expect(JSON.stringify(fileInfos) == JSON.stringify(ROOTFILE)).assertTrue();
      for (let i = 0; i < fileInfos.length; i++) {
        expect(typeof fileInfos[i].name == "string").assertTrue();
        expect(typeof fileInfos[i].path == "string").assertTrue();
        expect(typeof fileInfos[i].type == "string").assertTrue();
        expect(typeof fileInfos[i].size == "number").assertTrue();
        expect(typeof fileInfos[i].added_time == "number").assertTrue();
        expect(typeof fileInfos[i].modified_time == "number").assertTrue();
      }
      done();
    } catch (error) {
      console.log("filemanager_test_get_root_async_003 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_GET_ROOT_0040
   * @tc.name filemanager_test_get_root_async_004
   * @tc.desc Test getRoot() interfaces, when the parameter type is wrong,throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it("filemanager_test_get_root_async_004", 0, async function (done) {
    try {
      let options = {
        dev:{
          "name":1
        }
      }
      await filemanager.getRoot(options);
    } catch (error) {
      console.log("filemanager_test_get_root_async_004 has failed for " + error);
      expect(error.message == "GetRoot func get dev para fails").assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0000
   * @tc.name filemanager_test_list_file_async_000
   * @tc.desc Test listFile() interfaces，when the type parameters as the "image", return path of files information accordingly.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_list_file_async_000", 0, async function (done) {
    try {
      let path = IMAGE_ROOT;
      let fileInfos = await filemanager.listFile(path, "image");
      expect(Array.isArray(fileInfos)).assertTrue();
      for (let i = 0; i < fileInfos.length; i++) {
        if (fileInfos[i].name == "image") {
          console.log(JSON.stringify(fileInfos[i]))
          IMAGE_ALBUM = fileInfos[i].path;
          console.log(LOG_ + JSON.stringify(fileInfos[i]))
          console.log(LOG_ + IMAGE_ALBUM)
        }
      }
      done();
    } catch (error) {
      console.log("filemanager_test_list_file_async_000 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0010
   * @tc.name filemanager_test_list_file_async_001
   * @tc.desc Test listFile() interfaces， when the type parameters as the "image", return path of files information accordingly.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_list_file_async_001", 0, async function (done) {
    try {
      let path = IMAGE_ALBUM;
      let fileInfos = await filemanager.listFile(path, "image");
      expect(Array.isArray(fileInfos)).assertTrue();
      done();
    } catch (error) {
      console.log("filemanager_test_list_file_async_001 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0020
   * @tc.name filemanager_test_list_file_async_002
   * @tc.desc Test listFile() interfaces, when the type parameters as the "file", return path of files information accordingly.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_list_file_async_002", 0, async function (done) {
    try {
      let path = FILE_ROOT;
      let options = {
        dev:{
          "name":"local"
        },
        offset:0
      };
      let fileInfos = await filemanager.listFile(path, "file",options);
      LENGTH = fileInfos.length;
      expect(Array.isArray(fileInfos)).assertTrue();
      done();
    } catch (error) {
      console.log("filemanager_test_list_file_async_002 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0030
   * @tc.name filemanager_test_list_file_async_003
   * @tc.desc Test listFile() interfaces,  When the number of query files is two,return path of files information accordingly and The number of files is two.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_list_file_async_003", 0, async function (done) {
    try {
      let path = FILE_ROOT;
      let options = {
        dev: {
            "name":"local",
        },
        offset:0,
        count:2
      };
      filemanager.listFile(path, "file", options, function(error,fileInfos){
        expect(Array.isArray(fileInfos)).assertTrue();
        expect(fileInfos.length == 2).assertTrue();
        done();
      });
    } catch (error) {
      console.log("filemanager_test_list_file_async_003 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0040
   * @tc.name filemanager_test_list_file_async_004
   * @tc.desc Test listFile() interfaces, When the number of offset files is two, The number of returned files is the total number of files minus two.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it("filemanager_test_list_file_async_004", 0, async function (done) {
    try {
      let path = FILE_ROOT;
      let options = {
        dev: {
            "name":"local",
        },
        offset:2
    };
      filemanager.listFile(path, "file", options, function(error,fileInfos){
        expect(Array.isArray(fileInfos)).assertTrue();
        expect(fileInfos.length == LENGTH-2).assertTrue();
        done();
      });
    } catch (error) {
      console.log("filemanager_test_list_file_async_004 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0050
   * @tc.name filemanager_test_list_file_async_005
   * @tc.desc Test listFile() interfaces, when the number of query files is two, return path of files information accordingly.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it("filemanager_test_list_file_async_005", 0, async function (done) {
    try {
      let path = FILE_ROOT;
      let options = {
        dev: {
            "name":"local",
        },
        count:2
      };
      filemanager.listFile(path, "file", options, function(error,fileInfos){
        expect(Array.isArray(fileInfos)).assertTrue();
        expect(fileInfos.length == 2).assertTrue();
        done();
      });
    } catch (error) {
      console.log("filemanager_test_list_file_async_005 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0060
   * @tc.name filemanager_test_list_file_async_006
   * @tc.desc Test listFile() interfaces, when the dev parameter does not exist, return path of files information accordingly.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it("filemanager_test_list_file_async_006", 0, async function (done) {
    try {
      let path = FILE_ROOT;
      let options = {
        offset:1,
        count:2
      };
      filemanager.listFile(path, "file", options, function(error,fileInfos){
        expect(Array.isArray(fileInfos)).assertTrue();
        expect(fileInfos.length == 2).assertTrue();
        done();
      });
    } catch (error) {
      console.log("filemanager_test_list_file_async_006 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0070
   * @tc.name filemanager_test_list_file_async_007
   * @tc.desc Test listFile() interfaces, when the type parameter to an empty string, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_list_file_async_007", 0, async function (done) {
    try {
      let path = FILE_ROOT;
      await filemanager.listFile(path, " ");
    } catch (error) {
      console.log("filemanager_test_list_file_async_007 has failed for " + error);
      expect(error.message == "Not a directory").assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_LIST_FILE_0080
   * @tc.name filemanager_test_list_file_async_008
   * @tc.desc Test listFile() interfaces, when there are no parameters, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
   it("filemanager_test_list_file_async_008", 0, async function (done) {
    try {
      await filemanager.listFile();
    } catch (error) {
      console.log("filemanager_test_list_file_async_008 has failed for " + error);
      expect(error.message == "Number of argments unmatched").assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0000
   * @tc.name filemanager_test_createfile_async_000
   * @tc.desc Test createFile() interfaces promise way to create a file normally.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_000", 0, async function (done) {
    try {
      let name = "create00" + new Date().getTime() + ".jpg";
      let path = IMAGE_ROOT;
      let uri = await filemanager.createFile(path, name);
      console.log(LOG_ + name + " " + path)
      console.log(LOG_ + uri)
      expect(uri != undefined).assertTrue();
      done();
    } catch (error) {
      console.log("filemanager_test_createfile_async_000 has failed for " + error);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0010
   * @tc.name filemanager_test_createfile_async_001
   * @tc.desc Test createFile() interfaces callback way to create a file normally.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_001", 0, async function (done) {
    let name = "create00" + new Date().getTime() + ".txt";
    let path = IMAGE_ROOT;
    filemanager.createFile(path, name, function(err, uri) {
      expect(uri != undefined).assertTrue();
      console.log(LOG_ + uri)
      done();
    });
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0020
   * @tc.name filemanager_test_createfile_async_002
   * @tc.desc Test createFile() interfaces to create an existing file, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_002", 0, async function (done) {
    try {
      let name = "1.jpg";
      let path = IMAGE_ROOT;
      filemanager.createFile(path, name)
      .then(function(uri){
        console.log("filemanager_test_createfile_async_002 uri " + uri);
        expect(null).assertFail();
      }).catch(function(e) {
        console.log("filemanager_test_createfile_async_002 e " + e);
        expect(e.message == "Operation not permitted").assertTrue();
        done();
      })
    } catch (error) {
      console.log("filemanager_test_createfile_async_002 for " + error);
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0030
   * @tc.name filemanager_test_createfile_async_003
   * @tc.desc Test createFile() interfaces to create a file, the path error parameters, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_003", 0, async function (done) {
    try {
      let name = "filemanager_test_createfile_async_003";
      filemanager.createFile("123", name)
        .then(function(uri) {
          console.log("filemanager_test_createfile_async_003 uri " + uri);
          expect(null).assertFail();
        }).catch(function(error) {
        console.log("filemanager_test_createfile_async_003 e " + error);
        expect(error.message == "No such file or directory").assertTrue();
        done();
        })
    } catch (error) {
      console.log("filemanager_test_createfile_async_003 has failed for " + error);
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0040
   * @tc.name filemanager_test_createfile_async_004
   * @tc.desc Test createFile() interfaces to create a file, without any parameters, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_004", 0, async function (done) {
    try {
      filemanager.createFile()
        .then(function(uri) {
          console.log("filemanager_test_createfile_async_004 uri " + uri);
          expect(null).assertFail();
        }).catch(function(e) {
          console.log("filemanager_test_createfile_async_004 e " + e);
        })
    } catch (error) {
      console.log("filemanager_test_createfile_async_004 has failed for1 " + error);
      expect(error.toString().indexOf("Number of arguments unmatched") !=-1).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0050
   * @tc.name filemanager_test_createfile_async_005
   * @tc.desc Test createFile() interfaces to create a file, all parameters to an empty string, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_005", 0, async function (done) {
    try {
      filemanager.createFile("", "", "").then(function(uri) {
        console.log("filemanager_test_createfile_async_005 uri " + uri);
        expect(null).assertFail();
      }).catch(function(error) {
        console.log("filemanager_test_createfile_async_005 e " + error);
        expect(error.message == "No such file or directory").assertTrue();
        done();
       })
    } catch (error) {
      console.log("filemanager_test_createfile_async_005 has failed for " + error);
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0060
   * @tc.name filemanager_test_createfile_async_006
   * @tc.desc Test createFile() interfaces create a file, parameter path contains escape characters, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_006", 0, async function (done) {
    try {
      let name = "filemanager_test_createfile_async_006";
      filemanager.createFile("internal://cache%d%sfile", name)
        .then(function(uri) {
          console.log("filemanager_test_createfile_async_006 uri " + uri);
          expect(null).assertFail();
        }).catch(function(error) {
          console.log("filemanager_test_createfile_async_006 e " + error);
          expect(error.message == "No such file or directory").assertTrue();
          done();
        })
    } catch (error) {
      console.log("filemanager_test_createfile_async_006 has failed for " + error);
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0070
   * @tc.name filemanager_test_createfile_async_007
   * @tc.desc Test createFile() interfaces a file, parameter path contain special characters, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_007", 0, async function (done) {
    try {
      filemanager.createFile("internal://cache/@#$&^", "filemanager_test_createfile_async_007")
        .then(function(uri) {
          console.log("filemanager_test_createfile_async_007 uri " + uri);
          expect(null).assertFail();
        }).catch(function(error) {
          console.log("filemanager_test_createfile_async_007 e " + error);
          expect(error.message == "No such file or directory").assertTrue();
          done();
        })
    } catch (error) {
      console.log("filemanager_test_createfile_async_007 has failed for " + error);
    }
  });

  /**
   * @tc.number SUB_DF_FILEMANAGER_CREATEFILE_0080
   * @tc.name filemanager_test_createfile_async_008
   * @tc.desc Test createFile() interfaces to create a file, lack of path parameters, throw an exception.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it("filemanager_test_createfile_async_008", 0, async function (done) {
    let name = "filemanager_test_createfile_async_008";
    try {
      filemanager.createFile(name)
        .then(function(uri) {
          console.log("filemanager_test_createfile_async_008 uri " + uri);
          expect(null).assertFail();
        }).catch(function(e) {
          console.log("filemanager_test_createfile_async_008 e " + e);
        })
    } catch (error) {
      console.log("filemanager_test_createfile_async_008 has failed for " + error);
      expect(error.toString().indexOf("Number of arguments unmatched") !=-1).assertTrue();
      done();
    }
  });
});