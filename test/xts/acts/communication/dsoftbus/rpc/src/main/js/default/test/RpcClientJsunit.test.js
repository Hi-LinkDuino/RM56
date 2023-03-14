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
import fileio from '@ohos.fileio';
import FA from '@ohos.ability.featureability'
import {describe, expect, beforeAll, it} from 'deccjsunit/index'

var gIRemoteObject = undefined;

describe('ActsRpcClientJsTest', function(){
    console.log("-----------------------SUB_Softbus_IPC_MessageParce_Test is starting-----------------------");

    const CODE_WRITE_BYTEARRAY = 1;
    const CODE_WRITE_INTARRAY = 2;
    const CODE_WRITE_FLOATARRAY = 3;
    const CODE_WRITE_SHORT = 4;
    const CODE_WRITE_LONG = 5;
    const CODE_WRITE_DOUBLE = 6;
    const CODE_WRITE_BOOLEAN = 7;
    const CODE_WRITE_CHAR = 8;
    const CODE_WRITE_STRING = 9;
    const CODE_WRITE_BYTE = 10;
    const CODE_WRITE_INT = 11;
    const CODE_WRITE_FLOAT = 12;
    const CODE_WRITE_RAWDATA = 13;
    const CODE_WRITE_REMOTEOBJECT = 14;
    const CODE_WRITE_SEQUENCEABLE = 15;
    const CODE_WRITE_NOEXCEPTION = 16;
    const CODE_WRITE_SEQUENCEABLEARRAY = 17;
    const CODE_WRITE_REMOTEOBJECTARRAY = 18;
    const CODE_ALL_TYPE = 20;
    const CODE_ALL_ARRAY_TYPE = 21;
    const CODE_WRITEINT8_ASHMEM = 22;
    const CODE_WRITESEQUENCEABLE = 23
    const CODE_WRITE_SHORT_MULTI = 24;
    const CODE_WRITE_BYTE_MULTI = 25;
    const CODE_WRITE_INT_MULTI = 26;
    const CODE_TRANSACTION = 27;
    const CODE_IPCSKELETON = 28;
    const CODE_FILESDIR = 29;
    const CODE_WRITE_REMOTEOBJECTARRAY_1 = 30;
    const CODE_WRITE_REMOTEOBJECTARRAY_2 = 31;

    function connectAbility() {
        let want = {
            "bundleName":"ohos.rpc.test.server",
            "abilityName": "ohos.rpc.test.server.ServiceAbility",
        };
        let connect = {
            onConnect:function (elementName, remoteProxy) {
                console.log('RpcClient: onConnect called, instance of proxy: '
                             + (remoteProxy instanceof rpc.RemoteProxy))
                gIRemoteObject = remoteProxy

            },
            onDisconnect:function (elementName) {
                console.log("RpcClient: onDisconnect")
            },
            onFailed:function () {
                console.log("RpcClient: onFailed")
                gIRemoteObject = null
            }
        };
        FA.connectAbility(want, connect)
        return new Promise((resolve, reject) =>{
            console.log("start connect local ability, wait 5 seconds")
            setTimeout(()=>{
                console.log("resolve proxy: " + gIRemoteObject)
                resolve(gIRemoteObject)
            }, 5000)
        })
    }

    function sleep(numberMillis)
    {
        var now = new Date();
        var exitTime = now.getTime() + numberMillis;
        while (true) {
            now = new Date();
            if (now.getTime() > exitTime)
            return;
        }
    }

    class TestRemoteObject extends rpc.RemoteObject {
        constructor(descriptor) {
            super(descriptor);
        }
    }

    class MyDeathRecipient {
        constructor(gIRemoteObject, done) {
            this.gIRemoteObject = gIRemoteObject
            this.done = done
        }

        onRemoteDied() {
            console.info("server died")
            expect(this.proxy.removeDeathRecipient(this, 0)).assertTrue()
            let _done = this.done
            setTimeout(function() {
                _done()
            }, 1000)
        }
    }

    class TestAbility extends rpc.RemoteObject {
        asObject() {
            return this;
        }
    }

    class TestAbilityStub extends rpc.RemoteObject {
        constructor(descriptor) {
            super(descriptor)
        }

        onRemoteRequest(code, data, reply, option) {
            console.info("TestAbilityStub: onRemoteRequest called, code: " + code)
            let descriptor = data.readInterfaceToken()
            if (descriptor !== "TestAbilityStub") {
                console.error("received unknown descriptor: " + descriptor)
                return false
            }
            switch (code) {
                case 1:
                {
                    let tmp1 = data.readByte()
                    let tmp2 = data.readByte()
                    let tmp3 = data.readShort()
                    let tmp4 = data.readShort()
                    let tmp5 = data.readInt()
                    let tmp6 = data.readInt()
                    let tmp7 = data.readLong()
                    let tmp8 = data.readLong()
                    let tmp9 = data.readFloat()
                    let tmp10 = data.readFloat()
                    let tmp11 = data.readDouble()
                    let tmp12 = data.readDouble()
                    let tmp13 = data.readBoolean()
                    let tmp14 = data.readBoolean()
                    let tmp15 = data.readChar()
                    let tmp16 = data.readString()
                    let s = new MySequenceable(null, null)
                    data.readSequenceable(s)
                    reply.writeNoException()
                    reply.writeByte(tmp1)
                    reply.writeByte(tmp2)
                    reply.writeShort(tmp3)
                    reply.writeShort(tmp4)
                    reply.writeInt(tmp5)
                    reply.writeInt(tmp6)
                    reply.writeLong(tmp7)
                    reply.writeLong(tmp8)
                    reply.writeFloat(tmp9)
                    reply.writeFloat(tmp10)
                    reply.writeDouble(tmp11)
                    reply.writeDouble(tmp12)
                    reply.writeBoolean(tmp13)
                    reply.writeBoolean(tmp14)
                    reply.writeChar(tmp15)
                    reply.writeString(tmp16)
                    reply.writeSequenceable(s)
                    return true
                }
                default:
                {
                    console.error("default case, code: " + code)
                    return false
                }
            }
        }
    }


    class TestListener extends rpc.RemoteObject {
        constructor(descriptor, checkResult) {
            super(descriptor);
            this.checkResult = checkResult
        }
        onRemoteRequest(code, data, reply, option) {
            let result = false
            if (code  == 1) {
                console.log("onRemoteRequest called, descriptor: " + this.getInterfaceDescriptor())
                result = true
            } else {
                console.log("unknown code: " + code)
            }
            let _checkResult = this.checkResult
            let _num = data.readInt()
            let _str = data.readString()
            setTimeout(function(){
                _checkResult(_num, _str)
            }, 2*1000);
            return result
        }
    }

    class MySequenceable {
        constructor(num, string) {
            this.num = num;
            this.str = string;
        }
        marshalling(messageParcel) {
            messageParcel.writeInt(this.num);
            messageParcel.writeString(this.str);
            return true;
        }
        unmarshalling(messageParcel) {
            this.num = messageParcel.readInt();
            this.str = messageParcel.readString();
            return true;
        }
    }

    class Stub extends rpc.RemoteObject {
        onRemoteRequest(code, data, reply, option) {
            let callerPid = rpc.IPCSkeleton.getCallingPid();
            console.log("RpcServer: getCallingPid result: " + callerPid);
            let callerUid = rpc.IPCSkeleton.getCallingUid();
            console.log("RpcServer: getCallingUid result: " + callerUid);
            let callerDeviceID  = rpc.IPCSkeleton.getCallingDeviceID();
            console.log("RpcServer: getCallingUid result: " + callerDeviceID );
            let localDeviceID = rpc.IPCSkeleton.getLocalDeviceID();
            console.log("RpcServer: localDeviceID is: " + localDeviceID);
            return true;
        }
    }

    function assertArrayElementEqual(actual, expected) {
        expect(actual.length).assertEqual(expected.length)
        for (let i = 0; i < actual.length; i++) {
            expect(actual[i]).assertEqual(expected[i])
        }
    }

    beforeAll(async function (done) {
        console.info('beforeAll called')
        await connectAbility().then((remote) => {
            console.log("got remote proxy: " + remote)
        }).catch((err) => {
            console.log("got exception: " + err)
        })
        done()
        console.log("beforeAll done")
    })

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3600
     * @tc.name    Call the writebytearray interface, write the array to the messageparcel instance,
     *             and call readbytearray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3600", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3600: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            let ByteArrayVar = new Int8Array([1, 2, 3, 4, 5]);
            var writeShortArrayResult = data.writeByteArray(ByteArrayVar);
            console.log("SUB_Softbus_IPC_MessageParcel_3600: run writeShortArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_3600: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BYTEARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_3600: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var shortArryDataReply = result.reply.readByteArray();
                console.log("SUB_Softbus_IPC_MessageParcel_3600: run readByteArray is success, result is "
                             + shortArryDataReply);
                expect(shortArryDataReply[0] == ByteArrayVar[0]).assertTrue();
                expect(shortArryDataReply[1] == ByteArrayVar[1]).assertTrue();
                expect(shortArryDataReply[2] == ByteArrayVar[2]).assertTrue();
                expect(shortArryDataReply[3] == ByteArrayVar[3]).assertTrue();
                expect(shortArryDataReply[4] == ByteArrayVar[4]).assertTrue();
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3600: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3700
     * @tc.name    Call the writebytearray interface, write the array to the messageparcel instance,
     *             and call readbytearray (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3700", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3700: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            let ByteArrayVar = new Int8Array([1, 2, 3, 4, 5]);
            var writeShortArrayResult = data.writeByteArray(ByteArrayVar);
            console.log("SUB_Softbus_IPC_MessageParcel_3700: run writeShortArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_3700: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BYTEARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_3700: sendRequest success, result is " + result.errCode);

                var newArr = new Int8Array(5)
                result.reply.readByteArray(newArr);
                console.log("SUB_Softbus_IPC_MessageParcel_3700: run readByteArray is success, result is "
                + newArr);
                expect(newArr[0] == ByteArrayVar[0]).assertTrue()
                expect(newArr[1] == ByteArrayVar[1]).assertTrue()
                expect(newArr[2] == ByteArrayVar[2]).assertTrue()
                expect(newArr[3] == ByteArrayVar[3]).assertTrue()
                expect(newArr[4] == ByteArrayVar[4]).assertTrue()
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3700: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3800
     * @tc.name    Writebytearray interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3800", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3800---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3800: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            let ByteArrayVar = new Int8Array([-128, 0, 1, 2, 127]);
            var writeShortArrayResult = data.writeByteArray(ByteArrayVar);
            console.log("SUB_Softbus_IPC_MessageParcel_3800: run writeShortArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_3800: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BYTEARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_3800: sendRequest success, result is " + result.errCode);

                var newArr = new Int8Array(5)
                result.reply.readByteArray(newArr);
                console.log("SUB_Softbus_IPC_MessageParcel_3800: run readByteArray is success, result is "
                + newArr);
                expect(newArr[0] == ByteArrayVar[0]).assertTrue()
                expect(newArr[1] == ByteArrayVar[1]).assertTrue()
                expect(newArr[2] == ByteArrayVar[2]).assertTrue()
                expect(newArr[3] == ByteArrayVar[3]).assertTrue()
                expect(newArr[4] == ByteArrayVar[4]).assertTrue()
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3800: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_3900
     * @tc.name    Writebytearray interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_3900", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_3900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_3900: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            let ByteArrayVar = new Int8Array([-129, 0, 1, 2, 127]);
            var writeShortArrayResult = data.writeByteArray(ByteArrayVar);
            console.log("SUB_Softbus_IPC_MessageParcel_3900: run writeShortArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_3900: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BYTEARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_3900: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var shortArryDataReply = result.reply.readByteArray();
                console.log("SUB_Softbus_IPC_MessageParcel_3900: run readByteArray is success, result is "
                + shortArryDataReply);
                expect(shortArryDataReply[0] == 127).assertTrue();
                expect(shortArryDataReply[1] == ByteArrayVar[1]).assertTrue();
                expect(shortArryDataReply[2] == ByteArrayVar[2]).assertTrue();
                expect(shortArryDataReply[3] == ByteArrayVar[3]).assertTrue();
                expect(shortArryDataReply[4] == ByteArrayVar[4]).assertTrue();
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_3600: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_3600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4000
     * @tc.name    Call the writeintarray interface, write the array to the messageparcel instance,
     *             and call readintarray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4000", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4000: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var intArryData = [100, 111, 112];
            var writeShortArrayResult = data.writeIntArray(intArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4000: run writeShortArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4000: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INTARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4000: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var shortArryDataReply = result.reply.readIntArray();
                console.log("SUB_Softbus_IPC_MessageParcel_4000: run readByteArray is success, result is "
                + shortArryDataReply);
                expect(shortArryDataReply[0] == intArryData[0]).assertTrue();
                expect(shortArryDataReply[1] == intArryData[1]).assertTrue();
                expect(shortArryDataReply[2] == intArryData[2]).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4000: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4100
     * @tc.name    Call the writeintarray interface, write the array to the messageparcel instance,
     *             and call readintarray (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4100", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4100: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var intArryData = [100, 111, 112];
            var writeShortArrayResult = data.writeIntArray(intArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4100: run writeShortArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4100: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INTARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4100: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var newArr = []
                result.reply.readIntArray(newArr);
                console.log("SUB_Softbus_IPC_MessageParcel_4100: run readIntArray is success, intArryDataReply is "
                + newArr);
                expect(newArr[0] == intArryData[0]).assertTrue();
                expect(newArr[1] == intArryData[1]).assertTrue();
                expect(newArr[2] == intArryData[2]).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4200
     * @tc.name    Writeintarray interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4200", 0, async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4200: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var intArryData = [-2147483648, 0, 1, 2, 2147483647];
            var writeIntArrayResult = data.writeIntArray(intArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4200: run writeShortArray success, result is "
            + writeIntArrayResult);
            expect(writeIntArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4200: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INTARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4200: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var shortArryDataReply = result.reply.readIntArray();
                console.log("SUB_Softbus_IPC_MessageParcel_4200: run readByteArray is success, result is "
                + shortArryDataReply);
                expect(shortArryDataReply[0] == intArryData[0]).assertTrue();
                expect(shortArryDataReply[1] == intArryData[1]).assertTrue();
                expect(shortArryDataReply[2] == intArryData[2]).assertTrue();
                expect(shortArryDataReply[3] == intArryData[3]).assertTrue();
                expect(shortArryDataReply[4] == intArryData[4]).assertTrue();
            });

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4200: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4300
     * @tc.name    Writeintarray interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4300", 0, async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4300: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var intArryData = [-2147483649, 0, 1, 2, 2147483647];
            var writeIntArrayResult = data.writeIntArray(intArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4300: run writeShortArray success, result is "
            + writeIntArrayResult);
            expect(writeIntArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4300: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INTARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4300: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var shortArryDataReply = result.reply.readIntArray();
                console.log("SUB_Softbus_IPC_MessageParcel_4300: run readByteArray is success, result is "
                + shortArryDataReply);
                expect(shortArryDataReply[0] == 2147483647).assertTrue();
                expect(shortArryDataReply[1] == intArryData[1]).assertTrue();
                expect(shortArryDataReply[2] == intArryData[2]).assertTrue();
                expect(shortArryDataReply[3] == intArryData[3]).assertTrue();
                expect(shortArryDataReply[4] == intArryData[4]).assertTrue();
            });

            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4300: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4400
     * @tc.name    Call the writefloatarray interface, write the array to the messageparcel instance,
     *             and call readfloatarray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4400", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4400: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var floatArryData = [1.2, 1.3, 1.4];
            var writeShortArrayResult = data.writeFloatArray(floatArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4400: run writeFloatArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();


            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4400: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_FLOATARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4400: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var floatArryDataReply = result.reply.readFloatArray();
                console.log("SUB_Softbus_IPC_MessageParcel_4400: run readFloatArray is success, floatArryDataReply is "
                + floatArryDataReply);
                expect(floatArryDataReply[0] == floatArryData[0]).assertTrue();
                expect(floatArryDataReply[1] == floatArryData[1]).assertTrue();
                expect(floatArryDataReply[2] == floatArryData[2]).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4400: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4500
     * @tc.name    Call the writefloatarray interface, write the array to the messageparcel instance,
     *             and call readfloatarray (datain: number []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4500", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4500: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var floatArryData = [1.2, 1.3, 1.4]
            var writeShortArrayResult = data.writeFloatArray(floatArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4500: run writeFloatArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4500: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_FLOATARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4500: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var newArr = []
                result.reply.readFloatArray(newArr);
                console.log("SUB_Softbus_IPC_MessageParcel_4500: readFloatArray is success, floatArryDataReply is "
                + newArr);
                expect(newArr[0] == floatArryData[0]).assertTrue();
                expect(newArr[1] == floatArryData[1]).assertTrue();
                expect(newArr[2] == floatArryData[2]).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4500: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4600
     * @tc.name    Writefloatarray interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4600", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4600: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var floatArryData = [-3.40E+38, 1.3, 3.40E+38];
            var writeShortArrayResult = data.writeFloatArray(floatArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4600: run writeFloatArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4600: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_FLOATARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4600: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var newArr = result.reply.readFloatArray();
                console.log("SUB_Softbus_IPC_MessageParcel_4600: run readFloatArray is success, floatArryDataReply is "
                + newArr);
                expect(newArr[0] == floatArryData[0]).assertTrue();
                expect(newArr[1] == floatArryData[1]).assertTrue();
                expect(newArr[2] == floatArryData[2]).assertTrue();
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4600: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4700
     * @tc.name    Writefloatarray interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4700", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4700: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var floatArryData = [-4.40E+38, 1.3, 3.40E+38];
            var writeShortArrayResult = data.writeFloatArray(floatArryData);
            console.log("SUB_Softbus_IPC_MessageParcel_4700: run writeFloatArray success, result is "
            + writeShortArrayResult);
            expect(writeShortArrayResult == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4700: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_FLOATARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4700: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var newArr = result.reply.readFloatArray();
                console.log("SUB_Softbus_IPC_MessageParcel_4700: run readFloatArray is success, floatArryDataReply is "
                + newArr);
                expect(newArr[0] == floatArryData[0]).assertTrue();
                expect(newArr[1] == floatArryData[1]).assertTrue();
                expect(newArr[2] == floatArryData[2]).assertTrue();
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4600: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4600---------------------------");
    });


    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4800
     * @tc.name    Call the writeShort interface to write the short integer data to the messageparcel instance,
     *             and call readshort to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4800", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4800---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4800: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var short = 8;
            var writeShor = data.writeShort(short);
            console.log("SUB_Softbus_IPC_MessageParcel_4800: run writeShort success, writeShor is " + writeShor);
            expect(writeShor == true).assertTrue();


            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4800: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SHORT, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4800: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var readShort = result.reply.readShort();
                console.log("SUB_Softbus_IPC_MessageParcel_4800: run readFloatArray is success, readShort is "
                + readShort);
                expect(readShort == short).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4800: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_4900
     * @tc.name    WriteShort interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_4900", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_4900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_4900: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            const CODE_WRITE_SHORT_MULTI = 24;
            expect(data.writeShort(-32768) == true).assertTrue();
            expect(data.writeShort(0) == true).assertTrue();
            expect(data.writeShort(1) == true).assertTrue();
            expect(data.writeShort(2) == true).assertTrue();
            expect(data.writeShort(32767) == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_4900: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SHORT_MULTI, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_4900: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                expect(result.reply.readShort() == -32768).assertTrue();
                expect(result.reply.readShort() == 0).assertTrue();
                expect(result.reply.readShort() == 1).assertTrue();
                expect(result.reply.readShort() == 2).assertTrue();
                expect(result.reply.readShort() == 32767).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_4800: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_4800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5000
     * @tc.name    WriteShort interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5000", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5000: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            expect(data.writeShort(-32769) == true).assertTrue();
            expect(data.writeShort(0) == true).assertTrue();
            expect(data.writeShort(1) == true).assertTrue();
            expect(data.writeShort(2) == true).assertTrue();
            expect(data.writeShort(32768) == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5000: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SHORT_MULTI, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_5000: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();

                expect(result.reply.readShort() == 32767).assertTrue();
                expect(result.reply.readShort() == 0).assertTrue();
                expect(result.reply.readShort() == 1).assertTrue();
                expect(result.reply.readShort() == 2).assertTrue();
                expect(result.reply.readShort() == -32768).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5000: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5100
     * @tc.name    Call writelong interface to write long integer data to messageparcel instance
     *             and call readlong to read data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5100", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5100: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var short = 10000;
            var writelong = data.writeLong(short);
            console.log("SUB_Softbus_IPC_MessageParcel_5100: run writeLong success, writelong is " + writelong);
            expect(writelong == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5100: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_LONG, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_5100: run sendRequest success, result is "
                             + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var readlong = result.reply.readLong();
                console.log("SUB_Softbus_IPC_MessageParcel_5100: run readLong is success, readlong is " + readlong);
                expect(readlong == short).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5200
     * @tc.name    Writelong interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5200", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5200: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var short = 2147483647;
            var writelong = data.writeLong(short);
            console.log("SUB_Softbus_IPC_MessageParcel_5200: run writeLong success, writelong is " + writelong);
            expect(writelong == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5200: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_LONG, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_5200: run sendRequest success, result is "
                + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var readlong = result.reply.readLong();
                console.log("SUB_Softbus_IPC_MessageParcel_5200: run readLong is success, readlong is " + readlong);
                expect(readlong == short).assertTrue();
            });

            data.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5200: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5300
     * @tc.name    Writelong interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5300", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5300: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var short = 214748364887;
            var writelong = data.writeLong(short);
            console.log("SUB_Softbus_IPC_MessageParcel_5300: run writeLong success, writelong is " + writelong);
            expect(writelong == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5300: gIRemoteObject undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_LONG, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_5300: run sendRequest success, result is "
                + result.errCode);
                expect(result.errCode == 0).assertTrue();

                var readlong = result.reply.readLong();
                console.log("SUB_Softbus_IPC_MessageParcel_5300: run readLong is success, readlong is " + readlong);
                expect(readlong == short).assertTrue();
            });

            data.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5300: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5300---------------------------");
    });


    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5400
     * @tc.name    Call the parallel interface to read and write data to the double instance
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5400", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5400: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            var token = 10.2;
            var result = data.writeDouble(token);
            console.log("SUB_Softbus_IPC_MessageParcel_5400:run writeDouble success, result is " + result);
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5400: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_DOUBLE, data, reply, option).then((result) => {
                    console.log("SUB_Softbus_IPC_MessageParcel_5400: run sendRequest success, result is "
                                 + result.errCode);
                    var replyReadResult = reply.readDouble();
                    console.log("SUB_Softbus_IPC_MessageParcel_5400: run replyReadResult is success," +
                    "replyReadResult is " + replyReadResult);
                    expect(replyReadResult == token).assertTrue();
                });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5400:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5500
     * @tc.name    Writedouble interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5500", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5500: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 1.79E+308;
            var result = data.writeDouble(token);
            console.log("SUB_Softbus_IPC_MessageParcel_5500:run writeDouble success, result is " + result);
            expect(result == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5400: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_DOUBLE, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_5400: run sendRequest success, result is "
                + result.errCode);
                var replyReadResult = reply.readDouble();
                console.log("SUB_Softbus_IPC_MessageParcel_5400: run replyReadResult is success," +
                "replyReadResult is " + replyReadResult);
                expect(replyReadResult == token).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5500:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5600
     * @tc.name    Writedouble interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5600", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5600: create object successfully.");

            var flag = false;
            var token = "1.79E+465312156";
            var result = data.writeDouble(token);
            console.log("SUB_Softbus_IPC_MessageParcel_5600:run writeDouble success, result is " + result);

            flag = true;
            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5600:error = " + error);
            expect(flag == false).assertTrue();
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5700
     * @tc.name    Call the writeboolean interface to write the data to the messageparcel instance,
     *             and call readboolean to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5700", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5700: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = true;
            var result = data.writeBoolean(token);
            console.log("SUB_Softbus_IPC_MessageParcel_5700:run writeBoolean success, result is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5700: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BOOLEAN, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_5700: run sendRequest success, result is "
                             + result.errCode);
                var replyReadResult = result.reply.readBoolean();
                console.log("SUB_Softbus_IPC_MessageParcel_5700: run readBoolean is success," +
                "result is " + replyReadResult);
                expect(replyReadResult == token).assertTrue();
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5700:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5800
     * @tc.name    Writeboolean interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5800", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5800---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5800: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 9;
            var result = data.writeBoolean(token);
            console.log("SUB_Softbus_IPC_MessageParcel_5800:run writeBoolean success, result is " + result);
            expect(result == false).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5800:error = " + error);
        }
        data.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_5900
     * @tc.name    Call the writechar interface to write the data to the messageparcel instance,
     *             and call readchar to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_5900", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_5900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_5900: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 'a';
            var result = data.writeChar(token);
            console.log("SUB_Softbus_IPC_MessageParcel_5900:run writeChar success, result is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_5900: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_CHAR, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_5900: sendRequest success, result is " + result.errCode);
                var replyReadResult = result.reply.readChar();
                console.log("SUB_Softbus_IPC_MessageParcel_5900: run readChar is success," +
                "result is " + replyReadResult);
                expect(replyReadResult == token).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_5900:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_5900---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6000
     * @tc.name    Writechar interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6000", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6000: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 'ades';
            var result = data.writeChar(token);
            console.log("SUB_Softbus_IPC_MessageParcel_6000:run writeChar success, result is " + result);
            expect(result == true).assertTrue()
            var readresult = data.readChar();
            expect(readresult == 'a').assertTrue()

        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6000:error = " + error);
        }
        data.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6100
     * @tc.name    Call the writestring interface to write the data to the messageparcel instance,
     *             and call readstring() to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6100", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6100: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 'weqea';
            var result = data.writeString(token);
            console.log("SUB_Softbus_IPC_MessageParcel_6100:run writeString success, result is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6100: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_STRING, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6100: sendRequest success, result is " + result.errCode);
                var replyReadResult = result.reply.readString();
                console.log("SUB_Softbus_IPC_MessageParcel_6100: run readString is success," +
                "result is " + replyReadResult);
                expect(replyReadResult == token).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6100:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6200
     * @tc.name    Writestring interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6200", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6200: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 123;
            var result = data.writeString(token);
            console.log("SUB_Softbus_IPC_MessageParcel_6200:run writeString success, result is " + result);
            expect(result == false).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6200:error = " + error);
        }
        sleep(2000)
        data.reclaim();
        reply.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6300
     * @tc.name    Call the writebyte interface to write data to the messageparcel instance,
     *             and call readbyte to read data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6300", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6300: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 2;
            var result = data.writeByte(token);
            console.log("SUB_Softbus_IPC_MessageParcel_6300:run writeByte success, result is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6300: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BYTE, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6300: sendRequest success, result is " + result.errCode);
                var replyReadResult = result.reply.readByte();
                console.log("SUB_Softbus_IPC_MessageParcel_6300: run readByte is success," +
                "result is " + replyReadResult);
                expect(replyReadResult == token).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6300:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6400
     * @tc.name    Writebyte interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6400", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6400: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            expect(data.writeByte(-128) == true).assertTrue();
            expect(data.writeByte(0) == true).assertTrue();
            expect(data.writeByte(1) == true).assertTrue();
            expect(data.writeByte(2) == true).assertTrue();
            expect(data.writeByte(127) == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6400: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BYTE_MULTI, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6400: sendRequest success, result is " + result.errCode);

                expect(reply.readByte() == -128).assertTrue();
                expect(reply.readByte() == 0).assertTrue();
                expect(reply.readByte() == 1).assertTrue();
                expect(reply.readByte() == 2).assertTrue();
                expect(reply.readByte() == 127).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6400:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6500
     * @tc.name    Writebyte interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6500", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6500: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            expect(data.writeByte(-129) == true).assertTrue();
            expect(data.writeByte(0) == true).assertTrue();
            expect(data.writeByte(1) == true).assertTrue();
            expect(data.writeByte(2) == true).assertTrue();
            expect(data.writeByte(128) == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6500: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_BYTE_MULTI, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6500: sendRequest success, result is " + result.errCode);

                expect(reply.readByte() == 127).assertTrue();
                expect(reply.readByte() == 0).assertTrue();
                expect(reply.readByte() == 1).assertTrue();
                expect(reply.readByte() == 2).assertTrue();
                expect(reply.readByte() == -128).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6400:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6600
     * @tc.name    Call the writeint interface to write the data to the messageparcel instance,
     *             and call readint to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6600", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6600: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 2;
            var result = data.writeInt(token);
            console.log("SUB_Softbus_IPC_MessageParcel_6600:run writeInt success, result is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6600: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INT, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6600: sendRequest success, result is " + result.errCode);
                var replyReadResult = result.reply.readInt();
                console.log("SUB_Softbus_IPC_MessageParcel_6600: run readInt is success," +
                "result is " + replyReadResult);
                expect(replyReadResult == token).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6600:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6700
     * @tc.name    Writeint interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6700", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6700: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            expect(data.writeInt(-2147483648) == true).assertTrue();
            expect(data.writeInt(0) == true).assertTrue();
            expect(data.writeInt(1) == true).assertTrue();
            expect(data.writeInt(2) == true).assertTrue();
            expect(data.writeInt(2147483647) == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6700: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INT_MULTI, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6700: sendRequest success, result is " + result.errCode);
                expect(result.reply.readInt() == -2147483648).assertTrue();
                expect(result.reply.readInt() == 0).assertTrue();
                expect(result.reply.readInt() == 1).assertTrue();
                expect(result.reply.readInt() == 2).assertTrue();
                expect(result.reply.readInt() == 2147483647).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6700:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6800
     * @tc.name    Writeint interface, illegal value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6800", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6800: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            expect(data.writeInt(-2147483649) == true).assertTrue();
            expect(data.writeInt(0) == true).assertTrue();
            expect(data.writeInt(1) == true).assertTrue();
            expect(data.writeInt(2) == true).assertTrue();
            expect(data.writeInt(2147483648) == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6800: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INT_MULTI, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6800: sendRequest success, result is " + result.errCode);
                expect(result.reply.readInt() == 2147483647).assertTrue();
                expect(result.reply.readInt() == 0).assertTrue();
                expect(result.reply.readInt() == 1).assertTrue();
                expect(result.reply.readInt() == 2).assertTrue();
                expect(result.reply.readInt() == -2147483648).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6800:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_6900
     * @tc.name    Call the writefloat interface to write data to the messageparcel instance,
     *             and call readfloat to read data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_6900", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_6900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_6900: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 2.2;
            var result = data.writeFloat(token);
            console.log("SUB_Softbus_IPC_MessageParcel_6900:run writeDouble success, result is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_6900: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_FLOAT, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_6900: sendRequest success, result is " + result.errCode);
                var replyReadResult = result.reply.readFloat();
                console.log("SUB_Softbus_IPC_MessageParcel_6900: run readFloat is success," +
                "result is " + replyReadResult);
                expect(replyReadResult == token).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_6900:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_6900---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7000
     * @tc.name    Writefloat interface, boundary value verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7000", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7000: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var token = 3.4E+38;
            var result = data.writeFloat(token);
            console.log("SUB_Softbus_IPC_MessageParcel_7000:run writeFloat success, result is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_7000: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_FLOAT, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_7000: sendRequest success, result is " + result.errCode);
                var newReadResult = result.reply.readFloat();
                console.log("SUB_Softbus_IPC_MessageParcel_7000: readFloat result is " + newReadResult);
                expect(newReadResult == token).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7000:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7100
     * @tc.name    Writefloat interface, illegal value validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7100", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7100: create object successfully.");

            var token = 'a';
            var result = data.writeFloat(token);
            console.log("SUB_Softbus_IPC_MessageParcel_7100:run writeFloat success, result is " + result);
            expect(result == false).assertTrue();
            data.reclaim();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7100:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7200
     * @tc.name    Test messageparcel to deliver rawdata data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7200", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7200: create object successfully.");

            var Capacity = data.getRawDataCapacity()
            console.log("SUB_Softbus_IPC_MessageParcel_7200:run Capacity success, Capacity is " + Capacity);

            var rawdata = new Int8Array([1, 2, 3])
            var result = data.writeRawData(rawdata, rawdata.length);
            console.log("SUB_Softbus_IPC_MessageParcel_7200:run writeRawData success, result is " + result);
            expect(result == true).assertTrue();
            var newReadResult = data.readRawData(rawdata.length)
            console.log("SUB_Softbus_IPC_MessageParcel_7200:run readRawData success, result is " + newReadResult);
            expect(newReadResult[0] == rawdata[0]).assertTrue();
            expect(newReadResult[1] == rawdata[1]).assertTrue();
            expect(newReadResult[2] == rawdata[2]).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7200:error = " + error);
        }
        data.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7300
     * @tc.name    Illegal value passed in from writerawdata interface
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7300", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7300: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var Capacity = data.getRawDataCapacity()
            console.log("SUB_Softbus_IPC_MessageParcel_7300:run Capacity success, result is " + Capacity);
            var token = new Int8Array([2,1,4,3,129]) ;
            var result = data.writeRawData(token, 149000000);
            console.log("SUB_Softbus_IPC_MessageParcel_7300:run writeRawData success, result is " + result);
            expect(result == false).assertTrue();
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7300:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7400
     * @tc.name    Call the writeremoteobject interface to serialize the remote object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7400", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7400: create object successfully.");

            let testRemoteObject = new TestRemoteObject("testObject");
            var result = data.writeRemoteObject(testRemoteObject);
            console.log("SUB_Softbus_IPC_MessageParcel_7400: result is " + result);
            expect(result == true).assertTrue();
            data.readRemoteObject()           
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7400:error = " + error);
        }
        data.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7500
     * @tc.name    Call the writeremoteobject interface to serialize the remote object and pass in the empty object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7500", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7500: create object successfully.");

            var token = {}
            var result = data.writeRemoteObject(token);
            console.log("SUB_Softbus_IPC_MessageParcel_7500: result is " + result);
            expect(result == false).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7500:error = " + error);
        }
        data.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7600
     * @tc.name    Call the writesequenceable interface to write the custom serialized
     *             object to the messageparcel instance
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7600", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7600---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7600: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            let sequenceable = new MySequenceable(1, "aaa");
            let result = data.writeSequenceable(sequenceable);
            console.log("SUB_Softbus_IPC_MessageParcel_7600: writeSequenceable is " + result);
            let ret = new MySequenceable(0, "");

            let result2 = data.readSequenceable(ret);
            console.log("SUB_Softbus_IPC_MessageParcel_7600: readSequenceable is " + result2);
            expect(result2 == true).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7600:error = " + error);
        }
        data.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7700
     * @tc.name    After the server finishes processing, write noexception first before writing the result,
     *             and the client calls readexception to judge whether the server is abnormal
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7700", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7700---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7700: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            data.writeNoException();
            console.log("SUB_Softbus_IPC_MessageParcel_7700: run writeNoException success");
            expect(data.writeInt(6) == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_7700: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_NOEXCEPTION, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_7700: sendRequest success, result is " + result.errCode);
                result.reply.readException()
                var replyData = result.reply.readInt();
                console.log("SUB_Softbus_IPC_MessageParcel_7700: readResult is " + replyData);
                expect(replyData == 6).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7700:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7800
     * @tc.name    If the data on the server is abnormal, the client calls readexception
     *             to judge whether the server is abnormal
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7800", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7800---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7800: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            data.writeNoException();
            console.log("SUB_Softbus_IPC_MessageParcel_7800: run writeNoException success");
            expect(data.writeInt(1232222223444) == true).assertTrue();

            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_7800: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_NOEXCEPTION, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_7800: sendRequest success, result is " + result.errCode);
                result.reply.readException()
                var replyData = result.reply.readInt();
                console.log("SUB_Softbus_IPC_MessageParcel_7800: readResult is " + replyData);
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7800:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_7900
     * @tc.name    Serializable object marshaling and unmarshalling test
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_7900", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_7900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_7900: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var sequenceable = new MySequenceable(1, "aaa");
            var result = data.writeSequenceable(sequenceable);
            console.log("SUB_Softbus_IPC_MessageParcel_7900: writeSequenceable is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_7900: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SEQUENCEABLE, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_7900: sendRequest success, result is " + result.errCode);
                var s = new MySequenceable(null,null)
                var resultReply = result.reply.readSequenceable(s);
                console.log("SUB_Softbus_IPC_MessageParcel_7900: run readSequenceable is success,result is "
                             + resultReply);
                expect(s.str == sequenceable.str).assertTrue();
                expect(s.num == sequenceable.num).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_7900:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_7900---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8000
     * @tc.name    Non serializable object marshaling test
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8000", 0, async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8000: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var sequenceable = new MySequenceable(1, "aaa");
            var result = data.writeSequenceable(sequenceable);
            console.log("SUB_Softbus_IPC_MessageParcel_8000: writeSequenceable is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_8000: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SEQUENCEABLE, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_8000: sendRequest success, result is " + result.errCode);
                var s = new MySequenceable(null,null)
                var replyReadResult = reply.readSequenceable(s);
                console.log("SUB_Softbus_IPC_MessageParcel_8000: run readSequenceable is success," +
                "result is " + replyReadResult);
                expect(replyReadResult == true).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8000:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8100
     * @tc.name    The server did not send a serializable object, and the client was ungrouped
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8100", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8100: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var sequenceable = 10;
            var result = data.writeInt(sequenceable);
            console.log("RpcClient: writeInt is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_8100: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_INT, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_8100: sendRequest success, result is " + result.errCode);
                var s = new MySequenceable(0,null)
                var replyReadResult = result.reply.readSequenceable(s);
                console.log("SUB_Softbus_IPC_MessageParcel_8100: run readSequenceable is success," +
                "result is " + replyReadResult);
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8100:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8200
     * @tc.name    Call the writesequenceable interface to write the custom serialized object to the
     *             messageparcel instance, and call readsequenceable to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8200", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8200: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var sequenceable = new MySequenceable(2, "abc");
            var result = data.writeSequenceable(sequenceable);
            console.log("RpcClient: writeSequenceable is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_8200: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SEQUENCEABLE, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_8200: sendRequest success, result is " + result.errCode);
                var s = new MySequenceable(null,null)
                var replyReadResult = result.reply.readSequenceable(s);
                console.log("SUB_Softbus_IPC_MessageParcel_8200: run readSequenceable is success," +
                "result is " + replyReadResult);
                expect(s.str == sequenceable.str).assertTrue();
                expect(s.num == sequenceable.num).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8200:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8300
     * @tc.name    Call the writesequenceablearray interface to write the custom serialized object to the
     *             messageparcel instance, and call readsequenceablearray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8300", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8300: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var sequenceable = [new MySequenceable(1, "aaa"),
                                new MySequenceable(2, "bbb"), new MySequenceable(3, "ccc")];
            var result = data.writeSequenceableArray(sequenceable);
            console.log("SUB_Softbus_IPC_MessageParcel_8300: writeSequenceableArray is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_8300: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SEQUENCEABLEARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_8300: sendRequest success, result is " + result.errCode);
                var s = [new MySequenceable(null, null), new MySequenceable(null, null),
                         new MySequenceable(null, null)];
                result.reply.readSequenceableArray(s);
                console.log("SUB_Softbus_IPC_MessageParcel_8300: run readSequenceableArray is success.");
                for (let i = 0; i < s.length; i++) {
                    expect(s[i].str).assertEqual(sequenceable[i].str)
                    expect(s[i].num).assertEqual(sequenceable[i].num)
                }
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8300:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8400
     * @tc.name    Call the writesequenceablearray interface to write the custom serialized object to the
     *             messageparcel instance, and call readsequenceablearray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8400", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8400---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8400: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            var sequenceable = [new MySequenceable(4, "abc"),
            new MySequenceable(5, "bcd"), new MySequenceable(6, "cef")];
            var result = data.writeSequenceableArray(sequenceable);
            console.log("SUB_Softbus_IPC_MessageParcel_8400: writeSequenceable is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_8400: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_SEQUENCEABLEARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_8400: sendRequest success, result is " + result.errCode);
                var s = [new MySequenceable(null, null),
                         new MySequenceable(null, null), new MySequenceable(null, null)]
                result.reply.readSequenceableArray(s);
                console.log("SUB_Softbus_IPC_MessageParcel_8400: run readSequenceableArray is success.");
                for (let i = 0; i < s.length; i++) {
                    expect(s[i].str).assertEqual(sequenceable[i].str)
                    expect(s[i].num).assertEqual(sequenceable[i].num)
                }
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8400:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8500
     * @tc.name    Call the writesequenceablearray interface to write the custom
     *             serialized object to the messageparcel instance
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8500", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8500---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8500: create object successfully.");
            var sequenceable = 1;
            var result = data.writeSequenceableArray(sequenceable);
            console.log("SUB_Softbus_IPC_MessageParcel_8500: writeSequenceable is " + result);
            expect(result == false).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8500:error = " + error);
        }
        data.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8600
     * @tc.name    Call the writeremoteobjectarray interface to write the object array to the messageparcel
     *             instance, and call readremoteobjectarray to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8600", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8600---------------------------");
        try{
            let count = 0
            function checkResult(num, str) {
                expect(num).assertEqual(123)
                expect(str).assertEqual("rpcListenerTest")
                count++;
                console.info("check result done, count: " + count)
                if (count == 3) {
                    done()
                }
            }
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8600: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            expect(data.writeInterfaceToken("rpcTestAbility")).assertTrue()
            var listeners = [new TestListener("rpcListener", checkResult),
                                    new TestListener("rpcListener2", checkResult),
                                    new TestListener("rpcListener3", checkResult)];
            var result = data.writeRemoteObjectArray(listeners);
            console.log("SUB_Softbus_IPC_MessageParcel_8600: writeRemoteObjectArray is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_8600: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_REMOTEOBJECTARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_8600: sendRequest success, result is " + result.errCode);
                expect(result.errCode).assertEqual(0);
                expect(result.code).assertEqual(CODE_WRITE_REMOTEOBJECTARRAY);
                expect(result.data).assertEqual(data);
                expect(result.reply).assertEqual(reply);
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8600:error = " + error);
        }

        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8700
     * @tc.name    Call the writeremoteobjectarray interface to write the object array to the messageparcel instance,
     *             and call readremoteobjectarray (objects: iremoteobject []) to read the data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8700", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8700---------------------------");
        try{
            let count = 0
            function checkResult(num, str) {
                expect(num).assertEqual(123)
                expect(str).assertEqual("rpcListenerTest")
                count++;
                console.info("check result done, count: " + count)
                if (count == 3) {
                    done()
                }
            }
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8700: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            expect(data.writeInterfaceToken("rpcTestAbility")).assertTrue()
            var listeners = [new TestListener("rpcListener", checkResult),
                             new TestListener("rpcListener2", checkResult),
                             new TestListener("rpcListener3", checkResult)];
            var result = data.writeRemoteObjectArray(listeners);
            console.log("RpcClient: writeRemoteObjectArray is " + result);
            expect(result == true).assertTrue();
            if (gIRemoteObject == undefined)
            {
                console.log("SUB_Softbus_IPC_MessageParcel_8700: gIRemoteObject is undefined");
            }
            await gIRemoteObject.sendRequest(CODE_WRITE_REMOTEOBJECTARRAY, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_MessageParcel_8700: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();
            });

            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8700:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8800
     * @tc.name    Test messageparcel delivery file descriptor object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8800", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8800---------------------------");
        let context = FA.getContext()
        await context.getFilesDir()
            .then(async function(path) {
                expect(path != null).assertTrue()
                let basePath = path;
                let filePath = basePath + "/test1.txt";
                let fd = fileio.openSync(filePath, 0o2| 0o100 | 0o2000, 0o666);
                expect(fd >= 0).assertTrue()
                let str = "HELLO RPC"
                let bytesWr = fileio.writeSync(fd, str);
                let option = new rpc.MessageOption()
                let data = rpc.MessageParcel.create()
                let reply = rpc.MessageParcel.create()
                let result = data.containFileDescriptors()
                let writeInt = data.writeInt(bytesWr)
                expect(writeInt == true).assertTrue()
                let writeFileDescriptor = data.writeFileDescriptor(fd)
                expect(writeFileDescriptor == true).assertTrue()
                let result1 = data.containFileDescriptors()
                expect(data.containFileDescriptors()).assertTrue()
                await gIRemoteObject.sendRequest(CODE_FILESDIR, data, reply, option)
                    .then(function(result) {
                        expect(result.errCode).assertEqual(0)
                        let buf = new ArrayBuffer(str.length * 2);
                        let bytesRd = fileio.readSync(fd, buf, {position:0,});
                        let fdResult = reply.readFileDescriptor()
                        let content = String.fromCharCode.apply(null, new Uint8Array(buf));
                        expect(content).assertEqual(str + str)
                        let dupFd = rpc.MessageParcel.dupFileDescriptor(fd);
                        let buf2 = new ArrayBuffer(str.length * 2);
                        let byteRd2 = fileio.readSync(dupFd, buf2, {position:0,});
                        let content2 = String.fromCharCode.apply(null, new Uint8Array(buf2));
                        console.log("dupFd bytes read: " + byteRd2 + ", content2: " + content2);
                        expect(content2).assertEqual(str + str)
                        rpc.MessageParcel.closeFileDescriptor(fd);
                        rpc.MessageParcel.closeFileDescriptor(dupFd);
                    })
                try {
                    console.info("after close fd, write again")
                    fileio.writeSync(fd, str)
                    expect(0).assertEqual(1)
                } catch(e) {
                    console.error("got exception: " + e)
                }
            })
        done()
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8800---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_8900
     * @tc.name    Test messageparcel to deliver the reply message received in promise across processes
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_8900", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_8900---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8900: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            expect(data.writeByte(2)).assertTrue()
            expect(data.writeShort(3)).assertTrue()
            expect(data.writeInt(4)).assertTrue()
            expect(data.writeLong(5)).assertTrue()
            expect(data.writeFloat(1.2)).assertTrue()
            expect(data.writeDouble(10.2)).assertTrue()
            expect(data.writeBoolean(true)).assertTrue()
            expect(data.writeChar('a')).assertTrue()
            expect(data.writeString("HelloWorld")).assertTrue()
            expect(data.writeSequenceable(new MySequenceable(1, "aaa"))).assertTrue()

            await gIRemoteObject.sendRequest(CODE_ALL_TYPE, data, reply, option).then((result) => {
                console.info("sendRequest done, error code: " + result.errCode)
                expect(result.errCode).assertEqual(0)
                expect(result.reply.readByte()).assertEqual(2)
                expect(result.reply.readShort()).assertEqual(3)
                expect(result.reply.readInt()).assertEqual(4)
                expect(result.reply.readLong()).assertEqual(5)
                expect(result.reply.readFloat()).assertEqual(1.2)
                expect(result.reply.readDouble()).assertEqual(10.2)
                expect(result.reply.readBoolean()).assertTrue()
                expect(result.reply.readChar()).assertEqual('a')
                expect(result.reply.readString()).assertEqual("HelloWorld")
                let s = new MySequenceable(null, null)
                expect(result.reply.readSequenceable(s)).assertTrue()
                expect(s.num).assertEqual(1)
                expect(s.str).assertEqual("aaa")
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_8900:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_8900---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_9000
     * @tc.name    Test the cross process delivery of messageparcel and receive the reply message
     *             in the callback function
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_9000", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_9000---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_8900: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            expect(data.writeByte(2)).assertTrue()
            expect(data.writeShort(3)).assertTrue()
            expect(data.writeInt(4)).assertTrue()
            expect(data.writeLong(5)).assertTrue()
            expect(data.writeFloat(1.2)).assertTrue()
            expect(data.writeDouble(10.2)).assertTrue()
            expect(data.writeBoolean(true)).assertTrue()
            expect(data.writeChar('a')).assertTrue()
            expect(data.writeString("HelloWorld")).assertTrue()
            expect(data.writeSequenceable(new MySequenceable(1, "aaa"))).assertTrue()

            gIRemoteObject.sendRequest(CODE_ALL_TYPE, data, reply, option,(err, result) => {
                console.info("sendRequest done, error code: " + result.errCode)
                expect(result.errCode).assertEqual(0)
                expect(result.reply.readByte()).assertEqual(2)
                expect(result.reply.readShort()).assertEqual(3)
                expect(result.reply.readInt()).assertEqual(4)
                expect(result.reply.readLong()).assertEqual(5)
                expect(result.reply.readFloat()).assertEqual(1.2)
                expect(result.reply.readDouble()).assertEqual(10.2)
                expect(result.reply.readBoolean()).assertTrue()
                expect(result.reply.readChar()).assertEqual('a')
                expect(result.reply.readString()).assertEqual("HelloWorld")
                let s = new MySequenceable(null, null)
                expect(result.reply.readSequenceable(s)).assertTrue()
                expect(s.num).assertEqual(1)
                expect(s.str).assertEqual("aaa")
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_9000:error = " + error);
        }
        sleep(2000)
        data.reclaim();
        reply.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_9000---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_9100
     * @tc.name    Test the cross process transmission of messageparcel.
     *             After receiving the reply message in promise, read various types of arrays in order
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_9100", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_9100---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_9100: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            expect(data.writeByteArray(new Int8Array([1, 2, 3]))).assertTrue();
            expect(data.writeShortArray([4, 5, 6])).assertTrue()
            expect(data.writeIntArray([7, 8, 9])).assertTrue()
            expect(data.writeLongArray([10, 11, 12])).assertTrue()
            expect(data.writeFloatArray([1.1, 1.2, 1.3])).assertTrue()
            expect(data.writeDoubleArray([2.1, 2.2, 2.3])).assertTrue()
            expect(data.writeBooleanArray([true, true, false])).assertTrue()
            expect(data.writeCharArray(['a', 'b', 'c'])).assertTrue()
            expect(data.writeStringArray(['abc', 'seggg'])).assertTrue()
            let a = [new MySequenceable(1, "aaa"), new MySequenceable(2, "bbb"), new MySequenceable(3, "ccc")]
            expect(data.writeSequenceableArray(a)).assertTrue()
            gIRemoteObject.sendRequest(CODE_ALL_ARRAY_TYPE, data, reply, option,(err, result) => {
                expect(result.errCode).assertEqual(0)
                assertArrayElementEqual(result.reply.readByteArray(), [1, 2, 3])
                assertArrayElementEqual(result.reply.readShortArray(), [4, 5, 6])
                assertArrayElementEqual(result.reply.readIntArray(), [7, 8, 9])
                assertArrayElementEqual(result.reply.readLongArray(), [10, 11, 12])
                assertArrayElementEqual(result.reply.readFloatArray(), [1.1, 1.2, 1.3])
                assertArrayElementEqual(result.reply.readDoubleArray(), [2.1, 2.2, 2.3])
                assertArrayElementEqual(result.reply.readBooleanArray(), [true, true, false])
                assertArrayElementEqual(result.reply.readCharArray(), ['a', 'b', 'c'])
                assertArrayElementEqual(result.reply.readStringArray(), ['abc', 'seggg'])
                let b = [new MySequenceable(null, null), new MySequenceable(null, null),
                    new MySequenceable(null, null)]
                result.reply.readSequenceableArray(b)
                for (let i = 0; i < b.length; i++) {
                    expect(b[i].str).assertEqual(a[i].str)
                    expect(b[i].num).assertEqual(a[i].num)
                }
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_9100:error = " + error);
        }
        sleep(2000)
        data.reclaim();
        reply.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_9100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_9200
     * @tc.name    Test messageparcel cross process delivery. After receiving the reply message in promise,
     *             the client constructs an empty array in sequence and reads the data from the reply message
     *             into the corresponding array
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageParcel_9200", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_9200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_MessageParcel_9200: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            expect(data.writeByteArray(new Int8Array([1, 2, 3]))).assertTrue();
            expect(data.writeShortArray([4, 5, 6])).assertTrue()
            expect(data.writeIntArray([7, 8, 9])).assertTrue()
            expect(data.writeLongArray([10, 11, 12])).assertTrue()
            expect(data.writeFloatArray([1.1, 1.2, 1.3])).assertTrue()
            expect(data.writeDoubleArray([2.1, 2.2, 2.3])).assertTrue()
            expect(data.writeBooleanArray([true, true, false])).assertTrue()
            expect(data.writeCharArray(['a', 'b', 'c'])).assertTrue()
            expect(data.writeStringArray(['abc', 'seggg'])).assertTrue()
            let a = [new MySequenceable(1, "aaa"), new MySequenceable(2, "bbb"), new MySequenceable(3, "ccc")]
            expect(data.writeSequenceableArray(a)).assertTrue()
            await gIRemoteObject.sendRequest(CODE_ALL_ARRAY_TYPE, data, reply, option).then((result) => {
                expect(result.errCode).assertEqual(0)
                assertArrayElementEqual(result.reply.readByteArray(), [1, 2, 3])
                assertArrayElementEqual(result.reply.readShortArray(), [4, 5, 6])
                assertArrayElementEqual(result.reply.readIntArray(), [7, 8, 9])
                assertArrayElementEqual(result.reply.readLongArray(), [10, 11, 12])
                assertArrayElementEqual(result.reply.readFloatArray(), [1.1, 1.2, 1.3])
                assertArrayElementEqual(result.reply.readDoubleArray(), [2.1, 2.2, 2.3])
                assertArrayElementEqual(result.reply.readBooleanArray(), [true, true, false])
                assertArrayElementEqual(result.reply.readCharArray(), ['a', 'b', 'c'])
                assertArrayElementEqual(result.reply.readStringArray(), ['abc', 'seggg'])
                let b = [new MySequenceable(null, null), new MySequenceable(null, null),
                new MySequenceable(null, null)]
                result.reply.readSequenceableArray(b)
                for (let i = 0; i < b.length; i++) {
                    expect(b[i].str).assertEqual(a[i].str)
                    expect(b[i].num).assertEqual(a[i].num)
                }
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_MessageParcel_9200:error = " + error);
        }
        sleep(2000)
        data.reclaim();
        reply.reclaim();
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_9200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_9300
     * @tc.name    Test messageparcel to pass an object of type iremoteobject across processes
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_Softbus_IPC_MessageParcel_9300', 0, async function(done) {
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_9300---------------------------");
        function checkResult(num, str) {
            expect(num).assertEqual(123)
            expect(str).assertEqual("rpcListenerTest")
            done()
        }
    try{
        let option = new rpc.MessageOption()
        let data = rpc.MessageParcel.create()
        let reply = rpc.MessageParcel.create()

        let listener = new TestListener("rpcListener", checkResult)
        let result = data.writeRemoteObject(listener)
        expect(result == true).assertTrue()
        console.info("SUB_Softbus_IPC_MessageParcel_9300 result is:" + result)
        expect(data.writeInt(123)).assertTrue()
        expect(data.writeString("rpcListenerTest")).assertTrue()
        await gIRemoteObject.sendRequest(CODE_WRITE_REMOTEOBJECT, data, reply, option)
            .then((result)=> {
                console.info("sendRequest done, error code: " + result.errCode)
                expect(result.errCode).assertEqual(0)
                result.reply.readException()
            })

                data.reclaim()
                reply.reclaim()
                console.log("test done")
    } catch(error) {
            console.log("SUB_Softbus_IPC_MessageParcel_9300: error = " + error);
        }
            console.log("---------------------end SUB_Softbus_IPC_MessageParcel_9300---------------------------");
    })
    

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_9400
     * @tc.name    Test messageparcel to pass an array of iremoteobject objects across processes
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_Softbus_IPC_MessageParcel_9400', 0, async function(done) {
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_9400---------------------------");
        
        let count = 0;
        function checkResult(num, str) {
            expect(num).assertEqual(123)
            expect(str).assertEqual("rpcListenerTest")
            count++
            console.info("check result done, count: " + count)
            if (count == 3) {
                done()
            }
        }

        try{
            let option = new rpc.MessageOption()
            let data = rpc.MessageParcel.create()
            let reply = rpc.MessageParcel.create()
            let listeners = [new TestListener("rpcListener", checkResult),
            new TestListener("rpcListener2", checkResult),
            new TestListener("rpcListener3", checkResult)]
            let result = data.writeRemoteObjectArray(listeners)
            expect(result == true).assertTrue()
            console.info("SUB_Softbus_IPC_MessageParcel_9400 result is:" + result)
            expect(data.writeInt(123)).assertTrue()
            expect(data.writeString("rpcListenerTest")).assertTrue()
            await gIRemoteObject.sendRequest(CODE_WRITE_REMOTEOBJECTARRAY_1, data, reply, option)
            .then((result)=> {
                console.info("sendRequest done, error code: " + result.errCode)
                expect(result.errCode).assertEqual(0)
                result.reply.readException()
            })

                data.reclaim()
                reply.reclaim()
                console.log("test done")
        } catch(error) {
            console.log("SUB_Softbus_IPC_MessageParcel_9400: error = " + error);
            }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_9400---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_MessageParcel_9500
     * @tc.name    Test messageparcel to pass the array of iremoteobject objects across processes. The server
     *             constructs an empty array in onremoterequest and reads it from messageparcel
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_Softbus_IPC_MessageParcel_9500', 0, async function(done) {
        console.log("---------------------start SUB_Softbus_IPC_MessageParcel_9500---------------------------");
        let count = 0;
        function checkResult(num, str) {
            expect(num).assertEqual(123)
            expect(str).assertEqual("rpcListenerTest")
            count++
            console.info("check result done, count: " + count)
            if (count == 3) {
                done()
            }
        }

        try{
            let option = new rpc.MessageOption()
            let data = rpc.MessageParcel.create()
            let reply = rpc.MessageParcel.create()
            let listeners = [new TestListener("rpcListener", checkResult),
            new TestListener("rpcListener2", checkResult),
            new TestListener("rpcListener3", checkResult)]
            let result = data.writeRemoteObjectArray(listeners)
            expect(result == true).assertTrue()
            data.readRemoteObjectArray()
            console.info("SUB_Softbus_IPC_MessageParcel_9500 result is:" + result)
            expect(data.writeInt(123)).assertTrue()
            expect(data.writeString("rpcListenerTest")).assertTrue()
            await gIRemoteObject.sendRequest(CODE_WRITE_REMOTEOBJECTARRAY_2, data, reply, option)
            .then((result)=> {
                console.info("sendRequest done, error code: " + result.errCode)
                expect(result.errCode).assertEqual(0)
                result.reply.readException()
            })

                data.reclaim()
                reply.reclaim()
                console.log("test done")
        } catch(error) {
            console.log("SUB_Softbus_IPC_MessageParcel_9500: error = " + error);
            }
        console.log("---------------------end SUB_Softbus_IPC_MessageParcel_9500---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_MessageOption_0100
     * @tc.name    Basic method of testing messageoption
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageOption_0100",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageOption_0100---------------------------");
        try{
            let option = new rpc.MessageOption();
            console.log("SUB_Softbus_IPC_MessageOption_0100: create object successfully.");

            let time = option.getWaitTime();
            console.log("SUB_Softbus_IPC_MessageOption_0100: run getWaitTime success, time is " + time);
            expect(time == rpc.MessageOption.TF_WAIT_TIME).assertTrue();

            let flog = option.getFlags();
            console.log("SUB_Softbus_IPC_MessageOption_0100: run getFlags success, flog is " + flog);
            expect(flog == rpc.MessageOption.TF_SYNC).assertTrue();

            option.setFlags(rpc.MessageOption.TF_AYNC)
            console.log("SUB_Softbus_IPC_MessageOption_0100: run setFlags success");

            let flog2 = option.getFlags();
            console.log("SUB_Softbus_IPC_MessageOption_0100: run getFlags success, flog2 is " + flog2);

            option.setWaitTime(16);

            let time2 = option.getWaitTime();
            console.log("SUB_Softbus_IPC_MessageOption_0100: run getWaitTime success, time is " + time2);
            expect(time2 == 16).assertTrue();
        }catch(error){
            console.log("SUB_Softbus_IPC_MessageOption_0100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageOption_0100---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_MessageOption_0200
     * @tc.name    Setflags interface outlier detection
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageOption_0200",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageOption_0200---------------------------");
        try{
            let option = new rpc.MessageOption();
            console.log("SUB_Softbus_IPC_MessageOption_0200: create object successfully.");

            let time = option.getWaitTime();
            console.log("SUB_Softbus_IPC_MessageOption_0200: run getWaitTime success, time is " + time);
            expect(time == rpc.MessageOption.TF_WAIT_TIME).assertTrue();

            let flog = option.getFlags();
            console.log("SUB_Softbus_IPC_MessageOption_0200: run getFlags success, flog is " + flog);
            expect(flog == rpc.MessageOption.TF_SYNC);

            option.setFlags(3);
        }catch(error){
            console.log("SUB_Softbus_IPC_MessageOption_0200: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageOption_0200---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_MessageOption_0300
     * @tc.name    Setflags interface outlier detection
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_MessageOption_0300",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_MessageOption_0300---------------------------");
        try{
           expect(rpc.MessageOption.TF_SYNC).assertEqual(0);

           expect(rpc.MessageOption.TF_ASYNC).assertEqual(1);

           expect(rpc.MessageOption.TF_WAIT_TIME).assertEqual(4);

           expect(rpc.MessageOption.TF_ACCEPT_FDS).assertEqual(0x10);

        }catch(error){
            console.log("SUB_Softbus_IPC_MessageOption_0300: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_MessageOption_0300---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0100
     * @tc.name    Exception parameter validation of the created anonymous shared memory object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0100",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0100---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", -1)
            console.log("SUB_Softbus_IPC_Ashmem_0100: ashmem " + ashmem);

            let ashmem2 = rpc.Ashmem.createAshmem(null, 1024)
            console.log("SUB_Softbus_IPC_Ashmem_0100: ashmem2 " + ashmem2);
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0100---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0200
     * @tc.name    Call the getashmemsize interface to get the size of the shared memory object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0200",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0200---------------------------");
        try{
            let mapSize = 4096;
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", mapSize)
            console.log("SUB_Softbus_IPC_Ashmem_0200: run  createAshmem success");

            let size = ashmem.getAshmemSize()
            console.log("SUB_Softbus_IPC_Ashmem_0200: run getAshmemSize success, size is " + size);
            expect(size == mapSize).assertTrue();

            ashmem.closeAshmem();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0200: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0200---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0300
     * @tc.name    Call the getashmemsize interface to get the size of the shared memory object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0300",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0300---------------------------");
        try{
            let mapSize = 4096;
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", mapSize)
            console.log("SUB_Softbus_IPC_Ashmem_0300: run  createAshmem success");

            let size = ashmem.getAshmemSize()
            console.log("SUB_Softbus_IPC_Ashmem_0300: run getAshmemSize success, size is " + size);
            expect(size == mapSize).assertTrue();

            ashmem.closeAshmem();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0300: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0300---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0400
     * @tc.name    Writeashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0400",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0400---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_0400: ashmem " + ashmem);

            ashmem.closeAshmem()

            var data = rpc.MessageParcel.create();
            let writeAshmem = data.writeAshmem(ashmem);
            console.log("SUB_Softbus_IPC_Ashmem_0400: run writeAshmem success, writeAshmem is " + writeAshmem);
            expect(writeAshmem == false).assertTrue();

            data.reclaim();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0400: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0400---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0500
     * @tc.name    Readfromashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0500",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0500---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_0500: ashmem " + ashmem);

            ashmem.unmapAshmem()
            console.log("SUB_Softbus_IPC_Ashmem_0500: run unmapAshmem success");

            let bytes = new Int8Array([1, 2, 3, 4, 5])

            let ret = ashmem.readFromAshmem(bytes.length, 0);
            console.log("SUB_Softbus_IPC_Ashmem_0500: run readFromAshmem result is " + ret);
            expect(ret==null).assertTrue();

            ashmem.closeAshmem();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0500: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0500---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0600
     * @tc.name    Mapashmem interface creates shared file mappings
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0600",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0600---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_0600: ashmem " + ashmem);

            let result = ashmem.mapAshmem(rpc.Ashmem.PROT_READ);
            console.log("SUB_Softbus_IPC_Ashmem_0600: run mapAshmem success, result is " + result);
            expect(result == true).assertTrue();

            ashmem.closeAshmem()
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0600: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0600---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0700
     * @tc.name    Mapashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0700",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0700---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_0700: ashmem " + ashmem);

            let result = ashmem.mapAshmem(999);
            console.log("SUB_Softbus_IPC_Ashmem_0700: run mapAshmem success, result is " + result);
            expect(result == false).assertTrue();

            ashmem.closeAshmem()
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0700: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0700---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0800
     * @tc.name    Mapreadandwriteashmem interface creates a shared file map with the protection level of read-write
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0800",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0800---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 1024)
            console.log("SUB_Softbus_IPC_Ashmem_0800: ashmem " + ashmem);

            let result = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_0800: run mapAshmem success, result is " + result);

            ashmem.closeAshmem()

        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0800: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0800---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_0900
     * @tc.name    Mapreadandwriteashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_0900",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_0900---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_0900: ashmem " + ashmem);

            let result = ashmem.mapAshmem(rpc.Ashmem.PROT_READ);
            console.log("SUB_Softbus_IPC_Ashmem_0900: run mapAshmem success, result is " + result);
            expect(result == true).assertTrue();

            ashmem.unmapAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_0900: run unmapAshmem success");
            expect(ashmem.mapReadAndWriteAshmem()).assertFalse();
            let result2 = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_0900: run mapReadAndWriteAshmem success, result2 is " + result2);
            expect(result2 == false).assertTrue();

            ashmem.closeAshmem()
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_0900: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_0900---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1000
     * @tc.name    Mapreadonlyashmem interface creates a shared file map with the protection level of read-write
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1000",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1000---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_1000: ashmem " + ashmem);

            let result = ashmem.mapReadOnlyAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1000: run mapReadAndWriteAshmem success, result is " + result);
            expect(result == true).assertTrue();

            ashmem.closeAshmem();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1000: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1000---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1100
     * @tc.name    Mapreadonlyashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1100",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1100---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 1024)
            console.log("SUB_Softbus_IPC_Ashmem_1100: ashmem " + ashmem);

            let result = ashmem.mapAshmem(rpc.Ashmem.PROT_WRITE);
            console.log("SUB_Softbus_IPC_Ashmem_1100: run mapAshmem success, result is " + result);
            expect(result == true).assertTrue();

            ashmem.unmapAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1100: run unmapAshmem success");
            ashmem.closeAshmem()
            let result2 = ashmem.mapReadOnlyAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1100: run mapReadAndWriteAshmem success, result2 is " + result2);
            expect(result2 == false).assertTrue();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1100---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1200
     * @tc.name    Mapreadonlyashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1200",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1200---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 1024);

            let resultwrite = ashmem.setProtection(rpc.Ashmem.PROT_WRITE)
            console.log("SUB_Softbus_IPC_Ashmem_1200: run setProtection success, resultwrite is " + resultwrite);
            expect(resultwrite == true).assertTrue();

            let resultread = ashmem.setProtection(rpc.Ashmem.PROT_READ)
            console.log("SUB_Softbus_IPC_Ashmem_1200: run setProtection success, resultread is " + resultread);
            expect(resultread == false).assertTrue();

            let resultreadAndwrite = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1200: run setProtection success, mapReadAndWriteAshmem is "
                         + resultreadAndwrite);
            expect(resultreadAndwrite == false).assertTrue();

            let resultnone = ashmem.setProtection(rpc.Ashmem.PROT_NONE)
            console.log("SUB_Softbus_IPC_Ashmem_1200: run setProtection success, resultnone is " + resultnone);
            expect(resultnone == true).assertTrue();

            let resultread2 = ashmem.setProtection(rpc.Ashmem.PROT_READ)
            console.log("SUB_Softbus_IPC_Ashmem_1200: run setProtection success, resultread2 is " + resultread2);
            expect(resultread2 == false).assertTrue();

            ashmem.closeAshmem()
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1200: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1200---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1300
     * @tc.name    Setprotection exception input parameter verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1300",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1300---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 1024);
            console.log("SUB_Softbus_IPC_Ashmem_1300: ashmem " + ashmem);

            let result = ashmem.setProtection(3);
            console.log("SUB_Softbus_IPC_Ashmem_1300: run setProtection success, result is " + result);
            expect(result == true).assertTrue();

            ashmem.closeAshmem()
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1300: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1300---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1400
     * @tc.name    The writetoashmem interface writes the shared file associated with the object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1400",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1200---------------------------");
        try{
            let mapSize = 4096
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", mapSize)
            console.log("SUB_Softbus_IPC_Ashmem_1400: ashmem " + ashmem);

            let resultMapRAndW = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1400: run mapReadAndWriteAshmem success, result2 is "
                         + resultMapRAndW);
            expect(resultMapRAndW == true).assertTrue();

            let bytes = new Int8Array([1, 2, 3, 4, 5]);

            let result = ashmem.writeToAshmem(bytes, bytes.length, 0);
            console.log("SUB_Softbus_IPC_Ashmem_1400: run writeToAshmem success, result is " + result);
            expect(result == true).assertTrue();

            ashmem.closeAshmem();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1400: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1400---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1500
     * @tc.name    The writetoashmem interface writes the shared file associated with the object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1500",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1500---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_1500: ashmem " + ashmem);

            let resultMapRAndW = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1500: run mapReadAndWriteAshmem success, result2 is " 
                         + resultMapRAndW);
            expect(resultMapRAndW == true).assertTrue();

            let bytes = new Int8Array([1, 2, 3, 4, 5]);
            let result = ashmem.writeToAshmem(bytes, bytes.length, 0);
            console.log("SUB_Softbus_IPC_Ashmem_1500: run writeToAshmem success, result is " +result);
            expect(result == true).assertTrue();

            let resultread = ashmem.setProtection(rpc.Ashmem.PROT_READ);
            console.log("SUB_Softbus_IPC_Ashmem_1500: run setProtection success, resultread is " + resultread);
            expect(resultread == true).assertTrue()

            let result2 = ashmem.writeToAshmem(bytes, bytes.length, 0);
            console.log("SUB_Softbus_IPC_Ashmem_1500: run writeToAshmem success, result is2 " + result2);
            expect(result2 == false).assertTrue()

            ashmem.closeAshmem();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1500: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1500---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1600
     * @tc.name    Writetoashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1600",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1600---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096);
            console.log("SUB_Softbus_IPC_Ashmem_1600: ashmem " + ashmem);

            let resultMapRAndW = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1600: run mapReadAndWriteAshmem success, result2 is " 
                         + resultMapRAndW);
            expect(resultMapRAndW == true).assertTrue();

            let bytes = new Int8Array([1, 2, 3, 4, 5]);
            let size = bytes.length + 10;
            let result = ashmem.writeToAshmem(bytes, 3, 0);
            console.log("SUB_Softbus_IPC_Ashmem_1600: run writeToAshmem success, result is " + result);
            expect(result == true).assertTrue();

            ashmem.closeAshmem()

        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1600: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1600---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1700
     * @tc.name    Read data from the shared file associated with readfromashmem
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1700",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1700---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_1700: ashmem " + ashmem);

            let resultMapRAndW = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1600: run mapReadAndWriteAshmem success, result2 is " 
                         + resultMapRAndW);
            expect(resultMapRAndW == true).assertTrue();

            let bytes = new Int8Array([1, 2, 3, 4, 5]);
            let result = ashmem.writeToAshmem(bytes, bytes.length, 0);
            console.log("SUB_Softbus_IPC_Ashmem_1700: run writeToAshmem success, result is " + result);
            expect(result == true).assertTrue();

            var resultRead = ashmem.readFromAshmem(bytes.length, 0);
            console.log("SUB_Softbus_IPC_Ashmem_1700: run readFromAshmem success, result is " + resultRead);
            expect(resultRead[0] == bytes[0]).assertTrue();
            expect(resultRead[1] == bytes[1]).assertTrue();
            expect(resultRead[2] == bytes[2]).assertTrue();
            expect(resultRead[3] == bytes[3]).assertTrue();
            expect(resultRead[4] == bytes[4]).assertTrue();

            ashmem.closeAshmem()

        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1700: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1700---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1800
     * @tc.name    Readfromashmem exception validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1800",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1800---------------------------");
        try{

            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096);
            console.log("SUB_Softbus_IPC_Ashmem_1800: ashmem " + ashmem);

            let resultMapRAndW = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1800: run mapReadAndWriteAshmem success, result2 is " 
                         + resultMapRAndW);
            expect(resultMapRAndW == true).assertTrue();

            let bytes = new Int8Array([1, 2, 3, 4, 5]);
            let result = ashmem.writeToAshmem(bytes, bytes.length, 1);
            console.log("SUB_Softbus_IPC_Ashmem_1800: run writeToAshmem success, result is " + result);
            expect(result == true).assertTrue()

            let result2 = ashmem.readFromAshmem(bytes.length, 3);
            console.log("SUB_Softbus_IPC_Ashmem_1800: run readFromAshmem success, result2 is " + result2);
            expect(bytes[2] == result2[0]).assertTrue();
            expect(bytes[3] == result2[1]).assertTrue();
            expect(bytes[4] == result2[2]).assertTrue();

            ashmem.closeAshmem()

        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1800: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1800---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_1900
     * @tc.name    Createashmemfromexisting copies the ashmem object description and creates a new object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_1900",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_1900---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096)
            console.log("SUB_Softbus_IPC_Ashmem_1900: ashmem " + ashmem);

            let resultWriteAndRead = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1900:  run mapReadAndWriteAshmem result is " + resultWriteAndRead);
            expect(resultWriteAndRead == true).assertTrue();

            let bytes = new Int8Array([1, 2, 3]);
            let result = ashmem.writeToAshmem(bytes, bytes.length, 1);
            console.log("SUB_Softbus_IPC_Ashmem_1900: run writeToAshmem success, result is " + result);
            expect(result == true).assertTrue()

            let newashmem = rpc.Ashmem.createAshmemFromExisting(ashmem);
            let resultWriteAndRead2 = newashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_1900:  run mapReadAndWriteAshmem result is " + resultWriteAndRead2);
            expect(resultWriteAndRead2 == true).assertTrue();

            let result2 = newashmem.readFromAshmem(bytes.length, 1);
            console.log("SUB_Softbus_IPC_Ashmem_1900: run readFromAshmem success, result2 is " + result2);
            expect(result == true).assertTrue()
            expect(result2[0] == bytes[0]).assertTrue()
            expect(result2[1] == bytes[1]).assertTrue()
            expect(result2[2] == bytes[2]).assertTrue()

            ashmem.closeAshmem();
            newashmem.closeAshmem();
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_1900: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_1900---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_2000
     * @tc.name    Create a shared memory object and call writeashmem to write the shared anonymous
      object into the messageparcel object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_2000",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_2000---------------------------");
        try{
            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 1024);
            let data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_Ashmem_2000: ashmem " + ashmem);

            let resultMapRAndW = ashmem.mapReadAndWriteAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_2000: run mapReadAndWriteAshmem result is " + resultMapRAndW);
            expect(resultMapRAndW == true).assertTrue();

            let bytes = new Int8Array([1, 2, 3]);
            let result = ashmem.writeToAshmem(bytes, bytes.length, 1);

            console.log("SUB_Softbus_IPC_Ashmem_2000: run writeToAshmem success, result is " + result);
            expect(result == true).assertTrue()

            let result2 = data.writeAshmem(ashmem)
            console.log("SUB_Softbus_IPC_Ashmem_2000: run writeAshmem success, result is " + result2);
            expect(result2 == true).assertTrue();

            let retReadAshmem = data.readAshmem();
            console.log("SUB_Softbus_IPC_Ashmem_2000: run readAshmem is " + retReadAshmem);

            let retBytes = retReadAshmem.readFromAshmem(bytes.length, 1);
            console.log("SUB_Softbus_IPC_Ashmem_2000: run readFromAshmem result is " + retBytes);
            for (let i = 0; i < bytes.length; i++) {
                expect(retBytes[i]).assertEqual(bytes[i])
            }

            ashmem.closeAshmem()
            data.reclaim()
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_2000: error " +error);
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_2000---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_2100
     * @tc.name    Create a non shared memory object and call writeashmem to write the messageparcel object
      object into the messageparcel object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_Ashmem_2100",0,function(){
        console.log("---------------------start SUB_Softbus_IPC_Ashmem_2100---------------------------");
        try{
            let data = rpc.MessageParcel.create()
            let data2 = rpc.MessageParcel.create()
            console.log("SUB_Softbus_IPC_Ashmem_2100: create MessageParcel object success");

            var flag = false;
            let result = data.writeAshmem(data2)
            console.log("SUB_Softbus_IPC_Ashmem_2100: run writeAshmem success, result is " + result);

            flag = true;
            data.reclaim()
            data2.reclaim()
        }catch(error){
            console.log("SUB_Softbus_IPC_Ashmem_2100: error " + error);
            expect(flag == false).assertTrue();
        }
        console.log("---------------------end SUB_Softbus_IPC_Ashmem_2100---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_2200
     * @tc.name    Test messageparcel to pass ashmem object
     * @tc.desc    Function test
     * @tc.level   0
     */
//    it("SUB_Softbus_IPC_Ashmem_2200",0,async function(done){
//        console.log("---------------------start SUB_Softbus_IPC_Ashmem_2200---------------------------");
//        try{
//            let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 4096);
//            let data = rpc.MessageParcel.create()
//            var reply = rpc.MessageParcel.create()
//            let option = new rpc.MessageOption()
//            console.log("SUB_Softbus_IPC_Ashmem_2200: ashmem " + ashmem);
//
//            let resultMapRAndW = ashmem.mapReadAndWriteAshmem();
//            console.log("SUB_Softbus_IPC_Ashmem_2200: run mapReadAndWriteAshmem result is " + resultMapRAndW);
//            expect(resultMapRAndW == true).assertTrue();
//
//            let bytes = new Int8Array([1, 2, 3]);
//            let result = ashmem.writeToAshmem(bytes, bytes.length, 0);
//            console.log("SUB_Softbus_IPC_Ashmem_2200: run writeToAshmem success, result is " + result);
//
//            let resultMessage = data.writeAshmem(ashmem);
//            console.log("SUB_Softbus_IPC_Ashmem_2200: run writeAshmem success, resultMessage is " + resultMessage);
//
//            ashmem.unmapAshmem();
//            ashmem.closeAshmem();
//            await gIRemoteObject.sendRequest(CODE_WRITEINT8_ASHMEM, data, reply, option).then((result) => {
//                console.log("SUB_Softbus_IPC_Ashmem_2200: sendRequest success, result is " + result.errCode);
//                expect(result.errCode == 0).assertTrue();
//                result.reply.readException();
//                expect(result.reply.readInt()).assertEqual(bytes.length);
//                console.log("SUB_Softbus_IPC_Ashmem_2200:1111111111111111");
//                var replyAshmem = result.reply.readAshmem();
//                console.log("SUB_Softbus_IPC_Ashmem_2200:22222222222222222222222");
//
//                expect(replyAshmem.mapReadOnlyAshmem()).assertTrue()
//                console.log("SUB_Softbus_IPC_Ashmem_2200:33333333333333333333333333");
//
//                var replyByte = replyAshmem.readFromAshmem(bytes.length, 0);
//                console.log("SUB_Softbus_IPC_Ashmem_2200: run readByteArray is success, result is "
//                             + replyByte);
//                for (let i = 0; i < replyByte.length; i++) {
//                    expect(replyByte[i]).assertEqual(bytes[i]);
//                }
//                replyAshmem.unmapAshmem();
//                replyAshmem.closeAshmem();
//            });
//            data.reclaim();
//            reply.reclaim();
//            done();
//        }catch(error){
//            console.log("SUB_Softbus_IPC_Ashmem_2200: error " + error);
//        }
//        console.log("---------------------end SUB_Softbus_IPC_Ashmem_2200---------------------------");
//    })

 /*
     * @tc.number  SUB_Softbus_IPC_Ashmem_2300
     * @tc.name    Test the mapped memory is executable
     * @tc.desc    Function test
     * @tc.level   0
     */
 it("SUB_Softbus_IPC_Ashmem_2300",0,function(){
    console.log("---------------------start SUB_Softbus_IPC_Ashmem_2300---------------------------");
    try{
        let ashmem = rpc.Ashmem.createAshmem("JsAshmemTest", 1024);

        let resultwrite = ashmem.setProtection(rpc.Ashmem.PROT_EXEC)
        console.log("SUB_Softbus_IPC_Ashmem_2300: run setProtection success, resultwrite is " + resultwrite);
        expect(resultwrite == true).assertTrue();

        ashmem.closeAshmem()
    }catch(error){
        console.log("SUB_Softbus_IPC_Ashmem_2300: error " + error);
    }
    console.log("---------------------end SUB_Softbus_IPC_Ashmem_2300---------------------------");
})

    /*
     * @tc.number  SUB_Softbus_IPC_IRemoteObject_0100
     * @tc.name    Call sendrequestresult interface to send data
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IRemoteObject_0100",0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_IRemoteObject_0100---------------------------");
        try{
            let data = rpc.MessageParcel.create();
            let reply = rpc.MessageParcel.create();
            let option = new rpc.MessageOption();
            let sequenceable = new MySequenceable(1, "aaa");
            let result = data.writeSequenceable(sequenceable);
            console.log("SUB_Softbus_IPC_IRemoteObject_0100: run writeSequenceable success, result is " + result);

            await gIRemoteObject.sendRequest(CODE_WRITESEQUENCEABLE, data, reply, option).then((result) => {
                console.log("SUB_Softbus_IPC_IRemoteObject_0100: sendRequest success, result is " + result.errCode);
                expect(result.errCode == 0).assertTrue();
                let ret = new MySequenceable(0, "");
                var shortArryDataReply = result.reply.readSequenceable(ret);
                console.log("SUB_Softbus_IPC_IRemoteObject_0100: run readSequenceable is success, result is "
                             + shortArryDataReply);
                expect(shortArryDataReply == true).assertTrue()
                expect(ret.num).assertEqual(1)
                expect(ret.str).assertEqual("aaa")
            });

            data.reclaim();
            reply.reclaim();
            done();
        }catch(error){
            console.log("SUB_Softbus_IPC_IRemoteObject_0100: error " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IRemoteObject_0100---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_IRemoteObject_0200
     * @tc.name    Test that messageparcel passes through the same process, and the client
     *             receives the reply message in promise
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IRemoteObject_0200", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_IRemoteObject_0200---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_IRemoteObject_0200: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();
            expect(data.writeByte(1)).assertTrue()
            expect(data.writeShort(2)).assertTrue()
            expect(data.writeInt(3)).assertTrue()
            expect(data.writeLong(10000)).assertTrue()
            expect(data.writeFloat(1.2)).assertTrue()
            expect(data.writeDouble(10.2)).assertTrue()
            expect(data.writeBoolean(true)).assertTrue()
            expect(data.writeChar('a')).assertTrue()
            expect(data.writeString("HelloWorld")).assertTrue()
            expect(data.writeSequenceable(new MySequenceable(1, "aaa"))).assertTrue()

            await gIRemoteObject.sendRequest(CODE_ALL_TYPE, data, reply, option).then((result) => {
                console.info("SUB_Softbus_IPC_IRemoteObject_0200: sendRequest done, error code: " + result.errCode);
                expect(result.errCode).assertEqual(0)
                expect(result.reply.readByte()).assertEqual(1)
                expect(result.reply.readShort()).assertEqual(2)
                expect(result.reply.readInt()).assertEqual(3)
                expect(result.reply.readLong()).assertEqual(10000)
                expect(result.reply.readFloat()).assertEqual(1.2)
                expect(result.reply.readDouble()).assertEqual(10.2)
                expect(result.reply.readBoolean()).assertTrue()
                expect(result.reply.readChar()).assertEqual('a')
                expect(result.reply.readString()).assertEqual("HelloWorld")
                let s = new MySequenceable(0, '')
                expect(result.reply.readSequenceable(s)).assertTrue()
                expect(s.num).assertEqual(1)
                expect(s.str).assertEqual("aaa")
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_IRemoteObject_0200:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IRemoteObject_0200---------------------------");
        done();
    });

    /*
     * @tc.number  SUB_Softbus_IPC_IRemoteObject_0300
     * @tc.name    Test that messageparcel passes through the same process, and the client
     *             receives the reply message in the callback function
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IRemoteObject_0300", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_IRemoteObject_0300---------------------------");
        try{
            var data = rpc.MessageParcel.create();
            console.log("SUB_Softbus_IPC_IRemoteObject_0300: create object successfully.");
            var reply = rpc.MessageParcel.create();
            var option = new rpc.MessageOption();

            expect(data.writeByte(1)).assertTrue()
            expect(data.writeShort(2)).assertTrue()
            expect(data.writeInt(3)).assertTrue()
            expect(data.writeLong(10000)).assertTrue()
            expect(data.writeFloat(1.2)).assertTrue()
            expect(data.writeDouble(10.2)).assertTrue()
            expect(data.writeBoolean(true)).assertTrue()
            expect(data.writeChar('a')).assertTrue()
            expect(data.writeString("HelloWorld")).assertTrue()
            expect(data.writeSequenceable(new MySequenceable(1, "aaa"))).assertTrue()

            const CODE_IREMOTEOBJECT_0200 = 21;
            await gIRemoteObject.sendRequest(CODE_ALL_TYPE, data, reply, option, (err, result) => {
                console.info("SUB_Softbus_IPC_IRemoteObject_0300:sendRequest done, error code: " + result.errCode)
                expect(result.errCode).assertEqual(0)
                expect(result.reply.readByte()).assertEqual(1)
                expect(result.reply.readShort()).assertEqual(2)
                expect(result.reply.readInt()).assertEqual(3)
                expect(result.reply.readLong()).assertEqual(10000)
                expect(result.reply.readFloat()).assertEqual(1.2)
                expect(result.reply.readDouble()).assertEqual(10.2)
                expect(result.reply.readBoolean()).assertTrue()
                expect(result.reply.readChar()).assertEqual('a')
                expect(result.reply.readString()).assertEqual("HelloWorld")
                let s = new MySequenceable(0, '')
                expect(result.reply.readSequenceable(s)).assertTrue()
                expect(s.num).assertEqual(1)
                expect(s.str).assertEqual("aaa")
            });
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_IRemoteObject_0300:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IRemoteObject_0300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_IRemoteObject_0400
     * @tc.name    Iremoteobject, register death notification verification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IRemoteObject_0400", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_IRemoteObject_0400---------------------------");
        try{
            let object = new TestAbilityStub("Test1")
            var resultAdd1 = object.addDeathRecipient(null, 0)
            console.log("SUB_Softbus_IPC_IRemoteObject_0400:run addDeathRecipient first result is " + resultAdd1);
            expect(resultAdd1 == false).assertTrue();

            var resultRemove1 = object.removeDeathRecipient(null, 0)
            console.log("SUB_Softbus_IPC_IRemoteObject_0400:run removeDeathRecipient1 result is " + resultRemove1);
            expect(resultRemove1 == false).assertTrue();

            let isDead = object.isObjectDead()
            console.log("SUB_Softbus_IPC_IRemoteObject_0400:run  isDead result is " + isDead);
            expect(isDead == false).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_IRemoteObject_0400:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IRemoteObject_0400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_IRemoteObject_0500
     * @tc.name    Do not get the server agent, do not create a remoteobject instance, and directly getcallingpid,
     *             getcallingpid, getcallingdeviceid, getlocaldeviceid
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IRemoteObject_0500", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_IRemoteObject_0500---------------------------");
        try{
            let callingPid = rpc.IPCSkeleton.getCallingPid()
            console.log("SUB_Softbus_IPC_IRemoteObject_0500: run getCallingPid success, callingPid is " + callingPid);

            let callingUid = rpc.IPCSkeleton.getCallingUid()
            console.log("SUB_Softbus_IPC_IRemoteObject_0500: run getCallingPid success, callingPid is " + callingUid);

            let callingDeviceID = rpc.IPCSkeleton.getCallingDeviceID()
            console.log("SUB_Softbus_IPC_IRemoteObject_0500: run getCallingDeviceID success, callingDeviceID is "
                         + callingDeviceID);
            expect(callingDeviceID == "").assertTrue();

            let localDeviceID = rpc.IPCSkeleton.getLocalDeviceID()
            console.log("SUB_Softbus_IPC_IRemoteObject_0500: run getLocalDeviceID success, localDeviceID is "
            + localDeviceID);
            expect(localDeviceID == "").assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_IRemoteObject_0500:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IRemoteObject_0500---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_IRemoteObject_0600
     * @tc.name    Querylocalinterface searches for objects based on descriptors
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IRemoteObject_0600", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_IRemoteObject_0600---------------------------");
        try{
            let object = new TestAbilityStub("Test1");
            console.log("SUB_Softbus_IPC_IRemoteObject_0600: run TestAbilityStub success");

            let result = object.isObjectDead()
            console.log("SUB_Softbus_IPC_IRemoteObject_0600: run isObjectDead success, result is " + result);
            expect(result == false).assertTrue()

            let callingPid = object.getCallingPid()
            console.log("SUB_Softbus_IPC_IRemoteObject_0600: run getCallingPid success, callingPid is " + callingPid);

            let callingUid = object.getCallingUid()
            console.log("SUB_Softbus_IPC_IRemoteObject_0600: run getCallingPid success, callingPid is " + callingUid);

            object.attachLocalInterface(object, "Test1")
            console.log("SUB_Softbus_IPC_IRemoteObject_0600: run attachLocalInterface success");

            let res = object.queryLocalInterface("Test1")
            console.log("SUB_Softbus_IPC_IRemoteObject_0600: run queryLocalInterface success, res2 is " + res);
        } catch (error) {
            console.log("SUB_Softbus_IPC_IRemoteObject_0600:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IRemoteObject_0600---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_IRemoteObject_0700
     * @tc.name    Getinterfacedescriptor to get the interface description
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IRemoteObject_0700", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_IRemoteObject_0700---------------------------");
        try{
            let object = new TestAbilityStub("Test1223");

            let result = object.isObjectDead()
            console.log("SUB_Softbus_IPC_IRemoteObject_0700: run isObjectDead success, result is " + result);
            expect(result == false).assertTrue()

            let callingPid = object.getCallingPid()
            console.log("SUB_Softbus_IPC_IRemoteObject_0700: run getCallingPid success, callingPid is " + callingPid);

            let callingUid = object.getCallingUid()
            console.log("SUB_Softbus_IPC_IRemoteObject_0700: run getCallingPid success, callingPid is " + callingUid);

            object.attachLocalInterface(object, "test1")
            console.log("SUB_Softbus_IPC_IRemoteObject_0700: run attachLocalInterface success");

            let result2 = object.getInterfaceDescriptor();
            console.log("SUB_Softbus_IPC_IRemoteObject_0700: run getInterfaceDescriptor success, result2 is "
                         + result2);
            expect(result2 == "test1").assertTrue();

        } catch (error) {
            console.log("SUB_Softbus_IPC_IRemoteObject_0700:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IRemoteObject_0700---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_RemoteProxy_0100
     * @tc.name    Call adddeathrecipient to register the death notification
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_RemoteProxy_0100", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_RemoteProxy_0100---------------------------");
        try{
            let recipient = new MyDeathRecipient(gIRemoteObject, null)
            var resultAdd1 = gIRemoteObject.addDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0100:run addDeathRecipient first result is " + resultAdd1);
            expect(resultAdd1 == true).assertTrue();

            var resultAdd2 = gIRemoteObject.addDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0100:run addDeathRecipient second result is " + resultAdd2);
            expect(resultAdd2 == true).assertTrue();

            var resultRemove1 = gIRemoteObject.removeDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0100:run removeDeathRecipient1 result is " + resultRemove1);
            expect(resultRemove1 == true).assertTrue();

            var resultRemove2 = gIRemoteObject.removeDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0100:run  removeDeathRecipient2 result is " + resultRemove2);
            expect(resultRemove2 == true).assertTrue();

            var resultRemove3 = gIRemoteObject.removeDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0100:run  removeDeathRecipient3 result is " + resultRemove3);
            expect(resultRemove3 == false).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_RemoteProxy_0100:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_RemoteProxy_0100---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_RemoteProxy_0200
     * @tc.name    Call isobjectdead to check whether the object is dead
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_RemoteProxy_0200", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_RemoteProxy_0200---------------------------");
        try{
            let recipient = new MyDeathRecipient(proxy, null)
            var resultAdd1 = proxy.addDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0200:run addDeathRecipient first result is " + resultAdd1);
            expect(resultAdd1 == true).assertTrue();

            var isDead1 = proxy.isObjectDead();
            console.log("SUB_Softbus_IPC_RemoteProxy_0200: run isObjectDead result is " + isDead1);
            expect(isDead1 == true).assertTrue();

            var resultAdd2 = proxy.addDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0200:run addDeathRecipient second result is " + resultAdd2);
            expect(resultAdd2 == true).assertTrue();

            var resultRemove1 = proxy.removeDeathRecipient(recipient, 0)
            console.log("SUB_Softbus_IPC_RemoteProxy_0200:run removeDeathRecipient1 result is " + resultRemove1);
            expect(resultRemove1 == true).assertTrue();

            var isDead2 = proxy.isObjectDead();
            console.log("SUB_Softbus_IPC_RemoteProxy_0200: run isObjectDead2 result is " + isDead2);
            expect(isDead1 == false).assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_RemoteProxy_0200:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_RemoteProxy_0200---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_RemoteProxy_0300
     * @tc.name    Getinterfacedescriptor to get the object interface description
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_RemoteProxy_0300", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_RemoteProxy_0300---------------------------");
        try{
            let object = new TestAbilityStub("Test0300");

            let result = object.getInterfaceDescriptor()
            console.log("SUB_Softbus_IPC_RemoteProxy_0300: run getInterfaceDescriptor success, result is " + result);
            expect(result == "Test0300").assertTrue();

        } catch (error) {
            console.log("SUB_Softbus_IPC_RemoteProxy_0300:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_RemoteProxy_0300---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_RemoteProxy_0400
     * @tc.name    Querylocalinterface searches for objects based on descriptors
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_RemoteProxy_0400", 0,async function(){
        console.log("---------------------start SUB_Softbus_IPC_RemoteProxy_0400---------------------------");
        try{
            let object = new TestAbilityStub("Test0400");

            let result = object.isObjectDead()
            console.log("SUB_Softbus_IPC_RemoteProxy_0400: run getInterfaceDescriptor success, result is " + result);
            expect(result == false).assertTrue()

            let res = object.attachLocalInterface(object, "Test2")
            console.log("SUB_Softbus_IPC_RemoteProxy_0400: run attachLocalInterface success, res is " + res);

            let res2 = object.queryLocalInterface('Test2');
            console.log("SUB_Softbus_IPC_RemoteProxy_0400: run queryLocalInterface success, res2 is " + res2);

            let resultDescrip = object.getInterfaceDescriptor()
            console.log("SUB_Softbus_IPC_RemoteProxy_0400: run getInterfaceDescriptor success resultDescrip is "
                         + resultDescrip);
            expect(resultDescrip == "Test2").assertTrue();
        } catch (error) {
            console.log("SUB_Softbus_IPC_RemoteProxy_0400:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_RemoteProxy_0400---------------------------");
    });

    /*
     * @tc.number  SUB_Softbus_IPC_RemoteProxy_0500
     * @tc.name    Transaction constant validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_RemoteProxy_0500", 0, async function(){
        console.log("SUB_Softbus_IPC_RemoteProxy_0500 is starting-------------")
        try {
            
                expect(rpc.RemoteProxy.PING_TRANSACTION).assertEqual(1599098439);

                expect(rpc.RemoteProxy.DUMP_TRANSACTION).assertEqual(1598311760);
                
                expect(rpc.RemoteProxy.INTERFACE_TRANSACTION).assertEqual(1598968902);
               
                expect(rpc.RemoteProxy.MIN_TRANSACTION_ID).assertEqual(0x1);
             
                expect(rpc.RemoteProxy.MAX_TRANSACTION_ID).assertEqual(0x00FFFFFF);
                
           
        } catch (error) {
            console.log("SUB_Softbus_IPC_RemoteProxy_0500 error is" + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_RemoteProxy_0500---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_IPCSkeleton_1000
     * @tc.name    Create an empty object and verify the function of the flushcommands interface
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_Softbus_IPC_IPCSkeleton_1000', 0, async function() {
        console.log("---------------------start SUB_Softbus_IPC_IPCSkeleton_1000---------------------------");
        try {
            console.info("SUB_Softbus_IPC_IPCSkeleton_1000")
            let remoteObject = {};
            let ret = rpc.IPCSkeleton.flushCommands(remoteObject);
            console.log("RpcServer: flushCommands result: " + ret);
        }
        catch (error) {
            console.log("SUB_Softbus_IPC_IPCSkeleton_1000 error is :" + error)
        }
        console.log("---------------------end SUB_Softbus_IPC_IPCSkeleton_1000---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_IPCSkeleton_2000
     * @tc.name    Do not get the server agent, do not create a remoteobject instance, and directly getcallingpid,
     *             getcallingpid, getcallingdeviceid, getlocaldeviceid
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_Softbus_IPC_IPCSkeleton_2000', 0, async function() {
        console.log("---------------------start SUB_Softbus_IPC_IPCSkeleton_2000---------------------------");
        try{
            let getCallingPid = rpc.IPCSkeleton.getCallingPid();
            console.log("SUB_Softbus_IPC_IPCSkeleton_2000: run  getCallingPid result is :" + getCallingPid);
            expect(getCallingPid.assertEqual(nll)).assertFale();

            let getCallingUid = rpc.IPCSkeleton.getCallingUid();
            console.log("SUB_Softbus_IPC_IPCSkeleton_2000: run getCallingUid result is :" + getCallingUid);
            expect(getCallingUid.assertEqual(nll)).assertFale();

            let getCallingToKenId = rpc.IPCSkeleton.getCallingToKenId();
            console.log("SUB_Softbus_IPC_IPCSkeleton_2000: run getCallingToKenId result is :" + getCallingToKenId);
            expect(getCallingToKenId.assertEqual(nll)).assertFale();

            let getLocalDeviceID = rpc.IPCSkeleton.getLocalDeviceID();
            console.log("SUB_Softbus_IPC_IPCSkeleton_2000: run getLocalDeviceID result is :" + getLocalDeviceID);
            expect(getLocalDeviceID.assertEqual(nll)).assertFale();

            let getCallingDeviceID = rpc.IPCSkeleton.getCallingDeviceID();
            console.log("SUB_Softbus_IPC_IPCSkeleton_2000: run getCallingDeviceID result is :" + getCallingDeviceID);
            expect(getCallingDeviceID.assertEqual(nll)).assertFale();
        } catch (error){
            console.log("SUB_Softbus_IPC_IPCSkeleton_2000: error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IPCSkeleton_2000---------------------------");
    })

    /*
     * @tc.number  SUB_Softbus_IPC_IPCSkeleton_3000
     * @tc.name    Basic method of testing ipcskeleton
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("SUB_Softbus_IPC_IPCSkeleton_3000", 0,async function(done){
        console.log("---------------------start SUB_Softbus_IPC_IPCSkeleton_3000---------------------------");
        try{
            let object = rpc.IPCSkeleton.getContextObject();
            let callingPid = rpc.IPCSkeleton.getCallingPid();
            let callingUid = rpc.IPCSkeleton.getCallingUid();
            let callingDeviceID = rpc.IPCSkeleton.getCallingDeviceID();
            let localDeviceID = rpc.IPCSkeleton.getLocalDeviceID();
            let isLocalCalling = rpc.IPCSkeleton.isLocalCalling();
            let id = rpc.IPCSkeleton.resetCallingIdentity();
            let ret = rpc.IPCSkeleton.setCallingIdentity(id);
            expect(object.getInterfaceDescriptor()).assertEqual("");
            expect(callingDeviceID).assertEqual("");
            expect(localDeviceID).assertEqual("");
            expect(isLocalCalling).assertTrue();
            expect(id).assertEqual("");
            expect(ret).assertTrue();
            expect(rpc.IPCSkeleton.flushCommands(gIRemoteObject)).assertEqual(0);
            console.info("SUB_Softbus_IPC_IPCSkeleton_3000： callingPid: " + callingPid + ", callingUid: " + callingUid
                         + ", callingDeviceID: " + callingDeviceID + ", localDeviceID: " + localDeviceID
                         + ", isLocalCalling: " + isLocalCalling);
            let option = new rpc.MessageOption();
            let data = rpc.MessageParcel.create();
            let reply = rpc.MessageParcel.create();
            expect(data.writeInterfaceToken("rpcTestAbility")).assertTrue();
            console.info("SUB_Softbus_IPC_IPCSkeleton_3000： start send request");
            await gIRemoteObject.sendRequest(CODE_IPCSKELETON, data, reply, option)
                .then(function(result) {
                    console.info("SUB_Softbus_IPC_IPCSkeleton_3000： sendRequest done, error code: " + result.errCode)
                    expect(result.errCode).assertEqual(0);
                    result.reply.readException();
                    expect(result.reply.readInt()).assertEqual(callingPid);
                    expect(result.reply.readInt()).assertEqual(callingUid);
                    expect(result.reply.readString()).assertEqual("");
                    expect(result.reply.readString()).assertEqual("");
                    expect(result.reply.readBoolean()).assertTrue();
                    expect(result.reply.readInt()).assertEqual(callingPid);
                    expect(result.reply.readInt()).assertEqual(callingUid);
                    expect(result.reply.readInt()).assertEqual(callingPid);
                    expect(result.reply.readInt()).assertEqual(callingUid);
                    expect(result.reply.readInt()).assertEqual(101);
                })
            data.reclaim();
            reply.reclaim();
            done();
        } catch (error) {
            console.log("SUB_Softbus_IPC_IPCSkeleton_3000:error = " + error);
        }
        console.log("---------------------end SUB_Softbus_IPC_IPCSkeleton_3000---------------------------");
    });

    console.log("-----------------------SUB_Softbus_IPC_MessageParce_Test is end-----------------------");
});
