/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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
#include "js_fwk_common.h"
#include <climits>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#if ((defined(__WIN32)) || (defined(__WIN64)))
#include <shlwapi.h>
#endif
#include "ace_event_error_code.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "component.h"
#include "component_utils.h"
#include "dfx_assist.h"
#include "fatal_handler.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_profiler.h"
#if IS_ENABLED(CONSOLE_LOG_OUTPUT)
#include "presets/console_module.h"
#endif
#include "presets/console_log_impl.h"
#include "product_adapter.h"
#include "securec.h"
#include "task_manager.h"

#if ((defined __LITEOS__) || (defined __linux__) || (SCREENSIZE_SPECIFIED == 1))
#include <screen.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

namespace OHOS {
namespace ACELite {
void JerrySetNamedProperty(jerry_value_t object, const char * const name, jerry_value_t propValue)
{
    jerry_release_value(jerryx_set_property_str(object, name, propValue));
}

void JerrySetNumberProperty(jerry_value_t object, const char * const name, double value)
{
    jerry_value_t numValue = jerry_create_number(value);
    JerrySetNamedProperty(object, name, numValue);
    jerry_release_value(numValue);
}

void JerrySetStringProperty(jerry_value_t object, const char * const name, const char * const value)
{
    jerry_value_t strValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    JerrySetNamedProperty(object, name, strValue);
    jerry_release_value(strValue);
}

void JerrySetStringProperty(jerry_value_t object, const char * const name, const char * const value, uint32_t length)
{
    jerry_value_t strValue = jerry_create_string_sz_from_utf8(reinterpret_cast<const jerry_char_t *>(value), length);
    JerrySetNamedProperty(object, name, strValue);
    jerry_release_value(strValue);
}

char *JerryMallocStringProperty(const jerry_value_t object, const char * const name, uint16_t &length)
{
    jerry_value_t propValue = jerryx_get_property_str(object, name);
    char *res = MallocStringOf(propValue, &length);
    jerry_release_value(propValue);
    return res;
}

int16_t JerryGetIntegerProperty(jerry_value_t object, const char * const name)
{
    jerry_value_t value = jerryx_get_property_str(object, name);
    int16_t result = IntegerOf(value);
    jerry_release_value(value);
    return result;
}

bool JerryGetBoolProperty(jerry_value_t object, const char * const name, bool &outValue)
{
    jerry_value_t value = jerryx_get_property_str(object, name);
    if (!jerry_value_is_boolean(value)) {
        jerry_release_value(value);
        HILOG_ERROR(HILOG_MODULE_ACE, "js value is not bool.");
        return false;
    }
    outValue = jerry_value_to_boolean(value);
    jerry_release_value(value);
    return true;
}

void JerrySetFuncProperty(jerry_value_t object, const char * const name, jerry_external_handler_t handler)
{
    if (name == nullptr || !strlen(name)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to set function property cause by empty name.");
        return;
    }

    if (handler == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to set function property cause by empty handler.");
        return;
    }

    jerry_value_t func = jerry_create_external_function(handler);
    jerryx_set_property_str(object, name, func);
    jerry_release_value(func);
}

/* return value must be freed by caller, except the returned value is nullptr */
char *MallocStringOf(jerry_value_t source)
{
    uint16_t length = 0;
    return MallocStringOf(source, &length);
}

char *MallocStringOf(jerry_value_t source, uint16_t *strLength)
{
    if ((IS_UNDEFINED(source)) || (strLength == nullptr)) {
        return nullptr;
    }

    *strLength = 0;
    bool success = false;
    jerry_value_t target = UNDEFINED;
    jerry_char_t *buffer = nullptr;
    jerry_size_t length = 0;
    do {
        if (jerry_value_is_symbol(source)) {
            target = jerry_get_symbol_descriptive_string(source);
        } else {
            target = jerry_value_to_string(source);
        }
        if (IS_ERROR_VALUE(target)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "jerry_value_to_string failed, can not continue to generate char buffer");
            break;
        }

        jerry_size_t size = jerry_get_string_size(target);
        if (size >= UINT16_MAX) {
            break;
        }
        buffer = static_cast<jerry_char_t *>(ace_malloc(sizeof(jerry_char_t) * (size + 1)));
        if (buffer == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for convert jerry string failed");
            break;
        }
        if (size == 0) {
            // if here got matched, it means the JS string is empty itself
            success = true;
            break;
        }

        length = jerry_string_to_char_buffer(target, buffer, size);
        if ((length == 0) || (length >= UINT16_MAX) || (length > size)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "jerry string to char buffer failed, target size[%{public}d]", size);
            break;
        }
        success = true;
    } while (0);

    jerry_release_value(target);
    if (!success) {
        ACE_FREE(buffer);
        return nullptr;
    }
    // successful scenario, add end character
    buffer[length] = '\0';
    *strLength = length;
    return reinterpret_cast<char *>(buffer);
}

bool BoolOf(jerry_value_t source)
{
    if (!jerry_value_is_boolean(source)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "js value is not bool.");
    }
    return jerry_value_to_boolean(source);
}

int16_t IntegerOf(jerry_value_t source)
{
    if (!jerry_value_is_number(source)) {
        char *strValue = MallocStringOf(source);
        if (strValue == nullptr) {
            return 0;
        }
        int16_t intValue = static_cast<int16_t>(strtol(strValue, nullptr, DEC));
        ace_free(strValue);
        return intValue;
    }
    int number = (int)jerry_get_number_value(source);
    if (number >= INT16_MAX) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "js number value is out of range.");
        return INT16_MAX;
    }
    if (number <= INT16_MIN) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "js number value is out of range.");
        return INT16_MIN;
    }
    return static_cast<int16_t>(number);
}

