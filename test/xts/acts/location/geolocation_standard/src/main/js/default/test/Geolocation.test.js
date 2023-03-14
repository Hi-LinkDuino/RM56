/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import geolocation from '@ohos.geolocation';
import WantAgent from '@ohos.wantAgent';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
let LocationRequestScenario = {UNSET : 0x300 ,NAVIGATION : 0x301 ,
    TRAJECTORY_TRACKING : 0x302 ,CAR_HAILING : 0x303,
    DAILY_LIFE_SERVICE : 0x304 ,NO_POWER : 0x305}
let LocationRequestPriority = {UNSET : 0x200 ,ACCURACY : 0x201 ,LOW_POWER : 0x202 ,FIRST_FIX :0x203}

let LocationPrivacyType = {
    OTHERS : 0,
    STARTUP: 1,
    CORE_LOCATION : 2
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

async function applyPermission() {
    let appInfo = await bundle.getApplicationInfo('ohos.acts.location.geolocation.function', 0, 100);
    let atManager = abilityAccessCtrl.createAtManager();
    if (atManager != null) {
      let tokenID = appInfo.accessTokenId;
      console.info('[permission] case accessTokenID is ' + tokenID);
      let permissionName1 = 'ohos.permission.LOCATION';
      let permissionName2 = 'ohos.permission.LOCATION_IN_BACKGROUND';
      await atManager.grantUserGrantedPermission(tokenID, permissionName1, 1).then((result) => {
        console.info('[permission] case grantUserGrantedPermission success :' + result);
      }).catch((err) => {
        console.info('[permission] case grantUserGrantedPermission failed :' + err);
      });
      await atManager.grantUserGrantedPermission(tokenID, permissionName2, 1).then((result) => {
        console.info('[permission] case grantUserGrantedPermission success :' + result);
      }).catch((err) => {
        console.info('[permission] case grantUserGrantedPermission failed :' + err);
      });
    } else {
      console.info('[permission] case apply permission failed, createAtManager failed');
    }
  }

describe('geolocationTest', function () {
    console.log('#start AccessTokenTests#');
    beforeAll(async function () {
        await applyPermission();
        console.info('beforeAll case');
    })
    beforeEach(function () {
        sleep(3000);
        console.info('beforeEach case');
    })
    afterEach(function () {
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_ENABLE_LOCATION_PROMISE_0001
     * @tc.name testenableLocation promise
     * @tc.desc Test enableLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */	
    it('geolocation_enableLocation_promise_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        })
        done()
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_LOCATION_ENABLED_PROMISE_0001
     * @tc.name testisLocationEnabled promise
     * @tc.desc Test isLocationEnabled api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_isLocationEnabled_promise_test_001', 0, async function (done) {		
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        await geolocation.isLocationEnabled().then((result) => {
            console.info('[lbs_js] getLocationSwitchState result: ' + result);
            expect(result).assertTrue();         
        }).catch((error) => {
            expect().assertFail();
        });
        done();
    })
   		
    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_ADDR_FROM_LOCATION_NAME_CALLBACK_0001
     * @tc.name testgetAddressesFromLocationName callback
     * @tc.desc Test getAddressesFromLocationName api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_getAddressesFromLocationName_callback_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();        
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        let geocodeRequest = {"description": "上海市浦东新区金穗路1800号", "maxItems": 1,
            "locale": "zh",description:"",maxItems:"",minLatitude:"",
            minLongitude:"",maxLatitude:"",maxLongitude:""};
        geolocation.getAddressesFromLocationName(geocodeRequest, (err, data) => {
            if(err){
                switch(err){
                    case "101":
                        console.info("INPUT_PARAMS_ERROR: "+ err);
                        break;
                    case "102":
                        console.info("REVERSE_GEOCODE_ERROR: "+ err);
                        break;
                    case "103":
                        console.info("GEOCODE_ERROR: "+ err);
                        break;
                    case "104":
                        console.info("LOCATOR_ERROR: "+ err);
                        break;
                    case "105":
                        console.info("LOCATION_SWITCH_ERROR: "+ err);
                        break;
                    case "106":
                        console.info("LAST_KNOWN_LOCATION_ERROR: "+ err);
                        break;
                    case "107":
                        console.info("LOCATION_REQUEST_TIMEOUT_ERROR: "+ err);
                        break;
                    default:
                        console.info('[lbs_js]  getAddressesFromLocationName callback err is : ' + err);    
                        }              
            }else {
                console.info("[lbs_js] getAddressesFromLocationName callback data is: " + JSON.stringify(data));
                expect(true).assertEqual((JSON.stringify(data)) !=null);
                console.info("[lbs_js] getAddressesFromLocationName callback exit .");
            }
            done();
        });       
    })
	
    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_ADDR_FROM_LOCATION_CALLBACK_0001
     * @tc.name testgetAddressesFromLocation callback
     * @tc.desc Test getAddressesFromLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */ 
    it('geolocation_getAddressesFromLocation_callback_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        let reverseGeocodeRequest = {"latitude": 31.12, "longitude": 121.11, "maxItems": 1,"locale": "zh"};
        geolocation.getAddressesFromLocation(reverseGeocodeRequest, (err, data) => {
            if (err) {
                console.info('[lbs_js]  getAddressesFromLocation callback err is : ' + err);
            }else {
                console.info("[lbs_js]  getAddressesFromLocation callback data is: " + JSON.stringify(data));
                expect(true).assertEqual((JSON.stringify(data)) !=null);
                console.info('[lbs_js] getAddressesFromLocation addressUrl: ' + data[0].addressUrl + 
                ' administrativeArea: ' + data[0].administrativeArea +' countryCode: ' + data[0].countryCode
                +' countryName: ' + data[0].countryName+' descriptions: ' + data[0].descriptions+
                'descriptionsSize: ' + data[0].descriptionsSize+'latitude:' + data[0].latitude+' locale: ' 
                + data[0].locale +'locality: ' + data[0].locality+' longitude:' + data[0].longitude+'phoneNumber:' 
                + data[0].phoneNumber+' placeName: ' + data[0].placeName+' postalCode: ' + data[0].postalCode
                +' premises: ' + data[0].premises+'roadName: ' + data[0].roadName +' subAdministrativeArea: '
                + data[0].subAdministrativeArea+' subLocality: ' + data[0].subLocality
                +' subRoadName: ' + data[0].subRoadName);
            }
            done();
        });
    })

  /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_GEOSERVICE_AVAIL_PROMISE_0001
     * @tc.name testisGeoServiceAvailable promise
     * @tc.desc Test isGeoServiceAvailable api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */ 
    it('geolocation_isGeoServiceAvailable_promise_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        await geolocation.isGeoServiceAvailable().then( (result) => {
			console.info('[lbs_js] isGeoServiceAvailable result: ' + result);
            expect(result).assertTrue();           
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        done();
    })

  /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_LOCATION_ENABLED_CALLBACK_0001
     * @tc.name testisLocationEnabled callback
     * @tc.desc Test isLocationEnabled api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
   */  
    it('geolocation_isLocationEnabled_callback_test_001', 0, async function (done) {
    
        let promiseOne = new Promise((resolve, reject) => {
            geolocation.enableLocation((err, data) => {
                if (err) {
                    console.info('[lbs_js]  enableLocation callback err is : ' + err );
                }else {
                    console.info("[lbs_js] enableLocation callback data: " + data);
                    expect(data).assertTrue();                
                }
                resolve()
            });
        })  
        let promiseTwo = new Promise((resolve, reject) => {
            geolocation.isLocationEnabled((err, data) => {
                if (err) {
                    console.info('[lbs_js]  getLocationSwitchState : ' + err);
                }else {
                    console.info("[lbs_js] getLocationSwitchState data: " + data);
                    expect(data).assertTrue();                    
                }
                resolve()
            });
        })
        await promiseOne.then(()=>{
            return promiseTwo
        }).then(done)
    })

  /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_ADDR_FROM_LOCATION_NAME_PROMISE_0001
     * @tc.name testgetAddressesFromLocationName promise
     * @tc.desc Test getAddressesFromLocationName api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */ 
    it('geolocation_getAddressesFromLocationName_promise_test_001', 0, async function (done) {		
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();            
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        let geocodeRequest = {"description": "上海市浦东新区金穗路1800号", "maxItems": 1};
        await geolocation.getAddressesFromLocationName(geocodeRequest).then((result) => {
            console.info("[lbs_js]  getAddressesFromLocation callback data is: " + JSON.stringify(result));
            expect(true).assertEqual((JSON.stringify(result)) !=null);           
        }).catch((error) => {
            console.info("[lbs_js] getAddressesFromLocationName promise then error." + error);
            expect().assertFail();
        });
        done();
    })
	
  /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_REQUEST_ENABLE_LOCATION_CALLBACK_0001
     * @tc.name testrequestEnableLocation callback
     * @tc.desc Test requestEnableLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
   */  
    it('geolocation_requestEnableLocation_callback_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        geolocation.requestEnableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  requestEnableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] requestEnableLocation callback data: " + data);
                expect(data).assertTrue();            
            }
            done();
        });
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_DISABLE_LOCATION_CALLBACK_0001
     * @tc.name testdisableLocation callback
     * @tc.desc Test disableLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
	*/    
    it('geolocation_disableLocation_callback_test_001', 0, async function (done) {    
        geolocation.disableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  disableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] disableLocation callback data: " + data);
                expect(data).assertTrue();                
            }
            done()
        });
    })

    /** @tc.number SUB_LOCATION_geocode_DEVICE_JS_DISABLE_LOCATION_PROMISE_0001
     * @tc.name testdisableLocation promise
     * @tc.desc Test disableLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_disableLocation_promise_test_001', 0, async function (done) {
        console.info('[lbs_js]LBS disableLocation promise test start ...');
        await geolocation.disableLocation().then((data) => {
            console.info('[lbs_js] disableLocation data: ' + data);
            expect(data).assertTrue();
            done();
        });
    })   

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_ENABLE_LOCATION_CALLBACK_0001
     * @tc.name testEnableLocation callback
     * @tc.desc Test enableLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_enableLocation_callback_test_001', 0, async function (done) { 
        geolocation.enableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  enableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] enableLocation callback data: " + data);
                expect(data).assertTrue();
            }
            done()
        });
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_REQUEST_ENABLE_LOCATION_PROMISE_0001
     * @tc.name testrequestEnableLocation promise
     * @tc.desc Test requestEnableLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */ 
    it('geolocation_requestEnableLocation_promise_test_001', 0, async function(done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        await geolocation.requestEnableLocation()
            .then((result) => {
                console.info('[lbs_js] requestEnableLocation result: ' + result);
                expect(result).assertTrue();
                done();
            }).catch((error) => {
                console.info("[lbs_js] promise then error." + error.message);
                expect().assertFail();
        });
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_ADDR_FROM_LOCATION_PROMISE_0001
     * @tc.name testgetAddressesFromLocation promise
     * @tc.desc Test getAddressesFromLocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_getAddressesFromLocation_promise_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        let reverseGeocodeRequest = {"latitude": 31.12, "longitude": 121.11, "maxItems": 1};
        await geolocation.getAddressesFromLocation(reverseGeocodeRequest).then((data) => {
            console.info('[lbs_js] getAddressesFromLocation promise: ' +  JSON.stringify(data));
            expect(true).assertEqual((JSON.stringify(data)) !=null);         
            done();
        }).catch(error => {
            console.info("[lbs_js] getAddressesFromLocation promise then error." + error.message);
            expect().assertFail();
            done();
        });
    })

     /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_CURRENT_LOCATION_CALLBACK_0001
     * @tc.name testGetCurrentlocation callback
     * @tc.desc Test GetCurrentlocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_GetCurrentlocation_callback_test_001', 0, async function(done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        let currentLocationRequest = { "priority": 0x203, "scenario": 0x300,
                "timeoutMs": 5, "maxAccuracy": 0 };
        await geolocation.getCurrentLocation(currentLocationRequest, 
            (err, result) => {
            if (err){
                return console.info("getCurrentLocation callback err:  " + err)
            }
            console.info("getCurrentLocation callback, result:  " + JSON.stringify(result));
            let resultLength = Object.keys(result).length; 
            expect(true).assertEqual(resultLength >= 0);
        });
        done()
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_CURRENT_LOCATION_CALLBACK_0001
     * @tc.name testGetCurrentlocation callback
     * @tc.desc Test GetCurrentlocation api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_GetCurrentlocation_callback_test_002', 0, async function(done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        await geolocation.getCurrentLocation((err, result) => {
            if (err){
                return console.info("getCurrentLocation callback err:  " + err)
            }
            console.info("getCurrentLocation callback, result:  " + JSON.stringify(result));
            let resultLength = Object.keys(result).length; 
            expect(true).assertEqual(resultLength >= 0);
        });
        done()
    })

   /**
    * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_GEOSERVICE_AVAIL_PROMISE_0001
    * @tc.name getLastLocation promise
    * @tc.desc Test getLastLocation api .
    * @tc.size MEDIUM
    * @tc.type Function
    * @tc.level Level 2
    */
    it('geolocation_getCurrentLocation_promise_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
         }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        let currentLocationRequest = { "priority": 0x203, "scenario": 0x301,
                "timeoutMs": 5, "maxAccuracy": 0 };
       try {
        geolocation.getCurrentLocation(currentLocationRequest).then( (result) => {
            console.info('[lbs_js]getCurrentLocation promise result '+ JSON.stringify(result));
        });     
        }catch(e) {
            console.info('[lbs_js] getCurrentLocation promise err is : ' +JSON.stringify(e));
        }
        done();
    }) 
   
  /**
   * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_ADDR_FROM_LOCATION_NAME_CALLBACK_0001
   * @tc.desc Test getLastLocation api .
   * @tc.author
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 2
   */
   it('geolocation_getLastLocation_callback_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
       await geolocation.getLastLocation((err, data) => {
            if (err) {
                console.info('[lbs_js] getLastLocation callback err is : ' + err);
            }else {
                console.info("[lbs_js] getLastLocation callback data is: " + JSON.stringify(data));
                let resultLength = Object.keys(data).length; 
                expect(true).assertEqual(resultLength >= 0);
                console.info('[lbs_js] getLastLocation latitude: ' + data[0].latitude + 
                ' longitude: ' + data[0].longitude +' altitude: ' + data[0].altitude
                +' accuracy: ' + data[0].accuracy+' speed: ' + data[0].speed +
                'timeStamp: ' + data[0].timeStamp+'direction:' + data[0].direction+' timeSinceBoot: ' 
                + data[0].timeSinceBoot +'additions' + data[0].additions+'additionSize' + data[0].additionSize);
            }
        });
        done();
    }) 

    /**
    * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_GEOSERVICE_AVAIL_PROMISE_0001
    * @tc.name getLastLocation promise
    * @tc.desc Test getLastLocation api .
    * @tc.size MEDIUM
    * @tc.type Function
    * @tc.level Level 2
    */
    it('geolocation_getLastLocation_promise_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        geolocation.getLastLocation().then( (result) => {
            console.info('[lbs_js] getLastLocation promise result '+ JSON.stringify(result));
            let resultLength = Object.keys(result).length; 
            expect(true).assertEqual(resultLength >= 0);
            done();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect(true).assertEqual(error != null);
            done();
        });
    }) 
    
    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_GEOSERVICE_AVAIL_CALLBACK_0001
     * @tc.name testisGeoServiceAvailable callback
     * @tc.desc Test isGeoServiceAvailable api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2 
     */
    it('geolocation_isGeoServiceAvailable_callback_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        geolocation.isGeoServiceAvailable((err, data) => {
            if (err) {
                console.info('[lbs_js]  getGeoServiceState err is : ' + err );
                return
            }else {
                console.info('[lbs_js] isGeoServiceAvailable result: ' + data);
                expect(data).assertTrue();             
            }
            done()
        });   
    })
        
    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_ADDR_FROM_LOCATION_NAME_CALLBACK_0001
     * @tc.name testgetCachedGnssLocationsSize callback
     * @tc.desc Test getCachedGnssLocationsSize api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
     it('geolocation_getCachedGnssLocationsSize_callback_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        geolocation.getCachedGnssLocationsSize((err, data) => {
            if (err) {
                console.info('[lbs_js]  getCachedGnssLocationsSize callback err is : ' + err);
            }else {
                console.info("[lbs_js] getCachedGnssLocationsSize callback data is: " + JSON.stringify(data));
                expect(true).assertTrue(data != null);
            }
        });
        done();
    })

  /**
   * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_GEOSERVICE_AVAIL_PROMISE_0001
   * @tc.name testgetCachedGnssLocationsSize promise
   * @tc.desc Test getCachedGnssLocationsSize api .
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 2
   */
    it('geolocation_getCachedGnssLocationsSize_promise_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        geolocation.getCachedGnssLocationsSize().then( (result) => {
            console.info('[lbs_js] getCachedGnssLocationsSiz promise '+ JSON.stringify(result));
            expect(true).assertTrue(result != null);
            done();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
            done();
        });
    })
    
  /**
   * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_ADDR_FROM_LOCATION_NAME_CALLBACK_0001
   * @tc.name testflushCachedGnssLocations callback
   * @tc.desc Test flushCachedGnssLocations api .
   * @tc.author
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 2
    */
    it('geolocation_flushCachedGnssLocations_callback_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        geolocation.flushCachedGnssLocations((err, data) => {
            if (err) {
                console.info('[lbs_js]  flushCachedGnssLocations callback err is : ' + err);
            }else {
                console.info("[lbs_js] flushCachedGnssLocations callback data is: " + JSON.stringify(data));
                expect(true).assertTrue(data);
            }
        });
        done();
    }) 

  /**
   * @tc.number SUB_LOCATION_geocode_DEVICE_JS_IS_GEOSERVICE_AVAIL_PROMISE_0001
   * @tc.name testflushCachedGnssLocations promise
   * @tc.desc Test flushCachedGnssLocations api .
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 2
   */
    it('geolocation_flushCachedGnssLocations_promise_test_001', 0, async function (done) {
        await geolocation.enableLocation().then((result) => {
            console.info('[lbs_js] testenableLocation result: ' + result);
            expect(result).assertTrue();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
        });
        geolocation.flushCachedGnssLocations().then( (result) => {
            console.info('[lbs_js] flushCachedGnssLocations_ '+ JSON.stringify(result));
            expect(true).assertTrue(result);
            done();
        }).catch((error) => {
            console.info("[lbs_js] promise then error." + error.message);
            expect().assertFail();
            done();
        });
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_GET_CURRENT_LOCATION_CALLBACK_0001
     * @tc.name testgnssStatusChange callback
     * @tc.desc Test gnssStatusChange api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_gnssStatusChangeon_test_0001', 0, async function (done) {
        try {
            await geolocation.on('gnssStatusChange', function (data) {
                console.info('[lbs_js] gnssStatusChangestart' +JSON.stringify(data) );
                expect(true).assertEqual((JSON.stringify(data)) !=null);
                console.info('[lbs_js] SatelliteStatusInfo satellitesNumber: ' + data[0].satellitesNumber + 
                'satelliteIds' + data[0].satelliteIds +'carrierToNoiseDensitys'+ data[0].carrierToNoiseDensitys
                +'altitudes' + data[0].altitudes+' azimuths: ' + data[0].azimuths +
                'carrierFrequencies: ' + data[0].carrierFrequencies);
            });
        }catch(e) {
            expect(null).assertFail();
        }
        try {
            await geolocation.off('gnssStatusChange', function (data) {
                console.info("[lbs_js] gnssStatusChange off data:" + JSON.stringify(data));
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number nmeaMessageChange_test_0001
     * @tc.name testnmeaMessageChange callback
     * @tc.desc Test nmeaMessageChange api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_nmeaMessageChangeon_test_0001', 0, async function (done) {
        try {
            await geolocation.on('nmeaMessageChange', function (data) {
                console.info('[lbs_js] nmeaMessageChange' +JSON.stringify(data) );
            });
        }catch(e) {
            expect(null).assertFail();
        }
        try {
            await geolocation.off('nmeaMessageChange', function (data) {
                console.info("[lbs_js] nmeaMessageChange off data:" + JSON.stringify(data));
            });
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })
 
    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_LOCATION_SERVICE_OFF_0002
     * @tc.name Test isLocationPrivacyConfirmed callback
     * @tc.desc Test isLocationPrivacyConfirmed api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_LocationPrivacyConfirmed_callback_test_001', 0, async function (done) {
        let reqType = 1;
        let value = true;
        await geolocation.setLocationPrivacyConfirmStatus(reqType, value, (err, resp)=>{
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
        });   

        console.info('[lbs_js] isLocationPrivacyConfirmed callback test start ...');
        geolocation.isLocationPrivacyConfirmed(reqType ,(err,resp) => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
            done()
        });
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_LOCATION_SERVICE_OFF_0002
     * @tc.name Test setLocationPrivacyConfirmStatus promise
     * @tc.desc Test setLocationPrivacyConfirmStatus api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */
     it('geolocation_LocationPrivacyConfirmStatus_promise_test_001', 0, async function (done) {
        let reqType =1;
        let value = true;
        let type = "STARTUP";

        if(type == "OTHERS"){
            reqType = 0;
        }
        if(type == "STARTUP"){
            reqType = 1;
        }
        if(type == "CORE_LOCATION"){
            reqType = 2;
        }
        await geolocation.setLocationPrivacyConfirmStatus(reqType,value).then((resp) => {
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
        })
        geolocation.isLocationPrivacyConfirmed(reqType).then(resp => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
            done();
        })
    })

    /**
     * @tc.number geolocation_setLocationPrivacyConfirmStatus_promise_test_002
     * @tc.name Test setLocationPrivacyConfirmStatus promise
     * @tc.desc Test setLocationPrivacyConfirmStatus api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */
    it('geolocation_LocationPrivacyConfirmStatus_promise_test_002', 0, async function (done) {
        let reqType =1;
        let value = false;
        let type = "STARTUP";

        if(type == "OTHERS"){
            reqType = 0;
        }
        if(type == "STARTUP"){
            reqType = 1;
        }
        if(type == "CORE_LOCATION"){
            reqType = 2;
        }
        await geolocation.setLocationPrivacyConfirmStatus(reqType,value).then((resp) => {
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
        })
        geolocation.isLocationPrivacyConfirmed(reqType).then(resp => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertFalse();
            done();
        })
    })

    /**
     * @tc.number geolocation_setLocationPrivacyConfirmStatus_promise_test_003
     * @tc.name Test setLocationPrivacyConfirmStatus promise
     * @tc.desc Test setLocationPrivacyConfirmStatus api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */
    it('geolocation_LocationPrivacyConfirmStatus_promise_test_003', 0, async function (done) {
        let reqType =0;
        let value = false;
        let type = "STARTUP";

        if(type == "OTHERS"){
            reqType = 0;
        }
        if(type == "STARTUP"){
            reqType = 1;
        }
        if(type == "CORE_LOCATION"){
            reqType = 2;
        }
        await geolocation.setLocationPrivacyConfirmStatus(reqType,value).then((resp) => {
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
        })
        geolocation.isLocationPrivacyConfirmed(reqType).then(resp => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertFalse();
            done();
        })
    })

    /**
     * @tc.number geolocation_setLocationPrivacyConfirmStatus_promise_test_004
     * @tc.name Test setLocationPrivacyConfirmStatus promise
     * @tc.desc Test setLocationPrivacyConfirmStatus api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
    */
     it('geolocation_LocationPrivacyConfirmStatus_promise_test_004', 0, async function (done) {
        let reqType =2;
        let value = false;
        let type = "STARTUP";

        if(type == "OTHERS"){
            reqType = 0;
        }
        if(type == "STARTUP"){
            reqType = 1;
        }
        if(type == "CORE_LOCATION"){
            reqType = 2;
        }
        await geolocation.setLocationPrivacyConfirmStatus(reqType,value).then((resp) => {
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
        })
        geolocation.isLocationPrivacyConfirmed(reqType).then(resp => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertFalse();
            done();
        })
    })

    /**
     * @tc.number geolocation_LocationPrivacyConfirmStatus_promise_test_005
     * @tc.name Test isLocationPrivacyConfirmed promise
     * @tc.desc Test isLocationPrivacyConfirmed api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_LocationPrivacyConfirmStatus_promise_test_005', 0, async function (done) {
        let reqType = 1;
        let value = true;
        await geolocation.setLocationPrivacyConfirmStatus(reqType,value).then((resp) => {
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
        })
        geolocation.isLocationPrivacyConfirmed(reqType).then(resp => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
            done();
        })
    })
    
    /**
     * @tc.number geolocation_LocationPrivacyConfirmStatus_promise_test_006
     * @tc.name Test isLocationPrivacyConfirmed promise
     * @tc.desc Test isLocationPrivacyConfirmed api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_LocationPrivacyConfirmStatus_promise_test_006', 0, async function (done) {
        let reqType =0;
        let value = true;
        let type = "OTHERS";

        if(type == "OTHERS"){
            reqType = 0;
        }
        if(type == "STARTUP"){
            reqType = 1;
        }
        if(type == "CORE_LOCATION"){
            reqType = 2;
        }
        await geolocation.setLocationPrivacyConfirmStatus(reqType,value).then((resp) => {
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
        })
        console.info('LBS isLocationPrivacyConfirmed promise test start ...');
        geolocation.isLocationPrivacyConfirmed(reqType).then(resp => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
            done();
        })
    })

    /**
     * @tc.number geolocation_LocationPrivacyConfirmStatus_promise_test_007
     * @tc.name Test isLocationPrivacyConfirmed promise
     * @tc.desc Test isLocationPrivacyConfirmed api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_LocationPrivacyConfirmStatus_promise_test_007', 0, async function (done) {
        let reqType =2;
        let value = true;
        let type = "CORE_LOCATION";

        if(type == "OTHERS"){
            reqType = 0;
        }
        if(type == "STARTUP"){
            reqType = 1;
        }
        if(type == "CORE_LOCATION"){
            reqType = 2;
        }
        await geolocation.setLocationPrivacyConfirmStatus(reqType,value).then((resp) => {
            console.log("[lbs_js] setLocationPrivacyConfirmStatus current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
        })
        geolocation.isLocationPrivacyConfirmed(reqType).then(resp => {
            console.log("[lbs_js] isLocationPrivacyConfirmed current type is "+ JSON.stringify(resp))
            expect(resp).assertTrue();
            done();
        })
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_LOCATION_CHANGE_ON_0002
     * @tc.name testlocationChangeOn promise
     * @tc.desc Test locationChangeOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_locationChange_On_test_001', 0, async function (done) {
        let requestInfo = {"priority":0x0203, "scenario":0x0300, "timeInterval":5,
            "distanceInterval": 0, "maxAccuracy": 0};
        let locationChange = (location) => {
            console.log('locationChanger: ' + JSON.stringify(location));
        };
        await geolocation.on('locationChange',requestInfo,
           locationChange => {
            if(err){
                return console.info("onLocationChange callback  err:  " + err);
            }
            console.info("onLocationChange callback, result:  " + JSON.stringify(locationChange)); 
            expect(true).assertEqual(locationChange !=null);
            done();
        }); 
        geolocation.enableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  enableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] enableLocation callback data: " + data);
                expect(data).assertTrue();
                console.info("[lbs_js] enableLocation callback exit .");
            }
            done()
        });
        await geolocation.off('locationChange',requestInfo,
            locationChange => {
                if(err){
                    return console.info("onLocationChange callback  err:  " + err);
                }
                console.info("offLocationChange callback  " + JSON.stringify(locationChange)); 
                expect(true).assertEqual(locationChange !=null);
                done();
        });             
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_LOCATION_CHANGE_ON_0002
     * @tc.name testlocationChangeOn promise
     * @tc.desc Test locationChangeOn api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_fenceStatusChange_On_test_001', 0, async function (done) {
        geolocation.enableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  enableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] enableLocation callback data: " + data);
                expect(data).assertTrue();            
            }
            done()
        });
        let geofence = {"latitude": 31.12, "longitude": 121.11, "radius": 1,"expiration": ""};
        let geofenceRequest = {"priority":0x0203, "scenario":0x0300, "geofence": geofence};
        let want = (wantAgent) => {
            console.log('wantAgent: ' + JSON.stringify(wantAgent));
        };
        await geolocation.on('fenceStatusChange', geofenceRequest,
           (want) => {
                if(err){
                    return console.info("fenceStatusChange on callback  err:  " + err);
                }
                console.info("fenceStatusChange callback, result:  " + JSON.stringify(want)); 
                expect(true).assertEqual(want !=null);
                done();
        });     
        await geolocation.off('fenceStatusChange',geofenceRequest,
            (want) => {
                if(err){
                    return console.info("fenceStatusChange callback  err:  " + err);
                }
                console.info("offfenceStatusChange callback, result:  " + JSON.stringify(want)); 
                expect(true).assertEqual(want !=null);
                done();
        });         
    })
        
    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_LOCATION_CHANGE_ON_0002
     * @tc.name testlocationChangeOn promise
     * @tc.desc Test cachedGnssLocationsReporting api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_cachedGnssLocationsReporting_On_test_001', 0, async function (done) {        
        let request = {"reportingPeriodSec": 0, "wakeUpCacheQueueFull": true};        
        await geolocation.on('cachedGnssLocationsReporting',request,
            result => {
            if(err){
                return console.info("oncachedGnssLocationsReporting callback  err:  " + err);
            }
            console.info("cachedGnssLocationsReporting result:  " + JSON.stringify(result)); 
            expect(true).assertEqual(result !=null);
            done();
        }); 
        console.info('LBS enableLocation callback test start ...');
        geolocation.enableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  enableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] enableLocation callback data: " + data);
                expect(data).assertTrue();
                console.info("[lbs_js] enableLocation callback exit .");
            }
            done()
        });
        await geolocation.off('cachedGnssLocationsReporting',request,
          (result) => {
            if(err){
                return console.info("cachedGnssLocationsReporting callback  err:  " + err);
            }
            console.info("offcachedGnssLocationsReporting callback " + JSON.stringify(result)); 
            expect(true).assertEqual(result !=null);
            done();
        });             
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_ON_LOCATION_SERVICE_STATE_0001
     * @tc.name testOnLocationServiceState
     * @tc.desc Test OnLocationServiceState api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_OnLocationServiceState_test_001', 0, async function (done) {
        await geolocation.on('locationServiceState', result => {
                console.info("onlocationServiceState callback, result:  " + JSON.stringify(result)); 
                expect(true).assertEqual(result !=null);
                done();
        }); 
        geolocation.enableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  enableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] enableLocation callback data: " + data);
                expect(data).assertTrue();
            }
            done()
        });    
    })

    /**
     * @tc.number SUB_LOCATION_geocode_DEVICE_JS_ON_LOCATION_SERVICE_STATE_0001
     * @tc.name testOnLocationServiceState
     * @tc.desc Test offLocationServiceState api .
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 2
     */
    it('geolocation_offLocationServiceState_test_001', 0, async function (done) {     
        geolocation.on('locationServiceState', async result => {
             console.info("locationServiceState callback, result:" + JSON.stringify(result));
             expect(true).assertEqual(result !=null);
             let promise = new Promise((resolve) => {
                geolocation.off('locationServiceState', result => {
                    console.info("offlocationServiceState callback, result:  " + JSON.stringify(result));
                    expect(true).assertEqual(result !=null);
                    resolve()
                });
            })
            await promise.then(done)
         });  
         geolocation.enableLocation((err, data) => {
            if (err) {
                console.info('[lbs_js]  enableLocation callback err is : ' + err );
            }else {
                console.info("[lbs_js] enableLocation callback data: " + data);
                expect(data).assertTrue();
            }
        done();  
        });       
    })
})

