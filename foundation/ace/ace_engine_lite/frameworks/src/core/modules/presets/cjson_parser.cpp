/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "cjson_parser.h"
#if (FEATURE_LOCALIZATION_MODULE == 1)
#include "ace_log.h"
#include "global.h"
#include "js_fwk_common.h"
#if (defined(_WIN32) || defined(_WIN64))
#include <io.h>
#else
#include "gfx_utils/file.h"
#endif
#include "securec.h"
#include "string_util.h"
#include <string.h>

namespace OHOS {
namespace ACELite {
CJSONParser::CJSONParser()
    : isCached_(false),
      isAllocated_(true),
      startPos_(-1),
      countries_(nullptr),
      language_(nullptr),
      filePath_(nullptr),
      languageFile_(nullptr)
{
    char left = '{';
    leftDelemeter_ = left;
    char right = '}';
    rightDelemeter_ = right;
    usedOffset_ = sizeof(uint32_t) * MAX_KEY_NUM;
}

bool CJSONParser::Init()
{
    const char * const filePath = JsAppContext::GetInstance()->GetCurrentAbilityPath();
    const char * const folderName = "i18n";
    filePath_ = RelocateResourceFilePath(filePath, folderName);
    void *ram = ace_malloc(LOCALIZATION_SIZE);
    if (ram == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc cache ram failed");
        isAllocated_ = false;
        return false;
    }
    startPos_ = reinterpret_cast<uintptr_t>(ram);
    return (filePath_ != nullptr);
}

char *CJSONParser::FillPlaceholder(const char *format, jerry_value_t arg, jerry_length_t num, bool isPlural)
{
    if (format == nullptr) {
        return nullptr;
    }
    if (isPlural) {
#ifdef LOCALIZATION_PLURAL
        format = GetPluralValue(format, arg);
        if (format == nullptr) {
            return nullptr;
        }
#endif
    }
    ListNode *values = nullptr; // store the value after split.
#ifdef LOCALIZATION_PLURAL
    uint8_t length = FormatString(format, arg, values, isPlural, num);
    if (isPlural) {
        ACE_FREE(format);
    }
#else
    uint8_t length = FormatString(format, arg, values, num);
#endif // LOCALIZATION_PLURAL
    char *result = nullptr;
    if (values != nullptr) {
        result = ToString(values, length);
        ClearNode(values);
    }
    return result;
}

#ifdef LOCALIZATION_PLURAL
uint8_t CJSONParser::FormatString(const char *format,
                                  jerry_value_t arg,
                                  ListNode *&values,
                                  bool isPlural,
                                  jerry_length_t num)
#else
uint8_t CJSONParser::FormatString(const char *format, jerry_value_t arg, ListNode *&values, jerry_length_t num)
#endif
{
    uint8_t length = 0;
    int index = 0;
    int leftIndex = IndexOf(format, leftDelemeter_, index); // get the index of '{'
    while (leftIndex >= 0) {
        char *value = SubStr(format, index, leftIndex - index);
        if (AddNode(values, value)) {
            length++;
        } else {
            ACE_FREE(value);
        }
        leftIndex++;
        index = IndexOf(format, rightDelemeter_, leftIndex); // get the index of '}'
        if (index > 0 && num > 1) {
            char *content = SubStr(format, leftIndex, index - leftIndex);
#ifdef LOCALIZATION_PLURAL
            char *contentValue = GetParamValue(content, arg, isPlural);
#else
            char *contentValue = GetParamValue(content, arg);
#endif // LOCALIZATION_PLURAL
            ACE_FREE(content);
            if (AddNode(values, contentValue)) {
                length++;
            } else {
                ACE_FREE(contentValue);
            }
        }
        if ((index > 0) && (static_cast<size_t>(index) < strlen(format))) {
            index++; // move the index '}' to the next char
        } else {
            // add the remain character to node.eg.the value is {count}aa, add the last aa to the node
            char *currentVal = SubStr(format, index, strlen(format) - index);
            if (AddNode(values, currentVal)) {
                length++;
            } else {
                ACE_FREE(currentVal);
            }
        }
        leftIndex = IndexOf(format, leftDelemeter_, index);
    }
    if (leftIndex < 0 && (static_cast<size_t>(index) < strlen(format))) {
        char *currentVal = SubStr(format, index, strlen(format) - index);
        if (AddNode(values, currentVal)) {
            length++;
        } else {
            ACE_FREE(currentVal);
        }
    }
    return length;
}

#ifdef LOCALIZATION_PLURAL
char *CJSONParser::GetPluralValue(char *value, jerry_value_t param)
{
    ListNode *plurals = nullptr;
    int defaultNum = jerry_value_is_number(param) ? IntegerOf(param) : 1;
    const char delemeter = '|'; // if the function is get plural, split the value in json by '|'
    int len = Split(value, delemeter, plurals);
    const int maxLen = 2;
    // get the index in plural array
    if (len <= maxLen) {
        value = (defaultNum != 1) ? GetNode(plurals, 1) : GetNode(plurals, len);
    } else {
        int index = (abs(defaultNum) < maxLen) ? abs(defaultNum) : maxLen; // get the index in plurals
        value = GetNode(plurals, len - index);
    }
    char *result = SubStr(value, 0, strlen(value));
    if (plurals) {
        ClearNode(plurals);
    }
    return result;
}
#endif // LOCALIZATION_PLURAL

bool CJSONParser::AddNode(ListNode *&head, char *current)
{
    if (current == nullptr || strlen(current) == 0) {
        return false;
    }
    ListNode *node = new ListNode();
    if (node == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create node failed");
        return false;
    }
    node->next = head;
    node->value = current;
    head = node;
    return true;
}

char *CJSONParser::GetNode(ListNode *node, int index)
{
    for (int i = 0; i < index - 1; i++) {
        if (node == nullptr) {
            break;
        }
        node = node->next;
    }
    return (node == nullptr) ? nullptr : node->value;
}

void CJSONParser::ClearNode(ListNode *&node)
{
    while (node != nullptr) {
        ListNode *current = node->next;
        ACE_FREE(node->value);
        delete node;
        node = current;
    }
}

char *CJSONParser::ToString(ListNode *node, uint8_t length)
{
    if (node == nullptr || (length == 0)) {
        return nullptr;
    }
    char **temp = static_cast<char **>(ace_malloc(length * sizeof(char *)));
    if (temp == nullptr) {
        return nullptr;
    }
    // reverse node value
    uint8_t index = 0;
    uint16_t strLength = 0; // the length of result
    while (node != nullptr) {
        if ((length - index > 0) && (node->value != nullptr)) {
            size_t valueLen = strlen(node->value);
            if ((valueLen > (MAX_VALUE_LEN - 1)) || (strLength > (MAX_VALUE_LEN - 1)) ||
                (strLength + valueLen > (MAX_VALUE_LEN - 1))) {
                ACE_FREE(temp);
                return nullptr;
            }
            temp[length - 1 - index] = node->value;
            strLength += valueLen;
        }
        node = node->next;
        index++;
    }
    // the strLength is no more than maxValueLen_ - 1
    strLength++;
    char *result = static_cast<char *>(ace_malloc(strLength));
    if (result == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc localization result failed");
        ACE_FREE(temp);
        return nullptr;
    }
    // convert the value in node to string
    if (strcpy_s(result, strLength, temp[0]) != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "merge string failed");
        ACE_FREE(result);
        ACE_FREE(temp);
        return nullptr;
    }
    for (int i = 1; i < length; i++) {
        if (strcat_s(result, strLength, temp[i]) != 0) {
            HILOG_INFO(HILOG_MODULE_ACE, "merge string failed");
            ACE_FREE(result);
            ACE_FREE(temp);
            return nullptr;
        }
    }
    ACE_FREE(temp);
    return result;
}

char *CJSONParser::SubStr(const char *src, size_t start, size_t length)
{
    if (src == nullptr || start >= strlen(src)) {
        return nullptr;
    }
    if (length > (MAX_VALUE_LEN - 1)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the length is too long");
        return nullptr;
    }
    size_t len = length + 1;
    char *result = static_cast<char *>(ace_malloc(len));
    if (result == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc failed");
        return nullptr;
    }
    errno_t error = strncpy_s(result, len, src + start, length);
    if (error != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "copy string failed");
        ACE_FREE(result);
        return nullptr;
    }
    result[length] = '\0';
    return result;
}

