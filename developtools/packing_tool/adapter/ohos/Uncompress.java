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

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.file.attribute.FileTime;
import java.util.Enumeration;
import java.util.List;
import java.util.Locale;
import java.util.zip.CRC32;
import java.util.zip.CheckedOutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

/**
 * bundle uncompress.
 *
 */
public class Uncompress {
    private static final String HAP_SUFFIX = ".hap";
    private static final String APK_SUFFIX = ".apk";
    private static final String JSON_SUFFIX = ".json";
    private static final String PACK_INFO = "pack.info";
    private static final String HARMONY_PROFILE = "config.json";
    private static final String RESOURCE_INDEX = "resources.index";
    private static final String LINUX_FILE_SEPARATOR = "/";
    private static final String TEMP_PATH = "temp";
    private static final String HAP_SUFFIXI = ".hap";
    private static final String HAP_TYPE = "entry";
    private static final String SYSTEM_ACTION = "action.system.home";
    private static final String SYSTEM_ENTITY = "entity.system.home";
    private static final int READ_BUFFER_SIZE = 1024;
    private static final int BUFFER_SIZE = 10 * 1024;
    private static final long FILE_TIME = 1546272000000L;
    private static final String LIBS_DIR_NAME = "libs";
    private static final String CUT_ENTRY_FILENAME = "cut_entry.apk";
    private static final String SO_SUFFIX = ".so";
    private static final Log LOG = new Log(Uncompress.class.toString());

    /**
     * unpackage entrance.
     *
     * @param utility common data
     * @return unpackageProcess if unpackage succeed
     */
    static boolean unpackageProcess(Utility utility) {
        if (utility == null) {
            LOG.error("Uncompress::unpackageProcess utility is null!");
            return false;
        }

        boolean unpackageResult = true;
        File destFile = new File(utility.getOutPath());

        if (!destFile.exists()) {
            if (!destFile.mkdirs()) {
                LOG.error("Uncompress::unpackageProcess create out file directory failed!");
                return false;
            }
        }

        try {
            if (!utility.getForceRewrite().isEmpty() && "true".equals(utility.getForceRewrite())) {
                File outPath = new File(utility.getOutPath());
                deleteFile(outPath);
                outPath.mkdirs();
            }

            if (Utility.MODE_HAP.equals(utility.getMode())) {
                if ("true".equals(utility.getUnpackApk())) {
                    unzip(utility, utility.getHapPath(), utility.getOutPath(), APK_SUFFIX);
                    String[] temp = utility.getHapPath().replace("\\", "/").split("/");
                    String hapName = temp[temp.length - 1];
                    repackHap(utility.getHapPath(), utility.getOutPath(), hapName, utility.getUnpackApk());
                } else {
                    dataTransferAllFiles(utility.getHapPath(), utility.getOutPath());
                }
            } else if (Utility.MODE_HAR.equals(utility.getMode())) {
                dataTransferAllFiles(utility.getHarPath(), utility.getOutPath());
            } else {
                dataTransferFilesByApp(utility, utility.getAppPath(), utility.getOutPath());
            }
        } catch (BundleException ignored) {
            unpackageResult = false;
            LOG.error("Uncompress::unpackageProcess Bundle exception");
        }

        // return uncompress information.
        if (!unpackageResult) {
            LOG.error("Uncompress::unpackageProcess unpackage failed!");
        }

        return unpackageResult;
    }

    /**
     * uncompress app.
     *
     * @param utility common data
     * @return the uncompress result
     */
    static UncomperssResult uncompressApp(Utility utility) {
        UncomperssResult compressResult = new UncomperssResult();
        InputStream input = null;
        String srcPath = utility.getAppPath();
        String parseMode = utility.getParseMode();
        try {
            if (!parseMode.isEmpty() && UncompressEntrance.PARSE_MODE_HAPLIST.equals(parseMode)) {
                compressResult = uncompress(utility.getDeviceType(), srcPath, PACK_INFO);
            } else if (!parseMode.isEmpty() && UncompressEntrance.PARSE_MODE_HAPINFO.equals(parseMode)) {
                String tmpPath = (new File(srcPath)).getParent().replace(File.separator, LINUX_FILE_SEPARATOR) +
                        LINUX_FILE_SEPARATOR + TEMP_PATH;
                File tempDir = new File(tmpPath);
                boolean deletePath = false;
                if (!tempDir.exists()) {
                    tempDir.mkdir();
                    deletePath = true;
                }
                String hapName = utility.getHapName();
                if (!hapName.toLowerCase(Locale.ENGLISH).endsWith(HAP_SUFFIX)) {
                    hapName += HAP_SUFFIX;
                }
                unzipByName(srcPath, tmpPath, hapName);
                srcPath = tmpPath + LINUX_FILE_SEPARATOR + hapName;
                compressResult = uncompress(utility.getDeviceType(), srcPath, HARMONY_PROFILE);
                if (deletePath) {
                    deleteFile(tempDir);
                } else {
                    File destFile = new File(srcPath);
                    deleteFile(destFile);
                }
            } else if (!parseMode.isEmpty() && UncompressEntrance.PARSE_MODE_ALL.equals(parseMode)) {
                input = new BufferedInputStream(new FileInputStream(srcPath));
                compressResult = uncompressAllByInput(input);
            } else {
                LOG.error("Uncompress::uncompressApp parseMode is invalid!");
                compressResult.setResult(false);
                compressResult.setMessage("ParseApp parseMode is invalid");
            }
        } catch (BundleException | FileNotFoundException ignored) {
            LOG.error("Uncompress::uncompressApp Bundle exception");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp Bundle exception");
        } finally {
            Utility.closeStream(input);
        }
        return compressResult;
    }

