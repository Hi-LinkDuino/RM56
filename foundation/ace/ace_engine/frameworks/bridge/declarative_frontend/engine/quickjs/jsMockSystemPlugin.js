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
mockSystemPlugin()
function mockSystemPlugin () {
  global.systemplugin = {
    notification:{},
    vibrator: {},
    sensor: {},
    geolocation: {},
    network: {},
    brightness: {
      argsV: {
        value: 80
      },
      argsM: {
        mode: 0
      }
    },
    volume: {
      args: {value: 0.5}
    },
    battery: {},
    systemPackage: {},
    wifi: {},
    bluetooth: {},
    tts: {},
    alarm: {},
    request: {},
    fetch: {},
    storage: {},
    file: {}
  }
  mockNotification()
  mockFetch()
  mockRequest()
  mockStorage()
  mockFile()
  mockVibrator()
  mockSensor()
  mockGeolocation()
  mockNetwork()
  mockBrightness()
  mockVolume()
  mockBattery()
  mockSystemPackage()
}

function mockNotification () {
  global.systemplugin.notification = {
    show: function () {}
  }
}

function mockVibrator () {
  global.systemplugin.vibrator = {
    vibrate: function () {
      arguments[0].success()
      isComplete(arguments[0].complete)
    }
  }
}

function mockSensor () {
  mockAccelerometer()
  mockCompass()
  mockProximity()
  mockLight()
  mockStepCounter()
  mockBarometer()
  mockHeartRate()
  mockOnBodyState()
}

function mockAccelerometer () {
  Object.assign(global.systemplugin.sensor, {
    subscribeAccelerometer: function () {
      const time = {
        normal: 200,
        game: 20,
        ui: 60
      }
      let ret = {}
      let timer = 0
      if(!arguments[0].interval) {
        timer = time.normal
      } else {
        timer = time[arguments[0].interval]
      }
      this.unsubscribeAccelerometer()
      this.unsubscribeAcc = setInterval(() => {
        ret.x = Math.ceil(Math.random() * 10)
        ret.y = Math.ceil(Math.random() * 10)
        ret.z = Math.ceil(Math.random() * 10)
        arguments[0].success(ret)
      }, timer)
    },
    unsubscribeAccelerometer: function () {
      clearInterval(this.unsubscribeAcc)
      delete this.unsubscribeAcc
    }
  })
}

function mockCompass () {
  Object.assign(global.systemplugin.sensor, {
    subscribeCompass: function () {
      if(!this.unsubscribeDirection) {
        let ret = {}
        this.unsubscribeDirection = setInterval(() => {
          ret.direction = getRandomArbitrary(49, 50)
          arguments[0].success(ret)
        }, 100)
      }
    },
    unsubscribeCompass: function () {
      clearInterval(this.unsubscribeDirection)
      delete this.unsubscribeDirection
    }
  })
}

function mockProximity() {
  Object.assign(global.systemplugin.sensor, {
    subscribeProximity: function () {
      if(!this.unsubscribeDistance) {
        let ret = {}
        this.unsubscribeDistance = setInterval(() => {
          ret.distance = Math.ceil(Math.random() * 100)
          arguments[0].success(ret)
        }, 1000)
      }
    },
    unsubscribeProximity: function () {
      clearInterval(this.unsubscribeDistance)
      delete this.unsubscribeDistance
    }
  })
}

function mockLight () {
  Object.assign(global.systemplugin.sensor, {
    subscribeLight: function () {
      if(!this.unsubscribeIntensity) {
        let ret = {}
        this.unsubscribeIntensity = setInterval(() => {
          ret.intensity = getRandomArbitrary(660, 680)
          arguments[0].success(ret)
        }, 500)
      }
    },
    unsubscribeLight: function () {
      clearInterval(this.unsubscribeIntensity)
      delete this.unsubscribeIntensity
    },
  })
}

function mockStepCounter() {
  Object.assign(global.systemplugin.sensor, {
    subscribeStepCounter: function () {
      if(!this.unsubscribeSteps) {
        let ret = { steps: 0 }
        this.unsubscribeSteps = setInterval(() => {
          ret.steps += 1
          arguments[0].success(ret)
        }, 1000)
      }
    },
    unsubscribeStepCounter: function () {
      clearInterval(this.unsubscribeSteps)
      delete this.unsubscribeSteps
    }
  })
}

function mockBarometer() {
  Object.assign(global.systemplugin.sensor, {
    subscribeBarometer: function () {
      if(!this.unsubscribePressure) {
        let ret = {}
        this.unsubscribePressure = setInterval(() => {
          ret.pressure = getRandomArbitrary(1110, 1111)
          arguments[0].success(ret)
        }, 500)
      }
    },
    unsubscribeBarometer: function () {
      clearInterval(this.unsubscribePressure)
      delete this.unsubscribePressure
    }
  })
}

function mockHeartRate() {
  Object.assign(global.systemplugin.sensor, {
    subscribeHeartRate: function () {
      if(!this.unsubscribeRate) {
        let ret = {}
        this.unsubscribeRate = setInterval(() => {
          ret.heartRate = Math.ceil(Math.random() * 30)
          arguments[0].success(ret)
        }, 500)
      }
    },
    unsubscribeHeartRate: function () {
      clearInterval(this.unsubscribeRate)
      delete this.unsubscribeRate
    },
  })
}