jerry_value_t WatcherCallbackFunc(const jerry_value_t func,
                                  const jerry_value_t context,
                                  const jerry_value_t *args,
                                  const jerry_length_t argsLength)
{
    if (argsLength != ARG_LENGTH_WATCHER_CALLBACK) {
        return UNDEFINED;
    }

    START_TRACING(WATCHER_CALLBACK_FUNC);
    jerry_value_t value = args[0];
    const uint8_t optionIndex = 2;
    jerry_value_t options = args[optionIndex];
    jerry_value_t nativeElement = jerryx_get_property_str(options, ARG_WATCH_EL);
    jerry_value_t attrKey = jerryx_get_property_str(options, ARG_WATCH_ATTR);

    Component *component = ComponentUtils::GetComponentFromBindingObject(nativeElement);
    if (component != nullptr) {
        uint16_t attrKeyStrLen = 0;
        char *attrKeyStr = MallocStringOf(attrKey, &attrKeyStrLen);
        if (attrKeyStr != nullptr) {
            uint16_t attrKeyId = KeyParser::ParseKeyId(attrKeyStr, attrKeyStrLen);
            bool updateResult = component->UpdateView(attrKeyId, value);
            if (updateResult) {
                component->Invalidate();
            }
            ace_free(attrKeyStr);
            attrKeyStr = nullptr;
        }
    }
    STOP_TRACING();
    ReleaseJerryValue(attrKey, nativeElement, VA_ARG_END_FLAG);
    return UNDEFINED;
}

// print out error information from jerry value
void PrintErrorMessage(const jerry_value_t errorValue)
{
    DfxAssist dfxAssist;
    dfxAssist.DumpErrorCode(errorValue);
    dfxAssist.DumpErrorMessage(errorValue);
#if IS_ENABLED(ENGINE_DEBUGGER)
    FlushOutput();
#endif
}

// wrapper function for jerry_call_function
jerry_value_t CallJSFunction(const jerry_value_t func,
                             const jerry_value_t context,
                             const jerry_value_t args[],
                             jerry_size_t argsCount)
{
    jerry_value_t ret = jerry_call_function(func, context, args, argsCount);
    // trace out error information if the result contains error
    if (jerry_value_is_error(ret)) {
        PrintErrorMessage(ret);
    }
    return ret;
}

void CallJSFunctionAutoRelease(const jerry_value_t funcObj,
                               const jerry_value_t thisVal,
                               const jerry_value_t args[],
                               const jerry_size_t argc)
{
    jerry_release_value(CallJSFunction(funcObj, thisVal, args, argc));
}

jerry_value_t CallJSFunctionOnRoot(const jerry_value_t funcObj, /* function object to call */
                                   const jerry_value_t args[],  /* function's call arguments */
                                   const jerry_size_t argc)     /* number of the arguments */
{
    jerry_value_t globalObject = jerry_get_global_object();
    jerry_value_t pageViewModel = jerryx_get_property_str(globalObject, ATTR_ROOT);
    jerry_value_t ret = CallJSFunction(funcObj, pageViewModel, args, argc);
    ReleaseJerryValue(globalObject, pageViewModel, VA_ARG_END_FLAG);
    return ret;
}

// arg1:target object watched, arg2:watcher callback, arg3:other argument
jerry_value_t CallJSWatcher(jerry_value_t arg1,
                            jerry_value_t (*watcherCB)(const jerry_value_t func,
                                                       const jerry_value_t context,
                                                       const jerry_value_t *args,
                                                       const jerry_length_t argsLength),
                            jerry_value_t arg3)
{
    jerry_value_t globalObject = jerry_get_global_object();
    jerry_value_t appViewModel = jerryx_get_property_str(globalObject, ATTR_ROOT);
    const char * const attrWatch = "$watch";
    jerry_value_t watchFunction = jerryx_get_property_str(appViewModel, attrWatch);
    jerry_value_t callbackFunc = jerry_create_external_function(watcherCB);
    jerry_value_t args[ARG_LENGTH_WATCHER] = {arg1, callbackFunc, arg3};
    jerry_value_t watcher = CallJSFunction(watchFunction, appViewModel, args, ARG_LENGTH_WATCHER);
    ReleaseJerryValue(callbackFunc, watchFunction, appViewModel, globalObject, VA_ARG_END_FLAG);
    return watcher;
}

#ifdef JS_EXTRA_EVENT_SUPPORT
bool CallBaseEvent(const jerry_value_t func, const Event &event, const uint16_t id)
{
    if (!jerry_value_is_function(func)) {
        return false;
    }

    jerry_value_t *args = ConvertBaseEventInfo(event, id);
    jerry_release_value(CallJSFunctionOnRoot(func, args, 1));
    ClearEventListener(args, 1);
    return true;
}

