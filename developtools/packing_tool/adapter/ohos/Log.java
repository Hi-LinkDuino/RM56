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

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Class used to print logs and will remove if the official log module is ready.
 *
 */
public class Log {
    private static Logger log;

    /**
     * A constructor used to create a Log.
     *
     * @param className class name
     */
    public Log(String className) {
        log = Logger.getLogger(className);
        log.setLevel(Level.WARNING);
    }

    /**
     * print log in DEBUG level.
     *
     * @param msg log's content
     */
    public void debug(String msg) {
        log.info(LogType.buildTag(LogType.DEBUG) + msg);
    }

    /**
     * print log in INFO level.
     *
     * @param msg log's content
     */
    public void info(String msg) {
        log.info(LogType.buildTag(LogType.INFO) + msg);
    }

    /**
     * print log in ERROR level.
     *
     * @param msg log's content
     */
    public void error(String msg) {
        log.warning(LogType.buildTag(LogType.ERROR) + msg);
    }
}

enum LogType {
    DEBUG("Debug"),
    INFO("Info"),
    ERROR("Error");

    private String value;

    LogType(String value) {
        this.value = value;
    }

    /**
     * get log tag
     *
     * @param logType log type
     * @return format log tag
     */
    public static String buildTag(LogType logType) {
        return "Ohos BundleTool [" + logType.value + "]: ";
    }
}

