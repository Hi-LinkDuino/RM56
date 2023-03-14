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

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Optional;

/**
 * bin file to hap tool main class.
 *
 */
public class CollectBinInfo {
    private static final String JSON_FILE_NAME = "config.json";
    private static final String PROFILE_KEY = "app";
    private static final String VERSION_KEY = "version";
    private static final String CODE_KEY = "code";
    private static final String BIN_PATH = "--bin-path";
    private static final String VERSION_CODE = "--version-code";
    private static final int PARA_COUNT = 3;
    private static final int MAGIC_NUMBER = 190;
    private static final int INT_LENGTH = 4;
    private static final int LONG_LENGTH = 8;
    private static int bundleNameLen = 0;

    /**
     * get versionCode from bin file
     *
     * @param binPath: bin path
     * @return versionCode
     */
    public static String getVersionCode(final String binPath) {
        String versionCode = "";
        if (!FileUtils.checkFileIsExists(binPath)) {
            return versionCode;
        }

        Optional<String> absBinPath = FileUtils.getFormatedPath(binPath);
        RandomAccessFile binStream = null;
        try {
            binStream = new RandomAccessFile(absBinPath.get(), "r");
            if (!readBinFileHeader(binStream)) {
                return versionCode;
            }
            long fileDataPos = 1 + INT_LENGTH + bundleNameLen;
            byte [] fileDataByte = readBinFileData(JSON_FILE_NAME, binStream, fileDataPos);
            if (fileDataByte == null) {
                return versionCode;
            }
            String fileData = new String(fileDataByte, "UTF-8");
            JSONObject object = JSONObject.parseObject(fileData);
            if (object == null || !object.containsKey(PROFILE_KEY)) {
                return versionCode;
            }
            object = object.getJSONObject(PROFILE_KEY);
            if (object == null || !object.containsKey(VERSION_KEY)) {
                return versionCode;
            }
            object = object.getJSONObject(VERSION_KEY);
            if (object != null) {
                versionCode = object.getInteger(CODE_KEY).toString();
            }
            return versionCode;
        } catch (IOException msg) {
            return versionCode;
        } finally {
            if (binStream != null) {
                FileUtils.closeStream(binStream);
            }
        }
    }

    /**
     * read bin file header
     *
     * @param binStream: bin randAccessFileStream
     * @return true or false
     */
    private static boolean readBinFileHeader(RandomAccessFile binStream) {
        try {
            byte magic = binStream.readByte();
            if ((magic & 0xff) != MAGIC_NUMBER) {
                return false;
            }
            bundleNameLen = binStream.readInt();
            if (bundleNameLen >= 0) {
                byte [] bundleNameByte = new byte[bundleNameLen];
                binStream.readFully(bundleNameByte, 0, bundleNameLen);
                return true;
            }
            return false;
        } catch (IOException msg) {
            return false;
        }
    }

    /**
     * read bin file data
     *
     * @param fileName: file name
     * @param binStream: bin randAccessFileStream
     * @param fileDataPos: position which begin to read file data
     * @return fileData byte
     */
    private static byte [] readBinFileData(final String fileName, RandomAccessFile binStream, long fileDataPos) {
        try {
            long fileSize = binStream.length();
            long pos = fileDataPos;
            while (pos < fileSize) {
                int nameLen = binStream.readInt();
                if (nameLen <= 0) {
                    return null;
                }
                byte [] fileNameByte = new byte[nameLen];
                binStream.readFully(fileNameByte, 0, nameLen);
                int relativePathLen = binStream.readInt();
                if (relativePathLen < 0) {
                    return null;
                }
                binStream.seek(pos + INT_LENGTH + nameLen + INT_LENGTH + relativePathLen);
                long dataLen = binStream.readLong();
                if (dataLen > Integer.MAX_VALUE || dataLen <= 0) {
                    return null;
                }
                byte [] fileData = new byte[(int) dataLen];
                binStream.readFully(fileData, 0, (int) dataLen);
                pos = pos + INT_LENGTH + nameLen + INT_LENGTH + relativePathLen + LONG_LENGTH + dataLen;
                String name = new String(fileNameByte, "UTF-8");
                if (name.equals(fileName)) {
                    return fileData;
                }
            }
            return null;
        } catch (IOException msg) {
            return null;
        }
    }

    /**
     * tool main function.
     *
     * @param args command line
     */
    public static void main(String[] args) {
        if (args.length != PARA_COUNT) {
            return;
        }
        String binPath = "";
        int count = 0;
        boolean versionFlag = false;
        for (String para : args) {
            if (para.equals(BIN_PATH) && count < args.length - 1) {
                binPath = args[count + 1];
            }
            if (para.equals(VERSION_CODE) && count < args.length) {
                versionFlag = true;
            }
            count++;
        }
        if (!binPath.isEmpty() && versionFlag) {
            String versionCode = getVersionCode(binPath);
        }
    }
}