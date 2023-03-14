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

/**
 * command parser.
 *
 */
public class CommandParser {
    /**
     * Parses and returns the hap list that supports the device type.
     */
    public static final String PARSE_MODE_HAPLIST = "hap-list";

    /**
     * Parses and returns the information about the hap.
     */
    public static final String PARSE_MODE_HAPINFO = "hap-info";

    private static final String CMD_MODE = "--mode";
    private static final String CMD_JSON_PATH = "--json-path";
    private static final String CMD_PROFILE_PATH = "--profile-path";
    private static final String CMD_INDEX_PATH = "--index-path";
    private static final String CMD_JS_PATH = "--js-path";
    private static final String CMD_ETS_PATH = "--ets-path";
    private static final String CMD_RPCID_PATH = "--rpcid-path";
    private static final String CMD_SO_PATH = "--maple-so-path";
    private static final String CMD_SO_DIR = "--maple-so-dir";
    private static final String CMD_ABILITY_SO_PATH = "--ability-so-path";
    private static final String CMD_DEX_PATH = "--dex-path";
    private static final String CMD_ABC_PATH = "--abc-path";
    private static final String CMD_FILE_PATH = "--file-path";
    private static final String CMD_LIB_PATH = "--lib-path";
    private static final String CMD_RES_PATH = "--res-path";
    private static final String CMD_RESOURCES_PATH = "--resources-path";
    private static final String CMD_ASSETS_PATH = "--assets-path";
    private static final String CMD_APK_PATH = "--shell-apk-path";
    private static final String CMD_HAP_PATH = "--hap-path";
    private static final String CMD_APP_PATH = "--app-path";
    private static final String CMD_SIGNATURE_PATH = "--signature-path";
    private static final String CMD_CERTIFICATE_PATH = "--certificate-path";
    private static final String CMD_FORCE = "--force";
    private static final String CMD_OUT_PATH = "--out-path";
    private static final String CMD_PACK_INFO_PATH = "--pack-info-path";
    private static final String CMD_BIN_PATH = "--bin-path";
    private static final String CMD_JAR_PATH = "--jar-path";
    private static final String CMD_TXT_PATH = "--txt-path";
    private static final String CMD_HAR_PATH = "--har-path";
    private static final String CMD_PARSE_MODE = "--p";
    private static final String CMD_PACK_RES_PATH = "--pack-res-path";
    private static final String CMD_UNPACKAPK = "--unpackapk";
    private static final String CMD_UNPACK_CUT_ENTRY_APK = "--unpack-cut_entry";
    private static final String CMD_SHAREDLIBS_PATH = "--shared-libs-path";
    private static final String CMD_ENTRYCARD_PATH = "--entrycard-path";
    private static final Log LOG = new Log(CommandParser.class.toString());

    /**
     * judge args is null and enter parser.
     *
     * @param utility common data
     * @param args command line
     * @return commandParser if input valid
     */
    public static boolean commandParser(Utility utility, String[] args) {
        if (args == null) {
            LOG.error("CommandParser::commandParser args is null!");
            return false;
        }

        parser(utility, args);
        return true;
    }