jerry_value_t *ConvertBaseEventInfo(const Event &event, const uint16_t id)
{
    const uint8_t argsNum = 1;
    jerry_value_t *args = new jerry_value_t[argsNum];
    args[0] = jerry_create_object();
    JerrySetStringProperty(args[0], "type", KeyParser::GetKeyById(id));
    JerrySetNumberProperty(args[0], "timestamp", event.GetTimeStamp());
    jerry_value_t point = jerry_create_object();
    JerrySetNumberProperty(point, "x", event.GetCurrentPos().x);
    JerrySetNumberProperty(point, "y", event.GetCurrentPos().y);
    JerrySetNamedProperty(args[0], "point", point);
    jerry_release_value(point);
    return args;
}

jerry_value_t *ConvertDragEventInfo(const DragEvent &event, const uint16_t id)
{
    jerry_value_t *args = ConvertBaseEventInfo(event, id);
    jerry_value_t touches = jerry_create_array(1);
    jerry_value_t point = jerry_create_object();
    JerrySetNumberProperty(point, "globalX", event.GetCurrentPos().x);
    JerrySetNumberProperty(point, "globalY", event.GetCurrentPos().y);
    jerry_release_value(jerry_set_property_by_index(touches, 0, point));
    jerry_release_value(point);
    JerrySetNamedProperty(args[0], "touches", touches);
    jerry_release_value(touches);
    return args;
}

jerry_value_t *ConvertKeyEventInfo(const KeyEvent &event)
{
    const uint8_t argsNum = 1;
    jerry_value_t *args = new jerry_value_t[argsNum];
    args[0] = jerry_create_object();
    JerrySetNumberProperty(args[0], "code", event.GetState());
    JerrySetNumberProperty(args[0], "action", event.GetKeyId());
    JerrySetNumberProperty(args[0], "repeatCount", 0);
    JerrySetNumberProperty(args[0], "timeStampStart", event.GetTimeStamp());
    return args;
}

void ClearEventListener(const jerry_value_t args[], const uint8_t argc)
{
    for (uint8_t i = 0; i < argc; i++) {
        jerry_release_value(args[i]);
    }
    delete[] args;
    args = nullptr;
}
#endif

/**
 * only used in this file, caller must make sure the target buffer is big enough.
 * return the copied byte count.
 */
static size_t AppendTwoPath(char * const first, uint8_t startIndex, const char * const sec, const uint16_t destSize)
{
    if ((first == nullptr) || (sec == nullptr) || (strlen(sec) == 0)) {
        return 0;
    }

    size_t secLength = strlen(sec);
    if ((secLength >= PATH_LENGTH_MAX) || (destSize - startIndex < 0) ||
        (secLength > (size_t)(destSize - startIndex))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "secLength is too long.");
        return 0;
    }
    if (startIndex == 0) {
        // this is the first
        if (memcpy_s(first, destSize, sec, secLength) != 0) {
            return 0;
        }
        first[secLength] = 0;
        return secLength;
    }

    size_t copiedLength = 0;
    // we dont append '/' at the first index
    bool sepNeeded = (first[startIndex - 1] != PATH_SEPARATOR);
    if (sepNeeded) {
        first[startIndex] = PATH_SEPARATOR;
        copiedLength++;
        startIndex++;
    }

    if (sec[0] == PATH_SEPARATOR) {
        if (secLength > 1) {
            if (memcpy_s(first + startIndex, (destSize - startIndex), (sec + 1), (secLength - 1)) != 0) {
                HILOG_ERROR(HILOG_MODULE_ACE, "append path error");
                return 0;
            }

            copiedLength = copiedLength + (secLength - 1);
            startIndex = startIndex + (secLength - 1);
        }
    } else {
        // sec string do not have '/'
        if (memcpy_s(first + startIndex, (destSize - startIndex), sec, secLength) != 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "error happen when append path");
            return 0;
        }
        copiedLength = copiedLength + secLength;
        startIndex = startIndex + secLength;
    }
    first[startIndex] = 0;

    return copiedLength;
}

char *RelocateFilePath(const char *appRootPath, const char *subPath, const char *fileName)
{
    size_t appRootPathLength = 0;
    size_t fileNameLength = 0;
    if ((appRootPath == nullptr) || ((appRootPathLength = strlen(appRootPath)) == 0) || (fileName == nullptr) ||
        ((fileNameLength = strlen(fileName)) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "input path or fileName is invalid");
        return nullptr;
    }

    size_t subPathLength = (subPath == nullptr) ? 0 : strlen(subPath);
    if ((appRootPathLength >= PATH_LENGTH_MAX) || (subPathLength >= PATH_LENGTH_MAX) ||
        (fileNameLength >= NAME_LENGTH_MAX)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "input path or fileName is too long");
        return nullptr;
    }
    // always consume subPath does not have '/'
    const uint8_t addedLength = 2;
    size_t totalLength = appRootPathLength + subPathLength + fileNameLength + addedLength;
    char *fullPath = static_cast<char *>(ace_malloc(totalLength + 1));
    if (fullPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for path failed, needed length[%{public}u]", (totalLength + 1));
        return nullptr;
    }
    fullPath[0] = '\0';
    // copy root path
    size_t copiedLength = AppendTwoPath(fullPath, 0, appRootPath, totalLength);
    size_t index = copiedLength;
    // copy sub folder name if it's given
    if (subPathLength > 0) {
        copiedLength = AppendTwoPath(fullPath, index, subPath, totalLength);
        index += copiedLength;
    }
    // copy filename part
    index += AppendTwoPath(fullPath, index, fileName, totalLength);
    fullPath[index] = '\0';
    return fullPath;
}

// judge absolute path or relative path (start with . is absolute otherwise relative)
static bool PathIsRelative(const char * const resPath)
{
    return strncmp(PATH_PREFIX, resPath, strlen(PATH_PREFIX)) == 0;
}

