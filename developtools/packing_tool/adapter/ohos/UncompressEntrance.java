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

/**
 * bundle uncompress.
 * step1: parse arguments
 * step2: verity arguments
 * step3: uncompress arguments
 *
 */
public class UncompressEntrance {
    /**
     * Parses and returns the hap list that supports the device type.
     */
    public static final String PARSE_MODE_HAPLIST = "hap-list";

    /**
     * Parses and returns the information about the hap.
     */
    public static final String PARSE_MODE_HAPINFO = "hap-info";

    /**
     * Parses and returns the informations about the hap list that supports the device type and the haps in the app.
     */
    public static final String PARSE_MODE_ALL = "all";

    /**
     * Device type of default.
     */
    public static final String DEVICE_TYPE_DEFAULT = "default";

    /**
     * Device type of phone.
     */
    public static final String DEVICE_TYPE_PHONE = "phone";

    /**
     * Device type of tablet.
     */
    public static final String DEVICE_TYPE_TABLET = "tablet";

    /**
     * Device type of tv.
     */
    public static final String DEVICE_TYPE_TV = "tv";

    /**
     * Device type of car.
     */
    public static final String DEVICE_TYPE_CAR = "car";

    /**
     * Device type of smartWatch.
     */
    public static final String DEVICE_TYPE_SMARTWATCH = "smartWatch";

    /**
     * Device type of fitnessWatch.
     */
    public static final String DEVICE_TYPE_FITNESSWATCH = "fitnessWatch";

    /**
     * Device type of fitnessBand.
     */
    public static final String DEVICE_TYPE_FITNESSBAND = "fitnessBand";

    private static final int EXIT_STATUS_NORMAL = 0;
    private static final int EXIT_STATUS_EXCEPTION = 1;
    private static final Log LOG = new Log(UncompressEntrance.class.toString());

    /**
     * Unpack the app.
     *
     * @param appPath Indicates the app path.
     * @param outPath Indicates the out path.
     * @param deviceType Indicates the device type supported by the haps.If this parameter is null, all the haps will
     *                   be unpacked.
     * @param unpackApk Indicates whether to decompress the apk file in the hap.The default value is {@code false},
     *                  not unpack the apk file.
     * @return Return the unpack result.
     */
    public static boolean unpack(String appPath, String outPath, String deviceType, boolean unpackApk) {
        if (appPath == null || appPath.isEmpty()) {
            LOG.error("UncompressEntrance::unpack appPath is invalid!");
            return false;
        }

        if (outPath == null || outPath.isEmpty()) {
            LOG.error("UncompressEntrance::unpack outPath is invalid!");
            return false;
        }

        Utility utility = new Utility();
        utility.setMode(Utility.MODE_APP);
        utility.setAppPath(appPath);
        utility.setDeviceType(deviceType == null ? "" : deviceType);
        utility.setOutPath(outPath);
        utility.setUnpackApk(String.valueOf(unpackApk));
        utility.setForceRewrite("true");

        if (!UncompressVerify.commandVerify(utility)) {
            LOG.error("CompressEntrance::unpack verity failed");
            return false;
        }

        if (!Uncompress.unpackageProcess(utility)) {
            LOG.error("UncompressEntrance::unpackageProcess failed");
            return false;
        }

        return true;
    }

    /**
     * Unpack the hap.
     *
     * @param hapPath Indicates the hap path.
     * @param outPath Indicates the out path.
     * @param unpackApk Indicates whether to decompress the apk file in the hap.The default value is {@code false},
     *                  not unpack the apk file.
     * @return Return the unpack result.
     */
    public static boolean unpackHap(String hapPath, String outPath, boolean unpackApk) {
        if (hapPath == null || hapPath.isEmpty()) {
            LOG.error("UncompressEntrance::unpackHap hapPath is invalid!");
            return false;
        }

        if (outPath == null || outPath.isEmpty()) {
            LOG.error("UncompressEntrance::unpackHap outPath is invalid!");
            return false;
        }

        Utility utility = new Utility();
        utility.setMode(Utility.MODE_HAP);
        utility.setHapPath(hapPath);
        utility.setDeviceType("");
        utility.setOutPath(outPath);
        utility.setUnpackApk(String.valueOf(unpackApk));
        utility.setForceRewrite("true");

        if (!UncompressVerify.commandVerify(utility)) {
            LOG.error("CompressEntrance::unpackHap verity failed");
            return false;
        }

        if (!Uncompress.unpackageProcess(utility)) {
            LOG.error("UncompressEntrance::unpackageProcess failed");
            return false;
        }

        return true;
    }

    /**
     * Parse the app.
     *
     * @param appPath Indicates the app path.
     * @param parseMode Indicates the parse mode, which can be {@link #PARSE_MODE_HAPLIST}, {@link #PARSE_MODE_HAPINFO},
     *                  {@link #PARSE_MODE_ALL}.
     * @param deviceType Indicates the device type supported by the haps, This parameter is required
     *                   when {@code #parseMode} is {@link #PARSE_MODE_HAPLIST}.
     * @param hapName Indicates the hap name, This parameter is required when {@code #parseMode}
     *                is {@link #PARSE_MODE_HAPINFO}.
     * @param outPath Indicates the out path to unpack the files.
     * @return Return the uncomperss result of parseApp
     */
    public static UncomperssResult parseApp(String appPath, String parseMode, String deviceType, String hapName,
            String outPath) {
        UncomperssResult compressResult = new UncomperssResult();

        if (appPath == null || appPath.isEmpty()) {
            LOG.error("UncompressEntrance::parseApp appPath is invalid!");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp appPath is invalid");
            return compressResult;
        }

        if (parseMode == null || parseMode.isEmpty()) {
            LOG.error("UncompressEntrance::parseApp parseMode is invalid!");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp parseMode is invalid");
            return compressResult;
        }

        Utility utility = new Utility();
        utility.setMode(Utility.MODE_APP);
        utility.setAppPath(appPath);
        utility.setParseMode(parseMode);
        utility.setDeviceType(deviceType == null ? "" : deviceType);
        utility.setHapName(hapName == null ? "" : hapName);
        utility.setForceRewrite("true");
        if (outPath != null && !outPath.isEmpty()) {
            utility.setOutPath(outPath);
        }

        if (!UncompressVerify.commandVerify(utility)) {
            LOG.error("CompressEntrance::parseApp verity failed");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp verity failed");
            return compressResult;
        }

        compressResult = Uncompress.uncompressApp(utility);

        return compressResult;
    }

