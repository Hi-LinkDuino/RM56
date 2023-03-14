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

package ohos;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;
import java.util.Locale;

import ohos.utils.fastjson.JSONObject;
import ohos.utils.fastjson.JSONArray;

/**
 * Json Util.
 *
 */
public class JsonUtil {
    private static final String DEVICE_TYPE = "device-type";
    private static final String NAME = "name";
    private static final String MODULE_TYPE = "module-type";
    private static final String INSTALL_FLAG = "delivery-with-install";
    private static final String DEVICE_TYPE_NEW = "deviceType";
    private static final String MODULE_TYPE_NEW = "moduleType";
    private static final String INSTALL_FLAG_NEW = "deliveryWithInstall";
    private static final String ASSETS_DIR_NAME = "assets/";
    private static final Log LOG = new Log(JsonUtil.class.toString());

    private static final String SUMMARY = "summary";
    private static final String APP = "app";
    private static final String VERSION = "version";
    private static final String LEGACY_VERSION_CODE = "legacyVersionCode";
    private static final String LEGACY_VERSION_NAME = "legacyVersionName";
    private static final String MULTI_FRAMEWORK_BUNDLE = "multiFrameworkBundle";

    /**
     * parse hap list by device type
     *
     * @param deviceType target deviceType
     * @param jsonString uncompress json String
     * @return the parseHapList result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static List<PackInfo> parseHapList(String deviceType, String jsonString) throws BundleException {
        List<PackInfo> packInfos = new ArrayList<PackInfo>();
        JSONObject jsonObject = JSONObject.parseObject(jsonString);
        if (jsonObject == null || !jsonObject.containsKey("packages")) {
            LOG.error("Uncompress::parseHapList exception: packages is null");
            throw new BundleException("Parse hap list failed, packages is null");
        }
        JSONArray jsonList = JSONArray.parseArray(getJsonString(jsonObject, "packages"));
        if (jsonList == null) {
            LOG.error("Uncompress::parseHapList exception: packages is null");
            throw new BundleException("Parse hap list failed, packages is null");
        }
        int jsonListSize = jsonList.size();
        for (int i = 0; i < jsonListSize; i++) {
            JSONObject tmpObj = jsonList.getJSONObject(i);
            String deviceTypes = getJsonString(tmpObj, DEVICE_TYPE);
            if (deviceTypes == null || "".equals(deviceTypes)) {
                deviceTypes = getJsonString(tmpObj, DEVICE_TYPE_NEW);
            }
            if (deviceTypes != null && deviceTypes.toLowerCase(Locale.ENGLISH).contains(
                    deviceType.toLowerCase(Locale.ENGLISH))) {
                PackInfo packInfo = new PackInfo();
                packInfo.name = getJsonString(tmpObj, NAME);
                packInfo.moduleType = getJsonString(tmpObj, MODULE_TYPE);
                if (packInfo.moduleType == null || "".equals(packInfo.moduleType)) {
                    packInfo.moduleType = getJsonString(tmpObj, MODULE_TYPE_NEW);
                }
                packInfo.deviceType = JSONArray.parseArray(deviceTypes, String.class);
                String deliveryWithInstall = getJsonString(tmpObj, INSTALL_FLAG);
                if (deliveryWithInstall == null || "".equals(deliveryWithInstall)) {
                    deliveryWithInstall = getJsonString(tmpObj, INSTALL_FLAG_NEW);
                }
                packInfo.deliveryWithInstall = Boolean.parseBoolean(deliveryWithInstall);
                packInfos.add(packInfo);
            }
        }
        return packInfos;
    }

    private static boolean parseShellVersionInfoToAppInfo(String packInfoJsonStr, AppInfo appInfo) {
        LOG.info("Uncompress::parseShellVersionInfoToAppInfo: begin");
        if (!appInfo.isMultiFrameworkBundle()) {
            LOG.info("Uncompress::parseShellVersionInfoToAppInfo: is not a multi framewok bundle.");
            return false;
        }

        JSONObject jsonObject = JSONObject.parseObject(packInfoJsonStr);
        if (jsonObject == null) {
            LOG.error("Uncompress::parseShellVersionInfoToAppInfo error: summary is null");
            return false;
        }

        JSONObject summaryJson = jsonObject.getJSONObject(SUMMARY);
        if (summaryJson == null) {
            LOG.error("Uncompress::parseShellVersionInfoToAppInfo error: summary is null");
            return false;
        }
        JSONObject appJson = summaryJson.getJSONObject(APP);
        if (appJson == null) {
            LOG.error("Uncompress::parseShellVersionInfoToAppInfo error: app is null");
            return false;
        }
        JSONObject versionJson = appJson.getJSONObject(VERSION);
        if (versionJson == null) {
            LOG.error("Uncompress::parseShellVersionInfoToAppInfo error: version is null");
            return false;
        }

        if (!versionJson.containsKey(LEGACY_VERSION_CODE) || !versionJson.containsKey(LEGACY_VERSION_NAME)) {
            LOG.error("Uncompress::parseShellVersionInfoToAppInfo no legacy version info.");
            return false;
        }
        appInfo.setShellVersionCode(versionJson.getString(LEGACY_VERSION_CODE));
        appInfo.setShellVersionName(versionJson.getString(LEGACY_VERSION_NAME));
        return true;
    }

    private static void parseDeviceTypeToHapInfo(String packInfoJsonStr, HapInfo hapInfo, String hapName) {
        LOG.info("Uncompress::parseDeviceTypeToHapInfo: begin");
        JSONObject jsonObject = JSONObject.parseObject(packInfoJsonStr);
        if (jsonObject == null || !jsonObject.containsKey("packages")) {
            LOG.error("Uncompress::parseDeviceTypeToHapInfo error: no packages");
            return;
        }
        JSONArray jsonList = JSONArray.parseArray(getJsonString(jsonObject, "packages"));
        if (jsonList == null) {
            LOG.error("Uncompress::parseDeviceTypeToHapInfo error: packages is null");
            return;
        }

        for (int i = 0; i < jsonList.size(); i++) {
            JSONObject tmpObj = jsonList.getJSONObject(i);
            if (tmpObj == null) {
                LOG.error("Uncompress::parseDeviceTypeToHapInfo error: obj is null");
                continue;
            }
            String name = getJsonString(tmpObj, NAME);
            if (name != null && name.equals(hapName)) {
                String deviceTypes = getJsonString(tmpObj, DEVICE_TYPE_NEW);
                if (deviceTypes == null || deviceTypes.isEmpty()) {
                    deviceTypes = getJsonString(tmpObj, DEVICE_TYPE);
                }
                if (deviceTypes != null && !deviceTypes.isEmpty()) {
                    hapInfo.deviceType = JSONArray.parseArray(deviceTypes
                        .replace(UncompressEntrance.DEVICE_TYPE_DEFAULT, UncompressEntrance.DEVICE_TYPE_PHONE),
                        String.class);
                }
                break;
            }
        }
        return;
    }

    /**
     * parse hap profile info
     *
     * @param harmonyProfileJsonString uncompress json String
     * @param data resource index data
     * @param paclInfoJsonString pack.info json String
     * @param hapName hap file name
     * @return the parseProfileInfo result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static ProfileInfo parseProfileInfo(String harmonyProfileJsonString, byte[] data, String paclInfoJsonString,
        String hapName) throws BundleException {
        ProfileInfo profileInfo = new ProfileInfo();
        JSONObject jsonObject = JSONObject.parseObject(harmonyProfileJsonString);
        if (jsonObject == null || !jsonObject.containsKey(APP) || !jsonObject.containsKey("deviceConfig")
                || !jsonObject.containsKey("module")) {
            LOG.error("Uncompress::parseProfileInfo exception: app, deviceConfig or module is null");
            throw new BundleException("Parse profile info failed, app, deviceConfig or module is null");
        }
        if (jsonObject.containsKey(APP)) {
            JSONObject appJson = jsonObject.getJSONObject(APP);
            profileInfo.appInfo = parseAppInfo(appJson, data);
        }
        if (jsonObject.containsKey("module")) {
            JSONObject hapJson = jsonObject.getJSONObject("module");
            profileInfo.hapInfo = parseHapInfo(hapJson, data);
        }
        parseDeviceTypeToHapInfo(paclInfoJsonString, profileInfo.hapInfo, hapName);
        if (jsonObject.containsKey("deviceConfig")) {
            JSONObject deviceConfigJson = jsonObject.getJSONObject("deviceConfig");
            profileInfo.deviceConfig = parseDeviceConfigInfo(deviceConfigJson, profileInfo.hapInfo.deviceType);
        }
        if (!parseShellVersionInfoToAppInfo(paclInfoJsonString, profileInfo.appInfo)) {
            profileInfo.appInfo.setDefaultShellVersion();
        }
        if (!profileInfo.appInfo.appName.isEmpty()) {
            return profileInfo;
        }

        if (profileInfo.hapInfo.abilities.size() == 1) {
            profileInfo.appInfo.appName = profileInfo.hapInfo.abilities.get(0).label;
            profileInfo.appInfo.appNameEN = profileInfo.hapInfo.abilities.get(0).label;
        } else {
            for (AbilityInfo abilityInfo : profileInfo.hapInfo.abilities) {
                boolean isLabel = false;
                for (SkillInfo skill : abilityInfo.skills) {
                    if (skill.actions.contains("action.system.home") && skill.entities.contains("entity.system.home")) {
                        isLabel = true;
                        break;
                    }
                }
                if (isLabel) {
                    profileInfo.appInfo.appName = abilityInfo.label;
                    profileInfo.appInfo.appNameEN = abilityInfo.label;
                    break;
                }
            }
        }
        return profileInfo;
    }

    /**
     * parse app info
     *
     * @param appJson global json Object
     * @param data resource index data
     * @return the parseAppInfo result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static AppInfo parseAppInfo(JSONObject appJson, byte[] data) throws BundleException {
        AppInfo appInfo = new AppInfo();
        if (appJson == null) {
            LOG.error("Uncompress::parseAppInfo exception: appJson is null");
            throw new BundleException("Parse app info failed, appJson is null");
        }
        appInfo.bundleName = getJsonString(appJson, "bundleName");
        appInfo.vendor = getJsonString(appJson, "vendor");
        appInfo.relatedBundleName = getJsonString(appJson, "relatedBundleName");
        if (appJson.containsKey(VERSION)) {
            JSONObject version = appJson.getJSONObject(VERSION);
            appInfo.versionName = getJsonString(version, "name");
            appInfo.versionCode = getJsonString(version, "code");
        }
        if (appJson.containsKey("apiVersion")) {
            JSONObject apiVersion = appJson.getJSONObject("apiVersion");
            appInfo.compatibleApiVersion = apiVersion.getIntValue("compatible");
            appInfo.targetApiVersion = apiVersion.getIntValue("target");
            appInfo.releaseType = getJsonString(apiVersion, "releaseType");
        }
        String labelRes = "";
        if (appJson.containsKey("labelId")) {
            int labelId = appJson.getIntValue("labelId");
            labelRes = ResourcesParser.getBaseResourceById(labelId, data);
        }
        if (labelRes != null && !labelRes.isEmpty()) {
            appInfo.appName = labelRes;
            appInfo.appNameEN = labelRes;
        } else if (appJson.containsKey("label")) {
            appInfo.appName = getJsonString(appJson, "label");
            appInfo.appNameEN = getJsonString(appJson, "label");
        }
        appInfo.setMultiFrameworkBundle(appJson.getBooleanValue(MULTI_FRAMEWORK_BUNDLE));
        return appInfo;
    }

    /**
     * parse deviceConfig infos
     *
     * @param deviceConfigInfoJson deviceConfig json Object
     * @param deviceTypes the deviceTypes of the hap
     * @return the parseDeviceConfigInfo result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static Map<String, DeviceConfig> parseDeviceConfigInfo(JSONObject deviceConfigInfoJson,
            List<String> deviceTypes) throws BundleException {
        Map<String, DeviceConfig> deviceConfigs = new HashMap<>();
        if (deviceConfigInfoJson == null) {
            return deviceConfigs;
        }

        DeviceConfig defaultConfig = new DeviceConfig();
        if (deviceConfigInfoJson.containsKey("default")) {
            defaultConfig = parseDeviceConfig(deviceConfigInfoJson.getJSONObject("default"),
                    new DeviceConfig());
        }

        for (String deviceType : deviceTypes) {
            getTargetDeviceConfig(deviceConfigs, deviceConfigInfoJson, defaultConfig, deviceType);
        }

        if (deviceConfigs.isEmpty()) {
            deviceConfigs.put(UncompressEntrance.DEVICE_TYPE_PHONE, defaultConfig);
        }

        return deviceConfigs;
    }

    /**
     * parse deviceConfig infos
     *
     * @param deviceConfigs the parseDeviceConfigInfo result
     * @param deviceConfigInfoJson deviceConfig json Object
     * @param defaultConfig the default deviceConfig
     * @param targetDeviceType the target deviceType
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static void getTargetDeviceConfig(Map<String, DeviceConfig> deviceConfigs, JSONObject deviceConfigInfoJson,
            DeviceConfig defaultConfig, String targetDeviceType) throws BundleException {
        if (deviceConfigInfoJson.containsKey(targetDeviceType)) {
            DeviceConfig deviceConfig = parseDeviceConfig(
                    deviceConfigInfoJson.getJSONObject(targetDeviceType), defaultConfig);
            deviceConfigs.put(targetDeviceType, deviceConfig);
        } else {
            deviceConfigs.put(targetDeviceType, defaultConfig);
        }
    }

    /**
     * parse deviceConfig info
     *
     * @param deviceConfigJson deviceConfig json Object
     * @param defaultConfig default deviceConfig
     * @return the parseDeviceConfigInfo result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static DeviceConfig parseDeviceConfig(JSONObject deviceConfigJson, DeviceConfig defaultConfig)
            throws BundleException {
        DeviceConfig deviceConfig = new DeviceConfig();
        if (deviceConfigJson == null || defaultConfig == null) {
            LOG.error("Uncompress::parseDeviceConfigInfo exception: deviceConfigJson or defaultConfig is null");
            throw new BundleException("Parse device config info failed, deviceConfigJson or defaultConfig is null");
        }

        deviceConfig.jointUserid = getJsonString(deviceConfigJson, "jointUserid", defaultConfig.jointUserid);
        deviceConfig.process = getJsonString(deviceConfigJson, "process", defaultConfig.process);

        if (deviceConfigJson.containsKey("reqSdk")) {
            JSONObject reqSdk = deviceConfigJson.getJSONObject("reqSdk");
            deviceConfig.compatibleReqSdk = getJsonString(reqSdk, "compatible", defaultConfig.compatibleReqSdk);
            deviceConfig.targetReqSdk = getJsonString(reqSdk, "target", defaultConfig.targetReqSdk);
        }

        if (deviceConfigJson.containsKey("ark")) {
            JSONObject ark = deviceConfigJson.getJSONObject("ark");
            deviceConfig.arkFlag = getJsonString(ark, "flag", defaultConfig.arkFlag);
            if (ark.containsKey("reqVersion")) {
                JSONObject arkVersion = ark.getJSONObject("reqVersion");
                deviceConfig.targetArkVersion = getJsonString(arkVersion, "target", defaultConfig.targetArkVersion);
                deviceConfig.compatibleArkVersion = getJsonString(arkVersion, "compatible",
                        defaultConfig.compatibleArkVersion);
            }
        }

        if (deviceConfigJson.containsKey("directLaunch")) {
            deviceConfig.directLaunch = deviceConfigJson.getBoolean("directLaunch");
        } else {
            deviceConfig.directLaunch = defaultConfig.directLaunch;
        }

        return deviceConfig;
    }

    /**
     * parse hap info
     *
     * @param hapJson hap json Object
     * @param data resource index data
     * @return the parseHapInfo result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static HapInfo parseHapInfo(JSONObject hapJson, byte[] data) throws BundleException {
        HapInfo hapInfo = new HapInfo();
        if (hapJson == null) {
            LOG.error("Uncompress::parseHapInfo exception: hapJson is null");
            throw new BundleException("Parse hap info failed, hapJson is null");
        }
        hapInfo.packageStr = getJsonString(hapJson, "package");
        hapInfo.name = getJsonString(hapJson, "name");
        hapInfo.description = getJsonString(hapJson, "description");

        if (hapJson.containsKey("supportedModes")) {
            hapInfo.supportedModes = JSONObject.parseArray(getJsonString(hapJson, "supportedModes"),
                    String.class);
        }

        if (hapJson.containsKey("defPermissions")) {
            hapInfo.defPermissions = JSONArray.parseArray(getJsonString(hapJson, "defPermissions"),
                    DefPermission.class);
        }

        if (hapJson.containsKey("defPermissiongroups")) {
            hapInfo.defPermissionsGroups = JSONArray.parseArray(getJsonString(hapJson, "defPermissiongroups"),
                    DefPermissionGroup.class);
        }

        if (hapJson.containsKey("distro")) {
            getDistro(hapJson, hapInfo);
        }

        if (hapJson.containsKey("reqCapabilities")) {
            hapInfo.reqCapabilities = JSONArray.parseArray(getJsonString(hapJson, "reqCapabilities"),
                    String.class);
        }

        if (hapJson.containsKey("deviceType")) {
            hapInfo.deviceType = JSONArray.parseArray(getJsonString(hapJson, "deviceType")
                    .replace(UncompressEntrance.DEVICE_TYPE_DEFAULT, UncompressEntrance.DEVICE_TYPE_PHONE),
                    String.class);
        }

        if (hapJson.containsKey("metaData")) {
            JSONObject metaDataJson = hapJson.getJSONObject("metaData");
            hapInfo.metaData = parseMetaData(metaDataJson);
        }

        if (hapJson.containsKey("js")) {
            List<JsInfo> jsInfos = JSONObject.parseArray(getJsonString(hapJson, "js"), JsInfo.class);
            if (jsInfos != null && jsInfos.size() > 0) {
                hapInfo.isJs = true;
            }
        }

        if (hapJson.containsKey("reqPermissions")) {
            hapInfo.reqPermissions = JSONObject.parseArray(getJsonString(hapJson, "reqPermissions"),
                    ReqPermission.class);
        }

        if (hapJson.containsKey("commonEvents")) {
            hapInfo.commonEvents = JSONObject.parseArray(getJsonString(hapJson, "commonEvents"),
                    CommonEvent.class);
        }

        if (hapJson.containsKey("shortcuts")) {
            hapInfo.shortcuts = JSONObject.parseArray(getJsonString(hapJson, "shortcuts"), Shortcut.class);
        }

        if (hapJson.containsKey("abilities")) {
            JSONArray abilities = hapJson.getJSONArray("abilities");
            List<AbilityInfo> abilitieList = new ArrayList<AbilityInfo>();
            int abilitiesSize = abilities.size();
            for (int i = 0; i < abilitiesSize; i++) {
                JSONObject tmpObj = abilities.getJSONObject(i);
                abilitieList.add(parseAbility(tmpObj, data));
            }
            hapInfo.abilities = abilitieList;
        }
        if (hapJson.containsKey("distroFilter")) {
            hapInfo.distroFilter = JSONObject.parseObject(getJsonString(hapJson, "distroFilter"), DistroFilter.class);
        }
        return hapInfo;
    }

    /**
     * get distro
     *
     * @param hapJson hapJson json
     * @param hapInfo hapInfo json object
     */
    private static void getDistro(JSONObject hapJson, HapInfo hapInfo) {
        JSONObject distroObj = hapJson.getJSONObject("distro");
        Distro distro = new Distro();
        distro.moduleName = getJsonString(distroObj, "moduleName");
        distro.moduleType = getJsonString(distroObj, "moduleType");
        distro.deliveryWithInstall = distroObj.getBoolean("deliveryWithInstall");
        if (distroObj.containsKey("installationFree")) {
            boolean isFreeInstall = distroObj.getBoolean("installationFree");
            if (isFreeInstall) {
                distro.installationFree = 1;
            }else {
                distro.installationFree = 0;
            }
        } else {
            distro.installationFree = 2;
        }
        if (distroObj.containsKey("virtualMachine")) {
            distro.virtualMachine = getJsonString(distroObj, "virtualMachine");
        } else {
            distro.virtualMachine = "default";
        }
        hapInfo.distro = distro;
    }

