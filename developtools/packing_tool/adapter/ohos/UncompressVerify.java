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
import java.io.InputStream;
import java.util.Locale;

/**
 * uncompress command verify.
 *
 */
public class UncompressVerify {
    private static final String HAP_SUFFIX = ".hap";
    private static final String HAR_SUFFIX = ".har";
    private static final String APP_SUFFIX = ".app";
    private static final Log LOG = new Log(UncompressVerify.class.toString());

    /**
     * is args valid.
     *
     * @param utility common data
     * @return commandVerify if verify valid.
     */
    public static boolean commandVerify(Utility utility) {
        if (!utility.getForceRewrite().isEmpty() && !"true".equals(utility.getForceRewrite())
            && !"false".equals(utility.getForceRewrite())) {
            LOG.error("UncompressVerify::isVerifyVaild forceRewrite is invalid!");
            return false;
        }

        if (!utility.getParseMode().isEmpty() && !UncompressEntrance.PARSE_MODE_HAPLIST.equals(utility.getParseMode())
                && !UncompressEntrance.PARSE_MODE_HAPINFO.equals(utility.getParseMode())
                && !UncompressEntrance.PARSE_MODE_ALL.equals(utility.getParseMode())) {
            LOG.error("UncompressVerify::isVerifyVaild parseMode is invalid!");
            return false;
        }

        if (Utility.MODE_HAP.equals(utility.getMode())) {
            return hapCommandVerify(utility);
        } else if (Utility.MODE_HAR.equals(utility.getMode())) {
            return harCommandVerify(utility);
        } else if (Utility.MODE_APP.equals(utility.getMode())) {
            return appCommandVerify(utility);
        } else {
            LOG.error("UncompressVerify::commandVerify mode is invalid!");
            return false;
        }
    }

    /**
     * is args valid.
     *
     * @param utility common data
     * @param input the InputStream about the app package.
     * @return commandVerify if verify valid.
     */
    public static boolean inputParseVerify(Utility utility, InputStream input) {
        if (input == null) {
            LOG.error("UncompressVerify::isInputVerifyVaild input is invalid!");
            return false;
        }

        if (!utility.getForceRewrite().isEmpty() && !"true".equals(utility.getForceRewrite())
                && !"false".equals(utility.getForceRewrite())) {
            LOG.error("UncompressVerify::isInputVerifyVaild forceRewrite is invalid!");
            return false;
        }

        if (utility.getParseMode().isEmpty() || (!UncompressEntrance.PARSE_MODE_HAPLIST.equals(utility.getParseMode())
                && !UncompressEntrance.PARSE_MODE_HAPINFO.equals(utility.getParseMode()))
                && !UncompressEntrance.PARSE_MODE_ALL.equals(utility.getParseMode())) {
            LOG.error("UncompressVerify::isInputVerifyVaild parseMode is invalid!");
            return false;
        }

        if (Utility.MODE_HAP.equals(utility.getMode())) {
            return true;
        } else if (Utility.MODE_HAR.equals(utility.getMode())) {
            return true;
        } else if (Utility.MODE_APP.equals(utility.getMode())) {
            if (UncompressEntrance.PARSE_MODE_HAPINFO.equals(utility.getParseMode())
                    && utility.getHapName().isEmpty()) {
                LOG.error("UncompressVerify::isArgsVaildInParseMode hapName can't be empty!");
                return false;
            }

            if (!utility.getUnpackApk().isEmpty() && !"true".equals(utility.getUnpackApk())
                    && !"false".equals(utility.getUnpackApk())) {
                LOG.error("UncompressVerify::isVerifyVaild unpackapk is invalid!");
                return false;
            }

            return true;
        } else {
            LOG.error("UncompressVerify::InputcommandVerify mode is invalid!");
            return false;
        }
    }

    /**
     * parse and check args if valid in hap mode.
     *
     * @param utility common data
     * @return isVerifyValidInHapMode if verify valid in hap mode.
     */
    private static boolean hapCommandVerify(Utility utility) {
        utility.setHapPath(utility.getFormattedPath(utility.getHapPath()));
        File file = new File(utility.getHapPath());
        if (!file.isFile() || !file.getName().toLowerCase(Locale.ENGLISH).endsWith(HAP_SUFFIX)) {
            LOG.error("UncompressVerify::isArgsValidInHapMode hap-path must end with.hap!");
            return false;
        }

        return verifyOutPath(utility, file);
    }

    /**
     * parse and check args if valid in har mode.
     *
     * @param utility common data
     * @return isVerifyValidInHapMode if verify valid in hap mode.
     */
    private static boolean harCommandVerify(Utility utility) {
        utility.setHarPath(utility.getFormattedPath(utility.getHarPath()));
        File file = new File(utility.getHarPath());
        if (!file.isFile() || !file.getName().toLowerCase(Locale.ENGLISH).endsWith(HAR_SUFFIX)) {
            LOG.error("UncompressVerify::isArgsValidInHarMode har-path must end with.har!");
            return false;
        }

        if (utility.getOutPath().isEmpty()) {
            LOG.error("UncompressVerify::isVerifyVaild outPath is invalid!");
            return false;
        }

        return verifyOutPath(utility, file);
    }

    /**
     * parse and check args if valid in app mode.
     *
     * @param utility common data
     * @return isVerifyValidInHapMode if verify valid in hap mode.
     */
    private static boolean appCommandVerify(Utility utility) {
        if (UncompressEntrance.PARSE_MODE_HAPINFO.equals(utility.getParseMode())
                && utility.getHapName().isEmpty()) {
            LOG.error("UncompressVerify::isArgsVaildInParseMode hapName can't be empty!");
            return false;
        }

        utility.setAppPath(utility.getFormattedPath(utility.getAppPath()));
        File file = new File(utility.getAppPath());
        if (!file.isFile() || !file.getName().toLowerCase(Locale.ENGLISH).endsWith(APP_SUFFIX)) {
            LOG.error("UncompressVerify::isArgsValidInAppMode app-path must end with.app!");
            return false;
        }

        return verifyOutPath(utility, file);
    }


    /**
     * parse and check the outpath args.
     *
     * @param utility common data
     * @param file file to be verified
     * @return isVerifyValidInHapMode if verify valid in hap mode.
     */
    private static boolean verifyOutPath(Utility utility, File file) {
        if (utility.getOutPath().isEmpty() && utility.getParseMode().isEmpty()) {
            String tempStr = file.getName();
            File parentFile = file.getParentFile();
            if (parentFile == null) {
                utility.setOutPath(tempStr.substring(0, tempStr.lastIndexOf(".")));
            } else {
                utility.setOutPath(parentFile.toString() + File.separator
                    + tempStr.substring(0, tempStr.lastIndexOf(".")));
            }
        }

        File outFile = new File(utility.getOutPath());
        if (("false".equals(utility.getForceRewrite())) && (outFile.exists())) {
            LOG.error("UncompressVerify::isArgsValidInHapMode out file already existed!");
            return false;
        }
        return true;
    }

    /**
     * check path if valid
     *
     * @param path path input
     * @param isFile type input
     * @param flag flag input
     * @return isPathValid if path verify
     */
    public static boolean isPathValid(String path, boolean isFile, String flag) {
        File file = new File(path);
        if (isFile && (file.isFile()) && file.getName().toLowerCase(Locale.ENGLISH).endsWith(flag)) {
            return true;
        }
        return (!isFile) && file.isDirectory();
    }
}