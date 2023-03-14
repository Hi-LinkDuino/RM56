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
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Optional;
import java.util.Queue;

/**
 * generate binary file
 *
 */
public class BinaryTool {
    private static final String JSON_FILE_NAME = "config.json";
    private static final String PROFILE_KEY = "app";
    private static final String PACKAGE_KEY = "bundleName";
    private static final Log LOG = new Log(BinaryTool.class.toString());

    /**
     * generate binary file
     *
     * @param filePath absolute project path
     * @param binaryFilePath absolute binaryFilePath
     * @return true: success, false: fail
     */
    public static boolean generateBinaryFile(final String filePath, final String binaryFilePath) {
        Optional<String> absPath = FileUtils.getFormatedPath(filePath);
        if (!absPath.isPresent()) {
            return false;
        }
        Optional<String> absBinaryFilePath = FileUtils.getFormatedPath(binaryFilePath);
        if (!absBinaryFilePath.isPresent()) {
            return false;
        }

        boolean isSuccess = false;
        RandomAccessFile appStream = null;
        try {
            appStream = new RandomAccessFile(absBinaryFilePath.get(), "rwd");
            if (!writePackageInfo(absPath.get(), appStream)) {
                return false;
            }
            ArrayList<String> fileList = new ArrayList<>();
            FileUtils.getFileList(absPath.get(), fileList);
            ArrayList<String> writeFileList = new ArrayList<>();
            boolean isWindows = System.getProperty("os.name").toLowerCase().startsWith("win");
            writeFileInfo(absPath.get(), absPath.get(), isWindows, writeFileList, appStream);
            if (fileList.size() != writeFileList.size()) {
                return false;
            }
            isSuccess = true;
            return true;
        } catch (IOException msg) {
            LOG.error("IOException, msg is: " + msg.getMessage());
            return false;
        } finally {
            FileUtils.closeStream(appStream);
            if (!isSuccess) {
                LOG.error("build binary file failed!");
                FileUtils.deleteFile(absBinaryFilePath.get());
            }
        }
    }

    /**
     * write package Info in binary file
     *
     * @param filePath the file path which need to be writen
     * @param appStream file stream
     * @return true: success, false: fail
     */
    private static boolean writePackageInfo(final String filePath, RandomAccessFile appStream) {
        Optional<String> packageName = FileUtils.getValueFromJsonFileContent(PROFILE_KEY, PACKAGE_KEY,
            JSON_FILE_NAME, filePath);
        if (!packageName.isPresent()) {
            LOG.error("have no config.json or have no key of package!");
            return false;
        }
        try {
            appStream.write(0xbe);
            // add packageName
            int strLen = packageName.get().length();
            appStream.writeInt(strLen);
            appStream.writeBytes(packageName.get());
        } catch (IOException msg) {
            LOG.error("write packageInfo fail, msg is " + msg.getMessage());
            return false;
        }
        return true;
    }

    /**
     * write file Info in binary file
     *
     * @param filePath project file path
     * @param directory project dir path
     * @param isWindows flag which is for windows system or linux system
     * @param fileList the file path in directory
     * @param appStream file stream
     */
    private static void writeFileInfo(final String filePath, final String directory, boolean isWindows,
        ArrayList<String> fileList, RandomAccessFile appStream) {
        File file = new File(filePath);
        if (!file.exists()) {
            return;
        }

        Queue<File> fileDirList = new LinkedList<>();
        fileDirList.add(file);
        while (!fileDirList.isEmpty()) {
            File dirFile = fileDirList.poll();
            File[] files = dirFile.listFiles();
            for (File f : files) {
                try {
                    if (f.isFile()) {
                        // write name length
                        String name = f.getName();
                        appStream.writeInt(name.length());
                        // write name
                        appStream.writeBytes(name);
                        // write relative path length, 4Bytes
                        String relativePath = f.getParent().replace(directory, "");
                        if (isWindows) {
                            relativePath = relativePath.replace("\\", "/");
                        }
                        appStream.writeInt(relativePath.length());
                        // write relative path
                        appStream.writeBytes(relativePath);
                        // write file length, 8Bytes
                        appStream.writeLong(f.length());
                        // write file data
                        String absolutePath = f.getCanonicalPath();
                        appStream.write(FileUtils.getFileData(absolutePath));
                        fileList.add(absolutePath);
                    } else {
                        fileDirList.add(f);
                    }
                } catch (IOException msg) {
                    LOG.error("write fail, msg is: " + msg.getMessage());
                }
            }
        }
    }
}