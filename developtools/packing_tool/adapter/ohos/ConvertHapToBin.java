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
import java.util.Optional;

/**
 * hap package to bin package tool main class.
 *
 */
public class ConvertHapToBin {
    private static final String HAP_PATH = "--hap-path";
    private static final String BIN_PATH = "--bin-path";
    private static final String PROJECT_PATH = "--project-path";
    private static final String OUTPUT_PATH = "output";
    private static final int PARA_COUNT = 4;
    private static final Log LOG = new Log(ConvertHapToBin.class.toString());

    /**
     * generate binary file from hap package
     *
     * @param hapPath: hap path
     * @param binPath: bin path
     * @return true: success, false: fail
     */
    public static boolean packHapTobin(final String hapPath, final String binPath) {
        if (hapPath.isEmpty() || binPath.isEmpty()) {
            LOG.error("hapPath or binPath is null");
            return false;
        }

        if (!FileUtils.checkFileIsExists(hapPath)) {
            return false;
        }

        Optional<String> absHapPath = FileUtils.getFormatedPath(hapPath);
        File file = new File(absHapPath.get());
        String projectPath = file.getParent() + File.separator + OUTPUT_PATH;
        FileUtils.unzip(absHapPath.get(), projectPath);

        if (!BinaryTool.generateBinaryFile(projectPath, binPath)) {
            LOG.error("create bin file failed!");
            return false;
        }
        // delete project path
        FileUtils.deleteDirectory(projectPath);
        return true;
    }

    /**
     * generate binary file from hap project
     *
     * @param projectPath: project path
     * @param binPath: bin path
     * @return true: success, false: fail
     */
    public static boolean packProjectTobin(final String projectPath, final String binPath) {
        if (projectPath.isEmpty() || binPath.isEmpty()) {
            LOG.error("projectPath or binPath is null");
            return false;
        }

        if (!FileUtils.checkFileIsExists(projectPath)) {
            return false;
        }

        Optional<String> absProjectPath = FileUtils.getFormatedPath(projectPath);
        if (!BinaryTool.generateBinaryFile(absProjectPath.get(), binPath)) {
            LOG.error("create bin file failed!");
            return false;
        }
        return true;
    }

    /**
     * tool main function.
     *
     * @param args command line
     */
    public static void main(String[] args) {
        if (args.length != PARA_COUNT) {
            LOG.error("param length is wrong!");
            return;
        }
        String hapPath = "";
        String binPath = "";
        String projectPath = "";
        int count = 0;
        for (String para : args) {
            if (para.equals(HAP_PATH) && count < args.length - 1) {
                hapPath = args[count + 1];
            }

            if (para.equals(PROJECT_PATH) && count < args.length - 1) {
                projectPath = args[count + 1];
            }

            if (para.equals(BIN_PATH) && count < args.length - 1) {
                binPath = args[count + 1];
            }
            count++;
        }
        if (!hapPath.isEmpty() && !binPath.isEmpty()) {
            packHapTobin(hapPath, binPath);
        }

        if (!projectPath.isEmpty() && !binPath.isEmpty()) {
            packProjectTobin(projectPath, binPath);
        }
    }
}