CJSONParser::LanguageState CJSONParser::ChangeLanguage()
{
    char *currentLanguage = static_cast<char *>(ace_malloc(MAX_LANGUAGE_LENGTH));
    char *currentOrigion = static_cast<char *>(ace_malloc(MAX_REGION_LENGTH));
    if ((currentLanguage == nullptr) || (currentOrigion == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc system language info failed");
        ACE_FREE(currentLanguage);
        ACE_FREE(currentOrigion);
        return LanguageState::LANGUAGE_FAIL;
    }
    int langRet = GLOBAL_GetLanguage(currentLanguage, MAX_LANGUAGE_LENGTH);
    int regionRet = GLOBAL_GetRegion(currentOrigion, MAX_REGION_LENGTH);
    LanguageState result = LanguageState::LANGUAGE_CHANGED;
    if ((langRet != 0) || (regionRet != 0)) {
        result = LanguageState::LANGUAGE_FAIL;
    }
    if ((language_ != nullptr) && (strcmp(currentLanguage, language_) == 0) &&
        (countries_ != nullptr) && (strcmp(currentOrigion, countries_) == 0)) {
        result = LanguageState::LANGUAGE_NOT_CHANGE;
    }
    if ((result == LanguageState::LANGUAGE_NOT_CHANGE) || (result == LanguageState::LANGUAGE_FAIL)) {
        ace_free(currentOrigion);
        currentOrigion = nullptr;
        ace_free(currentLanguage);
        currentLanguage = nullptr;
        return result;
    }
    ACE_FREE(language_);
    language_ = currentLanguage;
    ACE_FREE(countries_);
    countries_ = currentOrigion;
    return LanguageState::LANGUAGE_CHANGED;
}

bool CJSONParser::ChangeLanguageFileName()
{
    if ((language_ == nullptr) || (countries_ == nullptr)) {
        if ((languageFile_ != nullptr) && (!strcmp(languageFile_, "en-US.json"))) {
            return false;
        }
        ACE_FREE(languageFile_); // avoid leaking
        languageFile_ = StringUtil::Copy("en-US.json");
        return true;
    }
    uint8_t addedLen = 7; // the length of '-', ".json" and '\0'
    uint8_t langLen = strlen(language_);
    size_t fileLen = langLen + strlen(countries_) + addedLen;
    char *languageFile = StringUtil::Malloc(fileLen);
    if (languageFile == nullptr) {
        return false;
    }
    errno_t error = strcpy_s(languageFile, fileLen, language_);
    languageFile[langLen] = '-';
    languageFile[langLen + 1] = '\0';
    error += strcat_s(languageFile, fileLen, countries_);
    error += strcat_s(languageFile, fileLen, ".json");
    if (error > 0) {
        ace_free(languageFile);
        languageFile = nullptr;
        return false;
    }
    if ((languageFile_ != nullptr) && (!strcmp(languageFile, languageFile_))) {
        ace_free(languageFile);
        languageFile = nullptr;
    } else {
        ACE_FREE(languageFile_);
        languageFile_ = languageFile;
    }
    return true;
}

bool CJSONParser::CacheFile()
{
    if (!ChangeLanguageFileName()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "prepare language file name failed, cache status[%{public}d]", isCached_);
        return true;
    }
    isCached_ = false;
    usedOffset_ = sizeof(uint32_t) * MAX_KEY_NUM;
    if (!isAllocated_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "cache buffer not ready");
        return false;
    }
    if (memset_s(reinterpret_cast<void *>(startPos_), usedOffset_, 0, usedOffset_) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "initial psram failed");
        return false;
    }
    if (memset_s(reinterpret_cast<void *>(startPos_ + usedOffset_), LOCALIZATION_SIZE - usedOffset_, '\0',
                 LOCALIZATION_SIZE - usedOffset_) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "init cache content failed");
        return false;
    }
    // set the keyIndex 0
    for (int i = 0; i < MAX_KEY_NUM; i++) {
        uint32_t *index = reinterpret_cast<uint32_t *>(startPos_ + i * sizeof(uint32_t));
        *index = 0;
    }
    if (languageFile_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "waring: no correct language file name presented, will cach en-US as default");
        // cache the default file en-US.json
        languageFile_ = StringUtil::Copy("en-US.json");
    }
    uint32_t fileContentLength = 0;
    char *content = ReadJSFile(filePath_, languageFile_, fileContentLength);
    if (content == nullptr || fileContentLength == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "cache failing, read language file failed");
        return false;
    }
    cJSON *json = cJSON_Parse(content);
    ace_free(content);
    content = nullptr;
    if (json == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "cache failing, cJSON_Parse failed");
        return false;
    }
    isCached_ = CacheValue(nullptr, *json);
    cJSON_Delete(json);
    json = nullptr;
    return isCached_;
}