    /**
     * uncompress app.
     *
     * @param utility common data
     * @param input the InputStream about the app package.
     * @return the uncompress result
     */
    static UncomperssResult uncompressAppByInput(Utility utility, InputStream input) {
        UncomperssResult compressResult = new UncomperssResult();
        String parseMode = utility.getParseMode();
        ByteArrayOutputStream byteOutput = null;
        InputStream parseInput = null;
        InputStream unpackInput = null;

        try {
            byteOutput = new ByteArrayOutputStream();
            byte[] buffer = new byte[BUFFER_SIZE];
            int len;
            while ((len = input.read(buffer)) > -1 ) {
                byteOutput.write(buffer, 0, len);
            }
            byteOutput.flush();

            parseInput = new ByteArrayInputStream(byteOutput.toByteArray());
            if (!parseMode.isEmpty() && UncompressEntrance.PARSE_MODE_HAPLIST.equals(parseMode)) {
                compressResult = uncompressByInput(utility.getDeviceType(), parseInput, PACK_INFO);
            } else if (!parseMode.isEmpty() && UncompressEntrance.PARSE_MODE_HAPINFO.equals(parseMode)) {
                compressResult = getProfileInfoByInput(utility.getDeviceType(), parseInput, utility.getHapName());
            } else if (!parseMode.isEmpty() && UncompressEntrance.PARSE_MODE_ALL.equals(parseMode)) {
                compressResult = uncompressAllByInput(parseInput);
            } else {
                LOG.error("Uncompress::uncompressAppByInput parseMode is invalid!");
                compressResult.setResult(false);
                compressResult.setMessage("ParseApp parseMode is invalid");
            }

            unpackInput = new ByteArrayInputStream(byteOutput.toByteArray());
        } catch (BundleException | IOException exception) {
            LOG.error("Uncompress::uncompressAppByInput Bundle exception");
            compressResult.setResult(false);
            compressResult.setMessage("ParseApp Bundle exception");
        } finally {
            Utility.closeStream(byteOutput);
            Utility.closeStream(parseInput);
            Utility.closeStream(unpackInput);
        }
        return compressResult;
    }

    /**
     * uncompress hap.
     *
     * @param utility common data
     * @return the uncompress result
     */
    static UncomperssResult uncompressHap(Utility utility) {
        UncomperssResult compressResult = new UncomperssResult();
        try {
            compressResult = uncompress(utility.getDeviceType(), utility.getHapPath(), HARMONY_PROFILE);
        } catch (BundleException ignored) {
            LOG.error("Uncompress::uncompressHap Bundle exception");
            compressResult.setResult(false);
            compressResult.setMessage("uncompressHap Bundle exception");
        }
        return compressResult;
    }

    /**
     * uncompress hap.
     *
     * @param utility common data
     * @param input the InputStream about the app package.
     * @return the uncompress result
     */
    static UncomperssResult uncompressHapByInput(Utility utility, InputStream input) {
        UncomperssResult compressResult = new UncomperssResult();
        try {
            compressResult = uncompressByInput(utility.getDeviceType(), input, HARMONY_PROFILE);
        } catch (BundleException ignored) {
            LOG.error("Uncompress::uncompressHapByInput Bundle exception");
            compressResult.setResult(false);
            compressResult.setMessage("uncompressHapByInput Bundle exception");
        }
        return compressResult;
    }

