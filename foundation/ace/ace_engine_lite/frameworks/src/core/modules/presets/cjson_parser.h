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
#ifndef OHOS_ACELITE_CJSON_PARSER_H
#define OHOS_ACELITE_CJSON_PARSER_H
#include "acelite_config.h"

#if (FEATURE_LOCALIZATION_MODULE == 1)
#include <cJSON.h>
#include <jerryscript.h>
#ifdef __LITEOS_A__
#include "dirent.h"
#include "unistd.h"
#endif
#include "ace_mem_base.h"
#include "js_app_context.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class CJSONParser final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(CJSONParser);
    struct ListNode : public MemoryHeap {
        char *value = nullptr;
        ListNode *next = nullptr;
    };

    // can't create the node object.it is used to store the json node is psram/ram
    struct Node {
        double valueIndex = 0;
        uint32_t pathIndex = 0; // the path value index in psram
        uint16_t nextIndex = 0; // the conflicy Node index in array
        bool isNumber = false;
    };

    enum LanguageState : uint8_t {
        LANGUAGE_FAIL,
        LANGUAGE_NOT_CHANGE,
        LANGUAGE_CHANGED
    };

    CJSONParser();

    ~CJSONParser()
    {
        void *value = reinterpret_cast<void *>(startPos_);
        ACE_FREE(value);
        ACE_FREE(countries_);
        ACE_FREE(language_);
        ACE_FREE(filePath_);
        ACE_FREE(languageFile_);
    }

    /**
     * @brief: replace the placeholder in result with the data defined by user
     * if plural is true, it will replace the number in placeholder,
     * replace the same name in js object.
     */
    char *FillPlaceholder(const char *format, jerry_value_t arg, jerry_length_t num, bool isPlural = false);

    /**
     * @brief: init localization, get the resource file path
     * and get the localization cache ram
     */
    bool Init();

    /**
     * @brief: merge string destination and resource, save the merge result in destination
     * if check is true, it will free destination string
     */
    char *SubStr(const char *src, size_t start, size_t length);

    /**
     * @brief: split the string by delemeter and save the split result in node
     *
     * @return the length of node
     */
    uint8_t Split(const char *target, char delemeter, ListNode *&node);

    static bool AddNode(ListNode *&head, char *current);
    static char *GetNode(ListNode *node, int index);
    static void ClearNode(ListNode *&node);
    static char *ToString(ListNode *node, uint8_t length);

    /**
     * @brief get localization format result
     * @param key: the json path
     * @param args: the args defined the value of placeholder
     * @return the JS value of localization result
     */
    jerry_value_t GetValue(const char *key, const jerry_value_t args[], jerry_size_t argsNum);

    /**
     * @brief judge the system language and origion is changed
     * @return 1: the system language is changed
     *         0: the system language
     *         -1: get system language failed
     */
    LanguageState ChangeLanguage();

    /**
     * @brief cache the frist file in fileList to PSRAM/RAM
     * @return the cache result, true: cache file success
     * fail: the fileList is null or the file is too large
     */
    bool CacheFile();

private:
    enum conValue {
        PRIME_OFFSET_1 = 3,
        PRIME_OFFSET_2 = 5,
        PRIME_OFFSET_3 = 7,
        PRIME_OFFSET_4 = 13,
        PRIME_OFFSET_5 = 17,
        MAX_KEY_NUM = 500,
        MAX_VALUE_LEN = 1024,
        LOCALIZATION_SIZE = 10240,
        NFV_PRIME = 16777619
    };

    /**
     * @Brief: get the position of delemeter after index
     * @return if the index is upper than the length of string, return -1
     * else if the index is not defined in string, return the length, else return the position of it
     */
    int IndexOf(const char *string, char delemeter, int index);

    /**
     * @Brief: split the string defined in value by '|', and get value in the split string
     * For example: the string is car|cars, if the value of param is odd, assign value car,else cars
     */
#ifdef LOCALIZATION_PLURAL
    char *GetPluralValue(char *value, jerry_value_t param);
    uint8_t FormatString(const char *format, jerry_value_t arg, ListNode *&node, bool isPlural, jerry_value_t num);
    /**
     * @brief: get the value defined in param
     * @param: attrName:the attribute name in param, for example "name"
     *         param: the object defined attrName, for example {"name":"Jane"}
     * @return: the value in param
     */
    char *GetParamValue(const char *attrName, jerry_value_t param, bool isPlural);
#else
    uint8_t FormatString(const char *format, jerry_value_t arg, ListNode *&node, jerry_value_t num);
    char *GetParamValue(const char *attrName, jerry_value_t param);
#endif

    char* GetMergeKey(const char *key, const char *addKey);

    bool CacheValue(const char *key, cJSON item);

    bool PutNumOrStrValue(const char *key, cJSON item);

    bool CacheStrValue(const char *value);

    /**
     * @brief get value from file in fileList
     * @param key JSON path
     * @param args the value to replace the placeholder
     * @param argsNum the num of args
     * @param languageFile the target json file name
     * @return the js value of target value
     */
    jerry_value_t GetValueFromFile(const char *key,
                                   jerry_value_t args,
                                   jerry_size_t argsNum,
                                   const char *languageFile,
                                   bool &nullValueFlag);

    /**
     * @brief get 4-byte aligned offset
     * @param addr the used offset
     * @return the nearest 4-byte aligned offset
     */
    uint32_t GetUseLen(uint32_t usedOffset)
    {
        const uint8_t align = 2;
        uint32_t len = usedOffset + (1 << align);
        len = (len >> align) << align;
        if (len > LOCALIZATION_SIZE) {
            isCached_ = false; // cache json file failed
            return 0;
        }
        return len;
    }

    /**
     * @brief get the string hash value.the hash algorithm is NFV-1a hash
     * the Algorithm URL is https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
     * @param key the string need to hash
     * @return the hash result which is the index array number
     */
    uint16_t FNVHash(const char *key)
    {
        uint64_t hash = 2166136261;
        uint32_t len = strlen(key);
        for (uint8_t i = 0; i < len; i++) {
            uint8_t keyVal = key[i];
            hash = (hash ^ keyVal) * NFV_PRIME;
        }
        hash += hash << PRIME_OFFSET_4;
        hash ^= hash >> PRIME_OFFSET_3;
        hash += hash << PRIME_OFFSET_1;
        hash ^= hash >> PRIME_OFFSET_5;
        hash += hash << PRIME_OFFSET_2;
        return hash % MAX_KEY_NUM;
    }

    Node *GetValueFromCache(const char *key);

    bool ChangeLanguageFileName();

    bool isCached_;
    bool isAllocated_;
    char leftDelemeter_;
    char rightDelemeter_;
    uint32_t usedOffset_;
    uintptr_t startPos_;
    char *countries_; // the current country and origion
    char *language_;  // the current system language
    char *filePath_;
    char *languageFile_;
};
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_LOCALIZATION_MODULE
#endif // OHOS_ACELITE_CJSON_PARSER_H
