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
import { AsyncCallback, Callback } from './basic.d.ts';
import WantAgent from '@ohos.wantAgent';

 /**
 * Provides interfaces for initiating location requests, ending the location service,
 * and obtaining the location result cached by the system.
 *
 * @since 7
 * @syscap SystemCapability.Location.Location.Core
 * @import import geolocation from '@ohos.geolocation'
 * @permission ohos.permission.LOCATION
 */
declare namespace geolocation {
    /**
     * subscribe location changed
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param request Indicates the location request parameters.
     * @param callback Indicates the callback for reporting the location result.
     */
    function on(type: 'locationChange', request: LocationRequest, callback: Callback<Location>) : void;

    /**
     * unsubscribe location changed
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the location result.
     */
    function off(type: 'locationChange', callback?: Callback<Location>) : void;

    /**
     * subscribe location switch changed
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the location result.
     */
    function on(type: 'locationServiceState', callback: Callback<boolean>) : void;

    /**
     * unsubscribe location switch changed
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the location result.
     */
    function off(type: 'locationServiceState', callback?: Callback<boolean>) : void;

    /**
     * subscribe to cache GNSS locations update messages
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param request Indicates the cached GNSS locations request parameters.
     * @param callback Indicates the callback for reporting the cached GNSS locations.
     */
    function on(type: 'cachedGnssLocationsReporting', request: CachedGnssLocationsRequest, callback: Callback<Array<Location>>) : void;

    /**
     * unsubscribe to cache GNSS locations update messages
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the cached gnss locations.
     */
    function off(type: 'cachedGnssLocationsReporting', callback?: Callback<Array<Location>>) : void;

    /**
     * subscribe gnss status changed
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the gnss status change.
     */
    function on(type: 'gnssStatusChange', callback: Callback<SatelliteStatusInfo>) : void;

    /**
     * unsubscribe gnss status changed
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the gnss status change.
     */
    function off(type: 'gnssStatusChange', callback?: Callback<SatelliteStatusInfo>) : void;

    /**
     * subscribe nmea message changed
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the nmea message.
     */
    function on(type: 'nmeaMessageChange', callback: Callback<string>) : void;

    /**
     * unsubscribe nmea message changed
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the nmea message.
     */
    function off(type: 'nmeaMessageChange', callback?: Callback<string>) : void;

    /**
     * add a geofence and subscribe geo fence status changed
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Geofence
     * @permission ohos.permission.LOCATION
     * @param request Indicates the Geo-fence configuration parameters.
     * @param callback Indicates the callback for reporting the fence status.
     */
    function on(type: 'fenceStatusChange', request: GeofenceRequest, want: WantAgent) : void;

    /**
     * remove a geofence and unsubscribe geo fence status changed
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Geofence
     * @permission ohos.permission.LOCATION
     * @param request Indicates the Geo-fence configuration parameters.
     * @param callback Indicates the callback for reporting the remove fence result.
     */
    function off(type: 'fenceStatusChange', request: GeofenceRequest, want: WantAgent) : void;

    /**
     * obtain current location
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the location result.
     */
    function getCurrentLocation(request: CurrentLocationRequest, callback: AsyncCallback<Location>) : void;
    function getCurrentLocation(callback: AsyncCallback<Location>) : void;
    function getCurrentLocation(request?: CurrentLocationRequest) : Promise<Location>;

    /**
     * obtain last known location
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the location result.
     */
    function getLastLocation(callback: AsyncCallback<Location>) : void;
    function getLastLocation() : Promise<Location>;

    /**
     * obtain current location switch status
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the location switch result.
     */
    function isLocationEnabled(callback: AsyncCallback<boolean>) : void;
    function isLocationEnabled() : Promise<boolean>;

    /**
     * request enable location
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the location switch status.
     */
    function requestEnableLocation(callback: AsyncCallback<boolean>) : void;
    function requestEnableLocation() : Promise<boolean>;

