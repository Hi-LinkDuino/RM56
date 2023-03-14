/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import router from '@system.router';
import deviceManager from '@ohos.distributedHardware.deviceManager';
function uint8ArrayToBase64(array) {
    array = new Uint8Array(array);
    let table = ['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'],
        base64Str = '', length = array.byteLength, i = 0;
    for(i = 0; length - i >= 3; i += 3) {
        let num1 = array[i], num2 = array[i + 1], num3 = array[i + 2];
        base64Str += table[num1 >>> 2] + table[((num1 & 0b11) << 4) | (num2 >>> 4)] + table[((num2 & 0b1111) << 2) | (num3 >>> 6)] + table[num3 & 0b111111];
    }
    const lastByte = length - i;
    if (lastByte === 1) {
        const lastNum1 = array[i];
        base64Str += table[lastNum1 >>> 2] + table[((lastNum1 & 0b11) << 4)] + '==';
    } else if (lastByte === 2) {
        const lastNum1 = array[i];
        const lastNum2 = array[i + 1];
        base64Str += table[lastNum1 >>> 2] + table[((lastNum1 & 0b11) << 4) | (lastNum2 >>> 4)] + table[(lastNum2 & 0b1111) << 2] + '=';
    }
    return 'data:image/png;base64,' + base64Str;
}
const TAG = "DeviceManagerUI:";
let dmClass;

