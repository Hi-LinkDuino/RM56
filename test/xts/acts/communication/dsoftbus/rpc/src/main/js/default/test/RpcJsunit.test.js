/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import rpc from '@ohos.rpc'
import {describe, beforeAll, beforeEach, afterEach, afterAll, expect, it} from 'deccjsunit/index'

describe('ActsRpcJsTest', function(){
    console.log("-----------------------ActsRpcJsTest is starting-----------------------");

    beforeAll(function (){
        console.info('beforeAll called');
    });

    beforeEach(function (){
        console.info('beforeEach called');
    });

    afterEach(function (){
        console.info('afterEach called');
    });

    afterAll(function (){
        console.info('afterAll called');
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0100
     * @tc.name    Call the writeinterfacetoken interface, write the interface descriptor, and read interfacetoken
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0100", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0100: create object successfully.");

            var token = "hello ruan zong xian";
            var result = data.writeInterfaceToken(token);
            console.log("SUB_Softbus_IPC_MessageParcel_0100:run writeInterfaceToken success, result is " + result);
            expect(result == true).assertTrue();

            var resultToken = data.readInterfaceToken();
            console.log("SUB_Softbus_IPC_MessageParcel_0100:run readInterfaceToken success, result is " + resultToken);
            expect(resultToken == token);

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0100:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0200
     * @tc.name    Call the writeinterfacetoken interface to write a non string interface descriptor
                   and read interfacetoken
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0200", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0200: create object successfully.");

            var token = 123;
            var result = data.writeInterfaceToken(token);

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0200: error = " + error);
            expect(error != null).assertTrue();
      }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0300
     * @tc.name    The data size of the messageparcel obtained by calling the getSize interface
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0300", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0300: create object successfully.");

            var size = data.getSize();
            console.log("SUB_Softbus_IPC_MessageParcel_0300:run getSize is success, result is " + size);
            expect(size == 0).assertTrue();

            var addData = 1;
            var result = data.writeInt(addData);
            console.log("SUB_Softbus_IPC_MessageParcel_0300:run writeInt is success, result is " + result);
            expect(result == true).assertTrue();

            size = data.getSize();
            console.log("SUB_Softbus_IPC_MessageParcel_0300:run getSize is success, result is " + size);
            expect(size == 4).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0300: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0400
     * @tc.name    The capacity of the messageparcel obtained by calling the getcapacity interface
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0400", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0400: create object successfully.");

            var size = data.getCapacity();
            console.log("SUB_Softbus_IPC_MessageParcel_0400:run getCapacity is success, result is " + size);
            expect(size == 0).assertTrue();

            var addData = 1;
            var result = data.writeInt(addData);
            console.log("SUB_Softbus_IPC_MessageParcel_0400:run writeInt is success, result is " + result);
            expect(result == true).assertTrue();

            size = data.getCapacity();
            console.log("SUB_Softbus_IPC_MessageParcel_0400:run getCapacity is success, result is " + size);
            expect(size == 64).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0400: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0500
     * @tc.name    Call the SetSize interface to set the data size of messageparcel
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0500", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0500: create object successfully.");

            var addData = 1;
            var result = data.writeInt(addData);
            console.log("SUB_Softbus_IPC_MessageParcel_0500:run writeInt is success, result is " + result);
            expect(result == true).assertTrue();

            var size = 6;
            var setResult = data.setSize(size);
            console.log("SUB_Softbus_IPC_MessageParcel_0500:run setSize success, result is " + setResult);
            expect(setResult == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0500: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0600
     * @tc.name    Call the SetSize interface to set the data size of messageparcel. The write data size
                   does not match the set value
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0600", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0600: create object successfully.");

            var capacity = 64;
            var setResult = data.setCapacity(capacity);
            console.log("SUB_Softbus_IPC_MessageParcel_0600:run setCapacity success, result is " + setResult);
            expect(setResult == true).assertTrue();

            var size = 4;
            setResult = data.setSize(size);
            console.log("SUB_Softbus_IPC_MessageParcel_0600:run setSize success, result is " + setResult);
            expect(setResult == true).assertTrue();

            var addData = 2;
            var result = data.writeLong(addData);
            console.log("SUB_Softbus_IPC_MessageParcel_0600:run writeInt is success, result is " + result);
            expect(result == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0600: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0700
     * @tc.name    Call the setcapacity interface to set the capacity of messageparcel
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0700", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0700: create object successfully.");

            var size = 64;
            var setResult = data.setCapacity(size);
            console.log("SUB_Softbus_IPC_MessageParcel_0700:run setSize success, result is " + setResult);
            expect(setResult == true).assertTrue();

            var addData = 1;
            var result = data.writeInt(addData);
            console.log("SUB_Softbus_IPC_MessageParcel_0700:run writeInt is success, result is " + result);
            expect(result == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0700: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0800
     * @tc.name    Call the setcapacity interface to set the capacity of messageparcel. 
     *             The write data capacity is inconsistent with the set value
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0800", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0800---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0800: create object successfully.");

            var size = 4;
            var setResult = data.setCapacity(size);
            console.log("SUB_Softbus_IPC_MessageParcel_0800:run setSize success, result is " + setResult);
            expect(setResult == true).assertTrue();

            var addData = [1, 2, 3, 4, 5, 6, 7, 8];
            var result = data.writeIntArray(addData);
            console.log("SUB_Softbus_IPC_MessageParcel_0800:run writeInt is success, result is " + result);
            expect(result == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0800: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_0900
     * @tc.name    Empty object to obtain the readable byte space, read location, 
     *             writable byte space and write location information of messageparcel
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_0900", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_0900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_0900: create object successfully.");

            var result1 = data.getWritableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_0900: run getWritableBytes is success, result is " + result1);
            expect(result1 == 0).assertTrue();

            var result2 = data.getReadableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_0900: run getReadableBytes is success, result is " + result2);
            expect(result2 == 0).assertTrue();

            var result3 = data.getReadPosition();
            console.log("SUB_Softbus_IPC_MessageParcel_0900: run getReadPosition is success, result is " + result2);
            expect(result3 == 0).assertTrue();

            var result4 = data.getWritePosition();
            console.log("SUB_Softbus_IPC_MessageParcel_0900: run getWritePosition is success, result is " + result2);
            expect(result4 == 0).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_0900: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_0900---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1000
     * @tc.name    Create an object and write data to obtain the readable byte space, read location, 
     *             writable byte space and write location information of messageparcel
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1000", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1000: create object successfully.");

            var dataInt = 1;
            var resultInt = data.writeInt(dataInt);
            console.log("SUB_Softbus_IPC_MessageParcel_1000: run writeInt success, result is " + resultInt);

            var dataLong = 2;
            var resultLong = data.writeLong(dataLong);
            console.log("SUB_Softbus_IPC_MessageParcel_1000: run writeLong success, result is " + resultLong);

            var result1 = data.getWritableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_1000: run getWritableBytes is success, result is " + result1);
            expect(result1 == 52).assertTrue();

            var result2 = data.getReadableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_1000: run getReadableBytes is success, result is " + result2);
            expect(result2 == 12).assertTrue();

            var result3 = data.getReadPosition();
            console.log("SUB_Softbus_IPC_MessageParcel_1000: run getReadPosition is success, result is " + result3);
            expect(result3 == 0).assertTrue();

            var result4 = data.getWritePosition();
            console.log("SUB_Softbus_IPC_MessageParcel_1000: run getWritePosition is success, result is " + result4);
            expect(result4 == 12).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1000: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1100
     * @tc.name    Call rewindread interface to offset the read position to the specified position
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1100", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            expect(data.getWritableBytes() == 0).assertTrue();
            expect(data.getReadableBytes() == 0).assertTrue();
            expect(data.getReadPosition() == 0).assertTrue();
            expect(data.getWritePosition() == 0).assertTrue();

            var dataInt = 1;
            var resultInt = data.writeInt(dataInt);
            console.log("SUB_Softbus_IPC_MessageParcel_1100: run writeInt success, result is " + resultInt);
            var dataLong = 2;
            var resultLong = data.writeLong(dataLong);
            console.log("SUB_Softbus_IPC_MessageParcel_1100: run writeLong success, result is " + resultLong);

            expect(data.getWritableBytes() == 52).assertTrue();
            expect(data.getReadableBytes() == 12).assertTrue();
            expect(data.getReadPosition() == 0).assertTrue();
            expect(data.getWritePosition() == 12).assertTrue();

            var readIntData = data.readInt();
            console.log("SUB_Softbus_IPC_MessageParcel_1100: run readInt is success, result is " + readIntData);
            expect(readIntData == dataInt).assertTrue();

            var writePosition = 0;
            var writeResult = data.rewindWrite(writePosition);
            console.log("SUB_Softbus_IPC_MessageParcel_1100: run rewindWrite is success, result is " + writeResult);
            expect(writeResult == true).assertTrue();

            expect(data.getWritePosition() == 0).assertTrue();
            dataInt = 3;
            resultInt = data.writeInt(dataInt);
            console.log("SUB_Softbus_IPC_MessageParcel_1100: run writeInt success, result is " + resultInt);

            var readPosition = 0;
            var readResult = data.rewindRead(readPosition);
            console.log("SUB_Softbus_IPC_MessageParcel_1100: run rewindWrite is success, result is " + readResult);
            expect(readResult == true).assertTrue();

            readIntData = data.readInt();
            console.log("SUB_Softbus_IPC_MessageParcel_1100: run readInt is success, result is " + readIntData);
            expect(readIntData == dataInt).assertTrue();
            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1100: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1200
     * @tc.name    The rewindread interface is called to re offset the read position to the specified position.
                   The specified position is out of range
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1200", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: create object successfully.");

            var result1 = data.getWritableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getWritableBytes success, result is " + result1);
            expect(result1 == 0).assertTrue();
            var result2 = data.getReadableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getReadableBytes success, result is " + result2);
            expect(result2 == 0).assertTrue();
            var result3 = data.getReadPosition();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getReadPosition success, result is " + result3);
            expect(result3 == 0).assertTrue();
            var result4 = data.getWritePosition();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getWritePosition success, result is " + result4);
            expect(result4 == 0).assertTrue();

            var dataInt = 1;
            var resultInt = data.writeInt(dataInt);
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run writeInt success, result is " + resultInt);
            expect(resultInt == true).assertTrue();
            var dataLong = 2;
            var resultLong = data.writeLong(dataLong);
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run writeLong success, result is " + resultLong);
            expect(resultLong == true).assertTrue();

            result1 = data.getWritableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getWritableBytes is success, result is " + result1);
            expect(result1 == 52).assertTrue();
            result2 = data.getReadableBytes();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getReadableBytes is success, result is " + result2);
            expect(result2 == 12).assertTrue();
            result3 = data.getReadPosition();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getReadPosition is success, result is " + result3);
            expect(result3 == 0).assertTrue();
            result4 = data.getWritePosition();
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run getWritePosition is success, result is " + result4);
            expect(result4 == 12).assertTrue();

            var readPosition = 100;
            var readResult = data.rewindRead(readPosition);
            console.log("SUB_Softbus_IPC_MessageParcel_1200: run rewindRead is success, result is " + readResult);
            expect(readResult == false).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1200: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1300
     * @tc.name    Call rewindwrite and the interface offsets the write position to the specified position
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1300", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1300: create object successfully.");

            var dataInt = 1;
            var resultInt = data.writeInt(dataInt);
            console.log("SUB_Softbus_IPC_MessageParcel_1300: run writeInt success, result is " + resultInt);
            expect(resultInt == true).assertTrue();

            var readIntData = data.readInt();
            console.log("SUB_Softbus_IPC_MessageParcel_1300: run readInt success, result is " + readIntData);
            expect(readIntData == dataInt).assertTrue();

            var writePosition = 0;
            var rewindWriteResult = data.rewindWrite(writePosition);
            console.log("SUB_Softbus_IPC_MessageParcel_1300: run rewindWrite success, result is " + rewindWriteResult);
            expect(rewindWriteResult == true).assertTrue();

            dataInt = 3;
            resultInt = data.writeInt(dataInt);
            console.log("SUB_Softbus_IPC_MessageParcel_1300: run writeInt success, result is " + resultInt);
            expect(resultInt == true).assertTrue();

            var readPosition = 0;
            var rewindReadResult = data.rewindRead(readPosition);
            console.log("SUB_Softbus_IPC_MessageParcel_1300: run rewindRead success, result is " + rewindReadResult);
            expect(rewindReadResult == true);

            readIntData = data.readInt();
            console.log("SUB_Softbus_IPC_MessageParcel_1300: run readInt success, result is " + readIntData);
            expect(readIntData == dataInt).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1300: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1400
     * @tc.name    Call rewindwrite and the interface offsets the write position to the specified position.
                   The specified position is out of range
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1400", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1400: create object successfully.");

            var dataInt = 1;
            var resultInt = data.writeInt(dataInt);
            console.log("SUB_Softbus_IPC_MessageParcel_1400: run writeInt success, result is " + resultInt);
            expect(resultInt == true).assertTrue();

            var readIntData = data.readInt();
            console.log("SUB_Softbus_IPC_MessageParcel_1400: run readInt success, result is " + readIntData);
            expect(readIntData == dataInt).assertTrue();

            var writePosition = 99;
            var rewindWriteResult = data.rewindWrite(writePosition);
            console.log("SUB_Softbus_IPC_MessageParcel_1400: run rewindWrite failed, result is " + rewindWriteResult);
            expect(rewindWriteResult == false).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1400: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1500
     * @tc.name    Call the writeshortarray interface, write the array to the messageparcel instance, 
     *             and call readshortarray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1500", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1500: create object successfully.");

            var wShortArryData = [3, 5, 9];
            var writeShortArrayResult = data.writeShortArray(wShortArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1500: run writeShortArray success, result is "
                         + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            var rShortArryData = data.readShortArray();
            console.log("SUB_Softbus_IPC_MessageParcel_1500: run readShortArray is success, result is "
                         + rShortArryData);
            expect(wShortArryData[0] == rShortArryData[0]).assertTrue();
            expect(wShortArryData[1] == rShortArryData[1]).assertTrue();
            expect(wShortArryData[2] == rShortArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1500: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1600
     * @tc.name    Call the writeshortarray interface, write the short integer array to the messageparcel instance, 
     *             and call readshortarray (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1600", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1600: create object successfully.");

            var wShortArryData = [3, 5, 9];
            var writeShortArrayResult = data.writeShortArray(wShortArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1600: run writeShortArray success, result is "
                         + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            var rShortArryData = [];
            data.readShortArray(rShortArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1600: run readShortArray is success, result is "
                         + rShortArryData);
            expect(wShortArryData[0] == rShortArryData[0]).assertTrue();
            expect(wShortArryData[1] == rShortArryData[1]).assertTrue();
            expect(wShortArryData[2] == rShortArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1600: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1700
     * @tc.name    Writeshortarray interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1700", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1700: create object successfully.");

            var wShortArryData = [-32768, 0, 1, 2, 32767];
            var writeShortArrayResult = data.writeShortArray(wShortArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1700: run writeShortArray success, result is "
                         + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            var rShortArryData = [];
            data.readShortArray(rShortArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1700: run readShortArray is success, result is "
                         + rShortArryData);
            expect(wShortArryData[0] == rShortArryData[0]).assertTrue();
            expect(wShortArryData[1] == rShortArryData[1]).assertTrue();
            expect(wShortArryData[2] == rShortArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1700: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1800
     * @tc.name    Writeshortarray interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1800", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1800---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1800: create object successfully.");

            var wShortArryData = [-32768, 0, 1, 2, 32767];
            var writeShortArrayResult = data.writeShortArray(wShortArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1800: run writeShortArray success, result is "
                         + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            var errorShortArryData = [-32769, 32768];
            var errorWriteShortArrayResult = data.writeShortArray(errorShortArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1800: run writeShortArray success, result is "
                         + errorWriteShortArrayResult);
            expect(errorWriteShortArrayResult == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1800: error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_1900
     * @tc.name    Call the writelongarray interface, write the long integer array to the messageparcel instance, 
     *             and call readlongarray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_1900", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_1900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_1900: create object successfully.");

            var wLongArryData = [3276826, 1234567, 99999999];
            var writeLongArrayResult = data.writeLongArray(wLongArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_1900: run writeShortArray success, result is "
                         + writeLongArrayResult);
            expect(writeLongArrayResult == true).assertTrue();

            var rLongArryData = data.readLongArray();
            console.log("SUB_Softbus_IPC_MessageParcel_1900: run readShortArray is success, result is "
                         + rLongArryData);
            expect(rLongArryData[0] == wLongArryData[0]).assertTrue();
            expect(rLongArryData[1] == wLongArryData[1]).assertTrue();
            expect(rLongArryData[2] == wLongArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_1900: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_1900---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2000
     * @tc.name    Call the writelongarray interface, write the long integer array to the messageparcel instance, 
     *             and call readlongarray (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2000", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2000: create object successfully.");

            var wLongArryData = [3276826, 1234567, 99999999];
            var writeLongArrayResult = data.writeLongArray(wLongArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2000: run writeShortArray success, result is "
                         + writeLongArrayResult);
            expect(writeLongArrayResult == true).assertTrue();

            var rLongArryData = [];
            data.readLongArray(rLongArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2000: run readShortArray is success, result is "
                         + rLongArryData);
            expect(rLongArryData[0] == wLongArryData[0]).assertTrue();
            expect(rLongArryData[1] == wLongArryData[1]).assertTrue();
            expect(rLongArryData[2] == wLongArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2000: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2100
     * @tc.name    Writelongarray interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2100", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2100: create object successfully.");

            var wLongArryData = [-2147483647, 0, 1, 2, 2147483647];
            var writeLongArrayResult = data.writeLongArray(wLongArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2100: run writeShortArray success, result is "
                         + writeLongArrayResult);
            expect(writeLongArrayResult == true).assertTrue();

            var rLongArryData = data.readLongArray();
            console.log("SUB_Softbus_IPC_MessageParcel_2100: run readShortArray is success, result is "
                         + rLongArryData);
            expect(rLongArryData[0] == wLongArryData[0]).assertTrue();
            expect(rLongArryData[1] == wLongArryData[1]).assertTrue();
            expect(rLongArryData[2] == wLongArryData[2]).assertTrue();
            expect(rLongArryData[3] == wLongArryData[3]).assertTrue();
            expect(rLongArryData[4] == wLongArryData[4]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2200
     * @tc.name    Writelongarray interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2200", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2200: create object successfully.");

            var errorLongArryData = [-2147483649, 0, 1, 2, 2147483649];
            var errorWriteLongArrayResult = data.writeLongArray(errorLongArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2200: run writeShortArray success, result is "
                         + errorWriteLongArrayResult);
            expect(errorWriteLongArrayResult == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2200: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2300
     * @tc.name    Call the writedoublearray interface, write the array to the messageparcel instance, 
     *             and call readdoublearra to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2300", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2300: create object successfully.");

            var wDoubleArryData = [1.2, 235.67, 99.76];
            var writeDoubleArrayResult = data.writeDoubleArray(wDoubleArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2300: run writeShortArray success, result is "
                         + writeDoubleArrayResult);
            expect(writeDoubleArrayResult == true).assertTrue();

            var rDoubleArryData = data.readDoubleArray();
            console.log("SUB_Softbus_IPC_MessageParcel_2300: run readShortArray is success, result is "
                         + rDoubleArryData);
            expect(rDoubleArryData[0] == wDoubleArryData[0]).assertTrue();
            expect(rDoubleArryData[1] == wDoubleArryData[1]).assertTrue();
            expect(rDoubleArryData[2] == wDoubleArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2300: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2400
     * @tc.name    Call the writedoublearray interface, write the array to the messageparcel instance, 
     *             and call readdoublearra (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2400", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2400: create object successfully.");

            var wDoubleArryData = [1.2, 235.67, 99.76];
            var writeDoubleArrayResult = data.writeDoubleArray(wDoubleArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2400: run writeShortArray success, result is "
                         + writeDoubleArrayResult);
            expect(writeDoubleArrayResult == true).assertTrue();

            var rDoubleArryData = [];
            data.readDoubleArray(rDoubleArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2400: run readShortArray is success, result is "
                         + rDoubleArryData);
            expect(rDoubleArryData[0] == wDoubleArryData[0]).assertTrue();
            expect(rDoubleArryData[1] == wDoubleArryData[1]).assertTrue();
            expect(rDoubleArryData[2] == wDoubleArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2400: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2500
     * @tc.name    Writedoublearray interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2500", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2500: create object successfully.");

            var wDoubleArryData = [-1235453.2, 235.67, 9987659.76];
            var writeDoubleArrayResult = data.writeDoubleArray(wDoubleArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2500: run writeShortArray success, result is "
                         + writeDoubleArrayResult);
            expect(writeDoubleArrayResult == true).assertTrue();

            var rDoubleArryData = data.readDoubleArray();
            console.log("SUB_Softbus_IPC_MessageParcel_2500: run readShortArray is success, result is "
                         + rDoubleArryData);
            expect(rDoubleArryData[0] == wDoubleArryData[0]).assertTrue();
            expect(rDoubleArryData[1] == wDoubleArryData[1]).assertTrue();
            expect(rDoubleArryData[2] == wDoubleArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2500: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2600
     * @tc.name    Writedoublearray interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2600", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2600: create object successfully.");

            var errorDoubleArryData = [-12354883737337373873853.2, 235.67, 99999999999999993737373773987659.76];
            var errorWriteDoubleArrayResult = data.writeDoubleArray(errorDoubleArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2600: run writeDoubleArray success, result is "
                         + errorWriteDoubleArrayResult);
            expect(errorWriteDoubleArrayResult == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2600: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2700
     * @tc.name    Call the writeboolean array interface, write the array to the messageparcel instance, 
     *             and call readboolean array to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2700", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2700: create object successfully.");

            var wBooleanArryData = [true, false, false];
            var writeBooleanArrayResult = data.writeBooleanArray(wBooleanArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2700: run writeShortArray success, result is "
                         + writeBooleanArrayResult);
            expect(writeBooleanArrayResult == true).assertTrue();

            var rBooleanArryData = data.readBooleanArray();
            console.log("SUB_Softbus_IPC_MessageParcel_2700: run readShortArray is success, result is "
                         + rBooleanArryData);
            expect(rBooleanArryData[0] == wBooleanArryData[0]).assertTrue();
            expect(rBooleanArryData[1] == wBooleanArryData[1]).assertTrue();
            expect(rBooleanArryData[2] == wBooleanArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2700: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2800
     * @tc.name    Call the writeboolean array interface, write the array to the messageparcel instance, 
     *             and call readboolean array (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2800", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2800---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2800: create object successfully.");

            var wBooleanArryData = [true, false, false];
            var writeBooleanArrayResult = data.writeBooleanArray(wBooleanArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2800: run writeShortArray success, result is "
                         + writeBooleanArrayResult);
            expect(writeBooleanArrayResult == true).assertTrue();

            var rBooleanArryData = [];
            data.readBooleanArray(rBooleanArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2800: run readShortArray is success, result is "
                         + rBooleanArryData);
            expect(rBooleanArryData[0] == wBooleanArryData[0]).assertTrue();
            expect(rBooleanArryData[1] == wBooleanArryData[1]).assertTrue();
            expect(rBooleanArryData[2] == wBooleanArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2800: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_2900
     * @tc.name    Writeboolean array interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_2900", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_2900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_2900: create object successfully.");

            var errorBooleanArryData = [true, 'abc', false];
            var errorWriteBooleanArrayResult = data.writeBooleanArray(errorBooleanArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_2900: run writeShortArray success, result is "
                         + errorWriteBooleanArrayResult);
            expect(errorWriteBooleanArrayResult == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_2900: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_2900---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3000
     * @tc.name    Call the writechararray interface, write the array to the messageparcel instance, 
     *             and call readchararray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3000", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3000: create object successfully.");

            var wCharArryData = ['e', 'u', 'a'];
            var writeCharArrayResult = data.writeCharArray(wCharArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3000: run writeShortArray success, result is "
                         + writeCharArrayResult);
            expect(writeCharArrayResult == true).assertTrue();

            var rCharArryData = data.readCharArray();
            console.log("SUB_Softbus_IPC_MessageParcel_3000: run readShortArray is success, result is "
                         + rCharArryData);
            expect(rCharArryData[0] == wCharArryData[0]).assertTrue();
            expect(rCharArryData[1] == wCharArryData[1]).assertTrue();
            expect(rCharArryData[2] == wCharArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3000: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3100
     * @tc.name    Call the writechararray interface, write the array to the messageparcel instance, 
     *             and call readchararray (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3100", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3100: create object successfully.");

            var wCharArryData = ['e', 'u', 'a'];
            var writeCharArrayResult = data.writeCharArray(wCharArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3100: run writeShortArray success, result is "
                         + writeCharArrayResult);
            expect(writeCharArrayResult == true).assertTrue();


            var rCharArryData = [];
            data.readCharArray(rCharArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3100: run readShortArray is success, result is "
                         + rCharArryData);
            expect(rCharArryData[0] == wCharArryData[0]).assertTrue();
            expect(rCharArryData[1] == wCharArryData[1]).assertTrue();
            expect(rCharArryData[2] == wCharArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3200
     * @tc.name    Writechararray interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3200", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3200: create object successfully.");

            var errorCharArryData = ['e', 'asfgdgdtu', 'a'];
            var errorWriteCharArrayResult = data.writeCharArray(errorCharArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3200: run writeShortArray success, result is "
                         + errorWriteCharArrayResult);
            expect(errorWriteCharArrayResult == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3200: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3300
     * @tc.name    Call the writestringarray interface, write the array to the messageparcel instance, 
     *             and call readstringarray (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3300", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3300: create object successfully.");

            var wStringArryData = ['abc', 'hello', 'beauty'];
            var writeStringArrayResult = data.writeStringArray(wStringArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3300: run writeShortArray success, result is "
                         + writeStringArrayResult);
            expect(writeStringArrayResult == true).assertTrue();

            var rStringArryData = data.readStringArray();
            console.log("SUB_Softbus_IPC_MessageParcel_3300: run readShortArray is success, result is "
                         + rStringArryData);
            expect(rStringArryData[0] == wStringArryData[0]).assertTrue();
            expect(rStringArryData[1] == wStringArryData[1]).assertTrue();
            expect(rStringArryData[2] == wStringArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3300: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3400
     * @tc.name    Call the writestringarray interface, write the array to the messageparcel instance, 
     *             and call readstringarray() to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3400", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3400: create object successfully.");

            var wStringArryData = ['abc', 'hello', 'beauty'];
            var writeStringArrayResult = data.writeStringArray(wStringArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3400: run writeShortArray success, result is "
                         + writeStringArrayResult);
            expect(writeStringArrayResult == true).assertTrue();


            var rStringArryData = [];
            reply.readStringArray(rStringArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3400: run readShortArray is success, result is "
                         + rStringArryData);
            expect(rStringArryData[0] == wStringArryData[0]).assertTrue();
            expect(rStringArryData[1] == wStringArryData[1]).assertTrue();
            expect(rStringArryData[2] == wStringArryData[2]).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3400: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3500
     * @tc.name    Writestringarray interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3500", 0, function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3500: create object successfully.");

            var errorStringArryData = ['abc' , '123' , 'beauty'];
            var errorWriteStringArrayResult = data.writeStringArray(errorStringArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_3500: run writeStringArray success, result is "
                         + errorWriteStringArrayResult);
            expect(errorWriteStringArrayResult == true).assertTrue();

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3500: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3500---------------------------");
    });

    console.log("-----------------------ActsRpcJsTest is end-----------------------");
});