char *RelocateFilePathRelative(const char * const appRootPath, const char * const resFileName)
{
    const char * const jsPath = JsAppContext::GetInstance()->GetCurrentJsPath();
    if (jsPath == nullptr) {
        return nullptr;
    }
    const char * const ret = strrchr(jsPath, RESOURCE_SEPARATOR);
    if (ret == nullptr) {
        return nullptr;
    }
    size_t jsPathLen = strlen(jsPath);
    if (jsPathLen >= PATH_LENGTH_MAX) {
        return nullptr;
    }
    int len = jsPathLen - strlen(ret);
    if (len <= 0) {
        return nullptr;
    }
    char *dirPath = static_cast<char *>(ace_malloc(len + 1));
    if (dirPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc dirPath memory heap failed.");
        return nullptr;
    }
    if (memcpy_s(dirPath, len, jsPath, len) != 0) {
        ace_free(dirPath);
        dirPath = nullptr;
        return nullptr;
    }
    dirPath[len] = '\0';
    // first splice resFileName with directory path
    char *filePath = RelocateFilePath(dirPath, SRC_SUB_FOLDER_NAME, resFileName);
    if (filePath != nullptr) {
        ace_free(dirPath);
        filePath = nullptr;
    }
    // second splice root path with res file path
    char *realPath = nullptr;
    if (filePath != nullptr) {
        realPath = RelocateFilePath(appRootPath, SRC_SUB_FOLDER_NAME, filePath);
        ace_free(filePath);
    }
    return realPath;
}

// relocate file name to appRootPath/common/fileName
// e.x. /system/app/73709738-2d9d-4947-ac63-9858dcae7ccb/common/right.png
char *RelocateResourceFilePath(const char * const appRootPath, const char * const resFileName)
{
    if (PathIsRelative(resFileName)) {
        // deal with relative path
        return RelocateFilePathRelative(appRootPath, resFileName);
    }

    // deal with absolute path
    return RelocateFilePath(appRootPath, SRC_SUB_FOLDER_NAME, resFileName);
}

// relocate file name to appRootPath/src/fileName
// e.x. /system/app/73709738-2d9d-4947-ac63-9858dcae7ccb/src/index.js
// NOTE: srcFileName must be the relative path to src folder
char *RelocateJSSourceFilePath(const char * const appRootPath, const char * const srcFileName)
{
    return RelocateFilePath(appRootPath, SRC_SUB_FOLDER_NAME, srcFileName);
}

char *ReadJSFile(const char * const appPath, const char * const jsFileName)
{
    uint32_t contentLength = 0;
    return ReadJSFile(appPath, jsFileName, contentLength);
}

/**
 * Check if the given file exists or not.
 */
bool IsFileExisted(const char * const filePath)
{
    if (filePath == nullptr) {
        return false;
    }
    struct stat info = {0};
    if (stat(filePath, &info) != 0) {
        return false;
    }
    return true;
}

/**
 * Whether file existed and 0 represents file not existed.
 */
int32_t GetFileSize(const char * const filePath)
{
    if (filePath == nullptr) {
        return 0;
    }
    struct stat info = {0};
    int32_t ret = stat(filePath, &info);
    if (ret < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "file doesn't exit or it's empty, [%{public}s]", filePath);
    }
    return info.st_size;
}

static int32_t OpenFileInternal(const char * const orgFullPath, bool binary = false)
{
    const char *path = orgFullPath;
#if (QT_SIMULATOR != 1)
#ifndef __LITEOS_M__ // no path canonicalization on M core
    char fullPath[PATH_MAX + 1] = {0};
#if ((defined(__WIN32)) || (defined(__WIN64)))
    if (!PathCanonicalize(fullPath, orgFullPath)) {
        return -1;
    }
#else
    if (realpath(orgFullPath, fullPath) == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "realpath handle failed, [%{public}s]", orgFullPath);
        return -1;
    }
#endif
    path = fullPath;
#endif
#endif // QT_SIMULATOR
    return open(path, O_RDONLY);
}

static void OutputFileMaxLimitationTrace(const char * const fullPath, size_t limitation)
{
    if (fullPath == nullptr || strlen(fullPath) == 0) {
        return;
    }
    const char *ptr = strrchr(fullPath, PATH_SEPARATOR);
    const size_t fileNameMaxLen = 32;
    if (ptr == nullptr || strlen(ptr) > fileNameMaxLen) {
        HILOG_ERROR(HILOG_MODULE_ACE, "file name too long.");
        return;
    }
    const size_t traceDataLen = 128;
    char traceData[traceDataLen] = {0};
    const size_t oneKB = 1024;
    size_t currentLimitation = limitation / oneKB;
    if (sprintf_s(traceData, traceDataLen, "%s is bigger than %d KB.\n", (ptr + 1), currentLimitation) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "splice trace data failed.");
        return;
    }

    LogOutLevel(LogLevel::LOG_LEVEL_ERR);
    LogString(LogLevel::LOG_LEVEL_ERR, traceData);
}

/**
 * Check if the file length is under MAX limitation.
 */