    /**
     * unzip process
     *
     * @param utility common data
     * @param srcPath source file path
     * @param destDirPath destination file path
     * @param suffix suffix for judgment
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void unzip(Utility utility, String srcPath, String destDirPath, String suffix)
            throws BundleException {
        if (utility == null) {
            LOG.error("Uncompress::unzip utility is null!");
            throw new BundleException("Unzip failed, utility is null");
        }

        if (srcPath.isEmpty() || !UncompressVerify.isPathValid(srcPath, true, "")) {
            LOG.error("Uncompress::unzip srcPath is invalid!");
            throw new BundleException("Unzip failed, srcPath is invalid");
        }

        if (destDirPath.isEmpty() || !UncompressVerify.isPathValid(destDirPath, false, null)) {
            LOG.error("Uncompress::unzip destDirPath is invalid!");
            throw new BundleException("Unzip failed, destDirPath is invalid");
        }
        unzipFromFile(utility, srcPath, destDirPath, suffix);
    }

    /**
     * unzip process from the file
     *
     * @param utility common data
     * @param srcPath source file path
     * @param destDirPath destination file path
     * @param suffix suffix for judgment
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void unzipFromFile(Utility utility, String srcPath, String destDirPath, String suffix)
            throws BundleException {
        ZipFile zipFile = null;
        String hapNames = "";
        try {
            zipFile = new ZipFile(new File(srcPath));
            if (utility != null && !utility.getDeviceType().isEmpty() && HAP_SUFFIX.equals(suffix)) {
                List<PackInfo> packInfos = uncompress(utility.getDeviceType(), srcPath, PACK_INFO).getPackInfos();
                for (PackInfo packinfo : packInfos) {
                    hapNames += packinfo.name + ",";
                }
            }

            int entriesNum = 0;
            for (Enumeration<? extends ZipEntry> entries = zipFile.entries(); entries.hasMoreElements(); ) {
                entriesNum++;
                ZipEntry entry = entries.nextElement();
                String entryName = "";
                if (entry == null || entry.getName().isEmpty()) {
                    continue;
                }
                if (entry.getName().toLowerCase().endsWith(CUT_ENTRY_FILENAME) &&
                    "false".equals(utility.getUnpackCutEntryApk())) {
                    continue;
                }
                entryName = entry.getName();
                if (!entryName.toLowerCase(Locale.ENGLISH).endsWith(suffix) ||
                        (!hapNames.isEmpty() && !hapNames.contains(entryName.replace(suffix, "")))) {
                    continue;
                }
                String tempDir = destDirPath.replace(File.separator, LINUX_FILE_SEPARATOR);
                if (HAP_SUFFIX.equals(suffix) && "true".equals(utility.getUnpackApk())) {
                    tempDir = tempDir + LINUX_FILE_SEPARATOR + entryName.replace(suffix, "");
                    File destFileDir = new File(tempDir);
                    if (!destFileDir.exists()) {
                        destFileDir.mkdir();
                    }
                }
                if (APK_SUFFIX.equals(suffix) && "true".equals(utility.getUnpackApk())
                    && entryName.contains(LINUX_FILE_SEPARATOR)) {
                    // only unpack shell apk which in the root directory
                    continue;
                }
                String tempPath = tempDir + LINUX_FILE_SEPARATOR + entryName;
                File destFile = new File(tempPath);
                dataTransfer(zipFile, entry, destFile);
                if (JSON_SUFFIX.equals(suffix)) {
                    break;
                } else if (HAP_SUFFIX.equals(suffix) && "true".equals(utility.getUnpackApk())) {
                    unzip(utility, tempPath, tempDir, APK_SUFFIX);
                    repackHap(tempPath, tempDir, entryName, utility.getUnpackApk());
                }
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::unzipInHapMode file not found exception");
            throw new BundleException("Unzip in hap mode failed");
        } catch (IOException exception) {
            LOG.error("Uncompress::unzipInHapMode io exception: " + exception.getMessage());
            throw new BundleException("Unzip in hap mode failed");
        } finally {
            Utility.closeStream(zipFile);
        }
    }

    /**
     * unzip process by hap/app name
     *
     * @param srcPath source file path
     * @param destDirPath destination file path
     * @param entryName hap/app name
     * @throws BundleException FileNotFoundException|IOException.
     */
    static void unzipByName(String srcPath, String destDirPath, String entryName) throws BundleException {
        if (srcPath.isEmpty() || destDirPath.isEmpty() || entryName.isEmpty()) {
            LOG.error("Uncompress::unzipByName srcPath, destDirPath or entryName is empty!");
            throw new BundleException("Unzip by name failed, destDirPath or entryName is empty");
        }

        if (!UncompressVerify.isPathValid(srcPath, true, "")) {
            LOG.error("Uncompress::unzipByName srcPath is invalid!");
            throw new BundleException("Unzip by name failed, srcPath is invalid");
        }

        if (!UncompressVerify.isPathValid(destDirPath, false, null)) {
            LOG.error("Uncompress::unzipByName destDirPath is invalid!");
            throw new BundleException("Unzip by name failed, destDirPath is invalid");
        }

        ZipFile zipFile = null;
        try {
            zipFile = new ZipFile(new File(srcPath));
            ZipEntry entry = zipFile.getEntry(entryName);
            if (entry == null) {
                LOG.error("Uncompress::unzipByName " + entryName + " not found exception");
                throw new BundleException("Unzip by name failed");
            }
            String tempPath = destDirPath.replace(File.separator, LINUX_FILE_SEPARATOR) + LINUX_FILE_SEPARATOR
                    + entry.getName();
            File destFile = new File(tempPath);
            dataTransfer(zipFile, entry, destFile);
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::unzipByName file not found exception");
            throw new BundleException("Unzip by name failed");
        } catch (IOException exception) {
            LOG.error("Uncompress::unzipByName io exception: " + exception.getMessage());
            throw new BundleException("Unzip by name failed");
        } finally {
            Utility.closeStream(zipFile);
        }
    }