    /**
     * enable location switch
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @systemapi
     * @permission ohos.permission.MANAGE_SECURE_SETTINGS
     * @param callback Indicates the callback for reporting the location switch result.
     */
    function enableLocation(callback: AsyncCallback<boolean>) : void;
    function enableLocation() : Promise<boolean>;

    /**
     * disable location switch
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @systemapi
     * @permission ohos.permission.MANAGE_SECURE_SETTINGS
     * @param callback Indicates the callback for reporting the location switch result.
     */
    function disableLocation(callback: AsyncCallback<boolean>) : void;
    function disableLocation() : Promise<boolean>;

    /**
     * obtain address info from location
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Geocoder
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the address info.
     */
    function getAddressesFromLocation(request: ReverseGeoCodeRequest, callback: AsyncCallback<Array<GeoAddress>>) : void;
    function getAddressesFromLocation(request: ReverseGeoCodeRequest) : Promise<Array<GeoAddress>>;

    /**
     * obtain latitude and longitude info from location address
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Geocoder
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the latitude and longitude result.
     */
    function getAddressesFromLocationName(request: GeoCodeRequest, callback: AsyncCallback<Array<GeoAddress>>) : void;
    function getAddressesFromLocationName(request: GeoCodeRequest) : Promise<Array<GeoAddress>>;

    /**
     * obtain geocode service status
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Geocoder
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the geocode service status.
     */
    function isGeoServiceAvailable(callback: AsyncCallback<boolean>) : void;
    function isGeoServiceAvailable() : Promise<boolean>;

    /**
     * obtain the number of cached GNSS locations reported at a time
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the cached GNSS locations size.
     */
    function getCachedGnssLocationsSize(callback: AsyncCallback<number>) : void;
    function getCachedGnssLocationsSize() : Promise<number>;

    /**
     * all prepared GNSS locations are returned to the application through the callback function,
     * and the bottom-layer buffer is cleared.
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     * @param callback Indicates the callback for reporting the result.
     */
    function flushCachedGnssLocations(callback: AsyncCallback<boolean>) : void;
    function flushCachedGnssLocations() : Promise<boolean>;

    /**
     * send extended commands to location subsystem.
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param command Indicates the extended Command Message Body.
     * @param callback Indicates the callback for reporting the send command result.
     */
    function sendCommand(command: LocationCommand, callback: AsyncCallback<boolean>) : void;
    function sendCommand(command: LocationCommand) : Promise<boolean>;

    /**
     * satellite status information
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     */
    export interface SatelliteStatusInfo {
        satellitesNumber: number;
        satelliteIds: Array<number>;
        carrierToNoiseDensitys: Array<number>;
        altitudes: Array<number>;
        azimuths: Array<number>;
        carrierFrequencies: Array<number>;
    }

    /**
     * parameters for requesting to report cache location information
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Gnss
     * @permission ohos.permission.LOCATION
     */
    export interface CachedGnssLocationsRequest {
        reportingPeriodSec: number;
        wakeUpCacheQueueFull: boolean;
    }

    /**
     * configuring parameters in geo fence requests
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Geofence
     * @permission ohos.permission.LOCATION
     */
    export interface GeofenceRequest {
        priority: LocationRequestPriority;
        scenario: LocationRequestScenario;
        geofence: Geofence;
    }

    /**
     * configuring parameters in geo fence requests
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Geofence
     * @permission ohos.permission.LOCATION
     */
    export interface Geofence {
        latitude: number;
        longitude: number;
        radius: number;
        expiration: number;
    }

    /**
     * querying location privacy protocol confirmation status.
     *
     * @since 8
     * @systemapi
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param type indicates location privacy protocol type.
     * @param callback indicates the callback for reporting the location privacy protocol confirmation status.
     */
    function isLocationPrivacyConfirmed(type : LocationPrivacyType, callback: AsyncCallback<boolean>) : void;
    function isLocationPrivacyConfirmed(type : LocationPrivacyType,) : Promise<boolean>;

