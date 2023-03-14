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

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/**
 * compress commend parser.
 *
 */
public class CompressVerify {
    private static final String COMMA_SPLIT = ",";
    private static final String JSON_PROFILE = "config.json";
    private static final String MODULE_PROFILE = "module.json";
    private static final String PROFILE_NAME = "CAPABILITY.profile";
    private static final String INDEX_PROFILE = "resources.index";
    private static final String RPCID_PROFILE = "rpcid.sc";
    private static final String PACK_INFO = "pack.info";
    private static final String PACK_RES = "pack.res";
    private static final String HAP_SUFFIX = ".hap";
    private static final String HAR_SUFFIX = ".har";
    private static final String APP_SUFFIX = ".app";
    private static final String APK_SUFFIX = ".apk";
    private static final String DEX_SUFFIX = ".dex";
    private static final String ABC_SUFFIX = ".abc";
    private static final String SO_SUFFIX = ".so";
    private static final String JAR_SUFFIX = ".jar";
    private static final String TXT_SUFFIX = ".txt";
    private static final String PNG_SUFFIX = ".png";
    private static final String RES_SUFFIX = ".res";
    private static final String ENTRY_CARD_DIRECTORY_NAME = "EntryCard";

    private static final Log LOG = new Log(CompressVerify.class.toString());

    private static final boolean TYPE_FILE = true;
    private static final boolean TYPE_DIR = false;

    /**
     * if args valid.
     *
     * @param utility common data
     * @return commandVerify if command valid.
     */
    public static boolean commandVerify(Utility utility) {
        if (utility == null) {
            LOG.error("CompressVerify::commandVerify utility is null!");
            return false;
        }

        if (!utility.getForceRewrite().isEmpty() && !"true".equals(utility.getForceRewrite())
                && !"false".equals(utility.getForceRewrite())) {
            LOG.error("CompressVerify::commandVerify forceRewrite is invalid!");
            return false;
        }
        return commandPathVerify(utility);
    }

    /**
     * verify path.
     *
     * @param utility common data
     * @return commandPathVerify if command valid.
     */
    private static boolean commandPathVerify(Utility utility) {
        if (Utility.MODE_HAP.equals(utility.getMode())) {
            if (!utility.getJsonPath().isEmpty()) {
                File file = new File(utility.getJsonPath());
                if (!file.isFile()) {
                    LOG.error("CompressVerify::isArgsValidInHapMode json-path is not a file!");
                    return false;
                }
                if (!file.getName().equals(JSON_PROFILE) && !file.getName().equals(MODULE_PROFILE)) {
                    LOG.error("CompressVerify::isArgsValidInHapMode json-path must be config.json or module.json file!");
                    return false;
                }
            }

            if (!utility.getRpcidPath().isEmpty()) {
                File file = new File(utility.getRpcidPath());
                if (!file.isFile()) {
                    LOG.error("CompressVerify::isArgsValidInHapMode rpcid-path is not a file!");
                    return false;
                }
                if (!file.getName().equals(RPCID_PROFILE)) {
                    LOG.error("CompressVerify::isArgsValidInHapMode rpcid-path must be rpcid.sc file!");
                    return false;
                }
            }

            if (!utility.getApkPath().isEmpty() && !compatibleProcess(utility, utility.getApkPath(),
                    utility.getFormattedApkPathList(), APK_SUFFIX)) {
                LOG.error("CompressVerify::isArgsValidInHapMode shell-apk-path is invalid!");
                return false;
            }

            if (!utility.getProfilePath().isEmpty()) {
                File file = new File(utility.getProfilePath());
                if (!file.isFile() || !file.getName().equals(PROFILE_NAME)) {
                    LOG.error("CompressVerify::isArgsValidInHapMode profile-path must be CAPABILITY.profile file!");
                    return false;
                }
            }

            if (!utility.getDexPath().isEmpty() && !compatibleProcess(utility, utility.getDexPath(),
                    utility.getFormattedDexPathList(), DEX_SUFFIX)) {
                LOG.error("CompressVerify::isArgsValidInHapMode dex-path is invalid!");
                return false;
            }

            if (!utility.getAbcPath().isEmpty() && !compatibleProcess(utility, utility.getAbcPath(),
                    utility.getFormattedAbcPathList(), ABC_SUFFIX)) {
                LOG.error("CompressVerify::isArgsValidInHapMode abc-path is invalid!");
                return false;
            }

            return isVerifyValidInHapMode(utility);
        } else if (Utility.MODE_HAR.equals(utility.getMode())) {
            return isVerifyValidInHarMode(utility);
        } else if (Utility.MODE_APP.equals(utility.getMode())) {
            return isVerifyValidInAppMode(utility);
        } else if (Utility.MODE_RES.equals(utility.getMode())) {
            return isVerifyValidInResMode(utility);
        } else {
            LOG.error("CompressVerify::commandVerify mode is invalid!");
            return false;
        }
    }

