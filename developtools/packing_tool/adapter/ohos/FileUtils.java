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

import ohos.utils.fastjson.JSONObject;

import java.io.BufferedReader;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Optional;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

/**
 * file tools
 *
 */
public class FileUtils {
    private static final int BUFFER_SIZE = 1024;
    private static final Log LOG = new Log(FileUtils.class.toString());

    /**
     * generate fileData byte stream
     *
     * @param filePath file path
     * @return byte array
     */
    public static byte[] getFileData(final String filePath) {
        File file = new File(filePath);
        long fileSize = file.length();
        byte[] buffer = new byte[(int) fileSize];
        if (fileSize > Integer.MAX_VALUE) {
            LOG.error("file too big!");
            return buffer;
        }
        FileInputStream fi = null;
        try {
            fi = new FileInputStream(file);
            int offset = 0;
            int numRead = 0;
            while (offset < buffer.length && (numRead = fi.read(buffer, offset, buffer.length - offset)) >= 0) {
                offset += numRead;
            }
        } catch (IOException exception) {
            LOG.error("getFileData IOException error: " + exception.getMessage());
            return new byte[(int) fileSize];
        } finally {
            closeStream(fi);
        }
        return buffer;
    }

    /**
     * search filePath for special fileName
     *
     * @param fileName file name
     * @param directory dir path
     * @return filePath
     */
    private static Optional<String> searchFile(final String fileName, final String directory) {
        ArrayList<String> fileList = new ArrayList<>();
        getFileList(directory, fileList);
        for (String fileItem : fileList) {
            if (fileItem.endsWith(fileName)) {
                return Optional.ofNullable(fileItem);
            }
        }
        return Optional.empty();
    }

    /**
     * get file list in filePath
     *
     * @param filePath file path
     * @param fileList file path in arrayList
     */
    public static void getFileList(final String filePath, ArrayList<String> fileList) {
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
                    fileList.add(f.getCanonicalPath());
                } else if (f.isDirectory()) {
                    getFileList(f.getCanonicalPath(), fileList);
                } else {
                    LOG.error("It's not file or directory!");
                }
            } catch (IOException msg) {
                LOG.error("IOException error: " + msg.getMessage());
                return;
            }
        }
    }

    /**
     * get string from file
     *
     * @param filePath file path
     * @return String for file
     */
    private static Optional<String> getFileContent(final String filePath) {
        if (filePath.isEmpty()) {
            return Optional.empty();
        }
        BufferedReader reader = null;
        FileInputStream fileInputStream = null;
        InputStreamReader inputStreamReader = null;
        StringBuilder content = new StringBuilder();
        try {
            fileInputStream = new FileInputStream(filePath);
            inputStreamReader = new InputStreamReader(fileInputStream, "UTF-8");
            reader = new BufferedReader(inputStreamReader);
            String tempString;
            while ((tempString = reader.readLine()) != null) {
                content.append(tempString);
            }
        } catch (IOException msg) {
            LOG.error("get file content fail, msg is " + msg.getMessage());
            return Optional.empty();
        } finally {
            closeStream(reader);
            closeStream(inputStreamReader);
            closeStream(fileInputStream);
        }
        return Optional.ofNullable(content.toString());
    }

    /**
     * get special value from JSON String
     *
     * @param key json main key
     * @param subKey json sub key
     * @param jsonFileName json file name
     * @param filePath path which will be searched
     * @return value
     */
    public static Optional<String> getValueFromJsonFileContent(final String key, final String subKey,
        final String jsonFileName, final String filePath) {
        Optional<String> jsonFilePath = searchFile(jsonFileName, filePath);
        if (!jsonFilePath.isPresent()) {
            return Optional.empty();
        }

        Optional<String> jsonStr = getFileContent(jsonFilePath.get());
        if (!jsonStr.isPresent()) {
            return Optional.empty();
        }

        JSONObject jsonObject = JSONObject.parseObject(jsonStr.get());
        if (jsonObject == null) {
            return Optional.empty();
        }

        if (!jsonObject.containsKey(key)) {
            return Optional.empty();
        }

        JSONObject subObject = jsonObject.getJSONObject(key);
        String value = subObject.getString(subKey);
        return Optional.of(value);
    }

    /**
     * close file stream
     *
     * @param fileStream file stream
     */
    public static void closeStream(Closeable fileStream) {
        try {
            if (fileStream != null) {
                fileStream.close();
            }
        } catch (IOException msg) {
            LOG.error("stream close Error, msg is " + msg.getMessage());
        }
    }

    /**
     * delete file
     *
     * @param path file path which will be deleted
     */
    public static void deleteFile(final String path) {
        File file = new File(path);
        if (file.exists()) {
            file.delete();
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

                int count;
                int total = 0;
                byte[] data = new byte[BUFFER_SIZE];
                while ((count = bis.read(data, 0, BUFFER_SIZE)) != -1) {
                    bos.write(data, 0, count);
                    total += count;
                }

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
            closeStream(bos);
            closeStream(fos);
            closeStream(bis);
            closeStream(zipFile);
        }
    }

    /**
     * delete directory
     *
     * @param directory dir path which will be deleted
     */
    public static void deleteDirectory(final String directory) {
        File dir = new File(directory);
        if (!dir.exists()) {
            return;
        }

        File[] fileList = dir.listFiles();
        if (fileList == null) {
            return;
        }
        for (File file : fileList) {
            try {
                if (file.isFile()) {
                    file.delete();
                } else if (file.isDirectory()) {
                    deleteDirectory(file.getCanonicalPath());
                } else {
                    LOG.error("It's not file or directory!");
                }
            } catch (IOException msg) {
                LOG.error("deleteDirectory IOException : " + msg.getMessage());
            }
        }
        dir.delete();
    }

    /**
     * format filepath
     *
     * @param filePath file path which is need to format
     * @return canonicalPath for filePath
     */
    public static Optional<String> getFormatedPath(final String filePath) {
        String absPath;
        try {
            File file = new File(filePath);
            absPath = file.getCanonicalPath();
        } catch (IOException msg) {
            LOG.error("format path IOException : " + msg.getMessage());
            return Optional.empty();
        }
        return Optional.of(absPath);
    }

    /**
     * check file whether is exist or not
     *
     * @param filePath bin file path
     * @return true: file is exist, false: file is not exist
     */
    public static boolean checkFileIsExists(final String filePath) {
        if (filePath.isEmpty()) {
            return false;
        }

        Optional<String> absFilePath = getFormatedPath(filePath);
        if (!absFilePath.isPresent()) {
            return false;
        }

        File file = new File(absFilePath.get());
        if (!file.exists()) {
            return false;
        }
        return true;
    }
}