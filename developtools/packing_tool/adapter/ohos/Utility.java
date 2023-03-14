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

import java.io.Closeable;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/**
 * common resource.
 *
 */
public class Utility {
    static final String MODE_HAP = "hap";
    static final String MODE_HAR = "har";
    static final String MODE_APP = "app";
    static final String MODE_RES = "res";
    static final String FALSE_STRING = "false";

    private static final String CMD_PREFIX = "--";
    private static final Log LOG = new Log(Utility.class.toString());

    private String mode = "";
    private String jsonPath = "";
    private String profilePath = "";
    private String soPath = "";
    private String soDir = "";
    private String abilitySoPath = "";
    private String dexPath = "";
    private String abcPath = "";
    private String indexPath = "";
    private String hapPath = "";
    private String appPath = "";
    private String libPath = "";
    private String resPath = "";
    private String resourcesPath = "";
    private String assetsPath = "";
    private String apkPath = "";
    private String certificatePath = "";
    private String signaturePath = "";
    private String forceRewrite = "false";
    private String outPath = "";
    private String packInfoPath = "";
    private String binPath = "";
    private boolean isCompressNativeLibs = true;
    private String moduleName = "";
    private String harPath = "";
    private String jarPath = "";
    private String txtPath = "";
    private String parseMode = "";
    private String deviceType = "";
    private String hapName = "";
    private String sharedLibsPath = "";
    private String unpackApk = "false";
    private String unpackCutEntryApk = "false";
    private String entryCardPath = "";
    private String packRes = "";
    private String packResPath = "";
    private String filePath = "";
    private String jsPath = "";
    private String etsPath = "";
    private String rpcidPath = "";
    private int VersionCode = -1;
    private String VersionName = "";
    private boolean isModuleJson = false;

    private List<String> formattedSoPathList = new ArrayList<>();
    private List<String> formattedAbilitySoPathList = new ArrayList<>();
    private List<String> formattedDexPathList = new ArrayList<>();
    private List<String> formattedAbcPathList = new ArrayList<>();
    private List<String> formattedHapPathList = new ArrayList<>();
    private List<String> formattedApkPathList = new ArrayList<>();
    private List<String> formattedJarPathList = new ArrayList<>();
    private List<String> formattedTxtPathList = new ArrayList<>();
    private List<String> formattedEntryCardPathList = new ArrayList<>();
    private List<String> formNameList = new ArrayList<>();

    public String getMode() {
        return mode;
    }

    public void setMode(String mode) {
        if (!mode.startsWith(CMD_PREFIX)) {
            this.mode = mode.toLowerCase(Locale.ENGLISH);
        }
    }

    public String getJsonPath() {
        return jsonPath;
    }

    public void setJsonPath(String jsonPath) {
        if (!jsonPath.startsWith(CMD_PREFIX)) {
            this.jsonPath = getFormattedPath(jsonPath);
        }
    }

    public String getProfilePath() {
        return profilePath;
    }

    public void setProfilePath(String profilePath) {
        if (!profilePath.startsWith(CMD_PREFIX)) {
            this.profilePath = getFormattedPath(profilePath);
        }
    }

    public String getSoPath() {
        return soPath;
    }

    public void setSoPath(String soPath) {
        if (!soPath.startsWith(CMD_PREFIX)) {
            this.soPath = soPath;
        }
    }

    public String getAbilitySoPath() {
        return abilitySoPath;
    }

    public void setAbilitySoPath(String abilitySoPath) {
        if (!abilitySoPath.startsWith(CMD_PREFIX)) {
            this.abilitySoPath = abilitySoPath;
        }
    }

    public String getSoDir() {
        return soDir;
    }

    public void setSoDir(String soDir) {
        if (!soDir.startsWith(CMD_PREFIX)) {
            this.soDir = soDir;
        }
    }

    public String getDexPath() {
        return dexPath;
    }

    public void setDexPath(String dexPath) {
        if (!dexPath.startsWith(CMD_PREFIX)) {
            this.dexPath = dexPath;
        }
    }

    public String getAbcPath() {
        return abcPath;
    }

