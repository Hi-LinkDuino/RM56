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

import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.attribute.FileTime;
import java.util.Locale;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.zip.CRC32;
import java.util.zip.CheckedOutputStream;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipOutputStream;
import java.util.Enumeration;
import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;


/**
 * bundle compressor class, compress file and directory.
 *
 */
public class Compressor {
    private static final String JSON_SUFFIX = ".json";
    private static final String INFO_SUFFIX = ".info";
    private static final String HAP_SUFFIX = ".hap";
    private static final String PNG_SUFFIX = ".png";
    private static final String UPPERCASE_PNG_SUFFIX = ".PNG";
    private static final String CONFIG_JSON = "config.json";
    private static final String MODULE_JSON = "module.json";
    private static final String CODE = "code";
    private static final String NAME = "name";
    private static final String VERSION = "\"version\"";
    private static final String VERSION_CODE = "\"versionCode\"";
    private static final String NULL_DIR_NAME = "";
    private static final String RES_DIR_NAME = "res/";
    private static final String RESOURCES_DIR_NAME = "resources/";
    private static final String LIBS_DIR_NAME = "libs/";
    private static final String ASSETS_DIR_NAME = "assets/";
    private static final String SO_DIR_NAME = "maple/";
    private static final String SO_ARM64_DIR_NAME = "maple/arm64/";
    private static final String LINUX_FILE_SEPARATOR = "/";
    private static final String DISTRO = "distro";
    private static final String FORMS = "forms";
    private static final String MODULE_NAME = "module-name";
    private static final String MODULE_NAME_NEW = "moduleName";
    private static final String JSON_END = "}";
    private static final String SEMICOLON = "\"";
    private static final String APPLICATION = "deviceConfig";
    private static final String COMPRESS_NATIVE_LIBS = "compressNativeLibs";
    private static final String SHARED_LIBS_DIR_NAME = "shared_libs/";
    private static final String DEVICE_TYPE = "deviceType";
    private static final String DEVICE_TYPE_FITNESSWATCH = "fitnessWatch";
    private static final String DEVICE_TYPE_FITNESSWATCH_NEW = "liteWearable";
    private static final String COLON = ":";
    private static final String COMMA = ",";
    private static final String ENTRYCARD_NAME = "EntryCard/";
    private static final String PACKINFO_NAME = "pack.info";
    private static final String ENTRYCARD_BASE_NAME = "base";
    private static final String ENTRYCARD_SNAPSHOT_NAME = "snapshot";
    private static final String VERTICAL = "vertical";
    private static final String HORIZONTAL = "horizontal";
    private static final String CAR = "car";
    private static final String TV = "tv";
    private static final String SDIP = "sdip";
    private static final String MDIP = "mdip";
    private static final String WEARABLE = "wearable";
    private static final String PIC_1X2 = "1x2";
    private static final String PIC_2X2 = "2x2";
    private static final String PIC_2X4 = "2x4";
    private static final String PIC_4X4 = "4x4";
    private static final String REGEX_LANGUAGE = "^[a-z]{2}$";
    private static final String REGEX_SCRIPT = "^[A-Z][a-z]{3}$";
    private static final String REGEX_COUNTRY = "^[A-Z]{2,3}|[0-9]{3}$";
    private static final String REGEX_ORIENTATION = "^vertical|horizontal$";
    private static final String REGEX_DEVICE_TYPE = "^phone|tablet|car|tv|wearable|liteWearable$";
    private static final String REGEX_SCREEN_DENSITY = "^sdpi|mdpi|ldpi|xldpi|xxldpi$";
    private static final String REGEX_COLOR_MODE = "^light|dark$";
    private static final String REGEX_SHAPE = "^circle$";
    private static final String JS_PATH = "js/";
    private static final String ETS_PATH = "ets/";


    // set timestamp to get fixed MD5
    private static final long FILE_TIME = 1546272000000L;

    // set buffer size of each read
    private static final int BUFFER_SIZE = 10 * 1024;
    private static final Log LOG = new Log(Compressor.class.toString());
    private static String versionCode = "";
    private static String versionName = "";

    private ZipOutputStream zipOut = null;
    private boolean mIsContain2x2EntryCard = true;

    private List<String> list = new ArrayList<String>();
    private List<String> formNamesList = new ArrayList<String>();
    private List<String> fileNameList = new ArrayList<String>();
    private List<String> supportDimensionsList = Arrays.asList(PIC_1X2, PIC_2X2, PIC_2X4, PIC_4X4);

    /**
     * check path if is a module.json file
     *
     * @param path   path input
     * @return true if path is a module file
     */
    private static boolean isModuleJSON(String path)
    {
        File file = new File(path);
        if ((file.isFile()) && file.getName().equals(MODULE_JSON)) {
            return true;
        }
        return false;
    }

    /**
     * start compress.
     * file orders as follows:
     * for hap: 1.config.json 2.lib 3.res 4.assets 5.*.so 6.*.dex 7.*.apk 8.resources.index
     * for app: 1.certificate 2.signature 3.pack.info 4.hap (1 and 2 may not be used)
     *
     * @param utility common data
     * @return compressProcess if compress succeed
     */
    public boolean compressProcess(Utility utility) {
        boolean compressResult = true;
        File destFile = new File(utility.getOutPath());

        // if out file directory not exist, mkdirs.
        File outParentFile = destFile.getParentFile();
        if ((outParentFile != null) && (!outParentFile.exists())) {
            if (!outParentFile.mkdirs()) {
                LOG.error("Compressor::compressProcess create out file parent directory failed!");
                return false;
            }
        }

        FileOutputStream fileOut = null;
        CheckedOutputStream checkedOut = null;
        try {
            fileOut = new FileOutputStream(destFile);
            checkedOut = new CheckedOutputStream(fileOut, new CRC32());
            zipOut = new ZipOutputStream(checkedOut);
            if (Utility.MODE_HAP.equals(utility.getMode())) {
                if (isModuleJSON(utility.getJsonPath())) {
                    compressHapModeForModule(utility);
                } else {
                    compressHapMode(utility);
                }
            } else if (Utility.MODE_HAR.equals(utility.getMode())) {
                compressHarMode(utility);
            } else if (Utility.MODE_APP.equals(utility.getMode())) {
                compressAppMode(utility);
            } else {
                compressPackResMode(utility);
            }
        } catch (FileNotFoundException exception) {
            compressResult = false;
            LOG.error("Compressor::compressProcess file not found exception" + exception.getMessage());
        } catch (BundleException ignored) {
            compressResult = false;
            LOG.error("Compressor::compressProcess Bundle exception");
        } finally {
            closeZipOutputStream();
            Utility.closeStream(zipOut);
            Utility.closeStream(checkedOut);
            Utility.closeStream(fileOut);
        }

        // if compress failed, delete out file.
        if (!compressResult) {
            LOG.error("Compressor::compressProcess compress failed!");
            if (!destFile.delete()) {
                LOG.error("Compressor::compressProcess delete dest file failed");
            }
        }
        return compressResult;
    }