    /**
     * uncompress dataTransfer
     *
     * @param zipFile input zip file
     * @param entry input file in zip
     * @param destFile output file path
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void dataTransfer(ZipFile zipFile, ZipEntry entry, File destFile) throws BundleException {
        InputStream fileInputStream = null;
        FileOutputStream fileOutStream = null;
        try {
            fileInputStream = zipFile.getInputStream(entry);
            fileOutStream = new FileOutputStream(destFile);
            byte[] data = new byte[BUFFER_SIZE];
            int count = fileInputStream.read(data);
            int total = 0;
            while (count > 0) {
                fileOutStream.write(data, 0, count);
                total += count;
                count = fileInputStream.read(data);
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::dataTransfer file not found exception");
            throw new BundleException("DataTransfer failed");
        } catch (IOException exception) {
            LOG.error("Uncompress::dataTransfer io exception: " + exception.getMessage());
            throw new BundleException("DataTransfer failed");
        } finally {
            Utility.closeStream(fileOutStream);
            Utility.closeStream(fileInputStream);
        }
    }

    /**
     * uncompress dataTransfer
     *
     * @param input the InputStream about the app package.
     * @param destFile output file path
     * @param fileName target file name.
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void dataTransferByInput(InputStream input, File destFile, String fileName) throws BundleException {
        FileOutputStream fileOutStream = null;
        BufferedInputStream bufIn = null;
        ZipInputStream zipIn = null;
        try {
            ZipEntry entry = null;
            bufIn = new BufferedInputStream(input);
            zipIn = new ZipInputStream(bufIn);
            fileOutStream = new FileOutputStream(destFile);
            int entriesNum = 0;
            while ((entry = zipIn.getNextEntry()) != null) {
                entriesNum++;
                String entryName = entry.getName();
                if (fileName.equals(entryName)) {
                    byte[] data = new byte[BUFFER_SIZE];
                    int count = zipIn.read(data);
                    int total = 0;
                    while (count > 0) {
                        fileOutStream.write(data, 0, count);
                        total += count;
                        count = zipIn.read(data);
                    }

                    break;
                }
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::dataTransferByInput file not found exception");
            throw new BundleException("DataTransfer by input failed");
        } catch (IOException exception) {
            LOG.error("Uncompress::dataTransferByInput io exception: " + exception.getMessage());
            throw new BundleException("DataTransfer by input failed");
        } finally {
            Utility.closeStream(fileOutStream);
            Utility.closeStream(zipIn);
            Utility.closeStream(bufIn);
        }
    }

    /**
     * uncompress dataTransfer all files.
     *
     * @param srcPath source file path
     * @param destDirPath destination file path
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void dataTransferAllFiles(String srcPath, String destDirPath) throws BundleException {
        ZipFile zipFile = null;
        try {
            zipFile = new ZipFile(new File(srcPath));
            int entriesNum = 0;
            for (Enumeration<? extends ZipEntry> entries = zipFile.entries(); entries.hasMoreElements(); ) {
                entriesNum++;
                ZipEntry entry = entries.nextElement();
                if (entry == null) {
                    continue;
                }
                String tempPath = destDirPath + LINUX_FILE_SEPARATOR + entry.getName();
                File destFile = new File(tempPath);
                if (destFile != null && destFile.getParentFile() != null && !destFile.getParentFile().exists()) {
                    destFile.getParentFile().mkdirs();
                }
                dataTransfer(zipFile, entry, destFile);
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::unzipApk file not found exception");
            throw new BundleException("Unzip Apk failed");
        } catch (IOException exception) {
            LOG.error("Uncompress::unzipApk io exception: " + exception.getMessage());
            throw new BundleException("Unzip Apk failed");
        } finally {
            Utility.closeStream(zipFile);
        }
    }

    private static void dataTransferFilesByApp(Utility utility, String srcPath, String destDirPath)
            throws BundleException {
        ZipFile zipFile = null;
        try {
            zipFile = new ZipFile(new File(srcPath));
            int entriesNum = 0;
            for (Enumeration<? extends ZipEntry> entries = zipFile.entries(); entries.hasMoreElements(); ) {
                entriesNum++;
                ZipEntry entry = entries.nextElement();
                if (entry == null) {
                    continue;
                }
                String filePath = destDirPath + LINUX_FILE_SEPARATOR + entry.getName();
                File destFile = new File(filePath);
                if (destFile != null && destFile.getParentFile() != null && !destFile.getParentFile().exists()) {
                    destFile.getParentFile().mkdirs();
                }
                boolean isUnpackApk = "true".equals(utility.getUnpackApk());
                if (isUnpackApk && filePath.toLowerCase().endsWith(HAP_SUFFIX)) {
                    dataTransfer(zipFile, entry, destFile);
                    unzip(utility, filePath, destDirPath, APK_SUFFIX);
                    String[] temp = filePath.replace("\\", "/").split("/");
                    String hapName = temp[temp.length - 1];
                    repackHap(filePath, destDirPath, hapName, utility.getUnpackApk());
                } else {
                    dataTransfer(zipFile, entry, destFile);
                }
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::unzipApk file not found exception");
            throw new BundleException("Unzip Apk failed");
        } catch (IOException exception) {
            LOG.error("Uncompress::unzipApk io exception: " + exception.getMessage());
            throw new BundleException("Unzip Apk failed");
        } finally {
            Utility.closeStream(zipFile);
        }
    }

    private static byte[] getResourceDataFromHap(ZipFile zipFile) throws BundleException, IOException {
        int entriesNum = 0;
        InputStream indexInputStream = null;
        try {
            for (Enumeration<? extends ZipEntry> entries = zipFile.entries(); entries.hasMoreElements(); ) {
                entriesNum++;
                ZipEntry indexEntry = entries.nextElement();
                if (indexEntry == null) {
                    continue;
                }
                if (indexEntry != null && !"".equals(indexEntry.getName()) &&
                    indexEntry.getName().toLowerCase().endsWith(RESOURCE_INDEX)) {
                    indexInputStream = zipFile.getInputStream(indexEntry);
                    return getByte(indexInputStream);
                }
            }
        } finally {
            Utility.closeStream(indexInputStream);
        }
        return null;
    }

    private static String readStringFromFile(String fileName, ZipFile zipFile)
        throws IOException {
        ZipEntry entry = zipFile.getEntry(fileName);
        if (entry == null) {
            LOG.error("Uncompress::readStringFromFile " + fileName + " not found exception");
            return "";
        }
        InputStream fileInputStream = null;
        BufferedReader bufferedReader = null;
        try {
            fileInputStream = zipFile.getInputStream(entry);
            bufferedReader = new BufferedReader(new InputStreamReader(fileInputStream));
            String line;
            StringBuilder sb = new StringBuilder();
            while ((line = bufferedReader.readLine()) != null) {
                sb.append(line);
            }
            return sb.toString();
        } finally {
            Utility.closeStream(bufferedReader);
            Utility.closeStream(fileInputStream);
        }
    }

    private static HapZipInfo unZipHapFileFromHapFile(String srcPath)
        throws BundleException, IOException {
        HapZipInfo hapZipInfo = new HapZipInfo();
        ZipFile zipFile = null;
        try {
            File srcFile = new File(srcPath);
            zipFile = new ZipFile(srcFile);
            hapZipInfo.setHarmonyProfileJsonStr(readStringFromFile(HARMONY_PROFILE, zipFile));
            hapZipInfo.setResDataBytes(getResourceDataFromHap(zipFile));
            hapZipInfo.setPackInfoJsonStr(readStringFromFile(PACK_INFO, zipFile));
            hapZipInfo.setHapFileName(getHapNameWithoutSuffix(srcFile.getName()));
        }  finally {
            Utility.closeStream(zipFile);
        }
        return hapZipInfo;
    }

    /**
     * uncompress from specified file name
     *
     * @param deviceType device type
     * @param srcPath source file path
     * @param fileName uncompress file name
     * @return the uncompress result
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static UncomperssResult uncompress(String deviceType, String srcPath, String fileName)
            throws BundleException {
        if (srcPath.isEmpty() || fileName.isEmpty()) {
            LOG.error("Uncompress::uncompress srcPath, fileName is empty!");
            throw new BundleException("Uncompress failed, srcPath or fileName is empty");
        }

        UncomperssResult result = new UncomperssResult();
        try {
            HapZipInfo hapZipInfo = unZipHapFileFromHapFile(srcPath);
            if (isPackInfo(fileName)) {
                uncompressPackInfo(deviceType, hapZipInfo, result);
            } else {
                uncompressProfileInfo(hapZipInfo, result);
            }
        } catch (IOException exception) {
            LOG.error("Uncompress::uncompress io exception: " + exception.getMessage());
            throw new BundleException("Uncompress failed");
        }
        return result;
    }

    private static void uncompressPackInfo(String deviceType, HapZipInfo hapZipInfo, UncomperssResult uncomperssResult)
        throws BundleException {
        List<PackInfo> packInfos = JsonUtil.parseHapList(deviceType, hapZipInfo.getPackInfoJsonStr());
        uncomperssResult.setPackInfoStr(hapZipInfo.getPackInfoJsonStr());
        uncomperssResult.setPackInfos(packInfos);
    }

    private static void uncompressProfileInfo(HapZipInfo hapZipInfo, UncomperssResult uncomperssResult)
        throws BundleException {
        ProfileInfo profileInfo = JsonUtil.parseProfileInfo(hapZipInfo.getHarmonyProfileJsonStr(),
            hapZipInfo.getResDataBytes(), hapZipInfo.getPackInfoJsonStr(), hapZipInfo.getHapFileName());
        profileInfo.hapName = hapZipInfo.getHapFileName();
        uncomperssResult.addProfileInfoStr(hapZipInfo.getHarmonyProfileJsonStr());
        uncomperssResult.addProfileInfo(profileInfo);
    }

    private static HapZipInfo unZipHapFileFromInputStream(InputStream input) throws BundleException, IOException {
        BufferedInputStream bufIn = null;
        ZipInputStream zipIn = null;
        BufferedReader bufferedReader = null;
        HapZipInfo hapZipInfo = new HapZipInfo();
        try {
            ZipEntry entry = null;
            bufIn = new BufferedInputStream(input);
            zipIn = new ZipInputStream(bufIn);
            int entriesNum = 0;
            while ((entry = zipIn.getNextEntry()) != null) {
                entriesNum++;
                if (entry.getName().toLowerCase().endsWith(RESOURCE_INDEX)) {
                    hapZipInfo.setResDataBytes(getByte(zipIn));
                    continue;
                }
                if (isPackInfo(entry.getName())) {
                    bufferedReader = new BufferedReader(new InputStreamReader(zipIn));
                    hapZipInfo.setPackInfoJsonStr(readStringFromInputStream(zipIn, bufferedReader));
                    continue;
                }
                if (isHarmonyProfile(entry.getName())) {
                    bufferedReader = new BufferedReader(new InputStreamReader(zipIn));
                    hapZipInfo.setHarmonyProfileJsonStr(readStringFromInputStream(zipIn, bufferedReader));
                }
            }
        } finally {
            Utility.closeStream(bufferedReader);
            Utility.closeStream(bufIn);
            Utility.closeStream(zipIn);
        }
        return hapZipInfo;
    }

    private static String readStringFromInputStream(ZipInputStream zipIn, BufferedReader bufferedReader)
        throws IOException {
        String line;
        StringBuilder sb = new StringBuilder();
        while ((line = bufferedReader.readLine()) != null) {
            sb.append(line);
        }
        return sb.toString();
    }

    /**
     * uncompress process by InputStream
     *
     * @param deviceType device type
     * @param input the InputStream about the package.
     * @param fileName uncompress file name
     * @return the uncompress result
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static UncomperssResult uncompressByInput(String deviceType, InputStream input, String fileName)
            throws BundleException {
        UncomperssResult result = new UncomperssResult();
        try {
            HapZipInfo hapZipInfo = unZipHapFileFromInputStream(input);
            if (isPackInfo(fileName)) {
                uncompressPackInfo(deviceType, hapZipInfo, result);
            } else {
                uncompressProfileInfo(hapZipInfo, result);
            }
        } catch (IOException exception) {
            LOG.error("Uncompress::uncompressByInput io exception: " + exception.getMessage());
            throw new BundleException("Uncompress by input failed");
        }
        return result;
    }

    /**
     * Get entry byte array.
     *
     * @param zis the InputStream about the entry.
     * @return Return the entry byte array.
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static byte[] getByte(InputStream zis) throws BundleException {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        byte[] buf = null;
        try {
            byte[] temp = new byte[READ_BUFFER_SIZE];
            int length = 0;

            while ((length = zis.read(temp, 0, READ_BUFFER_SIZE)) != -1) {
                bos.write(temp, 0, length);
            }

            buf = bos.toByteArray();
        } catch (IOException e) {
            LOG.error("Uncompress::getByte io exception: " + e.getMessage());
            throw new BundleException("Get byte failed");
        } finally {
            Utility.closeStream(bos);
        }
        return buf;
    }

    /**
     * get ProfileInfo by InputStream
     *
     * @param deviceType device type
     * @param input the InputStream about the package.
     * @param fileName uncompress file name
     * @return the uncompress result
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static UncomperssResult getProfileInfoByInput(String deviceType, InputStream input, String fileName)
            throws BundleException {
        File tempDir = new File(TEMP_PATH);
        boolean deletePath = false;
        if (!tempDir.exists()) {
            tempDir.mkdir();
            deletePath = true;
        }
        String srcPath = TEMP_PATH + LINUX_FILE_SEPARATOR + fileName;
        try {
            File verifyFile = new File(srcPath);
            srcPath = verifyFile.getCanonicalPath();
        } catch (IOException exception) {
            LOG.error("Uncompress::getProfileInfoByInput io exception: " + exception.getMessage());
            throw new BundleException("Get profile info all by input failed");
        }
        File destFile = new File(srcPath);
        dataTransferByInput(input, destFile, fileName);
        UncomperssResult compressResult = uncompress(deviceType, srcPath, HARMONY_PROFILE);
        if (deletePath) {
            deleteFile(tempDir);
        } else {
            deleteFile(destFile);
        }
        return compressResult;
    }

    /**
     * uncompress process by InputStream
     *
     * @param input the InputStream about the package.
     * @return the uncompress result
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static UncomperssResult uncompressAllByInput(InputStream input) throws BundleException {
        UncomperssResult result = new UncomperssResult();
        BufferedInputStream bufIn = null;
        ZipInputStream zipIn = null;
        BufferedReader bufferedReader = null;

        File tempDir = new File(TEMP_PATH);
        boolean deletePath = false;
        if (!tempDir.exists()) {
            tempDir.mkdir();
            deletePath = true;
        }

        try {
            ZipEntry entry = null;
            bufIn = new BufferedInputStream(input);
            zipIn = new ZipInputStream(bufIn);
            int entriesNum = 0;
            while ((entry = zipIn.getNextEntry()) != null) {
                entriesNum++;
                if (PACK_INFO.equals(entry.getName().toLowerCase(Locale.ENGLISH))) {
                    bufferedReader = new BufferedReader(new InputStreamReader(zipIn));
                    String line;
                    StringBuilder sb = new StringBuilder();
                    while ((line = bufferedReader.readLine()) != null) {
                        sb.append(line);
                    }
                    List<PackInfo> packInfos = JsonUtil.parseHapList("", sb.toString());
                    result.setPackInfoStr(sb.toString());
                    result.setPackInfos(packInfos);
                } else if (entry.getName().toLowerCase(Locale.ENGLISH).endsWith(HAP_SUFFIX)) {
                    String srcPath = TEMP_PATH + LINUX_FILE_SEPARATOR + entry.getName();
                    UncomperssResult compressResult = parseHapInfoByTempPath(zipIn, srcPath);
                    if (compressResult.getProfileInfos() != null && compressResult.getProfileInfos().size() > 0) {
                        result.addProfileInfo(compressResult.getProfileInfos().get(0));
                        result.addProfileInfoStr(compressResult.getProfileInfosStr().get(0));
                    }
                }
            }
            result = checkParseAllResult(result);
            result = obtainLabelAndIcon(result);
        } catch (IOException exception) {
            LOG.error("Uncompress::uncompressAllByInput io exception: " + exception.getMessage());
            throw new BundleException("Uncompress all by input failed");
        } finally {
            Utility.closeStream(bufferedReader);
            Utility.closeStream(bufIn);
            Utility.closeStream(zipIn);
        }
        if (deletePath) {
            deleteFile(tempDir);
        }
        return result;
    }

    /**
     * parse HapInfo By the temp path
     *
     * @param zipIn the zipFile InputStream
     * @param destPath destination file path
     * @return the uncompress result
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static UncomperssResult parseHapInfoByTempPath(ZipInputStream zipIn, String destPath)
            throws BundleException {
        FileOutputStream fileOutStream = null;
        UncomperssResult compressResult = new UncomperssResult();
        try {
            File verifyFile = new File(destPath);
            String verifyDestPath = verifyFile.getCanonicalPath();
            File destFile = new File(verifyDestPath);
            fileOutStream = new FileOutputStream(destFile);
            byte[] data = new byte[BUFFER_SIZE];
            int count = zipIn.read(data);
            int total = 0;
            while (count > 0) {
                fileOutStream.write(data, 0, count);
                total += count;
                count = zipIn.read(data);
            }
            compressResult = uncompress("", destPath, HARMONY_PROFILE);
            deleteFile(destFile);
        } catch (IOException exception) {
            LOG.error("Uncompress::uncompressAllByInput io exception: " + exception.getMessage());
            throw new BundleException("Uncompress all by input failed");
        } finally {
            Utility.closeStream(fileOutStream);
        }
        return compressResult;
    }

    /**
     * repack hap
     *
     * @param srcPath source file path
     * @param destDirPath destination file path
     * @param fileName target file name
     * @param unpackApk unpackApk flag
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void repackHap(String srcPath, String destDirPath, String fileName, String unpackApk)
            throws BundleException {
        if (srcPath.isEmpty() || destDirPath.isEmpty() || fileName.isEmpty()) {
            LOG.error("Uncompress::repackHap srcPath, destDirPath or fileName is empty!");
            throw new BundleException("Repack hap failed, srcPath, destDirPath or fileName is empty");
        }

        if (!UncompressVerify.isPathValid(srcPath, true, "")) {
            LOG.error("Uncompress::repackHap srcPath is invalid!");
            throw new BundleException("Repack hap failed, srcPath is invalid");
        }

        if (!UncompressVerify.isPathValid(destDirPath, false, null)) {
            LOG.error("Uncompress::repackHap destDirPath is invalid!");
            throw new BundleException("Repack hap failed, destDirPath is invalid");
        }

        String tempDir = destDirPath.replace(File.separator, LINUX_FILE_SEPARATOR) + LINUX_FILE_SEPARATOR +
                TEMP_PATH;
        dataTransferAllFiles(srcPath, tempDir);
        packFilesByPath(tempDir, destDirPath, fileName, unpackApk);
        File deleteFile = new File(tempDir);
        deleteFile(deleteFile);
    }

    /**
     * compress file directory.
     *
     * @param srcPath source file path
     * @param destDirPath destination file path
     * @param fileName target file name
     * @param unpackApk unpackApk flag
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void packFilesByPath(String srcPath, String destDirPath, String fileName, String unpackApk)
            throws BundleException {
        if (srcPath.isEmpty() || destDirPath.isEmpty() || fileName.isEmpty()) {
            LOG.error("Uncompress::packFilesByPath srcPath, destDirPath or fileName is empty!");
            throw new BundleException("Pack files by path failed, srcPath, destDirPath or fileName is empty");
        }

        if (!UncompressVerify.isPathValid(srcPath, false, null) ||
                !UncompressVerify.isPathValid(destDirPath, false, null)) {
            LOG.error("Uncompress::packFilesByPath srcPath or destDirPath is invalid!");
            throw new BundleException("Pack files by path failed, srcPath or destDirPath is invalid");
        }

        File srcDir = new File(srcPath);
        File[] srcFiles = srcDir.listFiles();
        if (srcFiles == null) {
            return;
        }
        FileOutputStream fileOut = null;
        CheckedOutputStream checkedOut = null;
        ZipOutputStream zipOut = null;

        try {
            File zipFile = new File(destDirPath + LINUX_FILE_SEPARATOR + fileName);
            fileOut = new FileOutputStream(zipFile);
            checkedOut = new CheckedOutputStream(fileOut, new CRC32());
            zipOut = new ZipOutputStream(checkedOut);
            for (int i = 0; i < srcFiles.length; i++) {
                File srcFile = srcFiles[i];
                if (srcFile.isDirectory()) {
                    if (srcFile.getPath().toLowerCase(Locale.ENGLISH).endsWith(LIBS_DIR_NAME)) {
                        compressDirectory(srcFile, "", zipOut, true);
                    } else {
                        compressDirectory(srcFile, "", zipOut, false);
                    }
                } else {
                    if (srcFile.getPath().toLowerCase(Locale.ENGLISH).endsWith(APK_SUFFIX) &&
                        "true".equals(unpackApk)) {
                        continue;
                    }
                    compressFile(srcFile, "", zipOut, false);
                }
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::packFilesByPath file not found exception");
            throw new BundleException("Pack files by path failed");
        } finally {
            Utility.closeStream(zipOut);
            Utility.closeStream(checkedOut);
            Utility.closeStream(fileOut);
        }
    }

    /**
     * compress file directory.
     *
     * @param dir file directory
     * @param baseDir base path for file
     * @param zipOut zip outPutStream
     * @param isCompression if need compression
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void compressDirectory(File dir, String baseDir, ZipOutputStream zipOut, boolean isCompression)
            throws BundleException {
        File[] files = dir.listFiles();
        if (files == null) {
            return;
        }
        for (File file : files) {
            if (file.isDirectory()) {
                compressDirectory(file, baseDir + dir.getName() + File.separator, zipOut, isCompression);
            } else {
                compressFile(file, baseDir + dir.getName() + File.separator, zipOut, isCompression);
            }
        }
    }

    /**
     * compress process.
     *
     * @param srcFile source file to zip
     * @param baseDir base path for file
     * @param zipOut zip outPutStream
     * @param isCompression if need compression
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static void compressFile(File srcFile, String baseDir, ZipOutputStream zipOut, boolean isCompression)
            throws BundleException {
        FileInputStream fileInputStream = null;
        BufferedInputStream bufferedInputStream = null;

        try {
            String entryName = (baseDir + srcFile.getName()).replace(File.separator, LINUX_FILE_SEPARATOR);
            ZipEntry zipEntry = new ZipEntry(entryName);
	    boolean isNeedCompress = isCompression;
	    if (srcFile.isFile() && srcFile.getName().toLowerCase(Locale.ENGLISH).endsWith(SO_SUFFIX)) {
                isNeedCompress = false;
            }
            if (isNeedCompress) {
                zipEntry.setMethod(ZipEntry.DEFLATED);
            } else {
                zipEntry.setMethod(ZipEntry.STORED);
                zipEntry.setCompressedSize(srcFile.length());
                zipEntry.setSize(srcFile.length());
                CRC32 crc = getCrcFromFile(srcFile);
                zipEntry.setCrc(crc.getValue());
            }
            FileTime fileTime = FileTime.fromMillis(FILE_TIME);
            zipEntry.setLastAccessTime(fileTime);
            zipEntry.setLastModifiedTime(fileTime);
            zipOut.putNextEntry(zipEntry);
            byte[] data = new byte[BUFFER_SIZE];
            fileInputStream = new FileInputStream(srcFile);
            bufferedInputStream = new BufferedInputStream(fileInputStream);
            int count = bufferedInputStream.read(data);
            while (count > 0) {
                zipOut.write(data, 0, count);
                count = bufferedInputStream.read(data);
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompress::compressFile file not found exception");
            throw new BundleException("Compress file failed");
        } catch (IOException exception) {
            LOG.error("Uncompress::compressFile io exception: " + exception.getMessage());
            throw new BundleException("Compress file failed");
        } finally {
            Utility.closeStream(fileInputStream);
            Utility.closeStream(bufferedInputStream);
        }
    }

    /**
     * get CRC32 from file.
     *
     * @param file source file
     * @return CRC32
     * @throws BundleException FileNotFoundException|IOException.
     */
    private static CRC32 getCrcFromFile(File file) throws BundleException {
        FileInputStream fileInputStream = null;
        CRC32 crc = new CRC32();
        try {
            fileInputStream = new FileInputStream(file);
            byte[] buffer = new byte[BUFFER_SIZE];

            int count = fileInputStream.read(buffer);
            while (count > 0) {
                crc.update(buffer, 0, count);
                count = fileInputStream.read(buffer);
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Uncompressor::getCrcFromFile file not found exception");
            throw new BundleException("Get Crc from file failed");
        } catch (IOException exception) {
            LOG.error("Uncompressor::getCrcFromFile io exception: " + exception.getMessage());
            throw new BundleException("Get Crc from file failed");
        } finally {
            Utility.closeStream(fileInputStream);
        }
        return crc;
    }

    /**
     * delete file
     *
     * @param file the file to be deleted
     */
    private static void deleteFile(File file) {
        if (file.exists()) {
            if (file.isDirectory()) {
                File[] files = file.listFiles();
                for (int i = 0; i < files.length; i++) {
                    deleteFile(files[i]);
                }
            }
            file.delete();
        }
    }

    /**
     * check
     *
     * @param result the result of parse app all mode
     * @return return the result of checkParseAllResult.
     */
    private static UncomperssResult checkParseAllResult(UncomperssResult result) {
        UncomperssResult errorResult = new UncomperssResult();
        errorResult.setResult(false);
        errorResult.setMessage("App package is invalid.");
        if (result == null || result.getPackInfos() == null || result.getProfileInfos() == null) {
            return errorResult;
        }

        List<PackInfo> packInfos = result.getPackInfos();
        List<ProfileInfo> profileInfos = result.getProfileInfos();
        int packInfoSize = packInfos.size();
        int profileInfoSize = profileInfos.size();
        if (packInfoSize == 0 || profileInfoSize == 0 || packInfoSize != profileInfoSize) {
            LOG.error("Uncompress::checkParseAllResult error, hapNum is invalid in app");
            return errorResult;
        }

        for (int i = 0; i < packInfoSize; i++) {
            if (packInfos.get(i) == null || packInfos.get(i).name.isEmpty()) {
                return errorResult;
            }
            boolean isHave = false;
            for (int j = 0; j < profileInfoSize; j++) {
                if (profileInfos.get(j) == null || profileInfos.get(j).hapName.isEmpty()) {
                    return errorResult;
                }
                if (profileInfos.get(j).hapName.replace(HAP_SUFFIXI, "")
                        .equals(packInfos.get(i).name.replace(HAP_SUFFIXI, ""))) {
                    isHave = true;
                    break;
                }
            }
            if (!isHave) {
                return errorResult;
            }
        }

        return result;
    }

    /**
     * get label and icon for application
     *
     * @param result the result of parse app all mode
     * @return return the result which contains icon and label
     */
    private static UncomperssResult obtainLabelAndIcon(UncomperssResult result) {
        List<ProfileInfo> profileInfos = result.getProfileInfos();
        if (profileInfos.isEmpty()) {
            return result;
        }
        for (ProfileInfo profileInfo : profileInfos) {
            if (profileInfo == null) {
                continue;
            }
            HapInfo hapInfo = profileInfo.hapInfo;
            if (hapInfo == null) {
                continue;
            }
            Distro distro = hapInfo.distro;
            if (distro == null) {
                continue;
            }
            String moduleType = distro.moduleType;
            if (HAP_TYPE.equals(moduleType.toLowerCase(Locale.ENGLISH))) {
                return obtainInnerLabelAndIcon(result, hapInfo, distro);
            }
        }
        return result;
    }

    /**
     * get label and icon for application
     *
     * @param result the result of parse app all mode
     * @param hapInfo hap info of entry hap
     * @return return the result which contains icon and label
     */
    private static UncomperssResult obtainInnerLabelAndIcon(UncomperssResult result, HapInfo hapInfo, Distro distro) {
        List<AbilityInfo> abilities = hapInfo.abilities;
        if ((abilities == null) || (abilities.isEmpty())) {
            result.setLabel(distro.moduleName);
            return result;
        }
        int size = 0;
        for (AbilityInfo info : abilities) {
            if (info == null) {
                size++;
                continue;
            }
            if ((info.skills == null) || (info.skills.isEmpty())) {
                continue;
            }
            for (SkillInfo skill : info.skills) {
                if (skill == null) {
                    continue;
                }
                List<String> actions = skill.actions;
                List<String> entities = skill.entities;
                if ((!actions.isEmpty()) && (actions.contains(SYSTEM_ACTION)) && (!entities.isEmpty()) &&
                    (entities.contains(SYSTEM_ENTITY))) {
                    result.setLabel(info.label);
                    result.setIcon(info.icon);
                    return result;
                }
            }
        }
        if (size == abilities.size()) {
            result.setLabel(distro.moduleName);
            return result;
        }
        for (AbilityInfo info : abilities) {
            if (info != null) {
                result.setLabel(info.label);
                result.setIcon(info.icon);
                break;
            }
        }
        return result;
    }


    private static boolean isHarmonyProfile(String fileName) {
        return HARMONY_PROFILE.equals(fileName);
    }

    private static boolean isPackInfo(String fileName) {
        return PACK_INFO.equals(fileName);
    }

    private static String getHapNameWithoutSuffix(String hapFileName) {
        if (hapFileName == null || hapFileName.isEmpty() || hapFileName.lastIndexOf(".") == -1) {
            return "";
        }
        return hapFileName.substring(0, hapFileName.lastIndexOf("."));
    }
}