static bool CheckFileLength(const char * const fullPath, int32_t &outFileSize)
{
    outFileSize = 0;
    int32_t fileSize = GetFileSize(fullPath);
    if (fileSize <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "open file[%{public}s] failed for reading.", fullPath);
        return false;
    }
    if (fileSize > FILE_CONTENT_LENGTH_MAX) {
        OutputFileMaxLimitationTrace(fullPath, FILE_CONTENT_LENGTH_MAX);
        ACE_ERROR_CODE_PRINT(EXCE_ACE_ROUTER_REPLACE_FAILED, EXCE_ACE_PAGE_FILE_TOO_HUGE);
        return false;
    }
    outFileSize = fileSize;
    return true;
}

/**
 * Read file content from one give full file path.
 * Return value must be freed by caller.
 */
char *ReadFile(const char * const fullPath, uint32_t &fileSize, const bool binary)
{
    UNUSED(binary);
    char *scriptBuffer = nullptr;
    int32_t fd = -1;
    bool success = false;

    do {
        int32_t scriptLength = 0;
        if (!CheckFileLength(fullPath, scriptLength)) {
            break;
        }

        fileSize = 0;
        fd = OpenFileInternal(fullPath, O_RDONLY);
        if (fd < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "open file[fd: %{public}d] failed for reading", fd);
            HILOG_ERROR(HILOG_MODULE_ACE, "open file[path: %{public}s] failed for reading", fullPath);
            break;
        }
        scriptBuffer = static_cast<char *>(ace_malloc(scriptLength + 1));
        if (scriptBuffer == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for file content failed, file length[%{public}d]",
                        scriptLength);
            break;
        }
        if (EOK != memset_s(scriptBuffer, (scriptLength + 1), 0, (scriptLength + 1))) {
            break;
        }
        int32_t count = read(fd, scriptBuffer, scriptLength);
        if ((count <= 0) || (count > scriptLength)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "read fail, count(%{public}d), length(%{public}u), path(%{public}s)", count,
                        scriptLength, fullPath);
            break;
        }
        scriptBuffer[count] = '\0';
        fileSize = count;
        success = true;
    } while (0);

    if (fd >= 0) {
        close(fd);
    }
    if (!success) {
        // failed, free buffer if it's already malloced out
        ACE_FREE(scriptBuffer);
        return nullptr;
    }
    return scriptBuffer;
}

/*
 * give app path, such as /system/app/uuid/, read script string from /system/app/uuid/src/index.js,
 * return value must be freed by caller.
 */
char *ReadJSFile(const char * const appPath, const char * const jsFileName, uint32_t &fileSize)
{
    fileSize = 0;
    // RelocateFilePath() will check the input parameters
    char *fullPath = RelocateJSSourceFilePath(appPath, jsFileName);
    if (fullPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "error index.js file path after locate");
        return nullptr;
    }

    if (strlen(fullPath) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "empty index.js file path after locate");
        ace_free(fullPath);
        fullPath = nullptr;
        return nullptr;
    }

    char *fileBuffer = ReadFile(fullPath, fileSize, JsAppEnvironment::GetInstance()->IsSnapshotMode());
    ace_free(fullPath);
    fullPath = nullptr;
    return fileBuffer;
}

static void CalculatePathPosFromUri(const char * const url, const size_t length, size_t *start, size_t *pathLength)
{
    const uint8_t minLength = 4;
    if ((url == nullptr) || (length <= minLength)) {
        return;
    }

    if (strstr(url, "url") != url) {
        // just the path itself
        *start = 0;
        *pathLength = length;
        return;
    }

    // consider this is the url pattern
    const uint8_t index = 3;
    const char *pathP = strstr((url + index), "(");
    if (pathP == nullptr) {
        return;
    }
    *start = (pathP - url) + 1;
    if (*start >= (length - 1)) {
        return; // '(' is at the last, not meet the pattern
    }

    // path end url(common)
    const char *pathPE = strstr((pathP + 1), ")");
    if (pathPE == nullptr) {
        return;
    }
    *pathLength = pathPE - (pathP + 1);
}

/**
 * @brief give out the path string from an url pattern, "url(common/logo.png)".
 *
 * @param url the given url
 *
 * @return the url itself, "common/logo.png"
 */
char *CreatePathStrFromUrl(const char * const url)
{
    if (url == nullptr) {
        return nullptr;
    }
    size_t length = strlen(url);
    if (length >= PATH_LENGTH_MAX) {
        return nullptr;
    }
    size_t start = 0;
    size_t pathLength = 0;
    CalculatePathPosFromUri(url, length, &start, &pathLength);
    if ((pathLength == 0) || (pathLength > (length - start))) {
        return nullptr;
    }
    char *filePath = static_cast<char *>(ace_malloc(pathLength + 1));
    if (filePath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for file path calculating from url, file length[%{public}d]",
                    pathLength);
        return nullptr;
    }
    if (memcpy_s(filePath, pathLength, (url + start), pathLength) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "append path error when calculating from url");
        ace_free(filePath);
        filePath = nullptr;
        return nullptr;
    }
    filePath[pathLength] = '\0';
    return filePath;
}

static uint32_t g_jsfwkErrorCount = 0;
void ThrowError()
{
    // just for debug mode, never used on release mode
    g_jsfwkErrorCount++;
    if (g_jsfwkErrorCount >= INT32_MAX) {
        g_jsfwkErrorCount = 0;
    }
}

void ReleaseJerryValue(jerry_value_t value, ...)
{
    va_list args;
    va_start(args, value);

    jerry_value_t member = value;
    do {
        jerry_release_value(member);
        member = va_arg(args, jerry_value_t);
    } while (member != (jerry_value_t)VA_ARG_END_FLAG);

    va_end(args);
}