    /**
     * compress in hap mode.
     *
     * @param utility common data
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressHapMode(Utility utility) throws BundleException {
        pathToFile(utility, utility.getJsonPath(), NULL_DIR_NAME, false);

        pathToFile(utility, utility.getProfilePath(), NULL_DIR_NAME, false);

        if (!utility.getIndexPath().isEmpty() && !utility.getModuleName().isEmpty()) {
            String assetsPath = ASSETS_DIR_NAME + utility.getModuleName() + LINUX_FILE_SEPARATOR;
            pathToFile(utility, utility.getIndexPath(), assetsPath, false);
        }

        if (!utility.getLibPath().isEmpty()) {
            pathToFile(utility, utility.getLibPath(), LIBS_DIR_NAME, utility.isCompressNativeLibs());
        }

        if (!utility.getFilePath().isEmpty()) {
            pathToFile(utility, utility.getFilePath(), NULL_DIR_NAME, false);
        }

        if (!utility.getResPath().isEmpty() && !utility.getModuleName().isEmpty()) {
            String resPath = ASSETS_DIR_NAME + utility.getModuleName() + LINUX_FILE_SEPARATOR
                    + RESOURCES_DIR_NAME;
            if (DEVICE_TYPE_FITNESSWATCH.equals(utility.getDeviceType().replace("\"", "").trim()) ||
                    DEVICE_TYPE_FITNESSWATCH_NEW.equals(utility.getDeviceType().replace("\"", "").trim())) {
                resPath = RES_DIR_NAME;
            }
            pathToFile(utility, utility.getResPath(), resPath, false);
        }

        if (!utility.getResourcesPath().isEmpty() && !utility.getModuleName().isEmpty()) {
            String resourcesPath = ASSETS_DIR_NAME + utility.getModuleName() + LINUX_FILE_SEPARATOR
                    + RESOURCES_DIR_NAME;
            pathToFile(utility, utility.getResourcesPath(), resourcesPath, false);
        }

        if (!utility.getRpcidPath().isEmpty()) {
            String rpcidPath = NULL_DIR_NAME;
            pathToFile(utility, utility.getRpcidPath(), rpcidPath, false);
        }

        if (!utility.getAssetsPath().isEmpty()) {
            pathToFile(utility, utility.getAssetsPath(), ASSETS_DIR_NAME, false);
        }

        if (!utility.getBinPath().isEmpty()) {
            pathToFile(utility, utility.getBinPath(), NULL_DIR_NAME, false);
        }

        if (!utility.getPackInfoPath().isEmpty()) {
            pathToFile(utility, utility.getPackInfoPath(), NULL_DIR_NAME, false);
        }
        compressHapModeMultiple(utility);
    }

    /**
     * compress in hap mode for module.json.
     *
     * @param utility common data
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressHapModeForModule(Utility utility) throws BundleException {
        pathToFile(utility, utility.getJsonPath(), NULL_DIR_NAME, false);

        pathToFile(utility, utility.getProfilePath(), NULL_DIR_NAME, false);

        if (!utility.getIndexPath().isEmpty() && isModuleJSON(utility.getJsonPath())) {
            String assetsPath = NULL_DIR_NAME;
            pathToFile(utility, utility.getIndexPath(), assetsPath, false);
        }

        if (!utility.getLibPath().isEmpty()) {
            pathToFile(utility, utility.getLibPath(), LIBS_DIR_NAME, utility.isCompressNativeLibs());
        }

        if (!utility.getFilePath().isEmpty()) {
            pathToFile(utility, utility.getFilePath(), NULL_DIR_NAME, false);
        }

        if (!utility.getResPath().isEmpty() && !utility.getModuleName().isEmpty()) {
            String resPath = ASSETS_DIR_NAME + utility.getModuleName() + LINUX_FILE_SEPARATOR
                    + RESOURCES_DIR_NAME;
            if (DEVICE_TYPE_FITNESSWATCH.equals(utility.getDeviceType().replace("\"", "").trim()) ||
                    DEVICE_TYPE_FITNESSWATCH_NEW.equals(utility.getDeviceType().replace("\"", "").trim())) {
                resPath = RES_DIR_NAME;
            }
            pathToFile(utility, utility.getResPath(), resPath, false);
        }

        if (!utility.getResourcesPath().isEmpty() && isModuleJSON(utility.getJsonPath())) {
            String resourcesPath = RESOURCES_DIR_NAME;
            pathToFile(utility, utility.getResourcesPath(), resourcesPath, false);
        }
        if (!utility.getJsPath().isEmpty() && isModuleJSON(utility.getJsonPath())) {
            String jsPath = JS_PATH;
            pathToFile(utility, utility.getJsPath(), jsPath, false);
        }

        if (!utility.getEtsPath().isEmpty() && isModuleJSON(utility.getJsonPath())) {
            String etsPath = ETS_PATH;
            pathToFile(utility, utility.getEtsPath(), etsPath, false);
        }

        if (!utility.getRpcidPath().isEmpty()) {
            String rpcidPath = NULL_DIR_NAME;
            pathToFile(utility, utility.getRpcidPath(), rpcidPath, false);
        }

        if (!utility.getAssetsPath().isEmpty()) {
            pathToFile(utility, utility.getAssetsPath(), ASSETS_DIR_NAME, false);
        }

        if (!utility.getBinPath().isEmpty()) {
            pathToFile(utility, utility.getBinPath(), NULL_DIR_NAME, false);
        }

        if (!utility.getPackInfoPath().isEmpty()) {
            pathToFile(utility, utility.getPackInfoPath(), NULL_DIR_NAME, false);
        }
        compressHapModeMultiple(utility);
    }

    /**
     * compress in hap mode multiple path.
     *
     * @param utility common data
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressHapModeMultiple(Utility utility) throws BundleException {
        for (String soPathItem : utility.getFormattedSoPathList()) {
            pathToFile(utility, soPathItem, SO_ARM64_DIR_NAME, false);
        }

        if (utility.getFormattedSoPathList().size() == 0 && !utility.getSoDir().isEmpty()) {
            pathToFile(utility, utility.getSoDir(), SO_DIR_NAME, false);
        }

        for (String soPathItem : utility.getFormattedAbilitySoPathList()) {
            pathToFile(utility, soPathItem, NULL_DIR_NAME, false);
        }

        for (String dexPathItem : utility.getFormattedDexPathList()) {
            pathToFile(utility, dexPathItem, NULL_DIR_NAME, false);
        }

        for (String abcPathItem : utility.getFormattedAbcPathList()) {
            pathToFile(utility, abcPathItem, NULL_DIR_NAME, false);
        }

        for (String apkPathItem : utility.getFormattedApkPathList()) {
            pathToFile(utility, apkPathItem, NULL_DIR_NAME, false);
        }

        for (String jarPathItem : utility.getFormattedJarPathList()) {
            pathToFile(utility, jarPathItem, NULL_DIR_NAME, false);
        }

        for (String txtPathItem : utility.getFormattedTxtPathList()) {
            pathToFile(utility, txtPathItem, NULL_DIR_NAME, false);
        }

        if (!utility.getSharedLibsPath().isEmpty()) {
            pathToFile(utility, utility.getSharedLibsPath(), SHARED_LIBS_DIR_NAME, utility.isCompressNativeLibs());
        }
    }

    /**
     * compress in har mode.
     *
     * @param utility common data
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressHarMode(Utility utility) throws BundleException {
        pathToFile(utility, utility.getJsonPath(), NULL_DIR_NAME, false);

        if (!utility.getLibPath().isEmpty()) {
            pathToFile(utility, utility.getLibPath(), LIBS_DIR_NAME, utility.isCompressNativeLibs());
        }

        if (!utility.getResPath().isEmpty()) {
            pathToFile(utility, utility.getResPath(), RESOURCES_DIR_NAME, false);
        }

        if (!utility.getResourcesPath().isEmpty()) {
            pathToFile(utility, utility.getResourcesPath(), RESOURCES_DIR_NAME, false);
        }

        if (!utility.getAssetsPath().isEmpty()) {
            pathToFile(utility, utility.getAssetsPath(), ASSETS_DIR_NAME, false);
        }

        for (String jarPathItem : utility.getFormattedJarPathList()) {
            pathToFile(utility, jarPathItem, NULL_DIR_NAME, false);
        }

        for (String txtPathItem : utility.getFormattedTxtPathList()) {
            pathToFile(utility, txtPathItem, NULL_DIR_NAME, false);
        }
    }

    /**
     * compress in app mode.
     *
     * @param utility common data
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressAppMode(Utility utility) throws BundleException {
        pathToFile(utility, utility.getJsonPath(), NULL_DIR_NAME, false);

        if (!utility.getCertificatePath().isEmpty()) {
            pathToFile(utility, utility.getCertificatePath(), NULL_DIR_NAME, false);
        }

        if (!utility.getSignaturePath().isEmpty()) {
            pathToFile(utility, utility.getSignaturePath(), NULL_DIR_NAME, false);
        }

        File appOutputFile = new File(utility.getOutPath().trim());
        List<String> fileList = new ArrayList<>();
        for (String hapPathItem : utility.getFormattedHapPathList()) {
            File hapFile = new File(hapPathItem.trim());
            String hapTempPath = appOutputFile.getParentFile().getParent() + File.separator + hapFile.getName();
            fileList.add(hapTempPath);
            try {
                compressPackinfoIntoHap(hapPathItem, hapTempPath, utility.getPackInfoPath());
            } catch ( IOException e) {
                LOG.error("Compressor::compressAppMode compress pack.info into hap failed");
                throw new BundleException("Compressor::compressAppMode compress pack.info into hap failed");
            }
        }
        for (String hapPath : fileList) {
            pathToFile(utility, hapPath, NULL_DIR_NAME, false);
        }

        for (String hapPath : fileList) {
            deleteFile(hapPath);
        }

        if (!utility.getEntryCardPath().isEmpty()) {
            String entryCardPath = ENTRYCARD_NAME + utility.getModuleName() + LINUX_FILE_SEPARATOR
                    + ENTRYCARD_BASE_NAME + ENTRYCARD_SNAPSHOT_NAME;
            for (String entryCardPathItem : utility.getformattedEntryCardPathList()) {
                pathToFile(utility, entryCardPathItem, entryCardPath, true);
            }
        }

        if (!utility.getPackResPath().isEmpty()) {
            pathToFile(utility, utility.getPackResPath(), NULL_DIR_NAME, false);
        }
        File file = new File(utility.getPackInfoPath());
        compressFile(utility, file, NULL_DIR_NAME, false);
    }

    private void copy(InputStream input, OutputStream output) throws IOException {
        int bytesRead;
        byte[] data = new byte[BUFFER_SIZE];
        while ((bytesRead = input.read(data, 0, BUFFER_SIZE)) != -1) {
            output.write(data, 0, bytesRead);
        }
    }

    private void compressPackinfoIntoHap(String hapPathItem, String outPathString, String packInfo)
            throws FileNotFoundException, IOException, BundleException {
        ZipFile sourceHapFile = new ZipFile(hapPathItem);
        ZipOutputStream append = new ZipOutputStream(new FileOutputStream(outPathString));
        try {
            Enumeration<? extends ZipEntry> entries = sourceHapFile.entries();
            while (entries.hasMoreElements()) {
                ZipEntry zipEntry = entries.nextElement();
                if (zipEntry.getName() != null && PACKINFO_NAME.equals(zipEntry.getName())) {
                    continue;
                }
                append.putNextEntry(zipEntry);
                if (!zipEntry.isDirectory()) {
                    copy(sourceHapFile.getInputStream(zipEntry), append);
                }
                append.closeEntry();
            }
            File packInfoFile = new File(packInfo);
            ZipEntry zipEntry = getStoredZipEntry(packInfoFile, PACKINFO_NAME);
            append.putNextEntry(zipEntry);
            FileInputStream in = new FileInputStream(packInfoFile);
            try {
                byte[] buf = new byte[BUFFER_SIZE];
                int len;
                while ((len = in.read(buf)) != -1) {
                    append.write(buf, 0, len);
                }
            } finally {
                in.close();
            }
            append.closeEntry();
        } catch (IOException exception) {
            LOG.error("Compressor::compressPackinfoIntoHap io exception");
            throw new BundleException("Compressor::compressPackinfoIntoHap io exception");
        } finally {
            sourceHapFile.close();
            append.close();
        }
    }

    /**
     * zipFile
     *
     * @param utility Utility
     * @param path  path
     * @param format destFileName
     */
    private void zipFile(String path) {
        FileOutputStream outputStream = null;
        ZipOutputStream out = null;
        try {
            File destFile = new File(path + HAP_SUFFIX);
            File outParentFile = destFile.getParentFile();
            if ((outParentFile != null) && (!outParentFile.exists())) {
                if (!outParentFile.mkdirs()) {
                    LOG.error("Compressor::compressProcess create out file parent directory failed!");
                }
            }
            outputStream = new FileOutputStream(destFile);
            out = new ZipOutputStream(new CheckedOutputStream(outputStream, new CRC32()));
            out.setMethod(ZipOutputStream.STORED);
            compress(new File(path), out, NULL_DIR_NAME, true);
        } catch (FileNotFoundException ignored) {
            LOG.error("zip file not found exception");
        } finally {
            Utility.closeStream(out);
            Utility.closeStream(outputStream);
            deleteFile(path);
        }
    }