char *CJSONParser::GetMergeKey(const char *key, const char *addKey)
{
    if (addKey == nullptr) {
        return const_cast<char *>(key);
    }
    uint16_t addKeyLen = strlen(addKey);
    uint32_t preLen = (key == nullptr) ? 0 : strlen(key);
    const uint8_t addedLen = 2;
    uint16_t mergeLen = preLen + addKeyLen + addedLen;
    char *mergeKey = reinterpret_cast<char *>(ace_malloc(mergeLen));
    if (mergeKey == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "merge key failed");
        return nullptr;
    }
    if (key != nullptr) {
        if ((strcpy_s(mergeKey, mergeLen, key) != EOK) || (strcat_s(mergeKey, mergeLen, ".") != EOK) ||
            (strcat_s(mergeKey, mergeLen, addKey) != EOK)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "copy preKey failed");
            ace_free(mergeKey);
            return nullptr;
        }
    } else {
        if (strcpy_s(mergeKey, mergeLen, addKey)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "copy json key failed\n");
            ace_free(mergeKey);
            return nullptr;
        }
    }
    return mergeKey;
}

bool CJSONParser::PutNumOrStrValue(const char *key, cJSON item)
{
    if (key == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the json key is nullptr");
        return false;
    }
    uint32_t index = FNVHash(key);
    uint32_t *valueIndex = reinterpret_cast<uint32_t *>(startPos_ + index * sizeof(uint32_t));
    if (valueIndex == nullptr) {
        return false;
    }
    if (*valueIndex != 0) {
        // the hash value is conflict
        Node *currentNode = reinterpret_cast<Node *>(startPos_ + (*valueIndex));
        while ((currentNode != nullptr) && (currentNode->nextIndex != 0)) {
            currentNode = reinterpret_cast<Node *>(startPos_ + currentNode->nextIndex);
        }
        if (currentNode != nullptr) {
            currentNode->nextIndex = usedOffset_;
        }
    } else {
        *valueIndex = usedOffset_;
    }
    Node *node = reinterpret_cast<Node *>(startPos_ + usedOffset_);
    usedOffset_ = GetUseLen(usedOffset_ + sizeof(Node));
    if (usedOffset_ == 0) {
        return false;
    }
    node->pathIndex = usedOffset_;
    if (!CacheStrValue(key)) {
        return false;
    }
    if (item.type == cJSON_Number) {
        node->isNumber = true;
        node->valueIndex = item.valuedouble;
        return true;
    }
    node->valueIndex = usedOffset_;
    return CacheStrValue(item.valuestring);
}