    public void setAbcPath(String abcPath) {
        if (!abcPath.startsWith(CMD_PREFIX)) {
            this.abcPath = abcPath;
        }
    }

    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        if (!filePath.startsWith(CMD_PREFIX)) {
            this.filePath = filePath;
        }
    }

    public String getIndexPath() {
        return indexPath;
    }

    public void setIndexPath(String indexPath) {
        if (!indexPath.startsWith(CMD_PREFIX)) {
            this.indexPath = getFormattedPath(indexPath);
        }
    }

    public String getBinPath() {
        return binPath;
    }

    public void setBinPath(String binPath) {
        if (!binPath.startsWith(CMD_PREFIX)) {
            this.binPath = getFormattedPath(binPath);
        }
    }

    public void setIsModuleJson(boolean isModuleJson) {
        this.isModuleJson = isModuleJson;
    }

    public boolean getIsModuleJson() {
        return isModuleJson;
    }

    public boolean isCompressNativeLibs() {
        return isCompressNativeLibs;
    }

    public void setIsCompressNativeLibs(boolean isCompress) {
        this.isCompressNativeLibs = isCompress;
    }

    public String getHapPath() {
        return hapPath;
    }

    public void setHapPath(String hapPath) {
        if (!hapPath.startsWith(CMD_PREFIX)) {
            this.hapPath = hapPath;
        }
    }

    public String getAppPath() {
        return appPath;
    }

    public void setAppPath(String appPath) {
        if (!appPath.startsWith(CMD_PREFIX)) {
            this.appPath = appPath;
        }
    }

    public String getLibPath() {
        return libPath;
    }

    public void setLibPath(String libPath) {
        if (!libPath.startsWith(CMD_PREFIX)) {
            this.libPath = getFormattedPath(libPath);
        }
    }

    public String getResPath() {
        return resPath;
    }

    public void setResPath(String resPath) {
        if (!resPath.startsWith(CMD_PREFIX)) {
            this.resPath = getFormattedPath(resPath);
        }
    }

    public String getResourcesPath() {
        return resourcesPath;
    }

    public void setResourcesPath(String resourcesPath) {
        if (!resourcesPath.startsWith(CMD_PREFIX)) {
            this.resourcesPath = getFormattedPath(resourcesPath);
        }
    }

    public String getAssetsPath() {
        return assetsPath;
    }

    public void setAssetsPath(String assetsPath) {
        if (!assetsPath.startsWith(CMD_PREFIX)) {
            this.assetsPath = getFormattedPath(assetsPath);
        }
    }

    public String getModuleName() {
        if (moduleName == null) {
            moduleName = "";
        }
        return moduleName;
    }

    public void setModuleName(String moduleName) {
        this.moduleName = moduleName;
    }

    public String getApkPath() {
        return apkPath;
    }

    public void setApkPath(String apkPath) {
        if (!apkPath.startsWith(CMD_PREFIX)) {
            this.apkPath = apkPath;
        }
    }

    public String getCertificatePath() {
        return certificatePath;
    }

    public void setCertificatePath(String certificatePath) {
        if (!certificatePath.startsWith(CMD_PREFIX)) {
            this.certificatePath = getFormattedPath(certificatePath);
        }
    }

    public String getSignaturePath() {
        return signaturePath;
    }

    public void setSignaturePath(String signaturePath) {
        if (!signaturePath.startsWith(CMD_PREFIX)) {
            this.signaturePath = getFormattedPath(signaturePath);
        }
    }

    public String getForceRewrite() {
        return forceRewrite;
    }

    public void setForceRewrite(String forceRewrite) {
        if (!forceRewrite.startsWith(CMD_PREFIX)) {
            this.forceRewrite = forceRewrite.toLowerCase(Locale.ENGLISH);
        }
    }

    public String getOutPath() {
        return outPath;
    }

    public void setOutPath(String outPath) {
        if (!outPath.startsWith(CMD_PREFIX)) {
            this.outPath = getFormattedPath(outPath);
        }
    }

    public String getPackInfoPath() {
        return packInfoPath;
    }

    public void setPackInfoPath(String packInfoPath) {
        if (!packInfoPath.startsWith(CMD_PREFIX)) {
            this.packInfoPath = getFormattedPath(packInfoPath);
        }
    }

    public String getJarPath() {
        return jarPath;
    }

    public void setJarPath(String jarPath) {
        if (!jarPath.startsWith(CMD_PREFIX)) {
            this.jarPath = jarPath;
        }
    }

    public String getTxtPath() {
        return txtPath;
    }

    public void setTxtPath(String txtPath) {
        if (!txtPath.startsWith(CMD_PREFIX)) {
            this.txtPath = txtPath;
        }
    }

    public String getHarPath() {
        return harPath;
    }

    public void setHarPath(String harPath) {
        if (!harPath.startsWith(CMD_PREFIX)) {
            this.harPath = harPath;
        }
    }

    public String getParseMode() {
        return parseMode;
    }

    public void setParseMode(String parseMode) {
        if (!parseMode.startsWith(CMD_PREFIX)) {
            this.parseMode = parseMode;
        }
    }

    public String getDeviceType() {
        return deviceType;
    }

    public void setDeviceType(String deviceType) {
        if (!deviceType.startsWith(CMD_PREFIX)) {
            this.deviceType = deviceType;
        }
    }

    public String getUnpackApk() {
        return unpackApk;
    }

    public void setUnpackApk(String unpackApk) {
        this.unpackApk = unpackApk;
    }

    public String getUnpackCutEntryApk() {
        return unpackCutEntryApk;
    }

    public void setUnpackCutEntryApk(String unpackCutEntryApk) {
        this.unpackCutEntryApk = unpackCutEntryApk;
    }

    public String getHapName() {
        return hapName;
    }

    public void setHapName(String hapName) {
        if (!hapName.startsWith(CMD_PREFIX)) {
            this.hapName = hapName;
        }
    }

    public String getSharedLibsPath() {
        return sharedLibsPath;
    }

    public void setSharedLibsPath(String sharedLibsPath) {
        if (!sharedLibsPath.startsWith(CMD_PREFIX)) {
            this.sharedLibsPath = sharedLibsPath;
        }
    }

    public String getEntryCardPath() {
        return entryCardPath;
    }

    public void setEntryCardPath(String entryCardPath) {
        if (!entryCardPath.startsWith(CMD_PREFIX)) {
            this.entryCardPath = entryCardPath;
        }
    }

    public String getPackRes() {
        return packRes;
    }

    public void setPackRes(String packRes) {
        this.packRes = packRes;
    }

    public String getPackResPath() {
        return packResPath;
    }

    public void setPackResPath(String packResPath) {
        this.packResPath = packResPath;
    }

    public String getJsPath() { return jsPath; }

    public void setJsPath(String jsPath) { this.jsPath = jsPath; }

    public String getEtsPath() { return etsPath; }

    public void setEtsPath(String etsPath) { this.etsPath = etsPath; }

    public String getRpcidPath() {
        return rpcidPath;
    }

    public void setRpcidPath(String rpcidPath) {
        this.rpcidPath = rpcidPath;
    }

    public String getVersionName() {
        return VersionName;
    }

    public void setVersionName(String VersionName) {
        this.VersionName = VersionName;
    }

    public int getVersionCode() {
        return VersionCode;
    }

    public void setVersionCode(int VersionCode) {
        this.VersionCode = VersionCode;
    }

    public List<String> getFormattedSoPathList() {
        return formattedSoPathList;
    }

    public List<String> getFormattedAbilitySoPathList() {
        return formattedAbilitySoPathList;
    }

    public List<String> getFormattedDexPathList() {
        return formattedDexPathList;
    }

    public List<String> getFormattedAbcPathList() {
        return formattedAbcPathList;
    }

    public List<String> getFormattedHapPathList() {
        return formattedHapPathList;
    }

    public List<String> getFormattedApkPathList() {
        return formattedApkPathList;
    }

    public List<String> getFormattedJarPathList() {
        return formattedJarPathList;
    }

    public List<String> getFormattedTxtPathList() {
        return formattedTxtPathList;
    }

    /**
     * get dEntryCard path
     *
     * @return formattedEntryCardPathList
     */
    public List<String> getformattedEntryCardPathList() {
        return formattedEntryCardPathList;
    }

    /**
     * get canonical path
     *
     * @param path path input
     * @return formatted path
     */
    public String getFormattedPath(String path) {
        if (path == null) {
            return "";
        }

        File file = new File(path);
        String canonicalPath;
        try {
            canonicalPath = file.getCanonicalPath();
        } catch (IOException exception) {
            canonicalPath = "";
            LOG.error("Utility::getFormattedPath exception," + exception.getMessage());
        }
        return canonicalPath;
    }

    /**
     * close stream.
     *
     * @param stream stream to close
     */
    static void closeStream(Closeable stream) {
        if (stream != null) {
            try {
                stream.close();
            } catch (IOException exception) {
                LOG.error("Utility::closeStream io close exception: " + exception.getMessage());
            }
        }
    }

    /**
     * Add form name.
     *
     * @param formName the name of form
     */
    public void addFormNameList(String formName) {
        this.formNameList.add(formName);
    }

    /**
     * Get all form names
     *
     * @return all form names
     */
    public List<String> getFormNameList() {
        return formNameList;
    }
}