function mockOnBodyState () {
  Object.assign(global.systemplugin.sensor, {
    subscribeOnBodyState: function () {
      if(!this.unsubscribeBodyState) {
        let ret = {}
        this.unsubscribeBodyState = setInterval(() => {
          ret.value = Math.ceil(Math.random() * 20)
          arguments[0].success(ret)
        }, 500)
      }
    },
    unsubscribeOnBodyState: function () {
      clearInterval(this.unsubscribeBodyState)
      delete this.unsubscribeBodyState
    }
  })
}

function mockGeolocation () {
  const data = {
    latitude: '121.61934',
    longitude: '31.257907',
    accuracy: '15',
    time: '160332896544'
  }
  global.systemplugin.geolocation = {
    getLocation: function () {
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    },
    getLocationType: function () {
      let args = {types: ['gps', 'network']}
      arguments[0].success(args)
      isComplete(arguments[0].complete)
    },
    getSupportedCoordTypes() {
      return ["wgs84"]
    },
    subscribe: function () {
      if(!this.unsubscribeLocation) {
        this.unsubscribeLocation = setInterval(() => {
          data.latitude = getRandomArbitrary(121, 122)
          data.longitude = getRandomArbitrary(31, 32)
          data.accuracy = getRandomArbitrary(14, 18)
          arguments[0].success(data)
        }, 1000)
      }
    },
    unsubscribe: function () {
      clearInterval(this.unsubscribeLocation)
      delete this.unsubscribeLocation
    }
  }
}

function mockNetwork () {
  const data = {
    metered: true,
    type: "5g"
  }
  global.systemplugin.network = {
    getType: function () {
      arguments[0].success(data)
      arguments[0].complete()
    },
    subscribe: function () {
      if(!this.unsubscribeNetwork) {
        this.unsubscribeNetwork = setInterval(() => {
          arguments[0].success(data)
        }, 3000)
      }
    },
    unsubscribe: function () {
      clearInterval(this.unsubscribeNetwork)
      delete this.unsubscribeNetwork
    }
  }
}

function mockBrightness () {
  Object.assign(global.systemplugin.brightness, {
    getValue: function () {
      arguments[0].success(this.argsV)
      isComplete(arguments[0].complete)
    },
    setValue: function () {
      if(arguments[0].value) {
        this.argsV.value = arguments[0].value
        arguments[0].success("brightness setValue successfully")
        isComplete(arguments[0].complete)
      }
    },
    getMode: function () {
      arguments[0].success(this.argsM)
      isComplete(arguments[0].complete)
    },
    setMode: function () {
      this.argsM.mode = arguments[0].mode
      arguments[0].success("brightness setMode successfully")
      isComplete(arguments[0].complete)
    },
    setKeepScreenOn: function () {
      arguments[0].success("brightness setKeepScreenOn successfully")
      isComplete(arguments[0].complete)
    }
  })
}

function mockVolume () {
  Object.assign(global.systemplugin.volume, {
    getMediaValue: function () {
      arguments[0].success(this.args)
      isComplete(arguments[0].complete)
    },
    setMediaValue: function () {
      if(arguments[0].value) {
        this.args.value = arguments[0].value
        arguments[0].success("set volume successfully")
        isComplete(arguments[0].complete)
      }
    }
  })
}

function mockBattery () {
  global.systemplugin.battery = {
    getStatus: function () {
      arguments[0].success.call(this, { level: 1, charging: false} )
      isComplete(arguments[0].complete)
    }
  }
}

function mockSystemPackage () {
  global.systemplugin.package = {
    hasInstalled: function () {
      arguments[0].success(true)
      isComplete(arguments[0].complete)
    }
  }
}

function mockRequest () {
  const data = {
    code: "[pc Preview]: no internet",
    data: "[pc Preview]: no internet",
    headers: "[pc Preview]: no internet",
    token: "[pc Preview]: no internet",
    uri: "[pc Preview]: no internet",
  }
  global.systemplugin.request = {
    upload: function () {
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    },
    download: function () {
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    },
    onDownloadComplete: function () {
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    }
  }
}