    /**
     *  copyFileUsingFileStreams pack.info
     *
     * @param source inputPath
     * @param dest outputPath
     *
     */
    private static void copyFileUsingFileStreams(String source, String dest) {
        FileInputStream input = null;
        FileOutputStream output = null;
        try {
            File inputFile = new File(source);
            File outputFile = new File(dest, PACKINFO_NAME);
            File outputFileParent = outputFile.getParentFile();
            if (!outputFileParent.exists()) {
                outputFileParent.mkdirs();
            }
            if (!outputFile.exists()) {
                outputFile.createNewFile();
            }
            input = new FileInputStream(inputFile);
            output = new FileOutputStream(outputFile);
            byte[] buf = new byte[1024];
            int bytesRead;
            while ((bytesRead = input.read(buf)) != -1) {
                output.write(buf, 0, bytesRead);
            }
            input.close();
            output.close();
        } catch (FileNotFoundException ignored) {
            LOG.error("copy file not found exception" + ignored.toString());
        } catch (IOException msg) {
            LOG.error("IOException : " + msg.getMessage());
        } finally {
            Utility.closeStream(input);
            Utility.closeStream(output);
        }
    }


    /**
     * unzip hap package to path
     *
     * @param hapPath zip file
     * @param destDir path after unzip file
     */
    public static void unzip(final String hapPath, final String destDir) {
        File file = new File(destDir);
        if (!file.exists()) {
            file.mkdirs();
        }
        ZipFile zipFile = null;
        BufferedInputStream bis = null;
        BufferedOutputStream bos = null;
        FileOutputStream fos = null;
        try {
            zipFile = new ZipFile(hapPath);
            Enumeration<? extends ZipEntry> entries = zipFile.entries();
            int entriesNum = 0;
            while (entries.hasMoreElements()) {
                entriesNum++;
                ZipEntry entry = entries.nextElement();
                if (entry == null) {
                    continue;
                }
                if (entry.isDirectory()) {
                    new File(destDir + File.separator + entry.getName()).mkdirs();
                    continue;
                }
                bis = new BufferedInputStream(zipFile.getInputStream(entry));
                File newFile = new File(destDir + File.separator + entry.getName());
                File parent = newFile.getParentFile();
                if (parent != null && (!parent.exists())) {
                    parent.mkdirs();
                }
                fos = new FileOutputStream(newFile);
                bos = new BufferedOutputStream(fos, BUFFER_SIZE);
                writeFile(bis, bos, entry);
                bos.flush();
                bos.close();
                fos.close();
                bis.close();
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("unzip file not found exception");
        } catch (IOException msg) {
            LOG.error("unzip IOException : " + msg.getMessage());
        } finally {
            Utility.closeStream(bos);
            Utility.closeStream(fos);
            Utility.closeStream(bis);
            Utility.closeStream(zipFile);
        }
    }

    /**
     *  unzipWriteFile
     *
     * @param bis BufferedInputStream
     * @param bos BufferedOutputStream
     * @param entry ZipEntry
     * @throws IOException IO
     */
    private static void writeFile(BufferedInputStream bis, BufferedOutputStream bos,
                                ZipEntry entry) throws IOException {
        int count;
        int total = 0;
        byte[] data = new byte[BUFFER_SIZE];
        while ((count = bis.read(data, 0, BUFFER_SIZE)) != -1) {
            bos.write(data, 0, count);
            total += count;
        }
    }

    /**
     * delete file
     *
     * @param path file path which will be deleted
     */
    private static void deleteFile(final String path) {
        File file = new File(path);
        if (file.exists()) {
            if (file.isDirectory()) {
                File[] files = file.listFiles();
                for (int i = 0; i < files.length; i++) {
                    deleteFile(files[i].toString());
                }
            }
            file.delete();
        }
    }

    /**
     * compress in res mode.
     *
     * @param utility common data
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressPackResMode(Utility utility) throws BundleException {
        if (!utility.getPackInfoPath().isEmpty()) {
            File file = new File(utility.getPackInfoPath());
            infoSpecialProcess(utility, file);
        }
        if (!utility.getEntryCardPath().isEmpty()) {
            getFileList(utility.getEntryCardPath());
            if (!mIsContain2x2EntryCard) {
                LOG.error("Compressor::compressPackResMode No 2x2 resource file exists");
                throw new BundleException("No 2x2 resource file exists");
            }
            for (String fileName : fileNameList) {
                if (fileName.endsWith(PNG_SUFFIX) || fileName.endsWith(UPPERCASE_PNG_SUFFIX)) {
                    String fName = fileName.trim();
                    String[] temp = fName.replace("\\", "/").split("/");
                    if (temp.length < 4) {
                        LOG.error("Compressor::compressPackResMode the hap file path is invalid, length: "
                            + temp.length);
                        continue;
                    }
                    String moduleName = temp[temp.length - 4];
                    if (!isModelName(moduleName)) {
                        LOG.error("Compressor::compressProcess compress pack.res failed, moduleName "
                            + moduleName + " is error, please check it in config.json");
                        throw new BundleException("Compress pack.res failed, moduleName Error");
                    }
                    String fileLanguageCountryName = temp[temp.length - 3];
                    if (!isThirdLevelDirectoryNameValid(fileLanguageCountryName)) {
                        LOG.error("Compressor::compressProcess compress failed third level directory name: "
                            + fileLanguageCountryName + " is invalid, please check it with reference to this example: "
                            + "zh_Hani_CN-vertical-car-mdpi-dark or zh_Hani_CN-vertical-car-mdpi");
                        throw new BundleException("Compress failed third level directory name Error!");
                    }
                    String filePicturingName = temp[temp.length - 1];
                    if (!isPicturing(filePicturingName, utility)) {
                        LOG.error("Compressor::compressProcess Compress pack.res failed, Invalid resource file" +
                            " name: " + filePicturingName + ", correct format example is formName-2x2.png");
                        throw new BundleException("Compress pack.res failed, Invalid resource file name: "
                            + filePicturingName + ", correct format example is formName-2x2.png");
                    }

                } else {
                    LOG.error("Compressor::compressProcess compress failed No image in PNG format is found!");
                    throw new BundleException("Compress pack.res failed, compress failed No image in"
                        + " PNG format is found");
                }
            }
            pathToFile(utility, utility.getEntryCardPath(), ENTRYCARD_NAME, false);
        }
    }

    /**
     * Check whether modelname meets specifications.
     *
     * @param name modelName
     * @return false and true
     */
    private boolean isModelName(String name) {
        for (String listName : list) {
            if (name.equals(listName)) {
                return true;
            }
        }
        return false;
    }

    private boolean isThirdLevelDirectoryNameValid(String thirdLevelDirectoryName) {
        if (thirdLevelDirectoryName == null || thirdLevelDirectoryName.isEmpty()) {
            return false;
        }
        if (ENTRYCARD_BASE_NAME.equals(thirdLevelDirectoryName)) {
            return true;
        }
        // example: zh_Hani_CN-vertical-car-mdpi-dark or zh_Hani_CN-vertical-car-mdpi
        int firstDelimiterIndex = thirdLevelDirectoryName.indexOf("_");
        if (firstDelimiterIndex < 0) {
            return false;
        }
        String language = thirdLevelDirectoryName.substring(0, firstDelimiterIndex);
        int secondDelimiterIndex = thirdLevelDirectoryName.indexOf("_", firstDelimiterIndex + 1);
        if (secondDelimiterIndex < 0) {
            return false;
        }
        String script = thirdLevelDirectoryName.substring(firstDelimiterIndex + 1, secondDelimiterIndex);
        int thirdDelimiterIndex = thirdLevelDirectoryName.indexOf("-", secondDelimiterIndex + 1);
        if (thirdDelimiterIndex < 0) {
            return false;
        }
        String country = thirdLevelDirectoryName.substring(secondDelimiterIndex + 1, thirdDelimiterIndex);
        if (!checkLanguage(language) || !checkScript(script) || !checkCountry(country)) {
            return false;
        }
        int forthDelimiterIndex = thirdLevelDirectoryName.indexOf("-", thirdDelimiterIndex + 1);
        if (forthDelimiterIndex < 0) {
            return false;
        }
        String orientation = thirdLevelDirectoryName.substring(thirdDelimiterIndex + 1, forthDelimiterIndex);
        int fifthDelimiterIndex = thirdLevelDirectoryName.indexOf("-", forthDelimiterIndex + 1);
        if (fifthDelimiterIndex < 0) {
            return false;
        }
        String deviceType = thirdLevelDirectoryName.substring(forthDelimiterIndex + 1, fifthDelimiterIndex);
        if (!checkOrientation(orientation) || !checkDeviceType(deviceType)) {
            return false;
        }
        int sixthDelimiterIndex = thirdLevelDirectoryName.indexOf("-", fifthDelimiterIndex + 1);
        if (sixthDelimiterIndex < 0) {
            String screenDensity = thirdLevelDirectoryName.substring(fifthDelimiterIndex + 1,
                    thirdLevelDirectoryName.length());
            return checkScreenDensity(screenDensity);
        } else {
            String screenDensity = thirdLevelDirectoryName.substring(fifthDelimiterIndex + 1, sixthDelimiterIndex);
            if (!checkScreenDensity(screenDensity)) {
                return false;
            }
        }
        int seventhDelimiterIndex = thirdLevelDirectoryName.indexOf("-", sixthDelimiterIndex + 1);
        if (seventhDelimiterIndex < 0) {
            String tmp = thirdLevelDirectoryName.substring(sixthDelimiterIndex + 1, thirdLevelDirectoryName.length());
            return checkColorModeOrShape(tmp);
        }
        if (!checkColorMode(thirdLevelDirectoryName.substring(sixthDelimiterIndex + 1, seventhDelimiterIndex))) {
            return false;
        }
        String shape = thirdLevelDirectoryName.substring(seventhDelimiterIndex + 1, thirdLevelDirectoryName.length());
        return checkShape(shape);
    }

    private boolean checkLanguage(String language) {
        if (!Pattern.compile(REGEX_LANGUAGE).matcher(language).matches()) {
            LOG.error("Compressor::compressProcess language " + language + " is not in ISO 639-1 list");
            return false;
        }
        return true;
    }

    private boolean checkScript(String script) {
        if (!Pattern.compile(REGEX_SCRIPT).matcher(script).matches()) {
            LOG.error("Compressor::compressProcess script " + script + " is not in ISO 15924 list");
            return false;
        }
        return true;
    }

    private boolean checkCountry(String country) {
        if (!Pattern.compile(REGEX_COUNTRY).matcher(country).matches()) {
            LOG.error("Compressor::compressProcess country " + country + " is not in ISO 3166-1 list");
            return false;
        }
        return true;
    }

    private boolean checkOrientation(String orientation) {
        if (!Pattern.compile(REGEX_ORIENTATION).matcher(orientation).matches()) {
            LOG.error("Compressor::compressProcess orientation " + orientation +
                " is not in {vertical, horizontal} list");
            return false;
        }
        return true;
    }

    private boolean checkDeviceType(String deviceType) {
        if (!Pattern.compile(REGEX_DEVICE_TYPE).matcher(deviceType).matches()) {
            LOG.error("Compressor::compressProcess deviceType " + deviceType +
                    " is not in {phone, tablet, car, tv, wearable, liteWearable} list");
            return false;
        }
        return true;
    }

    private boolean checkScreenDensity(String screenDensity) {
        if (!Pattern.compile(REGEX_SCREEN_DENSITY).matcher(screenDensity).matches()) {
            LOG.error("Compressor::compressProcess screenDensity " + screenDensity +
                    " is not in {sdpi, mdpi, ldpi, xldpi, xxldpi} list");
            return false;
        }
        return true;
    }

    private boolean checkColorMode(String colorMode) {
        if (!Pattern.compile(REGEX_COLOR_MODE).matcher(colorMode).matches()) {
            LOG.error("Compressor::compressProcess colorMode " + colorMode +
                    " is not in {light, dark} list");
            return false;
        }
        return true;
    }

    private boolean checkColorModeOrShape(String tmp) {
        if (Pattern.compile(REGEX_COLOR_MODE).matcher(tmp).matches() ||
            Pattern.compile(REGEX_SHAPE).matcher(tmp).matches()) {
            return true;
        }
        LOG.error("Compressor::compressProcess " + tmp +
                " is neither in colorMode list {light, dark} nor in shape list {circle}");
        return false;
    }

    private boolean checkShape(String shape) {
        if (Pattern.compile(REGEX_SHAPE).matcher(shape).matches()) {
            return true;
        }
        LOG.error("Compressor::compressProcess shape" + shape + " is not in {circle} list");
        return false;
    }

    /**
     * Check whether languageCountryName meets specifications.
     *
     * @param name languageCountryName
     * @return false and true
     */
    private boolean isLanguageCountry(String name) {
        if (!name.equals(ENTRYCARD_BASE_NAME)) {
            boolean isLanguage = false;
            String[] str = name.split("-");
            if (str.length > 1) {
                Locale[] ls = Locale.getAvailableLocales();
                for (int i = 0; i < ls.length; i++) {
                    if (ls[i].toString().equals(str[0])) {
                        isLanguage = true;
                    }
                }
                if (str[1].equals( VERTICAL ) || str[1].equals( HORIZONTAL )) {
                    isLanguage = true;
                }
                if (str[2].equals( CAR ) || str[2].equals( TV ) || str[2].equals( WEARABLE )) {
                    isLanguage = true;
                }
                if (str[3].equals( SDIP ) || str[3].equals( MDIP )) {
                    isLanguage = true;
                }
                return isLanguage;
            } else {
                return false;
            }
        } else {
            return true;
        }
    }

    /**
     * Check whether picturingName meets specifications.
     *
     * @param name picturingName
     * @param utility common data
     * @return false and true
     */
    private boolean isPicturing(String name, Utility utility) {
        boolean isSpecifications = false;
        if (name == null || name.isEmpty()) {
            return isSpecifications;
        }
        if (!name.endsWith(PNG_SUFFIX) && !name.endsWith(UPPERCASE_PNG_SUFFIX)) {
            LOG.error("isPicturing: the suffix is not .png or .PNG");
            return false;
        }
        int delimiterIndex = name.lastIndexOf("-");
        if (delimiterIndex < 0) {
            LOG.error("isPicturing: the entry card naming format is invalid and should be separated by '-'!");
            return false;
        }
        String formName = name.substring(0, delimiterIndex);
        if (!utility.getFormNameList().contains(formName)) {
            LOG.error("isPicturing: the name is not same as formName, name: " + formName + " is not in " +
                utility.getFormNameList().toString());
            return false;
        }
        String dimension = name.substring(delimiterIndex + 1, name.lastIndexOf("."));
        if (!supportDimensionsList.contains(dimension)) {
            LOG.error("isPicturing: the dimension: " + dimension + " is invalid, is not in the following list: "
                + "{1X2, 2X2, 2X4, 4X4}");
            return false;
        }
        return true;
    }

    private void getFileList(final String filePath) throws BundleException {
        File file = new File(filePath);
        if (!file.exists()) {
            LOG.error("getFileList: file is not exists");
            return;
        }
        File[] files = file.listFiles();
        if (files == null) {
            LOG.error("getFileList: no file in this file path");
            return;
        }
        for (File f : files) {
            try {
                if (f.isFile()) {
                    if (f.getName().endsWith(".DS_Store")) {
                        deleteFile(f.getCanonicalPath());
                        continue;
                    }
                    String snapshotDirectoryName = f.getParentFile().getName();
                    if (!ENTRYCARD_SNAPSHOT_NAME.equals(snapshotDirectoryName)) {
                        LOG.error("The level-4 directory of EntryCard must be named as snapshot" +
                            ", but current is: " + snapshotDirectoryName);
                        throw new BundleException("The level-4 directory of EntryCard must be named as snapshot" +
                            ", but current is: " + snapshotDirectoryName);
                    }
                    checkContain2x2EntryCard(f.getParentFile());
                    fileNameList.add(f.getCanonicalPath());
                } else if (f.isDirectory()) {
                    getFileList(f.getCanonicalPath());
                } else {
                    LOG.error("It's not file or directory!");
                }
            } catch (IOException msg) {
                LOG.error("IOException error: " + msg.getMessage());
                return;
            }
        }
    }

    private void checkContain2x2EntryCard(final File snapshotDirectory) throws IOException, BundleException {
        if (!snapshotDirectory.exists()) {
            LOG.error("checkContain2x2EntryCard: file is not exist: " + snapshotDirectory.getName());
            throw new BundleException("checkContain2x2EntryCard: file is not exist");
        }
        File[] files = snapshotDirectory.listFiles();
        if (files == null) {
            LOG.error("checkContain2x2EntryCard: no file in this file path");
            throw new BundleException("checkContain2x2EntryCard: no file in this file path");
        }

        for (File entryCardFile : files) {
            if (entryCardFile.isFile() && entryCardFile.getName().contains(PIC_2X2)) {
                return;
            }
        }
        mIsContain2x2EntryCard = false;
        LOG.error("checkContain2x2EntryCard: must contain 2x2 entryCard, please check it in "
            + snapshotDirectory.getCanonicalPath());
        throw new BundleException("checkContain2x2EntryCard: must contain 2x2 entryCard, please check it in "
            + snapshotDirectory.getCanonicalPath());
    }

    /**
     * compress file or directory.
     *
     * @param utility       common data
     * @param path          create new file by path
     * @param baseDir       base path for file
     * @param isCompression if need compression
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void pathToFile(Utility utility, String path, String baseDir, boolean isCompression)
            throws BundleException {
        if (path.isEmpty()) {
            return;
        }
        File fileItem = new File(path);
        if (fileItem.isDirectory()) {
            File[] files = fileItem.listFiles();
            if (files == null) {
                return;
            }
            for (File file : files) {
                if (file.isDirectory()) {
                    compressDirectory(utility, file, baseDir, isCompression);
                } else if (isCompression) {
                    compressFile(utility, file, baseDir, isCompression);
                } else {
                    compressFile(utility, file, baseDir, isCompression);
                }
            }
        } else {
            compressFile(utility, fileItem, baseDir, isCompression);
        }
    }

    /**
     * compress file directory.
     *
     * @param utility       common data
     * @param dir           file directory
     * @param baseDir       current directory name
     * @param isCompression if need compression
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressDirectory(Utility utility, File dir, String baseDir, boolean isCompression)
            throws BundleException {
        File[] files = dir.listFiles();
        if (files == null) {
            return;
        }
        for (File file : files) {
            if (file.isDirectory()) {
                compressDirectory(utility, file, baseDir + dir.getName() + File.separator, isCompression);
            } else {
                compressFile(utility, file, baseDir + dir.getName() + File.separator, isCompression);
            }
        }
    }

    /**
     * compress pack.info
     *
     * @param sourceFile source
     * @param zipOutputStream ZipOutputStream
     * @param name filename
     * @param KeepDirStructure Empty File
     */
    private void compress(File sourceFile, ZipOutputStream zipOutputStream, String name,
                                boolean KeepDirStructure) {
        FileInputStream in = null;
        try {
            byte[] buf = new byte[BUFFER_SIZE];
            if (sourceFile.isFile()) {
                ZipEntry zipEntry = getStoredZipEntry(sourceFile, name);
                zipOutputStream.putNextEntry(zipEntry);
                in = new FileInputStream(sourceFile);
                int len;
                while ((len = in.read(buf)) != -1) {
                    zipOutputStream.write(buf, 0, len);
                }
                zipOutputStream.closeEntry();
            } else {
                File[] listFiles = sourceFile.listFiles();
                if (listFiles == null || listFiles.length == 0) {
                    if (KeepDirStructure) {
                        if (!name.isEmpty()) {
                            ZipEntry zipEntry = getStoredZipEntry(sourceFile, name + "/");
                            zipOutputStream.putNextEntry(zipEntry);
                        } else {
                            ZipEntry zipEntry = getStoredZipEntry(sourceFile, name);
                            zipOutputStream.putNextEntry(zipEntry);
                        }
                        zipOutputStream.closeEntry();
                    }
                } else {
                    for (File file : listFiles) {
                        if (KeepDirStructure) {
                            isNameEmpty(zipOutputStream, name, KeepDirStructure, file);
                        } else {
                            compress(file, zipOutputStream, file.getName(), KeepDirStructure);
                        }
                    }
                }
            }
        } catch (FileNotFoundException ignored) {
            LOG.error("Compressor::compressFile file not found exception");
        } catch (IOException exception) {
            LOG.error("Compressor::compressFile io exception: " + exception.getMessage());
        } catch (BundleException bundleException) {
            LOG.error("Compressor::compressFile bundle exception" + bundleException.getMessage());
        } finally {
            Utility.closeStream(in);
        }
    }

    private ZipEntry getStoredZipEntry(File sourceFile, String name) throws BundleException {
        ZipEntry zipEntry = new ZipEntry(name);
        zipEntry.setMethod(ZipEntry.STORED);
        zipEntry.setCompressedSize(sourceFile.length());
        zipEntry.setSize(sourceFile.length());
        CRC32 crc = getCrcFromFile(sourceFile);
        zipEntry.setCrc(crc.getValue());
        FileTime fileTime = FileTime.fromMillis(FILE_TIME);
        zipEntry.setLastAccessTime(fileTime);
        zipEntry.setLastModifiedTime(fileTime);
        return zipEntry;
    }

    private CRC32 getCrcFromFile(File file) throws BundleException {
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
     * isNameEmpty
     *
     * @param zipOutputStream ZipOutputStream
     * @param name filename
     * @param KeepDirStructure KeepDirStructure
     * @param file file
     */
    private void isNameEmpty(ZipOutputStream zipOutputStream, String name, boolean KeepDirStructure, File file) {
        if (!name.isEmpty()) {
            compress(file, zipOutputStream, name + "/" + file.getName(), KeepDirStructure);
        } else {
            compress(file, zipOutputStream, file.getName(), KeepDirStructure);
        }
    }


    /**
     * compress process.
     *
     * @param utility       common data
     * @param srcFile       source file to zip
     * @param baseDir       current directory name of file
     * @param isCompression if need compression
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void compressFile(Utility utility, File srcFile, String baseDir, boolean isCompression)
            throws BundleException {
        BufferedInputStream bufferedInputStream = null;
        FileInputStream fileInputStream = null;
        try {
            String entryName = (baseDir + srcFile.getName()).replace(File.separator, LINUX_FILE_SEPARATOR);
            ZipEntry zipEntry = new ZipEntry(entryName);
            obtainModuleVersion(utility, srcFile, entryName);
            if (!utility.getIsModuleJson()) {
                if (!checkVersionInHaps(utility, srcFile, entryName)) {
                    LOG.error("Compressor::compressFile file checkVersionCodeInHaps failed");
                    throw new BundleException("Compressor::compressFile There are some haps with different version code!");
                }
            }
            if (srcFile.getName().toLowerCase(Locale.ENGLISH).endsWith(JSON_SUFFIX)) {
                zipEntry.setMethod(ZipEntry.STORED);
                jsonSpecialProcess(utility, srcFile, zipEntry);
                return;
            }

            if (isCompression) {
                zipEntry.setMethod(ZipEntry.DEFLATED);
            } else {
                zipEntry.setMethod(ZipEntry.STORED);

                // update size
                zipEntry.setCompressedSize(srcFile.length());
                zipEntry.setSize(srcFile.length());

                // update crc
                CRC32 crc = getCrcFromFile(utility, srcFile);
                zipEntry.setCrc(crc.getValue());
            }

            // update fileTime
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
            LOG.error("Compressor::compressFile file not found exception");
            throw new BundleException("CoompressFile failed");
        } catch (IOException exception) {
            LOG.error("Compressor::compressFile io exception: " + exception.getMessage());
            throw new BundleException("CoompressFile failed");
        } finally {
            Utility.closeStream(bufferedInputStream);
            Utility.closeStream(fileInputStream);
        }
    }

    /**
     * check json type code in haps.
     *
     * @param utility common data
     * @param srcFile source file to zip
     * @param baseDir current directory name of file
     * @return true is for successful and false is for failed
     * @throws BundleException FileNotFoundException|IOException.
     */
     private void obtainModuleVersion(Utility utility, File srcFile, String baseDir) throws BundleException {
         String moduleJson = "";
         moduleJson = checkModuleTypeInHaps(utility, srcFile, baseDir);
         if (utility.getIsModuleJson() && !moduleJson.equals("")) {
             Version version = ModuleJsonUtil.getVersion(moduleJson);

             if (utility.getVersionCode() == -1) {
                 utility.setVersionCode(version.versionCode);
             }
             if (utility.getVersionName().equals("")) {
                 utility.setVersionName(version.versionName);
             }
             if (!checkModuleVersion(utility, version)) {
                 LOG.error("Compressor::checkModuleVersion failed ");
                 throw new BundleException("Compressor::compressFile There are some haps with different version code!");
             }
         }
     }


    /**
     * check version in haps.
     *
     * @param utility common data
     * @param version current hap version
     * @throws BundleException FileNotFoundException|IOException.
     */
    private boolean checkModuleVersion(Utility utility, Version version) {
        if (version.versionName.equals("") || version.versionCode == -1) {
            return false;
        }
        if (!utility.getVersionName().equals(version.versionName)) {
            LOG.error("Compressor::checkModuleVersion versionName failed ");
            return false;
        }
        if (utility.getVersionCode() != version.versionCode) {
            LOG.error("Compressor::checkModuleVersion versionCode failed ");
            return false;
        }
        return true;
    }

    /**
     * check json type code in haps.
     *
     * @param utility common data
     * @param srcFile source file to zip
     * @param baseDir current directory name of file
     * @return true is for successful and false is for failed
     * @throws BundleException FileNotFoundException|IOException.
     */
    private String checkModuleTypeInHaps(Utility utility, File srcFile, String baseDir) throws BundleException {
        String fileStr = srcFile.getPath();
        if (!fileStr.toLowerCase(Locale.ENGLISH).endsWith(HAP_SUFFIX)) {
            return "";
        }
        ZipFile zipFile = null;
        FileInputStream zipInput = null;
        ZipInputStream zin = null;
        InputStream inputStream = null;
        InputStreamReader reader = null;
        BufferedReader br = null;
        ZipEntry entry = null;
        String moduleJson = "";
        try {
            zipFile = new ZipFile(srcFile);
            zipInput = new FileInputStream(fileStr);
            zin = new ZipInputStream(zipInput);
            while ((entry = zin.getNextEntry()) != null) {
                if (entry.getName().toLowerCase().equals(MODULE_JSON)) {
                    utility.setIsModuleJson(true);
                    inputStream = zipFile.getInputStream(entry);
                    reader = new InputStreamReader(inputStream);
                    br = new BufferedReader(reader);
                    if (br != null) {
                        moduleJson = br.readLine();
                        break;
                    }
                }
            }
        } catch (IOException exception) {
            LOG.error("Compressor::checkModuleTypeInHaps io exception: " + exception.getMessage());
            throw new BundleException("Compressor::checkModuleTypeInHaps failed");
        } finally {
            Utility.closeStream(zipFile);
            Utility.closeStream(zipInput);
            Utility.closeStream(zin);
            Utility.closeStream(inputStream);
            Utility.closeStream(reader);
            Utility.closeStream(br);
        }
        return moduleJson;
    }

    /**
     * check version code in haps.
     *
     * @param utility common data
     * @param srcFile source file to zip
     * @param baseDir current directory name of file
     * @return true is for successful and false is for failed
     * @throws BundleException FileNotFoundException|IOException.
     */
    private boolean checkVersionInHaps(Utility utility, File srcFile, String baseDir) throws BundleException {
        try {
            String fileStr = srcFile.getName();
            if ((fileStr == null) || (fileStr.isEmpty())) {
                throw new BundleException("Compressor::checkVersionInHaps get file name failed");
            }
            if (!fileStr.toLowerCase(Locale.ENGLISH).endsWith(HAP_SUFFIX)) {
                return true;
            }
            for (String hapPath : utility.getFormattedHapPathList()) {
                if ((hapPath == null) || (hapPath.isEmpty()) || (!hapPath.contains(baseDir))) {
                    continue;
                }
                String versionStr = obtainVersion(srcFile, hapPath);
                String code = obtainInnerVersionCode(versionStr);
                if ((code == null) || (code.isEmpty())) {
                    LOG.error("Compressor::checkVersionInHaps version code is null or empty");
                    return false;
                }
                if (!versionCode.isEmpty() && !versionCode.equals(code)) {
                    LOG.error("Compressor::checkVersionInHaps some haps with different version code");
                    return false;
                }
                String name = obtainInnerVersionName(versionStr);
                if ((name == null) || (name.isEmpty())) {
                    LOG.error("Compressor::checkVersionInHaps version name is null or empty");
                    return false;
                }
                if (!versionName.isEmpty() && !versionName.equals(name)) {
                    LOG.error("Compressor::checkVersionInHaps some haps with different version name");
                    return false;
                }
                versionCode = code;
                versionName = name;
            }
        } catch (BundleException exception) {
            LOG.error("Compressor::checkVersionInHaps io exception: " + exception.getMessage());
            throw new BundleException("Compressor::checkVersionInHaps failed");
        }
        return true;
    }

    /**
     * get CRC32 from file.
     *
     * @param utility common data
     * @param file    source file
     * @return CRC32
     * @throws BundleException FileNotFoundException|IOException.
     */
    private CRC32 getCrcFromFile(Utility utility, File file) throws BundleException {
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
            LOG.error("Compressor::getCrcFromFile file not found exception");
            throw new BundleException("Get Crc from file failed");
        } catch (IOException exception) {
            LOG.error("Compressor::getCrcFromFile io exception: " + exception.getMessage());
            throw new BundleException("Get Crc from file failed");
        } finally {
            Utility.closeStream(fileInputStream);
        }
        return crc;
    }

    private void infoSpecialProcess(Utility utility, File srcFile)
            throws BundleException {
        FileInputStream fileInputStream = null;
        BufferedReader bufferedReader = null;
        InputStreamReader inputStreamReader = null;

        try {
            fileInputStream = new FileInputStream(srcFile);
            inputStreamReader = new InputStreamReader(fileInputStream, StandardCharsets.UTF_8);
            bufferedReader = new BufferedReader(inputStreamReader);
            bufferedReader.mark((int) srcFile.length() + 1);
            // parse moduleName from pack.info
            parsePackModuleName(bufferedReader, utility);
            bufferedReader.reset();
            parsePackFormName(bufferedReader, utility);
            bufferedReader.reset();
            parseDeviceType(bufferedReader, utility);
            bufferedReader.reset();

            Pattern pattern = Pattern.compile(System.lineSeparator());
            String str = bufferedReader.readLine();
            StringBuilder builder = new StringBuilder();
            while (str != null) {
                Matcher matcher = pattern.matcher(str.trim());
                String dest = matcher.replaceAll("");
                builder.append(dest);
                str = bufferedReader.readLine();
            }
        } catch (IOException exception) {
            LOG.error("Compressor::jsonSpecialProcess io exception: " + exception.getMessage());
            throw new BundleException("Json special process failed");
        } finally {
            Utility.closeStream(bufferedReader);
            Utility.closeStream(inputStreamReader);
            Utility.closeStream(fileInputStream);
        }
    }

    /**
     * trim and remove "\r\n" in *.json file.
     *
     * @param utility common data
     * @param srcFile file input
     * @param entry   zip file entry
     * @throws BundleException FileNotFoundException|IOException.
     */
    private void jsonSpecialProcess(Utility utility, File srcFile, ZipEntry entry)
            throws BundleException {
        FileInputStream fileInputStream = null;
        BufferedReader bufferedReader = null;
        InputStreamReader inputStreamReader = null;

        try {
            fileInputStream = new FileInputStream(srcFile);
            inputStreamReader = new InputStreamReader(fileInputStream, StandardCharsets.UTF_8);
            bufferedReader = new BufferedReader(inputStreamReader);
            bufferedReader.mark((int) srcFile.length() + 1);
            // parse moduleName from config.json
            parseModuleName(bufferedReader, utility);
            bufferedReader.reset();
            parseCompressNativeLibs(bufferedReader, utility);
            bufferedReader.reset();
            parseDeviceType(bufferedReader, utility);
            bufferedReader.reset();

            Pattern pattern = Pattern.compile(System.lineSeparator());
            String str = bufferedReader.readLine();
            StringBuilder builder = new StringBuilder();
            while (str != null) {
                Matcher matcher = pattern.matcher(str.trim());
                String dest = matcher.replaceAll("");
                builder.append(dest);
                str = bufferedReader.readLine();
            }
            byte[] trimJson = builder.toString().getBytes(StandardCharsets.UTF_8);

            // update crc
            CRC32 crc = new CRC32();
            crc.update(trimJson);
            entry.setCrc(crc.getValue());

            // update size
            entry.setSize(trimJson.length);
            entry.setCompressedSize(trimJson.length);

            // update fileTime
            FileTime fileTime = FileTime.fromMillis(FILE_TIME);
            entry.setLastAccessTime(fileTime);
            entry.setLastModifiedTime(fileTime);

            // compress data
            zipOut.putNextEntry(entry);
            zipOut.write(trimJson);
        } catch (IOException exception) {
            LOG.error("Compressor::jsonSpecialProcess io exception: " + exception.getMessage());
            throw new BundleException("Json special process failed");
        } finally {
            Utility.closeStream(bufferedReader);
            Utility.closeStream(inputStreamReader);
            Utility.closeStream(fileInputStream);
        }
    }

    /**
     * get string of config.json from hap file.
     *
     * @param file    source file
     * @param baseDir current directory name of file
     * @return string of config.json
     * @throws BundleException FileNotFoundException|IOException.
     */
    private String obtainVersion(File srcFile, String baseDir) throws BundleException {
        ZipFile zipFile = null;
        FileInputStream zipInput = null;
        InputStream in = null;
        ZipInputStream zin = null;
        InputStream inputStream = null;
        InputStreamReader reader = null;
        BufferedReader br = null;
        ZipEntry entry = null;
        String configJson = "";
        try {
            zipFile = new ZipFile(srcFile);
            zipInput = new FileInputStream(baseDir);
            in = new BufferedInputStream(zipInput);
            zin = new ZipInputStream(in);
            while ((entry = zin.getNextEntry()) != null) {
                if (entry.getName().toLowerCase().equals(CONFIG_JSON)) {
                    inputStream = zipFile.getInputStream(entry);
                    reader = new InputStreamReader(inputStream);
                    br = new BufferedReader(reader);
                    if (br != null) {
                        configJson = br.readLine();
                        break;
                    }
                }
            }
        } catch (IOException exception) {
            LOG.error("Compressor::ObtainVersionCode io exception: " + exception.getMessage());
            throw new BundleException("Compressor::ObtainVersionCode failed");
        } finally {
            Utility.closeStream(zipFile);
            Utility.closeStream(zipInput);
            Utility.closeStream(in);
            Utility.closeStream(zin);
            Utility.closeStream(inputStream);
            Utility.closeStream(reader);
            Utility.closeStream(br);
        }
        return obtainInnerVersion(configJson);
    }

    private String obtainInnerVersion(String configJson) throws BundleException {
        try {
            if (configJson != null) {
                int indexOfVersion = configJson.indexOf(VERSION);
                if (indexOfVersion <= 0) {
                    LOG.error("Compressor::obtainInnerVersion obtain index of version failed");
                    throw new BundleException("Compressor::obtainInnerVersion obtain index of version failed");
                }
                int lastIndex = configJson.indexOf(JSON_END, indexOfVersion);
                if (lastIndex <= 0) {
                    LOG.error("Compressor::obtainInnerVersion obtain last index failed");
                    throw new BundleException("Compressor::obtainInnerVersion obtain last index failed");
                }
                String version = configJson.substring(indexOfVersion, lastIndex + 1);
                if (version == null || version.isEmpty()) {
                    LOG.error("Compressor::obtainInnerVersion version is null or empty");
                    throw new BundleException("Compressor::obtainInnerVersion failed due to null or empty version!");
                }
                return version.trim();
            }
        } catch (BundleException exception) {
            LOG.error("Compressor::obtainInnerVersion io exception: " + exception.getMessage());
            throw new BundleException("Compressor::obtainInnerVersion failed");
        }
        return "";
    }

    /**
     * get version code from hap file.
     *
     * @param configJson config.json from hap file
     * @return version code
     * @throws BundleException FileNotFoundException|IOException.
     */
    private String obtainInnerVersionCode(String configJson) throws BundleException {
        try {
            if (configJson != null) {
                int indexOfCode = configJson.indexOf(CODE);
                if (indexOfCode <= 0) {
                    LOG.error("Compressor::ObtainInnerVersionCode obtain index failed");
                    throw new BundleException("Compressor::ObtainInnerVersionCode obtain index failed");
                }
                int index = configJson.indexOf(COLON, indexOfCode);
                if (index <= 0) {
                    LOG.error("Compressor::ObtainInnerVersionCode obtain index failed");
                    throw new BundleException("Compressor::ObtainInnerVersionCode obtain index failed");
                }
                int lastIndex = configJson.indexOf(COMMA, index);
                if (lastIndex <= 0) {
                    LOG.error("Compressor::ObtainInnerVersionCode obtain index failed");
                    throw new BundleException("Compressor::ObtainInnerVersionCode obtain index failed");
                }
                String code = configJson.substring(index + 1, lastIndex);
                if (code == null || code.isEmpty()) {
                    LOG.error("Compressor::ObtainInnerVersionCode code is null or empty");
                    throw new BundleException("Compressor::ObtainInnerVersionCode failed due to null or empty code!");
                }
                return code.trim();
            }
        } catch (BundleException exception) {
            LOG.error("Compressor::ObtainInnerVersionCode io exception: " + exception.getMessage());
            throw new BundleException("Compressor::ObtainInnerVersionCode failed");
        }
        return "";
    }

    /**
     * get version name from hap file.
     *
     * @param configJson config.json from hap file
     * @return version name
     * @throws BundleException FileNotFoundException|IOException.
     */
    private String obtainInnerVersionName(String configJson) throws BundleException {
        try {
            if (configJson != null) {
                int indexOfCode = configJson.indexOf(NAME);
                if (indexOfCode <= 0) {
                    LOG.error("Compressor::obtainInnerVersionName obtain index failed");
                    throw new BundleException("Compressor::obtainInnerVersionName obtain index failed");
                }
                int index = configJson.indexOf(COLON, indexOfCode);
                if (index <= 0) {
                    LOG.error("Compressor::obtainInnerVersionName obtain index failed");
                    throw new BundleException("Compressor::obtainInnerVersionName obtain index failed");
                }
                int lastIndex = configJson.indexOf(JSON_END, index);
                if (lastIndex <= 0) {
                    LOG.error("Compressor::obtainInnerVersionName obtain index failed");
                    throw new BundleException("Compressor::obtainInnerVersionName obtain index failed");
                }
                String name = configJson.substring(index + 1, lastIndex);
                if (name == null || name.isEmpty()) {
                    LOG.error("Compressor::obtainInnerVersionName name is null or empty");
                    throw new BundleException("Compressor::obtainInnerVersionName failed due to null or empty name!");
                }
                return name.trim();
            }
        } catch (BundleException exception) {
            LOG.error("Compressor::obtainInnerVersionName io exception: " + exception.getMessage());
            throw new BundleException("Compressor::obtainInnerVersionName failed");
        }
        return "";
    }

    /**
     * Parse module name from config.json
     *
     * @param bufferedReader config.json buffered Reader
     * @param utility        common data
     * @throws BundleException IOException
     */
    private void parseModuleName(BufferedReader bufferedReader, Utility utility) throws BundleException {
        String lineStr = null;
        boolean isDistroStart = false;
        try {
            while ((lineStr = bufferedReader.readLine()) != null) {
                if (!isDistroStart) {
                    if (lineStr.contains(DISTRO)) {
                        isDistroStart = true;
                    }
                    continue;
                }
                if (lineStr.contains(JSON_END)) {
                    continue;
                }
                if (lineStr.contains(MODULE_NAME_NEW) || lineStr.contains(MODULE_NAME)) {
                    getModuleNameFromString(lineStr, utility);
                    break;
                }
            }
        } catch (IOException exception) {
            LOG.error("Compressor::parseModuleName io exception: " + exception.getMessage());
            throw new BundleException("Parse module name failed");
        }
    }

    /**
     * Parse module name from pack.info
     *
     * @param bufferedReader pack.info buffered Reader
     * @param utility        common data
     * @throws BundleException IOException
     */
    private void parsePackModuleName(BufferedReader bufferedReader, Utility utility) throws BundleException {
        String lineStr = null;
        boolean isDistroStart = false;
        try {
            while ((lineStr = bufferedReader.readLine()) != null) {
                if (lineStr.contains(DISTRO)) {
                    continue;
                }
                if (lineStr.contains(JSON_END)) {
                    continue;
                }
                if (lineStr.contains(MODULE_NAME_NEW) || lineStr.contains(MODULE_NAME)) {
                    getModuleNameFromString(lineStr, utility);
                }
            }
        } catch (IOException exception) {
            LOG.error("Compressor::parseModuleName io exception: " + exception.getMessage());
            throw new BundleException("Parse module name failed");
        }
    }

    /**
     * Parse Forms name from pack.info
     *
     * @param bufferedReader pack.info buffered Reader
     * @param utility        common data
     * @throws BundleException IOException
     */
    private void parsePackFormName(BufferedReader bufferedReader, Utility utility) throws BundleException {
        String lineStr = null;
        try {
            while ((lineStr = bufferedReader.readLine()) != null) {
                if (lineStr.contains("abilities")) {
                    continue;
                }
                if (lineStr.contains(FORMS)) {
                    continue;
                }
                if (lineStr.contains(JSON_END)) {
                    continue;
                }
                if (lineStr.contains(NAME)) {
                    getNameFromString(lineStr, utility);
                }
            }
        } catch (IOException exception) {
            LOG.error("Compressor::parseModuleName io exception: " + exception.getMessage());
            throw new BundleException("Parse module name failed");
        }
    }


    /**
     * Get name from line string
     *
     * @param lineStr line string
     * @param utility common data
     * @throws BundleException StringIndexOutOfBoundsException
     */
    private void getNameFromString(String lineStr, Utility utility) throws BundleException {
        try {
            int endIndex = lineStr.lastIndexOf(SEMICOLON);
            if (endIndex <= 0) {
                LOG.error("Compressor::getModuleNameFromString field the json is not standard.");
                throw new BundleException("Parse module name failed, module-name is invalid");
            }
            int startIndex = lineStr.lastIndexOf(SEMICOLON, endIndex - 1) + 1;
            String formName = lineStr.substring(startIndex, endIndex);
            if (formName == null || formName.isEmpty()) {
                LOG.error("Compressor::getModuleNameFromString field module-name is empty");
                throw new BundleException("Parse module name failed, module-name is empty");
            }
            String[] nameList = formName.split("\\.");
            if (nameList.length <= 1) {
                formNamesList.add(formName);
                utility.addFormNameList(formName);
            }
        } catch (StringIndexOutOfBoundsException exception) {
            LOG.error("Compressor::parseModuleName field module-name is fault: " + exception.getMessage());
            throw new BundleException("Parse module name failed, module-name is invalid");
        }
    }

    /**
     * Get module name from line string
     *
     * @param lineStr line string
     * @param utility common data
     * @throws BundleException StringIndexOutOfBoundsException
     */
    private void getModuleNameFromString(String lineStr, Utility utility) throws BundleException {
        try {
            int endIndex = lineStr.lastIndexOf(SEMICOLON);
            if (endIndex <= 0) {
                LOG.error("Compressor::getModuleNameFromString field the json is not standard.");
                throw new BundleException("Parse module name failed, module-name is invalid");
            }
            int startIndex = lineStr.lastIndexOf(SEMICOLON, endIndex - 1) + 1;
            String moduleName = lineStr.substring(startIndex, endIndex);
            list.add(moduleName);
            if (moduleName == null || moduleName.isEmpty()) {
                LOG.error("Compressor::getModuleNameFromString field module-name is empty");
                throw new BundleException("Parse module name failed, module-name is empty");
            }
            utility.setModuleName(moduleName);
        } catch (StringIndexOutOfBoundsException exception) {
            LOG.error("Compressor::parseModuleName field module-name is fault: " + exception.getMessage());
            throw new BundleException("Parse module name failed, module-name is invalid");
        }
    }

    private void parseCompressNativeLibs(BufferedReader bufferedReader, Utility utility) throws BundleException {
        String lineStr = null;
        try {
            while ((lineStr = bufferedReader.readLine()) != null) {
                if (lineStr.contains(COMPRESS_NATIVE_LIBS)) {
                    if (lineStr.contains(Utility.FALSE_STRING)) {
                        utility.setIsCompressNativeLibs(false);
                        break;
                    }
                }
            }
        } catch (IOException exception) {
            LOG.error("Compressor::parseCompressNativeLibs io exception: " + exception.getMessage());
            throw new BundleException("Parse compress native libs failed");
        }
    }

    /**
     * ZipOutputStream flush, closeEntry and finish.
     */
    private void closeZipOutputStream() {
        try {
            if (zipOut != null) {
                zipOut.flush();
            }
        } catch (IOException exception) {
            LOG.error("Compressor::closeZipOutputStream flush exception " + exception.getMessage());
        }
        try {
            if (zipOut != null) {
                zipOut.closeEntry();
            }
        } catch (IOException exception) {
            LOG.error("Compressor::closeZipOutputStream close entry io exception " + exception.getMessage());
        }
        try {
            if (zipOut != null) {
                zipOut.finish();
            }
        } catch (IOException exception) {
            LOG.error("Compressor::closeZipOutputStream finish exception " + exception.getMessage());
        }
    }

    /**
     * Parse device type from config.json
     *
     * @param bufferedReader config.json buffered Reader
     * @param utility        common data
     * @throws BundleException IOException
     */
    private void parseDeviceType(BufferedReader bufferedReader, Utility utility) throws BundleException {
        String lineStr = null;
        boolean isDeviceTypeStart = false;
        try {
            while ((lineStr = bufferedReader.readLine()) != null) {
                if (!isDeviceTypeStart) {
                    if (lineStr.contains(DEVICE_TYPE)) {
                        isDeviceTypeStart = true;
                    }
                    continue;
                }
                if (lineStr.contains(JSON_END)) {
                    break;
                }
                utility.setDeviceType(lineStr);
                break;
            }
        } catch (IOException exception) {
            LOG.error("Compressor::parseDeviceType io exception: " + exception.getMessage());
            throw new BundleException("Parse device type failed");
        }
    }
}