void InsertWatcherCommon(Watcher *&head, const jerry_value_t watcher)
{
    if (IS_UNDEFINED(watcher)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "watcher is undefined when inserting a watcher.");
        return;
    }
    Watcher *node = new Watcher();
    if (node == nullptr) {
        jerry_value_t unsubscribe = jerryx_get_property_str(watcher, "unsubscribe");
        ReleaseJerryValue(CallJSFunction(unsubscribe, watcher, nullptr, 0), unsubscribe, watcher, VA_ARG_END_FLAG);
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc memory watcher failed.");
        return;
    }
    node->watcher = watcher;
    if (head == nullptr) {
        head = node;
    } else {
        node->next = head;
        head = node;
    }
}

void ClearWatchersCommon(Watcher *&head)
{
    Watcher *node = head;
    while (node) {
        head = node->next;
        // avoid allocating any JS objects when JS runtime broken
        if (!(FatalHandler::GetInstance().IsJSRuntimeFatal())) {
            // call js watcher.unsubscribe to release watcher
            jerry_value_t watcher = node->watcher;
            jerry_value_t unsubscribe = jerryx_get_property_str(watcher, "unsubscribe");
            ReleaseJerryValue(CallJSFunction(unsubscribe, watcher, nullptr, 0), unsubscribe, watcher, VA_ARG_END_FLAG);
        }
        delete node;
        node = head;
    }
    head = nullptr;
}

/**
 * @brief Call jerry_has_property and return the result
 *
 * @param container the target JS object to check
 * @param property the property string value
 */
bool JerryHasProperty(const jerry_value_t container, const jerry_value_t property)
{
    if (IS_UNDEFINED(container) || IS_UNDEFINED(property)) {
        return false;
    }

    jerry_value_t result = jerry_has_property(container, property);
    bool res = jerry_value_to_boolean(result);
    jerry_release_value(result);
    return res;
}

/**
 * @brief Call jerry_has_own_property and return the result
 *
 * @param container the target JS object to check
 * @param property the property string value
 */
bool HasOwnProperty(const jerry_value_t container, const jerry_value_t property)
{
    if (IS_UNDEFINED(container) || IS_UNDEFINED(property)) {
        return false;
    }

    jerry_value_t result = jerry_has_own_property(container, property);
    bool res = jerry_value_to_boolean(result);
    jerry_release_value(result);
    return res;
}

// this callback is triggered by two conditions:
// 1. the array's length changed.(ie. array.push(value))
// 2. the array replaced by another new array but length is the same.(ie. array1 = array2)
jerry_value_t ListForWatcherCallbackFunc(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t *args,
                                         const jerry_length_t argsLength)
{
    if (argsLength != ARG_LENGTH_WATCHER_CALLBACK) {
        return UNDEFINED;
    }
    // args[0]==>new changed data watchered
    // args[1]==>original data watchered, the data here had been updated
    // args[2]==>new changed children of this component
    START_TRACING(FOR_WATCHER_CALLBACK_FUNC);
    const int argsNum = 2;
    jerry_value_t options = args[argsNum];
    jerry_value_t nativeElement = jerryx_get_property_str(options, ARG_WATCH_EL);
    Component *component = ComponentUtils::GetComponentFromBindingObject(nativeElement);
    if (component) {
        bool updateResult = component->UpdateForView();
        if (updateResult) {
            component->Invalidate();
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "No component binds element.");
    }
    STOP_TRACING();
    jerry_release_value(nativeElement);
    return UNDEFINED;
}
bool StartWith(const char *sequence, const char *prefix)
{
    while (*prefix) {
        if (*prefix++ != *sequence++) {
            return false;
        }
    }
    return true;
}

bool IsHexColor(const char * const target)
{
    return StartWith(target, PREFIX_HEX_COLOR);
}
bool IsRgbColor(const char * const target)
{
    return StartWith(target, PREFIX_RGB_COLOR);
}
bool IsRgbaColor(const char * const target)
{
    return StartWith(target, PREFIX_RGBA_COLOR);
}

constexpr uint8_t LENGTH_RGB = 3;
constexpr uint8_t LENGTH_RRGGBB = 6;
constexpr uint8_t LENGTH_ARGB = 8;
constexpr uint8_t IDX_RGB_RED = 1;
constexpr uint8_t IDX_RGB_GREEN = 2;
constexpr uint8_t IDX_RGB_BLUE = 3;
constexpr uint8_t LENGTH_ALPHA = 2;
constexpr uint8_t IDX_ARGB_ALPHA_BEGIN = 1;
constexpr uint8_t IDX_ARGB_ALPHA_END = 2;
constexpr uint8_t IDX_ARGB_COLOR_BEGIN = 3;
constexpr uint8_t BITS_PER_BYTE = 8;