bool CJSONParser::CacheStrValue(const char *value)
{
    if (strcpy_s(reinterpret_cast<char *>(startPos_ + usedOffset_), LOCALIZATION_SIZE - usedOffset_, value)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "copy value failed");
        return false;
    }
    usedOffset_ = GetUseLen(usedOffset_ + strlen(value) + 1);
    if (usedOffset_ == 0) {
        return false;
    }
    return true;
}

bool CJSONParser::CacheValue(const char *key, cJSON item)
{
    char *mergeKey = GetMergeKey(const_cast<char *>(key), item.string);
    bool result = true;
    if ((item.type == cJSON_Number) || (item.type == cJSON_String)) {
        result = PutNumOrStrValue(mergeKey, item);
    } else if (item.type == cJSON_Object) {
        if ((item.child == nullptr) || (!CacheValue(mergeKey, *item.child))) {
            result = false;
        }
    } else {
        // if the object is array, skip it. find the array value through read json file
    }
    ACE_FREE(mergeKey);
    if (!result) {
        return false;
    }
    return (item.next == nullptr) || (CacheValue(key, *item.next));
}

jerry_value_t CJSONParser::GetValueFromFile(const char *key,
                                            jerry_value_t args,
                                            jerry_size_t argsNum,
                                            const char *languageFile,
                                            bool &nullValueFlag)
{
    if (languageFile == nullptr || strlen(languageFile) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid language file name");
        return UNDEFINED;
    }
    ListNode *keys = nullptr;
    uint8_t keyCount = Split(key, '.', *&keys);
    char *content = ReadJSFile(filePath_, languageFile);
    cJSON *fileJson = cJSON_Parse(content);
    ACE_FREE(content);
    cJSON *curJsonItem = fileJson;
    uint8_t curKeyIndex = 0;
    do {
        // get the current splited key
        char *message = CJSONParser::GetNode(keys, keyCount - curKeyIndex);
        curJsonItem = cJSON_GetObjectItem(curJsonItem, message);
        curKeyIndex++;
    } while (!(curKeyIndex == keyCount || (curJsonItem == nullptr)));
    ClearNode(keys);
    jerry_value_t result = UNDEFINED;
    do {
        if (curJsonItem == nullptr) {
            break;
        }
        if (cJSON_IsNumber(curJsonItem)) {
            result = jerry_create_number(curJsonItem->valuedouble);
            break;
        }
        char *value = FillPlaceholder(curJsonItem->valuestring, args, argsNum);
        if (value == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "get nullptr value after place holder filling, keyLen[%{public}d]",
                        strlen(key));
            nullValueFlag = true;
            break;
        }
        if (strlen(value) == 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "warning: get 0 length str after place holder filling, keyLen[%{public}d]",
                        strlen(key));
        }
        result = jerry_create_string(reinterpret_cast<jerry_char_t *>(value));
        ace_free(value);
        value = nullptr;
    } while (0);
    cJSON_Delete(fileJson);
    fileJson = nullptr;
    return result;
}