function mockFetch () {
  global.sendGroupMessage = global.group.sendGroupMessage;
  let GroupMessenger = {
    create: function () {
      let messenger = {};
      messenger.send = function (groupName, functionName, ...args) {
        return new Promise(function (resolve, reject) {
          let params = messenger.prepareArgs(...args);
          sendGroupMessage(function (result) {
            resolve(messenger.parseJsonResult(result));
          }, function (error) {
            reject(messenger.parseJsonResult(error));
          }, groupName, functionName, ...params);
        })
      };
      messenger.parseJsonResult = function (data) {
        if (data && data.constructor == String) {
          try {
            data = JSON.parse(data);
          } catch (jsonParseErr) {
            console.warn("parse result exception: " + JSON.stringify(jsonParseErr));
          }
        }
        return data;
      };
      messenger.prepareArgs = function (...args) {
        let result = [...args];
        for (let i = 0; i < result.length; i++) {
          if (typeof result[i] === 'function') {
            result[i] = messenger.packageCallback(result[i]);
          }
        }
        return result;
      };
      messenger.packageCallback = function (func) {
        return function (data) {
          data = messenger.parseJsonResult(data);
          if (!Array.isArray(data)) {
            func(data);
          } else {
            func(...data);
          }
        };
      };
      return messenger;
    }
  };

  let CommonCallback = {
    commonCallback: function commonCallback(callback, flag, data, code) {
      if (typeof callback === 'function') {
        switch (flag) {
          case 'success':
            callback(data);
            break;
          case 'fail':
            callback(data, code);
            break;
          case 'cancel':
            callback(data);
            break;
          case 'complete':
            callback();
            break;
          default:
            break;
        }
      } else {
        console.warn('callback.' + flag + ' is not function or not present');
      }
    }
  };
  global.commonCallback = CommonCallback.commonCallback;
  let CommonCallbackEx = {
    commonCallbackEx: function commonCallbackEx(callback, result, pluginError) {
      if ((callback === undefined) || ((callback.success === undefined) && (callback.fail === undefined) && (callback.complete === undefined))) {
        return CommonCallbackEx.promiseMethod(result, pluginError);
      } else {
        return CommonCallbackEx.callbackMethod(callback, result, pluginError);
      }
    },
    promiseMethod: function promiseMethod(result, pluginError) {
      if (pluginError != undefined) {
        throw pluginError;
      }
      return result;
    },
    callbackMethod: function callbackMethod(callback, result, pluginError) {
      if (pluginError != undefined) {
        commonCallback(callback.fail, 'fail', pluginError.data, pluginError.code);
        commonCallback(callback.complete, 'complete');
        throw pluginError;
      }
      commonCallback(callback.success, 'success', result.data);
      commonCallback(callback.complete, 'complete');
      return result;
    },
    catching: function catching(promise, param) {
      return promise.then(ret => commonCallbackEx(param, ret))
        .catch(err => commonCallbackEx(param, null, err));
    }
  };
  global.commonCallbackEx = CommonCallbackEx.commonCallbackEx;
  global.systemplugin.catching = CommonCallbackEx.catching;

  let FetchObject = {
    getFetch: function () {
      let fetch = {}
      fetch.messenger = GroupMessenger.create();
      fetch.fetch = async function (param) {
        return await CommonCallbackEx.catching(this.messenger.send("groupName", "fetch", param), param);
      }
      return fetch
    }
  }

  global.systemplugin.fetch = FetchObject.getFetch();
}

function mockStorage () {
  global.systemplugin.storage = {
    get: function () {
      arguments[0].success("[pc Preview]: no system")
      isComplete(arguments[0].complete)
    },
    set: function () {
      arguments[0].success("[pc Preview]: no system")
      isComplete(arguments[0].complete)
    },
    clear: function () {
      arguments[0].success("[pc Preview]: no system")
      isComplete(arguments[0].complete)
    },
    delete: function () {
      arguments[0].success("[pc Preview]: no system")
      isComplete(arguments[0].complete)
    }
  }
}

function mockFile () {
  global.systemplugin.file = {
    move: function () {
      arguments[0].success(arguments[0].dstUri)
      isComplete(arguments[0].complete)
    },
    copy: function () {
      arguments[0].success(arguments[0].dstUri)
      isComplete(arguments[0].complete)
    },
    list: function () {
      const data = {
        fileList: [{
          uri:'[pc Preview]: no file',
          lastModifiedTime:"[pc Preview]: no file",
          length:"[pc Preview]: no file",
          type: 'file'}]
      }
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    },
    get: function () {
      const data = {
        uri:'[pc Preview]: no file',
        lastModifiedTime:"[pc Preview]: no file",
        length:"[pc Preview]: no file",
        type: 'file',
        subFiles: ["[pc Preview]: no file", "[pc Preview]: no file"]
      }
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    },
    delete: function () {
      arguments[0].success()
      isComplete(arguments[0].complete)
    },
    writeText: function () {
      arguments[0].success()
      isComplete(arguments[0].complete)
    },
    writeArrayBuffer: function () {
      arguments[0].success()
      isComplete(arguments[0].complete)
    },
    readText: function () {
      const data = {text: "[pc Preview]: success default"}
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    },
    readArrayBuffer: function () {
      const data = {buffer: ["[pc Preview]: default", "[pc Preview]: default", "[pc Preview]: default"]}
      arguments[0].success(data)
      isComplete(arguments[0].complete)
    },
    access: function () {
      arguments[0].success()
      isComplete(arguments[0].complete)
    },
    mkdir: function () {
      arguments[0].success()
      isComplete(arguments[0].complete)
    },
    rmdir: function () {
      arguments[0].success()
      isComplete(arguments[0].complete)
    }
  }
}

function isComplete() {
  if(arguments[0] === undefined) {
    return
  }
  arguments[0].call(this)
}

function getRandomArbitrary(min, max) {
  return Math.random().toFixed(6) * (max - min) + min;
}