bool ParseHexColor(const char * const source, uint32_t &color, uint8_t &alpha)
{
    if ((source == nullptr) || (strlen(source) <= 1)) {
        return false;
    }
    uint8_t length = strlen(source + 1);
    // Parse HEX color like #ABC
    if (length == LENGTH_RGB) {
        // #ABC equals to #AABBCC
        char buffer[LENGTH_RRGGBB + 1] = {source[IDX_RGB_RED],
                                          source[IDX_RGB_RED],
                                          source[IDX_RGB_GREEN],
                                          source[IDX_RGB_GREEN],
                                          source[IDX_RGB_BLUE],
                                          source[IDX_RGB_BLUE],
                                          0};
        color = strtol(buffer, nullptr, HEX);
        alpha = ALPHA_MAX;
        return true;
    }

    // Parse HEX color like #AABBCC
    if (length == LENGTH_RRGGBB) {
        color = strtol(source + 1, nullptr, HEX);
        alpha = ALPHA_MAX;
        return true;
    }

    // Parse HEX color like #FFAABBCC
    if (length == LENGTH_ARGB) {
        char buffer[LENGTH_ALPHA + 1] = {source[IDX_ARGB_ALPHA_BEGIN], source[IDX_ARGB_ALPHA_END], 0};
        alpha = strtol(buffer, nullptr, HEX);
        color = strtol(source + IDX_ARGB_COLOR_BEGIN, nullptr, HEX);
        return true;
    }

    return false;
}

bool ParseRgbaColor(const char * const source, uint32_t &color, uint8_t &alpha)
{
    uint8_t idxOpenBrace = 0;
    uint8_t idxCloseBrace = 0;
    if (source == nullptr) {
        return false;
    }
    size_t size = strlen(source);
    if (size >= UINT8_MAX) {
        return false;
    }
    for (uint8_t idx = 0; idx < size; ++idx) {
        if (source[idx] == BRACE_OPEN) {
            idxOpenBrace = idx;
            continue;
        }
        if (source[idx] == BRACE_CLOSE) {
            idxCloseBrace = idx;
            continue;
        }
    }

    if (idxOpenBrace == 0 || idxCloseBrace == 0 || idxCloseBrace <= idxOpenBrace) {
        return false;
    }

    uint8_t bufSize = idxCloseBrace - idxOpenBrace - 1;
    char *buffer = static_cast<char *>(ace_malloc(sizeof(char) * (bufSize + 1)));
    if (buffer == nullptr) {
        return false;
    }
    if (memcpy_s(buffer, bufSize, source + idxOpenBrace + 1, bufSize) != 0) {
        ace_free(buffer);
        buffer = nullptr;
        return false;
    }
    buffer[bufSize] = 0;
    char *token;
    // BITS_PER_BYTE is unsigned, result always equals to 16, can't overflow
    int8_t move = BITS_PER_BYTE << 1;
    char *next = nullptr;
    token = strtok_s(buffer, DELIMITER, &next);
    color = 0;
    while ((token != nullptr) && (move >= 0)) {
        long value = strtol(token, nullptr, DEC);
        if ((value < 0) || (value > UINT8_MAX)) {
            ace_free(buffer);
            buffer = nullptr;
            color = 0;
            return false;
        }
        color += ((uint8_t)value << (uint8_t)move);
        token = strtok_s(nullptr, DELIMITER, &next);
        move -= BITS_PER_BYTE;
    }
    if (token != nullptr) {
        alpha = (uint8_t)(strtod(token, nullptr) * ALPHA_MAX);
    } else {
        alpha = ALPHA_MAX;
    }
    ace_free(buffer);
    buffer = nullptr;
    return true;
}

bool ParseColor(const char * const source, uint32_t &color, uint8_t &alpha)
{
    if (source == nullptr) {
        return false;
    }

    if (IsHexColor(source)) {
        return ParseHexColor(source, color, alpha);
    }

    if (IsRgbaColor(source) || IsRgbColor(source)) {
        return ParseRgbaColor(source, color, alpha);
    }

    return false;
}

#if JS_PAGE_SPECIFIC
struct JSPageSpecific jsPageSpecific;
#endif // JS_PAGE_SPECIFIC

uint16_t GetHorizontalResolution()
{
// SCREENSIZE_SPECIFIED is temporarily set, when ui and graphic unifid, this can be removed
#if ((defined __LITEOS__) || (defined __linux__) || (SCREENSIZE_SPECIFIED == 1))
    return Screen::GetInstance().GetWidth();
#else
    constexpr uint16_t resConst = 454;
    uint16_t horizontalResolution = resConst;
    uint16_t verticalResolution = resConst;
    ProductAdapter::GetScreenSize(horizontalResolution, verticalResolution);
    return horizontalResolution;
#endif // OHOS_ACELITE_PRODUCT_WATCH
}

uint16_t GetVerticalResolution()
{
#if ((defined __LITEOS__) || (defined __linux__) || (SCREENSIZE_SPECIFIED == 1))
    return Screen::GetInstance().GetHeight();
#else
    uint16_t horizontalResolution = resConst;
    uint16_t verticalResolution = resConst;
    ProductAdapter::GetScreenSize(horizontalResolution, verticalResolution);
    return verticalResolution;
#endif // OHOS_ACELITE_PRODUCT_WATCH
}

JSValue GetRootAbilitySlice()
{
    return JSGlobal::Get(ATTR_ROOT);
}

JSValue CallWithRootAbilitySlice(JSValue func, JSValue args[], JSSize size)
{
    JSValue abilitySlice = GetRootAbilitySlice();
    JSValue result = JSFunction::Call(func, abilitySlice, args, size);
    JSRelease(abilitySlice);
    return result;
}

JSValue CallWithRootAbilitySlice(JSValue func)
{
    return CallWithRootAbilitySlice(func, nullptr, 0);
}