jerry_value_t CJSONParser::GetValue(const char *key, const jerry_value_t args[], const jerry_size_t argsNum)
{
    jerry_value_t arg = (argsNum > 1) ? args[1] : UNDEFINED;
    // try finding from cache first
    Node *node = GetValueFromCache(key);
    if (node == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "warning: get value from cache failed, keyLen[%{public}d]", strlen(key));
        // no node found from cache, searching from current language file then
        bool nullValueFlag = false;
        jerry_value_t resultFromFile = GetValueFromFile(key, arg, argsNum, languageFile_, nullValueFlag);
        if (nullValueFlag) {
            HILOG_INFO(HILOG_MODULE_ACE, "get undefined value for key[%{public}s]", key);
            return resultFromFile;
        }
        if (!IS_UNDEFINED(resultFromFile) && !IS_ERROR_VALUE(resultFromFile)) {
            // found
            return resultFromFile;
        }
        jerry_release_value(resultFromFile);
        HILOG_ERROR(HILOG_MODULE_ACE, "get error from current language file, try en-US last");
        // failed, get from default language file last
        resultFromFile = GetValueFromFile(key, arg, argsNum, "en-US.json", nullValueFlag);
        if (nullValueFlag) {
            HILOG_INFO(HILOG_MODULE_ACE, "get undefined value for key[%{public}s]", key);
            return resultFromFile;
        }
        if (!IS_UNDEFINED(resultFromFile) && !IS_ERROR_VALUE(resultFromFile)) {
            return resultFromFile;
        }
        jerry_release_value(resultFromFile);
        HILOG_ERROR(HILOG_MODULE_ACE, "get error from default file, return the key, keyLen[%{public}d]", strlen(key));
        return jerry_create_string((const jerry_char_t *)(key));
    }

    // cache node found, number case
    if (node->isNumber) {
        return jerry_create_number(node->valueIndex);
    }
    // NOT number case
    const char *result = reinterpret_cast<char *>(startPos_ + (uint32_t)(node->valueIndex));
    char *valueStr = FillPlaceholder(result, arg, argsNum);
    if (valueStr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "get null value after place holder filling, valudeIndex[%{public}d], return the key",
                    (uint32_t)(node->valueIndex));
        // empty value, return the key
        return jerry_create_string((const jerry_char_t *)(key));
    }
    if (strlen(valueStr) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "warning: get empty value after place holder filling, valudeIndex[%{public}d], keyLen[%{public}d]",
                    (uint32_t)(node->valueIndex), strlen(key));
    }
    jerry_value_t valueJSValue = jerry_create_string(reinterpret_cast<jerry_char_t *>(valueStr));
    ACE_FREE(valueStr);
    return valueJSValue;
}

