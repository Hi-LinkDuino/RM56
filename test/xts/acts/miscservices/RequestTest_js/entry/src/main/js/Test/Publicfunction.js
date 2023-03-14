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

import {describe, it, expect} from 'deccjsunit/index'
import request from '@ohos.request'

let RequestData = [{
    name: '', // Represents the name of the form element.
    value: '' // Represents the value of the form element.
}]

let RequestDataArray=new Array();

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function getUploadConfig(fileurl){
    let File = {
        filename: 'test', // When multipart is submitted, the file name in the request header.
        name: 'test', // When multipart is submitted, the name of the form item. The default is file.
        uri: 'internal://cache/test.txt',
        //The local storage path of the file
        // (please refer to the storage directory definition for path usage).
        type: 'txt'
        //The content type of the file is obtained by default
        // according to the suffix of the file name or path.
    }
    let FileArray=new Array();
    FileArray[0] = File;
    let headerHttp = { headers: 'http' }
    let UploadConfig = {
        url: 'http://192.168.112.124/upload_test/',// Resource address.
        header: headerHttp, // Adds an HTTP or HTTPS header to be included with the upload request.
        method: 'POST', // Request method: POST, PUT. The default POST.
        files: FileArray, // A list of files to be uploaded. Please use multipart/form-data to submit.
        data: RequestData // The requested form data.
    }
    return UploadConfig
}

//upload公共方法
function publicupload(UploadConfig){
    console.log(`Testupdate UploadConfig ${JSON.stringify(UploadConfig)}`)
    return new Promise(function(resolve, reject) {
        request.upload(UploadConfig, (err, data) => {
            console.log("Testupdate publiconprogress Updatetask =" + JSON.stringify(data));
            resolve(data);
        })
    })
}

//onprogress公共方法
function publiconprogress(Updatetask, Type){
    return new Promise(function(resolve, reject) {
        Updatetask.on(Type, function(data1 ,data2){
            let progress = {
                uploadedSize : data1,
                totalSize : data2
            }
            console.log("Testupdate publiconprogress uploadedSize =" + data1);
            console.log("Testupdate publiconprogress totalSize =" + data2);
            resolve(progress);
        })
    })
}

//offprogress公共方法
function publicoffprogress(Updatetask, Type){
    return new Promise(function(resolve, reject) {
        Updatetask.off(Type, function(data1 ,data2){
            let progress = {
                uploadedSize : data1,
                totalSize : data2
            }
            console.log("Testupdate publicoffprogress uploadedSize =" + data1);
            console.log("Testupdate publicoffprogress totalSize =" + data2);
            resolve(progress);
        })
    })
}

//其他on公共方法
function publicon(Updatetask, Type){
    return new Promise(function(resolve, reject) {
        Updatetask.on(Type, function(data){
            console.log("Testupdate publicon =" + data);
            resolve(data);
        })
    })
}

//其他off公共方法
function publicoff(Updatetask, Type){
    return new Promise(function(resolve, reject) {
        Updatetask.off(Type, function(data){
            console.log("Testupdate publicoff =" + data);
            resolve(data);
        })
    })
}

//remove公共方法
function publicremove(Updatetask, Type){
    return new Promise(function(resolve, reject) {
        Updatetask.remove((err,data) => {
            console.log("Testupdate publicremove =" + data);
            resolve(data);
        })
    })
}

export{publicupload,publicon,publicoff,publicremove,publiconprogress,publicoffprogress,getUploadConfig,sleep}