JSValue CreateWatcher(JSValue getter, JSHandler handler, JSValue options)
{
    JSValue abilitySlice = GetRootAbilitySlice();
    JSValue callback = JSFunction::Create(handler);
    JSValue args[ARG_LENGTH_WATCHER] = {getter, callback, options};
    JSValue watcher = JSObject::Call(abilitySlice, ATTR_WATCH, args, ARG_LENGTH_WATCHER);
    JSRelease(callback);
    JSRelease(abilitySlice);
    return watcher;
}

void ExpandImagePathMem(char *&imagePath, const int16_t dotPos, const int16_t suffixLen, const int16_t imagePathLen)
{
    if (dotPos < 0 || suffixLen < 0 || imagePathLen < 0) {
        return;
    }
    int16_t len = dotPos + 1 + suffixLen + 1;
    if (len <= 0) {
        return;
    }
    char *newImagePath = static_cast<char *>(ace_malloc(len));
    if (newImagePath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for path failed, needed length[%{public}u]",
                    (dotPos + 1 + suffixLen + 1));
        ACE_FREE(imagePath);
        return;
    }

    errno_t err = strcpy_s(newImagePath, len, imagePath);
    if (err != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "use strcpy_s secure function [errro:%{public}d]", err);
        ace_free(newImagePath);
        newImagePath = nullptr;
        ACE_FREE(imagePath);
        return;
    }

    ace_free(imagePath);
    imagePath = newImagePath;
}

#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
void CureImagePath(char *&imagePath)
{
    if (imagePath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "get imagePath failed!");
        return;
    }
    int16_t lastPathPos = -1;
    int16_t dotPos = -1;
    const int16_t suffixLen = 3;
    const size_t imagePathLen = strlen(imagePath);
    const char * const suffixName = "bin";

    if (imagePathLen >= PATH_LENGTH_MAX) {
        return;
    }
    // find the dot and last path position
    for (int16_t index = imagePathLen - 1; index >= 0; index--) {
        if (dotPos < 0) {
            if (*(imagePath + index) == PATH_PREFIX[0]) {
                dotPos = index;
            }
        }

        if (lastPathPos < 0) {
            if (*(imagePath + index) == PATH_SEPARATOR) {
                lastPathPos = index;
            }
        }
    }

    // if dot position - last path position > 1, the suffix need to be proceed,
    // else means the file name is wrong.
    if ((dotPos - lastPathPos) > 1) {
        // if suffix length < 3, need expand memory first.
        if (static_cast<int16_t>(imagePathLen) < (suffixLen + dotPos + 1)) {
            ExpandImagePathMem(imagePath, dotPos, suffixLen, imagePathLen);
            if (imagePath == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for path failed, needed length[%{public}d]",
                            (dotPos + 1 + suffixLen + 1));
                return;
            }
        }

        *(imagePath + dotPos + 1) = '\0';
        if (strcat_s(imagePath, dotPos + 1 + suffixLen + 1, suffixName) != 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "strcat_s failed!");
            ace_free(imagePath);
            imagePath = nullptr;
            return;
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "the image suffix error, please check!");
        ace_free(imagePath);
        imagePath = nullptr;
        return;
    }
    return;
}
#endif // OHOS_ACELITE_PRODUCT_WATCH

const char *ParseImageSrc(jerry_value_t source)
{
    uint16_t length = 0;
    char *rawSrc = MallocStringOf(source, &length);
    if (rawSrc == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to parse image src cause by empty source.");
        return nullptr;
    }
    if (length == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to parse image src cause by empty source.");
        ace_free(rawSrc);
        rawSrc = nullptr;
        return nullptr;
    }

    if ((strstr(rawSrc, PATH_RELATIVE_WHOLE) != nullptr) || (strstr(rawSrc, PATH_RELATIVE_PART) != nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Relative path is not allowed in image src.");
        ace_free(rawSrc);
        rawSrc = nullptr;
        return nullptr;
    }

    char *imageSrc = JsAppContext::GetInstance()->GetResourcePath(rawSrc);
    ace_free(rawSrc);
    rawSrc = nullptr;
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
    CureImagePath(imageSrc);
#endif // OHOS_ACELITE_PRODUCT_WATCH
    return imageSrc;
}

bool CopyFontFamily(char *&destination, const char * const fontFamily, uint32_t fontFamilyNameLen)
{
    // unused parameter for now, added for clean up warning
    UNUSED(fontFamilyNameLen);
    ACE_FREE(destination);
    if (fontFamily == nullptr) {
        return true; // the destination is already nullptr
    }
    size_t fontFamilyLen = strlen(fontFamily);
    if (fontFamilyLen >= UINT8_MAX) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the fontFamily is too long");
        return false;
    }
    size_t len = fontFamilyLen + 1;
    destination = static_cast<char *>(ace_malloc(len));
    if (destination == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc string failed");
        return false;
    }
    if (strcpy_s(destination, len, fontFamily) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "copy fontFamily failed");
        ace_free(destination);
        destination = nullptr;
        return false;
    }
    return true;
}

uint16_t ParseKeyIdFromJSString(const jerry_value_t str)
{
    // calculate key ID
    uint16_t keyId = K_UNKNOWN;
    uint16_t strLength = 0;
    char *keyStr = MallocStringOf(str, &strLength);
    if (keyStr != nullptr) {
        if (strLength != 0) {
            keyId = KeyParser::ParseKeyId(keyStr, strLength);
        }
        ace_free(keyStr);
        keyStr = nullptr;
    }
    return keyId;
}
} // namespace ACELite
} // namespace OHOS