    /**
     * set location privacy protocol confirmation status.
     *
     * @since 8
     * @systemapi
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     * @param type indicates location privacy protocol type.
     * @param isConfirmed indicates whether the location privacy protocol has been confirmed.
     * @param callback Indicates the callback for reporting whether the action is set successfully.
     */
    function setLocationPrivacyConfirmStatus(type : LocationPrivacyType, isConfirmed : boolean, callback: AsyncCallback<boolean>) : void;
    function setLocationPrivacyConfirmStatus(type : LocationPrivacyType, isConfirmed : boolean) : Promise<boolean>;

    /**
     * configuring parameters in reverse geocode requests
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Geocoder
     * @permission ohos.permission.LOCATION
     */
    export interface ReverseGeoCodeRequest {
        locale?: string;
        latitude: number;
        longitude: number;
        maxItems?: number;
    }

    /**
     * configuring parameters in geocode requests
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Geocoder
     * @permission ohos.permission.LOCATION
     */
    export interface GeoCodeRequest {
        locale?: string;
        description: string;
        maxItems?: number;
        minLatitude?: number;
        minLongitude?: number;
        maxLatitude?: number;
        maxLongitude?: number;
    }

    /**
     * data struct describes geographic locations.
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Geocoder
     * @permission ohos.permission.LOCATION
     */
    export interface GeoAddress {
        latitude?: number;
        longitude?: number;
        locale?: string;
        placeName?: string;
        countryCode?: string;
        countryName?: string;
        administrativeArea?: string;
        subAdministrativeArea?: string;
        locality?: string;
        subLocality?: string;
        roadName?: string;
        subRoadName?: string;
        premises?: string;
        postalCode?: string;
        phoneNumber?: string;
        addressUrl?: string;
        descriptions?: Array<string>;
        descriptionsSize?: number;
    }

    /**
     * configuring parameters in location requests
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export interface LocationRequest {
        priority?: LocationRequestPriority;
        scenario?: LocationRequestScenario;
        timeInterval?: number;
        distanceInterval?: number;
        maxAccuracy?: number;
    }

    /**
     * configuring parameters in current location requests
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export interface CurrentLocationRequest {
        priority?: LocationRequestPriority;
        scenario?: LocationRequestScenario;
        maxAccuracy?: number;
        timeoutMs?: number;
    }

    /**
     * provides information about geographic locations
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export interface Location {
        latitude: number;
        longitude: number;
        altitude: number;
        accuracy: number;
        speed: number;
        timeStamp: number;
        direction: number;
        timeSinceBoot: number;
        additions?: Array<string>;
        additionSize?: number;
    }

    /**
     * enum for location priority
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export enum LocationRequestPriority {
        UNSET = 0x200,
        ACCURACY,
        LOW_POWER,
        FIRST_FIX,
    }

    /**
     * enum for location scenario
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export enum LocationRequestScenario {
        UNSET = 0x300,
        NAVIGATION,
        TRAJECTORY_TRACKING,
        CAR_HAILING,
        DAILY_LIFE_SERVICE,
        NO_POWER,
    }

    /**
     * enum for error code
     *
     * @since 7
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export enum GeoLocationErrorCode {
        INPUT_PARAMS_ERROR = 101,
        REVERSE_GEOCODE_ERROR,
        GEOCODE_ERROR,
        LOCATOR_ERROR,
        LOCATION_SWITCH_ERROR,
        LAST_KNOWN_LOCATION_ERROR,
        LOCATION_REQUEST_TIMEOUT_ERROR,
    }

    /**
     * enum for location privacy type
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export enum LocationPrivacyType {
        OTHERS = 0,
        STARTUP,
        CORE_LOCATION,
    }

    /**
     * Location subsystem command structure
     *
     * @since 8
     * @syscap SystemCapability.Location.Location.Core
     * @permission ohos.permission.LOCATION
     */
    export interface LocationCommand {
        scenario: LocationRequestScenario;
        command: string;
    }
}

export default geolocation;