export default {
    data: {
        // showType:  ['main-pin','join-authorize','join-auth','join-auth-image','join-pin']
        status: "",
        // showInfo
        statusInfo: {
            deviceName: "AppName",
            appName: 'PackageName',
            appIcon: null,
            pinCode: '',
            pinToken: ''
        },
        // join: join-authorize timing
        timeRemaining: 0,
        // input pinCode
        pin: ['','','','','',''],
        // input pinCode next number
        pinNumb: 0
    },

    log(m) {
        console.info(TAG + m);
    },

    onDestroy() {
        if (dmClass != null) {
            dmClass.off('dmFaCallback');
            dmClass.off('deviceStateChange');
            dmClass.off('serviceDie');
            dmClass.release();
            dmClass = null
        }
    },

    onShow() {
        if (dmClass) {
            this.initStatue()
        } else {
            this.log('createDeviceManager')
            deviceManager.createDeviceManager('com.ohos.devicemanagerui', (err, dm) => {
                this.log("createDeviceManager err:" + JSON.stringify(err) + '  --success:' + JSON.stringify(dm))
                if (err) return;
                dmClass = dm;
                dmClass.on('dmFaCallback', () => router.back())
                this.initStatue()
            });
        }
    },

    onHide() {
        this.timeRemaining = 0
    },

    /**
     * Get authentication param
     */
    initStatue() {
        this.log('initStatue')
        const data = dmClass.getAuthenticationParam()
        this.log('getAuthenticationParam:' + JSON.stringify(data))
        // Authentication type, 1 for pin code.
        if (data && data.authType == 1) {
            this.statusInfo = {
                deviceName: data.extraInfo.PackageName,
                appName: data.extraInfo.appName,
                appIcon: uint8ArrayToBase64(data.appIcon),
                pinCode: data.extraInfo.pinCode + '',
                pinToken: data.extraInfo.pinToken
            }
            // direction: 1(main)/0(join)
            if (data.extraInfo.direction == 1) {
                this.mainPin()
            } else if (data.appIcon) {
                this.joinAuthImage()
            } else if (data.extraInfo.business == 0) {
                // business: 0(FA流转)/1(资源访问)
                this.joinAuth()
            } else {
                this.joinAuthorize()
            }
        }
    },

    /**
     * Set user Operation from devicemanager Fa, this interface can only used by devicemanager Fa.
     *
     * @param operateAction User Operation Actions.
     * ACTION_ALLOW_AUTH = 0, allow authentication
     * ACTION_CANCEL_AUTH = 1, cancel authentication
     * ACTION_AUTH_CONFIRM_TIMEOUT = 2, user operation timeout for authentication confirm
     * ACTION_CANCEL_PINCODE_DISPLAY = 3, cancel pinCode display
     * ACTION_CANCEL_PINCODE_INPUT = 4, cancel pinCode input
     */
    setUserOperation(operation) {
        this.log('setUserOperation: ' + operation)
        if (dmClass != null) {
            var data = dmClass.setUserOperation(operation);
            this.log('setUserOperation result: ' + JSON.stringify(data))
        } else {
            this.log('deviceManagerObject not exit')
        }
    },

    /**
     * verify auth info, such as pin code.
     * @param pinCode
     * @return
     */
    verifyAuthInfo(pinCode) {
        this.log('verifyAuthInfo: ' + pinCode)
        if (dmClass != null) {
            dmClass.verifyAuthInfo({
                "authType": 1,
                "token": this.statusInfo.pinToken,
                "extraInfo": {
                    "pinCode": +pinCode
                }
            }, (err, data) => {
                if (err) {
                    this.log("verifyAuthInfo err:" + JSON.stringify(err))
                }
                this.log("verifyAuthInfo result:" + JSON.stringify(data))
                router.back()
            });
        } else {
            this.log('deviceManagerObject not exit')
        }
    },

    /**
     * Input pinCode at the main control terminal
     */
    mainPin() {
        this.status = 'main-pin'
    },

    /**
     * Enter a number with the keyboard
     * @param s
     * @return
     */
    mainInputPin(s) {
        this.log('mainInputPin input: ' + s + '-' + this.pin)
        if (this.pinNumb == 6) return
        if (this.pinNumb < 6) {
            this.pin[this.pinNumb] = s
            ++this.pinNumb
            this.pin = [...this.pin]
        }
        this.log('mainInputPin pin: ' + this.pin + '-' + this.pin.join(''))
        if (this.pinNumb == 6) {
            // input end
            this.log('mainInputPin end: ' + this.pin + '-' + this.pin.join(''))
            this.verifyAuthInfo(this.pin.join(''))
        }
    },

    /**
     * Keyboard delete number
     */
    mainInputPinBack() {
        if (this.pinNumb > 0) {
            --this.pinNumb
            this.pin[this.pinNumb] = ''
            this.pin = [...this.pin]
        }
    },

    /**
     * Cancel pinCode input
     */
    mainInputPinCancel() {
        this.setUserOperation(4)
    },

    /**
     * Join end authorization, business(FA流转)/1(资源访问): 0
     */
    joinAuthorize() {
        this.status = 'join-authorize'
        this.timing(60, 'join-authorize', () => {
            this.setUserOperation(2)
            router.back()
        })
    },

    /**
     * Join end authorization, business(FA流转)/1(资源访问): 1
     */
    joinAuth() {
        this.status = 'join-auth'
        this.timing(60, 'join-auth', () => {
            this.setUserOperation(2)
            router.back()
        })
    },

    /**
     * Join end authorization, business(FA流转)/1(资源访问): 1, show application icon
     */
    joinAuthImage() {
        this.status = 'join-auth-image'
        this.timing(60, 'join-auth-image', () => {
            this.setUserOperation(2)
            router.back()
        })
    },

    /**
     * Display pinCode at join end
     */
    joinPin() {
        this.status = 'join-pin'
    },

    /**
     * Cancel authorization
     */
    joinAuthorizeCancel() {
        this.setUserOperation(1)
        router.back()
    },

    /**
     * Confirm authorization
     */
    joinAuthorizeOk() {
        this.setUserOperation(0)
        this.joinPin()
    },

    /**
     * Cancel authorization
     */
    joinAuthCancel() {
        this.setUserOperation(1)
        router.back()
    },

    /**
     * Confirm authorization
     */
    joinAuthOk() {
        this.setUserOperation(0)
        this.joinPin()
    },

    /**
     * Cancel authorization
     */
    joinAuthImageCancel() {
        this.setUserOperation(1)
        router.back()
    },

    /**
     * Confirm authorization
     */
    joinAuthImageOk() {
        this.setUserOperation(0)
        this.joinPin()
    },

    /**
     * Cancel authorization
     */
    joinPinCancel() {
        this.setUserOperation(3)
        router.back()
    },

    /**
     * Pure function countdown
     * @param numb second
     * @param status
     * @param callback
     * @return
     */
    timing(numb, status, callback) {
        this.timeRemaining = numb
        const next = () => {
            if (status != this.status) return
            --this.timeRemaining
            if (this.timeRemaining > 0) {
                setTimeout(next, 1000)
            } else {
                callback()
            }
        }
        next()
    }
}