    /**
     * Parse the app.
     *
     * @param input Indicates the InputStream about the app package.
     * @param parseMode Indicates the parse mode, which can be {@link #PARSE_MODE_HAPLIST}, {@link #PARSE_MODE_HAPINFO},
     *                  {@link #PARSE_MODE_ALL}.
     * @param deviceType Indicates the device type supported by the haps, This parameter is required
     *                   when {@code #parseMode} is {@link #PARSE_MODE_HAPLIST}.
     * @param hapName Indicates the hap name, This parameter is required when {@code #parseMode}
     *                is {@link #PARSE_MODE_HAPINFO}.
     * @param outPath Indicates the out path to unzip temp files.
     * @return Return the uncomperss result of parseApp
     */
    public static UncomperssResult parseApp(InputStream input, String parseMode, String deviceType, String hapName,
            String outPath) {
        UncomperssResult compressResult = new UncomperssResult();

        if (input == null) {
            LOG.error("UncompressEntrance::parseApp input is null!");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp input is null");
            return compressResult;
        }

        if (parseMode == null || parseMode.isEmpty()) {
            LOG.error("UncompressEntrance::parseApp parseMode is invalid!");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp parseMode is invalid");
            return compressResult;
        }

        Utility utility = new Utility();
        utility.setMode(Utility.MODE_APP);
        utility.setParseMode(parseMode);
        utility.setDeviceType(deviceType == null ? "" : deviceType);
        utility.setHapName(hapName == null ? "" : hapName);
        utility.setForceRewrite("true");
        utility.setOutPath(outPath == null ? "" : outPath);

        if (!UncompressVerify.inputParseVerify(utility, input)) {
            LOG.error("CompressEntrance::parseApp verity failed");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp verity failed");
            return compressResult;
        }

        compressResult = Uncompress.uncompressAppByInput(utility, input);

        return compressResult;
    }

    /**
     * Parse the hap.
     *
     * @param hapPath Indicates the hap path.
     * @return Return the uncomperss result of parseHap
     */
    public static UncomperssResult parseHap(String hapPath) {
        UncomperssResult compressResult = new UncomperssResult();

        if (hapPath == null || hapPath.isEmpty()) {
            LOG.error("UncompressEntrance::parseHap hapPath is invalid!");
            compressResult.setResult(false);
            compressResult.setMessage("ParseHap hapPath is invalid");
            return compressResult;
        }

        Utility utility = new Utility();
        utility.setMode(Utility.MODE_HAP);
        utility.setParseMode(PARSE_MODE_HAPINFO);
        utility.setHapPath(hapPath);
        utility.setForceRewrite("true");

        if (!UncompressVerify.commandVerify(utility)) {
            LOG.error("UncompressEntrance::parseHap verity failed");
            compressResult.setResult(false);
            compressResult.setMessage("ParseHap verity failed");
            return compressResult;
        }

        compressResult = Uncompress.uncompressHap(utility);

        return compressResult;
    }

    /**
     * Parse the hap.
     *
     * @param input Indicates the InputStream about the app package.
     * @return Return the uncomperss result of parseHap
     */
    public static UncomperssResult parseHap(InputStream input) {
        UncomperssResult compressResult = new UncomperssResult();

        if (input == null) {
            LOG.error("UncompressEntrance::parseHap input is null!");
            compressResult.setResult(false);
            compressResult.setMessage("ParseHap input is null");
            return compressResult;
        }

        Utility utility = new Utility();
        utility.setMode(Utility.MODE_HAP);
        utility.setParseMode(PARSE_MODE_HAPINFO);
        utility.setForceRewrite("true");

        if (!UncompressVerify.inputParseVerify(utility, input)) {
            LOG.error("UncompressEntrance::parseHap verity failed");
            compressResult.setResult(false);
            compressResult.setMessage("ParseHap verity failed");
            return compressResult;
        }

        compressResult = Uncompress.uncompressHapByInput(utility, input);

        return compressResult;
    }

    /**
     * uncompress tool main function.
     *
     * @param args command line
     */
    public static void main(String[] args) {
        Utility utility = new Utility();

        if (!CommandParser.commandParser(utility, args)) {
            LOG.error("UncompressEntrance::main exit, parser failed");
            ShowHelp.uncompressHelp();
            System.exit(EXIT_STATUS_EXCEPTION);
        }

        if (!UncompressVerify.commandVerify(utility)) {
            LOG.error("UncompressEntrance::main exit, verity failed");
            ShowHelp.uncompressHelp();
            System.exit(EXIT_STATUS_EXCEPTION);
        }

        if (!Uncompress.unpackageProcess(utility)) {
            LOG.error("UncompressEntrance::main exit, uncompress failed");
            ShowHelp.uncompressHelp();
            System.exit(EXIT_STATUS_EXCEPTION);
        }

        System.exit(EXIT_STATUS_NORMAL);
    }
}