    /**
     * parse and check args if valid in hap mode.
     *
     * @param utility common data
     * @return isVerifyValidInHapMode if verify valid in hap mode.
     */
    private static boolean isVerifyValidInHapMode(Utility utility) {
        File file = new File(utility.getIndexPath());
        if (!utility.getIndexPath().isEmpty() && !file.isFile() && file.getName().equals(INDEX_PROFILE)) {
            LOG.error("CompressVerify::isArgsValidInHapMode index-path must be resources.index file!");
            return false;
        }

        if (!utility.getSoPath().isEmpty() &&
                !compatibleProcess(utility, utility.getSoPath(), utility.getFormattedSoPathList(), SO_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInHapMode maple-so-path is invalid!");
            return false;
        }

        if (!utility.getAbilitySoPath().isEmpty() && !compatibleProcess(utility, utility.getAbilitySoPath(),
                utility.getFormattedAbilitySoPathList(), SO_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInHapMode ability-so-path is invalid!");
            return false;
        }

        if (isHapPathValid(utility.getSoDir())) {
            LOG.error("CompressVerify::isArgsValidInHapMode maple-so-dir is invalid!");
            return false;
        }

        if (isHapPathValid(utility.getLibPath())) {
            LOG.error("CompressVerify::isArgsValidInHapMode lib-path is invalid!");
            return false;
        }

        if (isHapPathValid(utility.getResPath())) {
            LOG.error("CompressVerify::isArgsValidInHapMode res-path is invalid!");
            return false;
        }

        if (isHapPathValid(utility.getResourcesPath())) {
            LOG.error("CompressVerify::isArgsValidInHapMode resources-path is invalid!");
            return false;
        }

        if (isHapPathValid(utility.getAssetsPath())) {
            LOG.error("CompressVerify::isArgsValidInHapMode assets-path is invalid!");
            return false;
        }

        if (isHapPathValid(utility.getSharedLibsPath())) {
            LOG.error("CompressVerify::isArgsValidInHapMode shared-libs-path is invalid!");
            return false;
        }

        if (!utility.getJarPath().isEmpty()
                && !compatibleProcess(utility, utility.getJarPath(), utility.getFormattedJarPathList(), JAR_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInHapMode jar-path is invalid!");
            return false;
        }

        if (!utility.getTxtPath().isEmpty()
                && !compatibleProcess(utility, utility.getTxtPath(), utility.getFormattedTxtPathList(), TXT_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInHapMode txt-path is invalid!");
            return false;
        }


        if (!utility.getPackInfoPath().isEmpty() && !isPathValid(utility.getPackInfoPath(), TYPE_FILE, PACK_INFO)) {
            LOG.error("CompressVerify::isArgsValidInHapMode pack-info-path is invalid!");
            return false;
        }

        return isOutPathValid(utility, HAP_SUFFIX);
    }

    /**
     * check hap path if valid
     *
     * @param path path input
     * @return isPathValid if path verify
     */
    private static boolean isHapPathValid(String path) {
        return (!path.isEmpty() && !isPathValid(path, TYPE_DIR, null));
    }

    /**
     * parse and check args if valid in har mode.
     *
     * @param utility common data
     * @return isVerifyValidInHarMode if verify valid in har mode.
     */
    private static boolean isVerifyValidInHarMode(Utility utility) {
        if (utility.getJsonPath().isEmpty()) {
            LOG.error("CompressVerify::isArgsValidInHarMode json-path is empty!");
            return false;
        }

        if (!isPathValid(utility.getJsonPath(), TYPE_FILE, JSON_PROFILE)
            && !isPathValid(utility.getJsonPath(), TYPE_FILE, MODULE_PROFILE)) {
            LOG.error("CompressVerify::isArgsValidInHarMode json-path must be config.json file!");
            return false;
        }

        if (!utility.getJarPath().isEmpty()
                && !compatibleProcess(utility, utility.getJarPath(), utility.getFormattedJarPathList(), JAR_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInHarMode jar-path is invalid!");
            return false;
        }

        if (!utility.getTxtPath().isEmpty()
                && !compatibleProcess(utility, utility.getTxtPath(), utility.getFormattedTxtPathList(), TXT_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInHarMode txt-path is invalid!");
            return false;
        }

        if (!utility.getLibPath().isEmpty() && !isPathValid(utility.getLibPath(), TYPE_DIR, null)) {
            LOG.error("CompressVerify::isArgsValidInHarMode lib-path is invalid!");
            return false;
        }

        if (!utility.getResPath().isEmpty() && !isPathValid(utility.getResPath(), TYPE_DIR, null)) {
            LOG.error("CompressVerify::isArgsValidInHarMode res-path is invalid!");
            return false;
        }

        if (utility.getResourcesPath().isEmpty() || !isPathValid(utility.getResourcesPath(), TYPE_DIR, null)) {
            LOG.error("CompressVerify::isArgsValidInHarMode resources-path is invalid!");
            return false;
        }

        if (!utility.getAssetsPath().isEmpty() && !isPathValid(utility.getAssetsPath(), TYPE_DIR, null)) {
            LOG.error("CompressVerify::isArgsValidInHarMode assets-path is invalid!");
            return false;
        }

        return isOutPathValid(utility, HAR_SUFFIX);
    }

    /**
     * parse and check args if valid in app mode.
     *
     * @param utility common data
     * @return isVerifyValidInAppMode if verify valid in app mode.
     */
    private static boolean isVerifyValidInAppMode(Utility utility) {
        if (utility.getHapPath().isEmpty()) {
            LOG.error("CompressVerify::isArgsValidInAppMode hap-path is empty!");
            return false;
        }

        if (!compatibleProcess(utility, utility.getHapPath(), utility.getFormattedHapPathList(), HAP_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInAppMode hap-path is invalid!");
            return false;
        }

        if (utility.getPackInfoPath().isEmpty()) {
            LOG.error("CompressVerify::isArgsValidInAppMode pack-info-path is empty!");
            return false;
        }

        File file = new File(utility.getPackInfoPath());
        if (!file.isFile() || !file.getName().equals(PACK_INFO)) {
            LOG.error("CompressVerify::isArgsValidInAppMode pack-info-path is invalid!");
            return false;
        }

        if (!utility.getSignaturePath().isEmpty() && !(new File(utility.getSignaturePath())).isFile()) {
            LOG.error("CompressVerify::isArgsValidInAppMode signature-path is invalid!");
            return false;
        }

        if (!utility.getCertificatePath().isEmpty() && !(new File(utility.getCertificatePath())).isFile()) {
            LOG.error("CompressVerify::isArgsValidInAppMode certificate-path is invalid!");
            return false;
        }

        if (!utility.getEntryCardPath().isEmpty() &&
                !compatibleProcess(utility, utility.getEntryCardPath(),
                        utility.getformattedEntryCardPathList(), PNG_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInAppMode entrycard-path is invalid!");
            return false;
        }
        if (!utility.getPackResPath().isEmpty() && !isPathValid(utility.getPackResPath(), TYPE_FILE, PACK_RES)) {
            LOG.error("CompressVerify::isArgsValidInAppMode pack-res-path is invalid!:" );
            return false;
        }

        return isOutPathValid(utility, APP_SUFFIX);
    }


    /**
     * parse and check args if valid in res mode.
     *
     * @param utility common data
     * @return isVerifyValidInAppMode if verify valid in app mode.
     */
    private static boolean isVerifyValidInResMode(Utility utility) {
        if (!isPathValid(utility.getPackInfoPath(), TYPE_FILE, PACK_INFO)) {
            LOG.error("CompressVerify::isArgsValidInResMode pack-info-path is invalid!");
            return false;
        }

        if (!isDirectoryValidStrictCase(utility.getEntryCardPath(), ENTRY_CARD_DIRECTORY_NAME)) {
            LOG.error("CompressVerify::isArgsValidInResMode the level-1 directory name must is EntryCard" +
                ", current is " + utility.getEntryCardPath());
            return false;
        }
        if (!compatibleProcess(utility, utility.getEntryCardPath(),
                utility.getformattedEntryCardPathList(), PNG_SUFFIX)) {
            LOG.error("CompressVerify::isArgsValidInResMode entrycard-path is invalid!");
            return false;
        }
        return isOutPathValid(utility, RES_SUFFIX);
    }


    /**
     * Compatible file input and directory input
     *
     * @param utility   common data
     * @param inputPath input path
     * @param fileList  save files' path with list
     * @param suffix    process type
     * @return Returns {@code true} if the compatible is successful; returns {@code false} otherwise.
     */
    private static boolean compatibleProcess(Utility utility, String inputPath,
        List<String> fileList, String suffix) {
        if (isPathValid(inputPath, TYPE_DIR, null)) {
            File inputFile = new File(inputPath);
            File[] files = inputFile.listFiles();
            if (files == null) {
                return true;
            }
            for (File fileItem : files) {
                if (fileItem.getName().toLowerCase(Locale.ENGLISH).endsWith(suffix)) {
                    fileList.add(fileItem.toString());
                }
            }
            return true;
        } else {
            String formattedPathItem = "";
            List<String> pathList = removeDuplicatePath(inputPath);
            for (String pathItem : pathList) {
                formattedPathItem = utility.getFormattedPath(pathItem);
                if (!isPathValid(formattedPathItem, TYPE_FILE, suffix)) {
                    return false;
                }
                fileList.add(formattedPathItem);
            }
            return true;
        }
    }

    /**
     * turn input path block to formatted path list
     *
     * @param utility common data
     * @param suffix  used to determine type
     * @return isVerifyValidInAppMode if verify valid in app mode.
     */
    private static boolean isOutPathValid(Utility utility, String suffix) {
        File outFile = new File(utility.getOutPath());

        if (("false".equals(utility.getForceRewrite())) && (outFile.exists())) {
            LOG.error("CompressVerify::isOutPathValid out file already existed!");
            return false;
        }

        if (HAP_SUFFIX.equals(suffix)) {
            if (!outFile.getName().toLowerCase(Locale.ENGLISH).endsWith(HAP_SUFFIX)) {
                LOG.error("CompressVerify::isOutPathValid out-path must end with .hap!");
                return false;
            } else {
                return true;
            }
        }

        if (HAR_SUFFIX.equals(suffix)) {
            if (!outFile.getName().toLowerCase(Locale.ENGLISH).endsWith(HAR_SUFFIX)) {
                LOG.error("CompressVerify::isOutPathValid out-path must end with .har!");
                return false;
            } else {
                return true;
            }
        }

        if (APP_SUFFIX.equals(suffix)) {
            if (!outFile.getName().toLowerCase(Locale.ENGLISH).endsWith(APP_SUFFIX)) {
                LOG.error("CompressVerify::isOutPathValid out-path must end with .app!");
                return false;
            } else {
                return true;
            }
        }

        if (RES_SUFFIX.equals(suffix)) {
            if (!outFile.getName().toLowerCase(Locale.ENGLISH).endsWith(RES_SUFFIX)) {
                LOG.error("CompressVerify::isOutPathValid out-path must end with .res!");
                return false;
            } else {
                return true;
            }
        }

        return false;
    }

    /**
     * check path if valid
     *
     * @param path   path input
     * @param isFile type input
     * @param flag   flag input
     * @return isPathValid if path verify
     */
    private static boolean isPathValid(String path, boolean isFile, String flag) {
        File file = new File(path);
        if (isFile && (file.isFile()) && file.getName().toLowerCase(Locale.ENGLISH).endsWith(flag)) {
            return true;
        }
        return (!isFile) && file.isDirectory();
    }

    private static boolean isDirectoryValidStrictCase(String path, String directoryName) {
        File file = new File(path);
        if (!file.exists()) {
            LOG.error("CompressVerify::isDirectoryValidStrictCase directory is not exist, directoryPath: "
                + path);
            return false;
        }
        if (file.isDirectory()) {
            return directoryName.equals(file.getName());
        }
        return false;
    }

    /**
     * remove duplicate in path.
     *
     * @param path input path, use comma separate.
     * @return result list
     */
    private static List<String> removeDuplicatePath(String path) {
        String[] array = path.split(COMMA_SPLIT);
        List<String> list = new ArrayList<>();

        for (String item : array) {
            if (!list.contains(item)) {
                list.add(item);
            }
        }
        return list;
    }
}