    /**
     * parse meta data
     *
     * @param metaDataJson meta data json
     * @return the parseMetaData result
     */
    private static MetaData parseMetaData(JSONObject metaDataJson) {
        MetaData metaData = new MetaData();
        if (metaDataJson == null) {
            LOG.error("Uncompress::parseMetaData : metaDataJson is null");
            return metaData;
        }

        if (metaDataJson.containsKey("parameters")) {
            metaData.parameters = JSONObject.parseArray(getJsonString(metaDataJson, "parameters"),
                    MetaDataInfo.class);
        }
        if (metaDataJson.containsKey("results")) {
            metaData.results = JSONObject.parseArray(getJsonString(metaDataJson, "results"),
                    MetaDataInfo.class);
        }
        if (metaDataJson.containsKey("customizeData")) {
            metaData.customizeDatas = JSONObject.parseArray(getJsonString(metaDataJson, "customizeData"),
                    CustomizeData.class);
        }
        return metaData;
    }

    /**
     * parse ability object.
     *
     * @param abilityJson ability json object
     * @param data resource index data
     * @return the parseAbility result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static AbilityInfo parseAbility(JSONObject abilityJson, byte[] data) throws BundleException {
        if (abilityJson == null) {
            LOG.error("Uncompress::parseAbility exception: abilityJson is null");
            throw new BundleException("Parse ability failed, abilityJson is null");
        }
        AbilityInfo ability = new AbilityInfo();
        ability.name = getJsonString(abilityJson, "name");
        if (abilityJson.containsKey("iconId")) {
            int iconId = abilityJson.getIntValue("iconId");
            String iconPath = ResourcesParser.getResourceById(iconId, data);
            if (iconPath != null && !iconPath.isEmpty()) {
                ability.iconPath = ASSETS_DIR_NAME + iconPath;
            }
        }
        ability.icon = ability.iconPath != null && !ability.iconPath.isEmpty() ? ability.iconPath :
                getJsonString(abilityJson, "icon");

        if (abilityJson.containsKey("descriptionId")) {
            int descriptionId = abilityJson.getIntValue("descriptionId");
            ability.descriptionRes = ResourcesParser.getBaseResourceById(descriptionId, data);
        }
        ability.description = ability.descriptionRes != null && !ability.descriptionRes.isEmpty() ?
                ability.descriptionRes : getJsonString(abilityJson, "description");

        if (abilityJson.containsKey("labelId")) {
            int labelId = abilityJson.getIntValue("labelId");
            ability.labelRes = ResourcesParser.getBaseResourceById(labelId, data);
        }
        if (ability.labelRes != null && !ability.labelRes.isEmpty()) {
            ability.label = ability.labelRes;
        } else if (!getJsonString(abilityJson, "label").isEmpty()) {
            ability.label = getJsonString(abilityJson, "label");
        } else {
            ability.label = ability.name;
        }

        ability.type = getJsonString(abilityJson, "type");
        ability.launchType = getJsonString(abilityJson, "launchType");
        ability.orientation = getJsonString(abilityJson, "orientation");
        ability.uri = getJsonString(abilityJson, "uri");
        if (abilityJson.containsKey("formEnabled")) {
            ability.formEnabled = abilityJson.getBoolean("formEnabled");
        }

        if (abilityJson.containsKey("metaData")) {
            JSONObject metaDataJson = abilityJson.getJSONObject("metaData");
            ability.metaData = parseMetaData(metaDataJson);
        }

        if (abilityJson.containsKey("skills")) {
            ability.skills = JSONObject.parseArray(getJsonString(abilityJson, "skills"), SkillInfo.class);
        }

        if (abilityJson.containsKey("backgroundModes")) {
            ability.backgroundModes = JSONObject.parseArray(getJsonString(abilityJson, "backgroundModes"),
                    String.class);
        }

        if (abilityJson.containsKey("visible")) {
            ability.visible = abilityJson.getBoolean("visible");
        }

        if (abilityJson.containsKey("configChanges")) {
            ability.configChanges = JSONObject.parseArray(getJsonString(abilityJson, "configChanges"), String.class);
        }

        parseAbilityPermissions(abilityJson, ability);

        if (abilityJson.containsKey("forms")) {
            JSONArray forms = abilityJson.getJSONArray("forms");
            List<AbilityFormInfo> formList = new ArrayList<AbilityFormInfo>();
            int formSize = forms.size();
            for (int i = 0; i < formSize; i++) {
                JSONObject tmpObj = forms.getJSONObject(i);
                formList.add(parseAbilityFormInfo(tmpObj));
            }
            ability.formInfos = formList;
        }

        return ability;
    }

    /**
     * parse ability object.
     *
     * @param abilityJson ability json object
     * @param ability the parseAbility result
     * @return the parseAbility Permissions result
     * @throws BundleException Throws this exception if the json is not standard.
     */
    static void parseAbilityPermissions(JSONObject abilityJson, AbilityInfo ability) throws BundleException {
        if (abilityJson == null || ability == null) {
            LOG.error("Uncompress::parseAbilityPermissions exception: abilityJson or ability is null");
            throw new BundleException("Parse ability permissions failed, abilityJson or ability is null");
        }
        if (abilityJson.containsKey("permissions")) {
            ability.permissions = JSONObject.parseArray(getJsonString(abilityJson, "permissions"), String.class);
        }

        if (abilityJson.containsKey("grantPermission")) {
            ability.grantPermission = abilityJson.getBoolean("grantPermission");
        }
        ability.readPermission = getJsonString(abilityJson, "readPermission");
        ability.writePermission = getJsonString(abilityJson, "writePermission");

        if (abilityJson.containsKey("uriPermission")) {
            JSONObject uriPermission = abilityJson.getJSONObject("uriPermission");
            ability.uriPermissionMode = getJsonString(uriPermission, "mode");
            ability.uriPermissionPath = getJsonString(uriPermission, "path");
        }

        ability.mission = getJsonString(abilityJson, "mission");
        ability.targetAbility = getJsonString(abilityJson, "targetAbility");

        if (abilityJson.containsKey("directLaunch")) {
            ability.directLaunch = abilityJson.getBoolean("directLaunch");
        }

        if (abilityJson.containsKey("multiUserShared")) {
            ability.multiUserShared = abilityJson.getBoolean("multiUserShared");
        }

        if (abilityJson.containsKey("supportPipMode")) {
            ability.supportPipMode = abilityJson.getBoolean("supportPipMode");
        }

        if (abilityJson.containsKey("form")) {
            JSONObject form = abilityJson.getJSONObject("form");
            FormInfo formInfo = new FormInfo();
            if (form.containsKey("formEntity")) {
                formInfo.formEntity = JSONObject.parseArray(getJsonString(form, "formEntity"), String.class);
            }
            formInfo.minWidth = getJsonString(form, "minWidth");
            formInfo.defaultWidth = getJsonString(form, "defaultWidth");
            formInfo.minHeight = getJsonString(form, "minHeight");
            formInfo.defaultHeight = getJsonString(form, "defaultHeight");
            ability.formInfo = formInfo;
        }
    }
    /**
     * parse ability forms object
     *
     * @param abilityFormJson ability form json object
     * @return the ability form info result
     */
    static AbilityFormInfo parseAbilityFormInfo(JSONObject abilityFormJson) {
        AbilityFormInfo abilityForm = new AbilityFormInfo();
        if (abilityFormJson == null) {
            LOG.error("Uncompress::parseAbilityFormInfo : abilityFormJson is null");
            return abilityForm;
        }
        abilityForm.name = getJsonString(abilityFormJson, "name");
        abilityForm.type = getJsonString(abilityFormJson, "type");
        if (abilityFormJson.containsKey("updateEnabled")) {
            abilityForm.updateEnabled = abilityFormJson.getBoolean("updateEnabled");
        }
        abilityForm.scheduledUpdateTime = getJsonString(abilityFormJson, "scheduledUpdateTime");
        if (abilityFormJson.containsKey("updateDuration")) {
            abilityForm.updateDuration = abilityFormJson.getIntValue("updateDuration");
        }
        if (abilityFormJson.containsKey("supportDimensions")) {
            abilityForm.supportDimensions = JSONObject.parseArray(getJsonString(abilityFormJson, "supportDimensions"),
                    String.class);
        }
        abilityForm.defaultDimension = getJsonString(abilityFormJson, "defaultDimension");
        if (abilityFormJson.containsKey("metaData")) {
            JSONObject metaDataJson = abilityFormJson.getJSONObject("metaData");
            abilityForm.metaData = parseMetaData(metaDataJson);
        }
        return abilityForm;
    }

    /**
     * get the String from JSONObject by the key.
     *
     * @param jsonObject uncompress json object
     * @param key value key
     * @return the result
     */
    private static String getJsonString(JSONObject jsonObject, String key) {
        String value = "";
        if (jsonObject != null && jsonObject.containsKey(key)) {
            value = jsonObject.get(key).toString();
        }
        return value;
    }

    /**
     * get the String from JSONObject by the key.
     *
     * @param jsonObject uncompress json object
     * @param key value key
     * @param defaultValue the default value
     * @return the result
     */
    private static String getJsonString(JSONObject jsonObject, String key, String defaultValue) {
        String value = defaultValue;
        if (jsonObject != null && jsonObject.containsKey(key)) {
            value = jsonObject.get(key).toString();
        }
        return value;
    }
}