CJSONParser::Node *CJSONParser::GetValueFromCache(const char *key)
{
    if (!isCached_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "warning: cache status is NO, keyLen[%{public}d]", strlen(key));
        return nullptr;
    }
    uint16_t index = FNVHash(key);
    uint32_t *valueIndex = reinterpret_cast<uint32_t *>(startPos_ + index * sizeof(uint32_t));
    while ((valueIndex != nullptr) && (*valueIndex != 0)) {
        Node *node = reinterpret_cast<Node *>(startPos_ + (*valueIndex));
        if (node == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE,
                        "get value from cache failed, valueIndex[%{public}d], keyLen[%{public}d]",
                        (*valueIndex), strlen(key));
            return nullptr;
        }
        char *nodeKey = reinterpret_cast<char *>(startPos_ + node->pathIndex);
        if ((nodeKey != nullptr) && (!strcmp(key, nodeKey))) {
            return node;
        } else {
            *valueIndex = node->nextIndex;
        }
    }
    return nullptr;
}

uint8_t CJSONParser::Split(const char *target, char delemeter, ListNode *&node)
{
    if (target == nullptr) {
        return 0;
    }
    uint8_t nodeCount = 0;
    char *current = nullptr;
    int startPos = 0;
    // get the position of delemeter
    int pos = IndexOf(target, delemeter, startPos);
    while (pos != -1) {
        // add the value between startPos and index to node
        current = SubStr(target, startPos, pos - startPos);
        if (AddNode(node, current)) {
            nodeCount++;
        } else {
            ACE_FREE(current);
        }
        startPos = pos + 1;
        pos = IndexOf(target, delemeter, startPos);
    }
    if ((startPos != -1) && (static_cast<size_t>(startPos) < strlen(target))) {
        current = SubStr(target, startPos, strlen(target) - startPos);
        if (AddNode(node, current)) {
            nodeCount++;
        } else {
            ACE_FREE(current);
        }
    }
    // deal the key from lastIndex to strlen(target), the last find would return -1
    return nodeCount;
}

int CJSONParser::IndexOf(const char *string, char delemeter, int index)
{
    if (string == nullptr) {
        return -1;
    }
    if ((index < 0) || (strlen(string) >= INT8_MAX) || (static_cast<size_t>(index) >= strlen(string))) {
        return -1;
    }
    int length = strlen(string);
    int i = index;
    do {
        if (string[i] == delemeter) {
            return i;
        }
        i++;
    } while (i < length);
    // if the delemeter is not find return -1
    return -1;
}

#ifdef LOCALIZATION_PLURAL
char *CJSONParser::GetParamValue(const char *attrName, jerry_value_t param, bool isPlural)
#else
char *CJSONParser::GetParamValue(const char *attrName, jerry_value_t param)
{
    if (attrName == nullptr) {
        return nullptr;
    }
    char *contentValue = nullptr; // store the value defined in param
    uint16_t contentLen = 0;
#ifdef LOCALIZATION_PLURAL
    if (jerry_value_is_number(param)) {
        contentValue = MallocStringOf(param, &contentLen);
    }
#else
    jerry_value_t propName = jerry_create_string(reinterpret_cast<jerry_char_t const *>(attrName));
    if (JerryHasProperty(param, propName)) { // get the placeholder {...} value defined in param
        jerry_value_t paramValue = jerry_get_property(param, propName);
        // the contentValue would be released in clearNode values
        contentValue = MallocStringOf(paramValue, &contentLen);
        jerry_release_value(paramValue);
    }
    jerry_release_value(propName);
#endif
    // if the length of attrVal is 0, the node wouldn't add to the list
    if ((contentValue != nullptr) && contentLen != 0) {
        return contentValue;
    }
    ACE_FREE(contentValue); // if the contentValue is \0
    return nullptr;
}
#endif
} // namespace ACELite
} // namespace OHOS
#endif