    /**
     * parse args.
     *
     * @param utility common data
     * @param args command line
     */
    private static void parser(Utility utility, String[] args) {
        for (int i = 0; i < args.length - 1; i++) {
            if (parseAppCmd(utility, args[i], args[i + 1]) || parseHapCmd(utility, args[i], args[i + 1])) {
                i++;
                continue;
            }
            switch (args[i]) {
                case CMD_MODE: {
                    utility.setMode(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_FORCE: {
                    utility.setForceRewrite(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_OUT_PATH: {
                    utility.setOutPath(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_BIN_PATH: {
                    utility.setBinPath(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_JAR_PATH: {
                    utility.setJarPath(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_TXT_PATH: {
                    utility.setTxtPath(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_HAR_PATH: {
                    utility.setHarPath(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_PARSE_MODE: {
                    utility.setParseMode(args[i + 1]);
                    if (PARSE_MODE_HAPLIST.equals(utility.getParseMode())) {
                        utility.setDeviceType(args[i + 2]);
                    } else if (PARSE_MODE_HAPINFO.equals(utility.getParseMode())) {
                        utility.setHapName(args[i + 2]);
                    }
                    i = i + 2;
                    break;
                }
                case CMD_UNPACKAPK: {
                    utility.setUnpackApk(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_UNPACK_CUT_ENTRY_APK: {
                    utility.setUnpackCutEntryApk(args[i + 1]);
                    i++;
                    break;
                }
                case CMD_PACK_INFO_PATH: {
                    utility.setPackInfoPath(args[i + 1]);
                    i++;
                    break;
                }

                case CMD_ENTRYCARD_PATH: {
                    utility.setEntryCardPath(args[i + 1]);
                    i++;
                    break;
                }

                case CMD_PACK_RES_PATH: {
                    utility.setPackResPath(args[i + 1]);
                    i++;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    /**
     * parse app args.
     *
     * @param utility common data
     * @param cmd command line
     * @param value the value to be set
     * @return parse result
     */
    private static boolean parseAppCmd(Utility utility, String cmd, String value) {
        switch (cmd) {
            case CMD_HAP_PATH: {
                utility.setHapPath(value);
                return true;
            }
            case CMD_APP_PATH: {
                utility.setAppPath(value);
                return true;
            }
            case CMD_CERTIFICATE_PATH: {
                utility.setCertificatePath(value);
                return true;
            }
            case CMD_SIGNATURE_PATH: {
                utility.setSignaturePath(value);
                return true;
            }
            case CMD_JSON_PATH: {
                utility.setJsonPath(value);
                return true;
            }
            case CMD_PACK_INFO_PATH: {
                utility.setPackInfoPath(value);
                return true;
            }
            case CMD_RES_PATH: {
                utility.setResPath(value);
                return true;
            }
            case CMD_PACK_RES_PATH: {
                utility.setPackResPath(value);
                return true;
            }
            default : {
                return false;
            }
        }
    }

    /**
     * parse hap args.
     *
     * @param utility common data
     * @param cmd command line
     * @param value the value to be set
     * @return parse result
     */
    private static boolean parseHapCmd(Utility utility, String cmd, String value) {
        switch (cmd) {
            case CMD_JSON_PATH: {
                utility.setJsonPath(value);
                return true;
            }
            case CMD_PROFILE_PATH: {
                utility.setProfilePath(value);
                return true;
            }
            case CMD_INDEX_PATH: {
                utility.setIndexPath(value);
                return true;
            }
            case CMD_SO_PATH: {
                utility.setSoPath(value);
                return true;
            }
            case CMD_ABILITY_SO_PATH: {
                utility.setAbilitySoPath(value);
                return true;
            }
            case CMD_SO_DIR: {
                utility.setSoDir(value);
                return true;
            }
            case CMD_DEX_PATH: {
                utility.setDexPath(value);
                return true;
            }
            case CMD_ABC_PATH: {
                utility.setAbcPath(value);
                return true;
            }
            case CMD_FILE_PATH: {
                utility.setFilePath(value);
                return true;
            }
            case CMD_LIB_PATH: {
                utility.setLibPath(value);
                return true;
            }
            case CMD_ASSETS_PATH: {
                utility.setAssetsPath(value);
                return true;
            }
            case CMD_RES_PATH: {
                utility.setResPath(value);
                return true;
            }
            case CMD_RESOURCES_PATH: {
                utility.setResourcesPath(value);
                return true;
            }
            case CMD_SHAREDLIBS_PATH: {
                utility.setSharedLibsPath(value);
                return true;
            }
            case CMD_APK_PATH: {
                utility.setApkPath(value);
                return true;
            }
            case CMD_JAR_PATH: {
                utility.setJarPath(value);
                return true;
            }
            case CMD_TXT_PATH: {
                utility.setTxtPath(value);
                return true;
            }
            case CMD_PACK_INFO_PATH: {
                utility.setPackInfoPath(value);
                return true;
            }
            case CMD_JS_PATH: {
                utility.setJsPath(value);
                return true;
            }
            case CMD_ETS_PATH: {
                utility.setEtsPath(value);
                return true;
            }
            case CMD_RPCID_PATH: {
                utility.setRpcidPath(value);
            }
            default : {
                return false;
            }
        }
    }
}
