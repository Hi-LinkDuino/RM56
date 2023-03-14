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

#include "dbm_kv_store.h"

#include <securec.h>

#include "dbm_def.h"
#include "dbm_errno.h"
#include "dbm_kv_store_env.h"

#include "utils_file.h"
#include "ohos_types.h"

#ifdef DBM_DEBUG_ENABLE
#define DBM_LOG_TAG "[low_lvl][kv]"
#endif

#define MAX_GET_VALUE_LEN               0x7FFFFFFF // max int
#define KV_SUM_FILE                     "KV_FILE_SUM"
#define KV_MAGIC                        "KVDB"
#define KV_MAGIC_SIZE                   4
#define KV_SUM_INDEX                    4
#define KV_SUM_BLOCK_SIZE               4
#define KV_SUM_DATA_ITEM_SIZE           44
#define MAX_KEY_PATH                    256
#define MAX_FILE_PATH                   256
#define SUM_FILE_HEADER_SIZE            20
#define SUM_FILE_DEFAULT_HEADER         "000\0"KV_MAGIC"\0\0\0\0"DBM_SW_VERSION"\0"KV_MAGIC

/*
sum file struct
|       4        |      4     |       4       |      4     |     4      |    44     |   repeat  |
***********************************************************************************...***********
|  item sum num  | KVDB magic | sum file flag | DB version | KVDB magic | Data Item | Data Item |
***********************************************************************************...***********

data item struct
|      4      |     4      |  32 |     4      |
***********************************************
|  item flag  | key length | key | KVDB magic |
***********************************************

item sum num: means total number of entries in the current database
KVDB magic: Constant KVDB for verification
sum file flag: 1st byte means sum num valid, 2nd byte means db invalid
Data Item: key content and status
    item flag: 1st byte constant 0, 2nd means item valid, 3rd means bak valid
    key length: cur constant 0
    key: key content
*/

#define DB_LOCK(db)                                                                     \
    do {                                                                                \
        if (((DBHandle)(db))->lock != NULL)   ((DBHandle)(db))->lock((DBHandle)(db));         \
    } while (0)

#define DB_UNLOCK(db)                                                                   \
    do {                                                                                \
        if (((DBHandle)(db))->unlock != NULL)   ((DBHandle)(db))->unlock((DBHandle)(db));     \
    } while (0)

static boolean IsValidPath(const char* path)
{
    if ((path == NULL) || (strlen(path) >= MAX_FILE_PATH)) {
        return FALSE;
    }
    return TRUE;
}

typedef enum {
    DBM_DB_TYPE_KV = 0,
    DBM_DB_TYPE_KV_NO_DIR = 1,
} DBType;

struct KVDBStatus {
    boolean isNeedRecovery;
    boolean isInvalid;
    boolean kvSumIndexValid;
} KVDBStatus;

/* database structure */
typedef struct DBM *DBHandle;
struct DBM {
    char magic[KV_MAGIC_SIZE + 1];                          // it is a buff special identification db
    const char* name;                                       // database name
    const char* dirPath;                                    // directory path for saving database
    DBType type;                                            // database type
    int kvItemSum;                                          // database max kv item size.
    boolean initOK;                                         // database has been init ok
    struct KVDBStatus sumFlag;                              // database status
    int sumFileFd;                                          // current index file object
    void (*lock)(DBHandle db);                              // lock the database operate
    void (*unlock)(DBHandle db);                            // unlock the database operate
    int version;                                            // database version
} DBM;

// data item struct def
typedef enum {
    BLOB,
    STR,
} ValueType;

struct ItemStatus {
    boolean isValid;
    boolean isBakValid;
    ValueType valueType;
} ItemStatus;

typedef struct KeyItem {
    char key[MAX_KEY_LEN + 1];
    struct ItemStatus flag;
    unsigned int len;
    int index;   // in sum file position of index
} KeyItem;

static const char* ItemPathFormat(const DBHandle db)
{
    if (db->type == DBM_DB_TYPE_KV_NO_DIR) {
        return "%s%s";
    }
    return "%s/%s";
}

/* begin GetKVStore */
static void DBMInitFinish(const DBHandle db, int ret)
{
    static boolean isLogShow = FALSE;
    if (ret == DBM_OK) {
        db->initOK = TRUE;
        if (!isLogShow) {
            DBM_INFO("DBM KV V%s is initialize success.", DBM_SW_VERSION);
            isLogShow = TRUE;
        }
    } else {
        DBM_INFO("Error: %s DB initialize fail (%d).",
            (db->type == DBM_DB_TYPE_KV) || (db->type == DBM_DB_TYPE_KV_NO_DIR) ? "KVDB" : "OTHERS_TYPE", ret);
    }
}

static void ReleaseKVDBHandle(DBHandle db)
{
    if (db == NULL) {
        return;
    }

    if (db->sumFileFd >= 0) {
        UtilsFileClose(db->sumFileFd);
        db->sumFileFd = -1;
    }
    free(db);
}

static DBHandle GetKVDBHandle(const char* path)
{
    DBHandle db = (DBHandle)malloc(sizeof(DBM));
    if (db == NULL) {
        return NULL;
    }

    int ret = memset_s(db, sizeof(DBM), 0, sizeof(DBM));
    if (ret != EOK) {
        free(db);
        return NULL;
    }

    if (strcpy_s(db->magic, KV_MAGIC_SIZE + 1, KV_MAGIC) != EOK) {
        free(db);
        return NULL;
    }
    db->dirPath = path;

    db->kvItemSum = 0;
    db->initOK = FALSE;
    db->version = DBM_SW_VERSION_NUM;
    db->sumFileFd = -1;

    db->type = (strlen(path) == 0) ? DBM_DB_TYPE_KV_NO_DIR : DBM_DB_TYPE_KV;

    ret = DBM_UserInit();
    DBMInitFinish(db, ret);
    if (ret != 0) {
        ReleaseKVDBHandle(db);
        return NULL;
    }
    return db;
}

static int FileReadCursor(int fd, int offset, unsigned int whence, char* buf, unsigned int cursorSize)
{
    int pos = UtilsFileSeek(fd, offset, whence);
    if (pos < 0) {
        return UtilsFileSeek(fd, 0, SEEK_END_FS);
    }

    pos = UtilsFileRead(fd, buf, cursorSize);
    if (pos < 0) {
        return -1;
    }
    return pos;
}

static int FileWriteCursor(int fd, int offset, unsigned int whence, const char* buf, unsigned int cursorSize)
{
    int pos = UtilsFileSeek(fd, offset, whence);
    if (pos < 0) {
        DBM_INFO("FileWriteCursor: seek file offset[%d] fail.", offset);
        return -1; // Invalid position
    }

    int ret = UtilsFileWrite(fd, buf, cursorSize);
    if (ret < 0) {
        DBM_INFO("FileWriteCursor: write buff to file fail. [%d]", ret);
        return -1;
    }
    return ret;
}

static boolean IsStrSame(const char* value, int len, const char* str)
{
    if ((unsigned int)len != strlen(str) + 1) {
        return FALSE;
    }

    for (int i = 0; i < len; ++i) {
        if (*value++ != *str++) {
            return FALSE;
        }
    }
    return TRUE;
}

// Is judge by magic num which save in sum file header
static boolean IsHeaderValid(const char* header)
{
    char magic[KV_MAGIC_SIZE + 1] = {0};
    if (memcpy_s(magic, KV_MAGIC_SIZE, header + KV_SUM_INDEX, KV_MAGIC_SIZE) != EOK &&
        !IsStrSame(magic, KV_MAGIC_SIZE + 1, KV_MAGIC)) {
        return FALSE;
    }

    // after 1st magic is block size flag
    // magic is after 4 block
    if (memcpy_s(magic, KV_MAGIC_SIZE, header + 4 * KV_SUM_BLOCK_SIZE, KV_MAGIC_SIZE) != EOK &&
        !IsStrSame(magic, KV_MAGIC_SIZE + 1, KV_MAGIC)) {
        return FALSE;
    }
    return TRUE;
}

static int GetSumFilePath(DBHandle db, char* sumFilePath, unsigned int len)
{
    if (!db) {
        return DBM_ERROR;
    }
    
    if (strlen(db->dirPath) == 0) {
        if (strcpy_s(sumFilePath, sizeof(KV_SUM_FILE), KV_SUM_FILE) != EOK) {
            return DBM_OK;
        }
    }

    if (sprintf_s(sumFilePath, len, ItemPathFormat(db), db->dirPath, KV_SUM_FILE) < 0) {
        return DBM_INVALID_ARGS;
    }
    return DBM_OK;
}

// return is fd, -1 means invalid
static int OpenSumFile(DBHandle db, unsigned int mode)
{
    char* sumFilePath = (char *)malloc(MAX_KEY_PATH + 1);
    if (sumFilePath == NULL) {
        return -1; // invalid fd
    }
    (void)memset_s(sumFilePath, MAX_FILE_PATH + 1, 0, MAX_FILE_PATH + 1);

    int ret = GetSumFilePath(db, sumFilePath, MAX_KEY_PATH + 1);
    if (ret < 0) {
        DBM_INFO("OpenSumFile: Get sum file path fail.");
        free(sumFilePath);
        return -1;
    }
    int fd = UtilsFileOpen(sumFilePath, mode, 0);
    free(sumFilePath);
    return fd;
}

static int LoadDataItem(DBHandle db, int index, KeyItem* item)
{
    if (item == NULL) {
        return DBM_INVALID_ARGS;
    }

    char itemContent[KV_SUM_DATA_ITEM_SIZE + 1] = {0};
    int ret = FileReadCursor(db->sumFileFd, SUM_FILE_HEADER_SIZE + index * KV_SUM_DATA_ITEM_SIZE,
        SEEK_SET_FS, itemContent, KV_SUM_DATA_ITEM_SIZE);
    if (ret < 0) {
        return DBM_ERROR;
    }

    // check last tail magic num
    if (!IsStrSame(itemContent + KV_SUM_DATA_ITEM_SIZE - KV_MAGIC_SIZE, KV_MAGIC_SIZE + 1, KV_MAGIC)) {
        return DBM_OK;
    }

    // 1st pos is default 0
    // 3 pos represented flag
    item->flag.isValid = *(itemContent + 1);
    item->flag.isBakValid = *(itemContent + 2);
    item->flag.valueType = *(itemContent + 3);

    char keyLen[KV_SUM_BLOCK_SIZE] = {0}; // default key content len is 32
    ret = memcpy_s(keyLen, KV_SUM_BLOCK_SIZE, itemContent + KV_SUM_BLOCK_SIZE, KV_SUM_BLOCK_SIZE);
    if (ret != EOK) {
        return DBM_ERROR;
    }

    int length = atoi(keyLen);
    if (length < 0) {
        return DBM_ERROR;
    }
    item->len = length;

    ret = memcpy_s(item->key, MAX_KEY_LEN, itemContent + 2 * KV_SUM_BLOCK_SIZE, MAX_KEY_LEN); // 2 block: index and len
    if (ret != EOK) {
        return DBM_ERROR;
    }
    item->key[MAX_KEY_LEN] = '\0'; // make sure key is a string
    item->index = index;
    return DBM_OK;
}

static int DeleteValueFromFile(DBHandle db, const char* key)
{
    char keyPath[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(keyPath, MAX_KEY_PATH + 1, ItemPathFormat(db), db->dirPath, key) < 0) {
        return DBM_ERROR;
    }
    int ret = UtilsFileDelete(keyPath);
    if (ret < 0) {
        DBM_INFO("%s delete value file failed ret[%d]", __func__, ret);
        return DBM_ERROR;
    }
    return DBM_OK;
}

static boolean IsNewItem(DBHandle db, const char* key)
{
    char keyPath[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(keyPath, MAX_KEY_PATH + 1, ItemPathFormat(db), db->dirPath, key) < 0) {
        return FALSE;
    }
    int fd = UtilsFileOpen(keyPath, O_RDONLY_FS, 0);
    if (fd < 0) {
        return TRUE;
    }

    int ret = UtilsFileClose(fd);
    fd = -1;
    if (ret < 0) {
        DBM_INFO("Is new item close file fail.");
    }
    return FALSE;
}

static inline int GetKeyItemOffset(int index)
{
    return SUM_FILE_HEADER_SIZE + index * KV_SUM_DATA_ITEM_SIZE;
}

static int DelItem(DBHandle db, const KeyItem* item)
{
    if (!IsNewItem(db, item->key) && DeleteValueFromFile(db, item->key) != DBM_OK) {
        return DBM_ERROR;
    }

    char bakKey[MAX_KEY_LEN + 8] = {0}; // 8 is bak file suffix len
    if (sprintf_s(bakKey, MAX_KEY_LEN + 8, "%s_dbm_kv", item->key) < 0) {
        return DBM_INVALID_ARGS;
    }

    if (!IsNewItem(db, bakKey) && DeleteValueFromFile(db, bakKey) != DBM_OK) {
        return DBM_ERROR;
    }

    char itemData[KV_SUM_DATA_ITEM_SIZE + 1] = {0};
    // erase all except magic num, means this item is useless but invalid
    int ret = FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index), SEEK_SET_FS,
        itemData, KV_SUM_DATA_ITEM_SIZE - KV_MAGIC_SIZE);
    if (ret < 0) {
        return DBM_ERROR;
    }
    return DBM_OK;
}

static int CopyValueToFile(DBHandle db, const char* src, const char* dest)
{
    char srcPath[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(srcPath, MAX_KEY_PATH + 1, ItemPathFormat(db), db->dirPath, src) < 0) {
        return DBM_ERROR;
    }

    char destPath[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(destPath, MAX_KEY_PATH + 1, ItemPathFormat(db), db->dirPath, dest) < 0) {
        return DBM_ERROR;
    }
    int ret = UtilsFileCopy(srcPath, destPath);
    if (ret < 0) {
        DBM_INFO("%s copy value file failed ret[%d]", __func__, ret);
        return DBM_ERROR;
    }

    return DBM_OK;
}

// recover backup item
static int RecoverItem(DBHandle db, const KeyItem* item)
{
    if (!IsNewItem(db, item->key) && DeleteValueFromFile(db, item->key) != DBM_OK) {
        return DBM_ERROR;
    }

    char bakFile[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(bakFile, MAX_KEY_PATH + 1, "%s_dbm_kv", item->key) < 0) {
        return DBM_INVALID_ARGS;
    }

    if (CopyValueToFile(db, bakFile, item->key) != DBM_OK) {
        return DBM_ERROR;
    }

    char itemFlagByte[1] = {0};
    int ret = FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index) + 1, SEEK_SET_FS, itemFlagByte, 1);
    if (ret < 0) {
        DBM_INFO("%s update flag failed ret[%d]", __func__, ret);
        return DBM_ERROR;
    }
    return DBM_OK;
}

static int CheckPointItem(DBHandle db, const KeyItem* item)
{
    if (strlen(item->key) == 0) {
        return DBM_OK;
    }
    if (db->sumFlag.isInvalid || !item->flag.isValid) {
        DBM_DEBUG("CheckPointItem item DBinvalid[%d] itemValid[%d]", db->sumFlag.isInvalid, item->flag.isValid);
        return DelItem(db, item);
    }

    char bakFile[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(bakFile, MAX_KEY_PATH + 1, "%s_dbm_kv", item->key) < 0) {
        return DBM_ERROR;
    }
    if (item->flag.isBakValid && IsNewItem(db, bakFile)) {
        return DelItem(db, item);
    }

    if (item->flag.isBakValid) {
        DBM_DEBUG("CheckPointItem item bak valid[%d]", item->flag.isBakValid);
        return RecoverItem(db, item);
    } else {
        if (!IsNewItem(db, bakFile) && DeleteValueFromFile(db, bakFile) != DBM_OK) {
            DBM_INFO("CheckPointItem: delete old backup data fail.");
            return DBM_ERROR;
        }
    }
    return DBM_OK;
}

// need check all item
static int VacuumStore(DBHandle db)
{
    int sumFileLen = UtilsFileSeek(db->sumFileFd, 0UL, SEEK_END_FS);
    if ((sumFileLen < SUM_FILE_HEADER_SIZE) && (sumFileLen != 0)) {
        DBM_INFO("sum file invalid length!");
        return DBM_ERROR;
    }

    int ret = UtilsFileSeek(db->sumFileFd, 0UL, SEEK_SET_FS);
    if (ret < 0) {
        return DBM_ERROR;
    }
    // 20 is header len, 44 is per item size
    int sumIndex = (sumFileLen - SUM_FILE_HEADER_SIZE) / KV_SUM_DATA_ITEM_SIZE;
    if (sumIndex > DBM_MAX_KV_SUM) {
        return DBM_ERROR;
    }
    if (!db->sumFlag.kvSumIndexValid) {
        db->kvItemSum = 0; // need recalculate
    }

    KeyItem* item = (KeyItem *)malloc(sizeof(KeyItem));
    if (item == NULL) {
        return DBM_ERROR;
    }
    (void)memset_s(item, sizeof(KeyItem), 0, sizeof(KeyItem));
    for (int index = 0; index < sumIndex; ++index) {
        ret = LoadDataItem(db, index, item);
        if (ret != DBM_OK) {
            DBM_DEBUG("VacuumStore: load data item fail!");
            continue;
        }
        ret = CheckPointItem(db, item);
        if (ret != DBM_OK) {
            DBM_INFO("VacuumStore: check point data item fail!");
            break;
        }

        if (!db->sumFlag.kvSumIndexValid && item->flag.isValid && !db->sumFlag.isInvalid) {
            db->kvItemSum++;
        }
        (void)memset_s(item, sizeof(KeyItem), 0, sizeof(KeyItem));
    }
    free(item);
    return ret;
}

static int SetSumIndexToFile(DBHandle db, int sumIndex)
{
    db->sumFlag.kvSumIndexValid = FALSE;
    // sum index in sum file pos(9)
    int pos = FileWriteCursor(db->sumFileFd, KV_SUM_INDEX + KV_MAGIC_SIZE, SEEK_SET_FS, "\0", 1);
    if (pos < 0) {
        return DBM_ERROR;
    }

    char value[KV_SUM_INDEX] = {0};
    if (sprintf_s(value, KV_SUM_INDEX, "%d", sumIndex) < 0) {
        return DBM_ERROR;
    }

    db->kvItemSum = sumIndex;
    pos = FileWriteCursor(db->sumFileFd, 0, SEEK_SET_FS, value, KV_SUM_INDEX);
    if (pos < 0) {
        return DBM_ERROR;
    }

    db->sumFlag.kvSumIndexValid = TRUE;
    pos = FileWriteCursor(db->sumFileFd, KV_SUM_INDEX + KV_MAGIC_SIZE, SEEK_SET_FS, "\1", 1);
    if (pos < 0) {
        return DBM_ERROR;
    }
    return DBM_OK;
}

static int SetSumIndex(DBHandle db)
{
    if (db->sumFlag.kvSumIndexValid) {
        return DBM_OK; // has been refresh in load header
    }
    return SetSumIndexToFile(db, db->kvItemSum);
}

static int CheckPoint(DBHandle db)
{
    if (!db->sumFlag.isNeedRecovery) {
        return DBM_OK;
    }

    int ret = VacuumStore(db);
    if (ret != DBM_OK) {
        return DBM_ERROR;
    }

    ret = SetSumIndex(db);
    if (ret != DBM_OK) {
        return DBM_ERROR;
    }

    char flag[KV_SUM_BLOCK_SIZE] = {0};
    ret = FileWriteCursor(db->sumFileFd, KV_SUM_INDEX + KV_SUM_BLOCK_SIZE, SEEK_SET_FS, flag, KV_SUM_BLOCK_SIZE);
    if (ret < 0) { // ret is fd offset
        return DBM_ERROR;
    }

    (void)memset_s(&(db->sumFlag), sizeof(KVDBStatus), 0, sizeof(KVDBStatus));
    db->sumFlag.isNeedRecovery = FALSE;
    return DBM_OK;
}

static void SetSumFlag(DBHandle db, const char* headerFlag)
{
    db->sumFlag.kvSumIndexValid = *headerFlag;
    db->sumFlag.isInvalid = *(headerFlag + 1);
    db->sumFlag.isNeedRecovery = TRUE;
}

static int LoadSumFileHeader(DBHandle db)
{
    char header[SUM_FILE_HEADER_SIZE + 1] = {0};
    int ret = FileReadCursor(db->sumFileFd, 0, SEEK_SET_FS, header, SUM_FILE_HEADER_SIZE);
    if (ret < 0) {
        return DBM_ERROR;
    }

    char sumIndex[KV_SUM_INDEX + 1] = {0};
    if (memcpy_s(sumIndex, KV_SUM_INDEX, header, KV_SUM_INDEX) != EOK) {
        return DBM_ERROR;
    }

    char headerFlag[KV_SUM_BLOCK_SIZE + 1] = {0};
    if (memcpy_s(headerFlag, KV_SUM_INDEX, header + KV_MAGIC_SIZE + KV_SUM_INDEX, KV_SUM_INDEX) != EOK) {
        return DBM_ERROR;
    }

    if (!IsHeaderValid(header)) {
        return DBM_ERROR;
    }

    SetSumFlag(db, headerFlag);
    if (db->sumFlag.kvSumIndexValid) {
        db->kvItemSum = atoi(sumIndex);
    }
    return DBM_OK;
}

static int InitKVStore(DBHandle db)
{
    db->sumFileFd = OpenSumFile(db, O_RDWR_FS | O_CREAT_FS);
    if (db->sumFileFd < 0) {
        return DBM_ERROR;
    }

    int sumFileLen = UtilsFileSeek(db->sumFileFd, 0UL, SEEK_END_FS);
    DBM_DEBUG("InitKVStore read sum file len [%d]", sumFileLen);
    if (sumFileLen < 0) {
        return DBM_ERROR;
    }

    // Is need this version or corrupt
    if ((sumFileLen < SUM_FILE_HEADER_SIZE) && (sumFileLen != 0) && (sumFileLen != KV_SUM_INDEX)) {
        DBM_INFO("sum file invalid length!");
        sumFileLen = 0;
    }

    int ret = UtilsFileSeek(db->sumFileFd, 0UL, SEEK_SET_FS); // reset pos to begin
    if (ret < 0) {
        return DBM_ERROR;
    }

    // sumFileLen==0 means it's a new sum file
    if (sumFileLen == 0) {
        ret = UtilsFileWrite(db->sumFileFd, SUM_FILE_DEFAULT_HEADER, SUM_FILE_HEADER_SIZE);
        if (ret < 0) {
            return DBM_ERROR;
        }
    }

    if (sumFileLen > 0) {
        ret = LoadSumFileHeader(db);
        if (ret != DBM_OK) {
            return DBM_ERROR;
        }

        ret = CheckPoint(db);
        if (ret != DBM_OK) {
            return DBM_ERROR;
        }
    }
    return DBM_OK;
}

int DBM_GetKVStore(const char* storeFullPath, KVStoreHandle* kvStore)
{
    DBM_DEBUG("Limitation:key len[%d],value len[%d], keys sum[%d]", MAX_KEY_LEN, MAX_VALUE_LEN, DBM_MAX_KV_SUM);
    if (kvStore == NULL) {
        return DBM_INVALID_ARGS;
    }

    if (!IsValidPath(storeFullPath)) {
        DBM_INFO("Get KVStore invalid path.");
        return DBM_INVALID_ARGS;
    }

    DBHandle db = GetKVDBHandle(storeFullPath);
    if (db == NULL) {
        DBM_INFO("Get KVDBHandle fail.");
        return DBM_ERROR;
    }

    int ret = InitKVStore(db);
    if (ret != DBM_OK) {
        DBM_INFO("Get Init KV store fail.");
        ReleaseKVDBHandle(db);
        return DBM_ERROR;
    }

    DBM_DEBUG("FInish Get KV store.");
    *kvStore = db;
    return DBM_OK;
}
/* Finish GetKVStore */

static boolean IsDataItemMatched(DBHandle db, const char* key, int index)
{
    if (index < 0) {
        return FALSE;
    }
    char itemContent[KV_SUM_DATA_ITEM_SIZE + 1] = {0};
    int ret = FileReadCursor(db->sumFileFd, SUM_FILE_HEADER_SIZE + index * KV_SUM_DATA_ITEM_SIZE,
        SEEK_SET_FS, itemContent, KV_SUM_DATA_ITEM_SIZE);
    if (ret < 0) {
        return FALSE;
    }
    return IsStrSame(itemContent + KV_MAGIC_SIZE + KV_SUM_BLOCK_SIZE, strlen(key) + 1, key);
}

static int IsNeedTransferValue(DBHandle db, const char* key, const char* fileRead, unsigned int fileLen, boolean* isNeed)
{
    if (fileLen <= KV_SUM_BLOCK_SIZE) { // value size < index, means old version data
        *isNeed = FALSE;
        return DBM_OK;
    }

    char keyIndex[KV_SUM_BLOCK_SIZE] = {0};
    int ret = memcpy_s(keyIndex, KV_SUM_BLOCK_SIZE - 1, fileRead, KV_SUM_BLOCK_SIZE - 1);
    if (ret != EOK) {
        return DBM_ERROR;
    }

    // output refer to value version
    *isNeed = IsDataItemMatched(db, key, atoi(keyIndex));
    return DBM_OK;
}

static int FormatValueByFile(boolean isNeedTrans, char* value, unsigned int len,
    const char* fileRead, unsigned int fileLen)
{
    unsigned int offset = isNeedTrans ? KV_SUM_BLOCK_SIZE : 0;
    if (fileLen - offset > len) {
        return -1;
    }

    int ret = memcpy_s(value, len, fileRead + offset, fileLen - offset);
    if (ret != EOK) {
        return -1;
    }
    return fileLen - offset;
}

static int GetValueByFile(DBHandle db, const char* key, const char* keyGet, char* value, unsigned int len)
{
    char keyPath[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(keyPath, MAX_KEY_PATH + 1, ItemPathFormat(db), db->dirPath, keyGet) < 0) {
        return -1;
    }

    unsigned int valueLen = 0;
    if (UtilsFileStat(keyPath, &valueLen) < 0) {
        return -1;
    }

    if (valueLen > MAX_VALUE_LEN + KV_SUM_BLOCK_SIZE) {
        return -1;
    }

    char* valueRead = (char *)malloc(valueLen);
    if (valueRead == NULL) {
        return -1;
    }
    (void)memset_s(valueRead, valueLen, 0, valueLen);

    int fd = UtilsFileOpen(keyPath, O_RDONLY_FS, 0);
    if (fd < 0) {
        free(valueRead);
        return -1;
    }

    int valueReadLen = UtilsFileRead(fd, valueRead, valueLen);
    UtilsFileClose(fd);
    fd = -1;
    if (valueReadLen < 0) {
        free(valueRead);
        return -1;
    }

    boolean isNeedTrans = TRUE;
    int ret = IsNeedTransferValue(db, key, valueRead, valueReadLen, &isNeedTrans);
    if (ret != DBM_OK) {
        free(valueRead);
        return -1;
    }

    ret = FormatValueByFile(isNeedTrans, value, len, valueRead, valueReadLen);
    free(valueRead);
    if (ret < 0) {
        DBM_INFO("Format file value fail.");
    }
    return ret;
}

static int Get(KVStoreHandle db, const char* key, void* value, unsigned int count, unsigned int* realValueLen)
{
    if (!IsValidKey(key) || (value == NULL) || (count > MAX_GET_VALUE_LEN)) {
        return DBM_INVALID_ARGS;
    }

    if (!db->initOK || !IsStrSame(db->magic, KV_MAGIC_SIZE + 1, KV_MAGIC)) {
        return DBM_INVALID_ARGS;
    }

    char bakFile[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(bakFile, MAX_KEY_PATH + 1, "%s_dbm_kv", key) < 0) {
        return DBM_INVALID_ARGS;
    }

    boolean isBakExist = !IsNewItem(db, bakFile);
    if (db->sumFlag.isInvalid || (IsNewItem(db, key) && !isBakExist)) {
        return DBM_NOT_FOUND;
    }

    const char* keyGet = isBakExist ? bakFile : key; // bak existed means update fail
    int ret = GetValueByFile(db, key, keyGet, value, count);
    if (ret < 0) {
        DBM_INFO("Get: get value from key path file fail.");
        return DBM_ERROR;
    }
    *realValueLen = ret;
    return DBM_OK;
}

int DBM_Get(KVStoreHandle db, const char* key, void* value, unsigned int count, unsigned int* realValueLen)
{
    if (db == NULL) {
        return DBM_INVALID_ARGS;
    }
    /* lock the KVDB */
    DB_LOCK(db);
    int ret = Get(db, key, value, count, realValueLen);
    DB_UNLOCK(db);
    return ret;
}

static int SetValueToFile(DBHandle db, const char* key, const char* buf, unsigned int len)
{
    char* keyPath = (char *)malloc(MAX_KEY_PATH + 1);
    if (keyPath == NULL) {
        return DBM_ERROR;
    }
    if (sprintf_s(keyPath, MAX_KEY_PATH + 1, ItemPathFormat(db), db->dirPath, key) < 0) {
        free(keyPath);
        return DBM_ERROR;
    }

    int fd = UtilsFileOpen(keyPath, O_RDWR_FS | O_CREAT_FS | O_TRUNC_FS, 0);
    free(keyPath);
    if (fd < 0) {
        DBM_INFO("SetValueToFile: open key path file fail. fd[%d]", fd);
        return DBM_ERROR;
    }
    int ret = UtilsFileWrite(fd, buf, len);
    UtilsFileClose(fd);
    fd = -1;
    if (ret < 0) {
        DBM_INFO("SetValueToFile: write buff to file fail.");
        return DBM_ERROR;
    }
    return DBM_OK;
}

static int BackupItem(DBHandle db, const KeyItem* item)
{
    if (!item->flag.isValid) {
        return DBM_OK;
    }

    char bakFile[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(bakFile, MAX_KEY_PATH + 1, "%s_dbm_kv", item->key) < 0) {
        return DBM_ERROR;
    }

    if (!IsNewItem(db, bakFile) && DeleteValueFromFile(db, bakFile) != DBM_OK) {
        DBM_INFO("BackupItem: delete old backup data fail.");
        return DBM_ERROR;
    }

    if (CopyValueToFile(db, item->key, bakFile) != DBM_OK) {
        DBM_INFO("BackupItem: copy to backup data fail. cur db sum[%d]", db->kvItemSum);
        DeleteValueFromFile(db, bakFile);
        return DBM_ERROR;
    }

    char itemFlagByte[1] = {1};
    // offset 2 pos is backup flag
    int ret = FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index) + 2, SEEK_SET_FS, itemFlagByte, 1);
    if (ret < 0) {
        DBM_INFO("BackupItem: set backup valid flag to file fail.");
        DeleteValueFromFile(db, bakFile);
        return DBM_ERROR;
    }
    return DBM_OK;
}

static int InitValue(int index, const void* value, unsigned int len, char* valueContent, unsigned int contentLen)
{
    if (valueContent == NULL || (contentLen < len + KV_SUM_INDEX)) {
        return -1;
    }

    if (sprintf_s(valueContent, KV_SUM_INDEX, "%d", index) < 0) {
        return -1;
    }

    errno_t ret = memcpy_s(valueContent + KV_SUM_INDEX, len, value, len);
    if (ret != EOK) {
        return -1;
    }

    return len + KV_SUM_INDEX;
}

static int InsertKV(DBHandle db, const KeyItem* item, const void* value, unsigned int len)
{
    char* valueContent = (char *)malloc(len + KV_SUM_BLOCK_SIZE); // Avoid stack overflows
    if (valueContent == NULL) {
        return DBM_ERROR;
    }
    (void)memset_s(valueContent, len + KV_SUM_BLOCK_SIZE, 0, len + KV_SUM_BLOCK_SIZE);

    int contentLen = InitValue(item->index, value, len, valueContent, len + KV_SUM_BLOCK_SIZE);
    if (contentLen < 0) {
        free(valueContent);
        return DBM_ERROR;
    }

    int ret = SetValueToFile(db, item->key, valueContent, contentLen);
    free(valueContent);
    if (ret != DBM_OK) {
        DelItem(db, item);
    }

    char itemData[1] = {1};
    int pos = FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index) + 1, SEEK_SET_FS, itemData, 1);
    if (pos < 0) {
        DBM_INFO("InsertKV: write flag to item fail.");
        DelItem(db, item);
        return DBM_ERROR;
    }
    return ret;
}

static int UpdateKV(DBHandle db, const KeyItem* item, const void* value, unsigned int len)
{
    int ret = BackupItem(db, item);
    if (ret != DBM_OK) {
        DBM_INFO("UpdateKV backup cur data fail.");
        return DBM_ERROR;
    }

    char* valueContent = (char *)malloc(len + KV_SUM_BLOCK_SIZE);
    if (valueContent == NULL) {
        return DBM_ERROR;
    }

    (void)memset_s(valueContent, len +KV_SUM_BLOCK_SIZE, 0, len + KV_SUM_BLOCK_SIZE);

    int contentLen = InitValue(item->index, value, len, valueContent, len + KV_SUM_BLOCK_SIZE);
    if (contentLen < 0) {
        free(valueContent);
        DBM_INFO("UpdateKV init value fail.");
        return DBM_ERROR;
    }

    ret = SetValueToFile(db, item->key, valueContent, contentLen);
    free(valueContent);
    if (ret != DBM_OK) {
        DBM_INFO("UpdateKV write value to file fail.");
        RecoverItem(db, item);
        return DBM_ERROR;
    }

    char itemData[1] = {0};
    // offset 2 pos is backup flag
    int pos = FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index) + 2, SEEK_SET_FS, itemData, 1);
    if (pos < 0) {
        DBM_INFO("UpdateKV set bak file is invalid flag fail.");
        return DBM_ERROR;
    }
    char bakFile[MAX_KEY_PATH + 1] = {0};
    if (sprintf_s(bakFile, MAX_KEY_PATH + 1, "%s_dbm_kv", item->key) < 0) {
        return DBM_ERROR;
    }

    if (!IsNewItem(db, bakFile) && DeleteValueFromFile(db, bakFile) != DBM_OK) {
        DBM_INFO("BackupItem: delete old backup data fail.");
        return DBM_ERROR;
    }
    DBM_DEBUG("UpdateKV finish!");
    return DBM_OK;
}

static int WriteNewItemToSumFile(DBHandle db, const KeyItem* item)
{
    char emptyBuf[2 * KV_SUM_BLOCK_SIZE] = {0}; // item flag and len 2 block size
    int ret = FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index), SEEK_SET_FS,
        emptyBuf, 2 * KV_SUM_BLOCK_SIZE);
    if (ret < 0) {
        DBM_INFO("WriteNewItemToSumFile: add new item flag fail.");
        return DBM_ERROR;
    }

    int strKeyOffset = GetKeyItemOffset(item->index) + 2 * KV_SUM_BLOCK_SIZE;
    ret = FileWriteCursor(db->sumFileFd, strKeyOffset, SEEK_SET_FS, item->key, MAX_KEY_LEN);
    if (ret < 0) {
        DBM_INFO("WriteNewItemToSumFile: add new item key fail.");
        return DBM_ERROR;
    }

    ret = FileWriteCursor(db->sumFileFd, 0, SEEK_CUR_FS, KV_MAGIC, KV_SUM_BLOCK_SIZE);
    if (ret < 0) {
        DBM_INFO("WriteNewItemToSumFile: add new item magic fail.");
        return DBM_ERROR;
    }
    return DBM_OK;
}

static int AddNewDataItem(DBHandle db, KeyItem* item)
{
    int itemIndex = -1;
    boolean isNewItem = TRUE;
    for (int index = 0; index < db->kvItemSum + 1; ++index) {
        char tmpKey[MAX_KEY_LEN + 1] = {0};
        if (itemIndex == -1 && IsDataItemMatched(db, tmpKey, index)) {
            itemIndex = index;
        }

        if (IsDataItemMatched(db, item->key, index)) { // find in sum file
            itemIndex = index;
            isNewItem = FALSE;
            break;
        }
    }

    if (isNewItem) {
        db->kvItemSum++;
    }

    item->index = (itemIndex == -1) ? (db->kvItemSum - 1) : itemIndex;
    if (item->index < 0) {
        DBM_INFO("AddNewDataItem: item index calcute err [%d][%d].", db->kvItemSum, item->index);
        return DBM_ERROR;
    }
    return WriteNewItemToSumFile(db, item);
}

static int FindDataItem(DBHandle db, KeyItem* item, boolean kvExisted)
{
    item->index = 0;
    if (kvExisted) {
        char* keyPath = (char *)malloc(MAX_KEY_PATH + 1);
        if (keyPath == NULL) {
            return DBM_ERROR;
        }
        if (sprintf_s(keyPath, MAX_KEY_PATH + 1, ItemPathFormat(db), db->dirPath, item->key) < 0) {
            free(keyPath);
            return DBM_ERROR;
        }

        int fd = UtilsFileOpen(keyPath, O_RDONLY_FS, 0);
        free(keyPath);
        if (fd < 0) {
            DBM_INFO("FindDataItem: open key path fail.");
            return DBM_ERROR;
        }

        char index[KV_SUM_BLOCK_SIZE] = {0};
        int ret = UtilsFileRead(fd, index, KV_SUM_BLOCK_SIZE);
        UtilsFileClose(fd);
        fd = -1;
        item->index = (ret < 0) ? 0 : atoi(index);
        if (IsDataItemMatched(db, item->key, item->index)) { // no need read flag, it has been checkpoint
            DBM_INFO("find data in sum file, index[%d]", item->index);
            return DBM_OK;
        }
    }
    return AddNewDataItem(db, item);
}

static int PrePut(DBHandle db, KeyItem* item, boolean* newItem)
{
    int ret = FindDataItem(db, item, !*newItem);
    if (ret != DBM_OK) {
        DBM_INFO("Put: find data item fail.");
        return DBM_ERROR;
    }

    ret = LoadDataItem(db, item->index, item);
    if (ret != DBM_OK) {
        DBM_INFO("Put: load data item fail.");
        return DBM_ERROR;
    }

    if (!*newItem) { // new item not need clean
        ret = CheckPointItem(db, item);
        if (ret != DBM_OK) {
            DBM_INFO("Put: load data item fail ret[%d].", ret);
            return DBM_ERROR;
        }
        // for last time key open but set flag fail or set delete flag but not delete the file
        if (db->sumFlag.isInvalid || !item->flag.isValid) {
            ret = WriteNewItemToSumFile(db, item);
            if (ret != DBM_OK) {
                return ret;
            }
            *newItem = TRUE; // cur item had been deleted
        }
    }
    return DBM_OK;
}

static int Put(KVStoreHandle db, const char* key, const void* value, unsigned int len)
{
    if (!IsValidKey(key) || (value == NULL) || (len > MAX_VALUE_LEN) || len <= 0 || db == NULL) {
        return DBM_INVALID_ARGS;
    }

    if (!db->initOK || !IsStrSame(db->magic, KV_MAGIC_SIZE + 1, KV_MAGIC)) {
        return DBM_INVALID_ARGS;
    }

    boolean newItem = IsNewItem(db, key);
    if ((db->kvItemSum >= DBM_MAX_KV_SUM) && newItem) {
        DBM_INFO("Put: over max limits.");
        return DBM_OVER_MAX_LIMITS;
    }

    KeyItem* item = (KeyItem *)malloc(sizeof(KeyItem));
    if (item == NULL) {
        return DBM_ERROR;
    }

    (void)memset_s(item, sizeof(KeyItem), 0, sizeof(KeyItem));
    if (strcpy_s(item->key, MAX_KEY_LEN + 1, key) != EOK) {
        free(item);
        return DBM_ERROR;
    }

    item->len = strlen(key);

    int ret = PrePut(db, item, &newItem);
    if (ret != DBM_OK) {
        free(item);
        DBM_INFO("Put: load data item fail.");
        return DBM_ERROR;
    }

    if (newItem) {
        ret = InsertKV(db, item, value, len);
    } else {
        ret = UpdateKV(db, item, value, len);
    }

    DBM_INFO("PUT index[%d] finish err[%d].", item->index, ret);
    free(item);
    return ret;
}

int DBM_Put(KVStoreHandle db, const char* key, const void* value, unsigned int len)
{
    if (db == NULL) {
        return DBM_INVALID_ARGS;
    }
    /* lock the KVDB */
    DB_LOCK(db);
    int ret = Put(db, key, value, len);
    DB_UNLOCK(db);
    return ret;
}

static int ExeDelete(KVStoreHandle db, const KeyItem* item, boolean newItem)
{
    char itemData[KV_SUM_DATA_ITEM_SIZE] = {0};
    int ret = FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index) + 1, SEEK_SET_FS, itemData, 1);
    db->kvItemSum--;
    if (ret < 0) {
        DBM_INFO("Delete: set data item delete flag fail. item[%s] index[%d]", item->key, item->index);
        return DBM_ERROR;
    }

    if (!newItem && DeleteValueFromFile(db, item->key) != DBM_OK) {
        return DBM_ERROR;
    }

    char bakKey[MAX_FILE_PATH] = {0};
    if (sprintf_s(bakKey, MAX_FILE_PATH, "%s_dbm_kv", item->key) < 0) {
        return DBM_ERROR;
    }

    if (!IsNewItem(db, bakKey)) {
        (void)DeleteValueFromFile(db, bakKey);
    }
    (void)FileWriteCursor(db->sumFileFd, GetKeyItemOffset(item->index), SEEK_SET_FS,
        itemData, KV_SUM_DATA_ITEM_SIZE - KV_MAGIC_SIZE);
    return DBM_OK;
}

static int Delete(KVStoreHandle db, const char* key)
{
    if (!IsValidKey(key) || db == NULL) {
        return DBM_INVALID_ARGS;
    }

    if (!db->initOK || !IsStrSame(db->magic, KV_MAGIC_SIZE + 1, KV_MAGIC)) {
        return DBM_INVALID_ARGS;
    }

    boolean newItem = IsNewItem(db, key);
    if (newItem) {
        return DBM_OK;
    }

    KeyItem* item = (KeyItem *)malloc(sizeof(KeyItem));
    if (item == NULL) {
        return DBM_ERROR;
    }
    (void)memset_s(item, sizeof(KeyItem), 0, sizeof(KeyItem));
    if (strcpy_s(item->key, MAX_KEY_LEN + 1, key) != EOK) {
        free(item);
        return DBM_ERROR;
    }
    item->len = strlen(key);

    int ret = FindDataItem(db, item, !newItem);
    if (ret != DBM_OK) {
        DBM_INFO("Delete: find data item fail. item[%s]", item->key);
        free(item);
        return DBM_ERROR;
    }

    ret = ExeDelete(db, item, newItem);
    free(item);
    return ret;
}

int DBM_Delete(KVStoreHandle db, const char* key)
{
    if (db == NULL) {
        return DBM_INVALID_ARGS;
    }
    /* lock the KVDB */
    DB_LOCK(db);
    int ret = Delete(db, key);
    DB_UNLOCK(db);
    DBM_DEBUG("Delete: key[%s] ret[%d]", key, ret);
    return ret;
}

static int CloseKVStore(KVStoreHandle db)
{
    if (!db->initOK || !IsStrSame(db->magic, KV_MAGIC_SIZE + 1, KV_MAGIC)) {
        return DBM_INVALID_ARGS;
    }

    int ret = SetSumIndexToFile(db, db->kvItemSum);
    if (ret != DBM_OK) {
        DBM_INFO("Write sum index to file fail."); // also need free resource
    }

    ret = UtilsFileClose(db->sumFileFd);
    if (ret < 0) {
        return DBM_ERROR;
    }
    db->sumFileFd = -1;
    return DBM_OK;
}

int DBM_CloseKVStore(KVStoreHandle db)
{
    if (db == NULL) {
        return DBM_OK;
    }

    /* lock the KVDB */
    DB_LOCK(db);
    int ret = CloseKVStore(db);
    DB_UNLOCK(db);

    ReleaseKVDBHandle(db);
    DBM_DEBUG("FInish Get KV store ret[%d].", ret);
    return ret;
}

static int DelSumFile(DBHandle db)
{
    char* sumFilePath = (char *)malloc(MAX_KEY_PATH + 1);
    if (sumFilePath == NULL) {
        return DBM_ERROR;
    }

    (void)memset_s(sumFilePath, MAX_FILE_PATH + 1, 0, MAX_FILE_PATH + 1);
    int ret = GetSumFilePath(db, sumFilePath, MAX_FILE_PATH + 1);
    if (ret != DBM_OK) {
        free(sumFilePath);
        return DBM_ERROR;
    }

    ret = UtilsFileClose(db->sumFileFd);
    if (ret < 0) {
        free(sumFilePath);
        return DBM_ERROR;
    }
    db->sumFileFd = -1;

    ret = UtilsFileDelete(sumFilePath);
    free(sumFilePath);
    if (ret < 0) {
        return DBM_ERROR;
    }
    return DBM_OK;
}

static int RemoveKVStoreFile(DBHandle db, int sumFileLen)
{
    char flag[KV_SUM_BLOCK_SIZE + 1] = {0};
    (void)memset_s(flag, KV_SUM_BLOCK_SIZE, 1, KV_SUM_BLOCK_SIZE);
    int ret = FileWriteCursor(db->sumFileFd, KV_SUM_INDEX + KV_SUM_BLOCK_SIZE, SEEK_SET_FS, flag, KV_SUM_BLOCK_SIZE);
    if (ret < 0) { // ret is fd offset
        DBM_INFO("RemoveKVStoreFile: write sum file delete all flag fail.");
        return DBM_ERROR;
    }

    if (sumFileLen > 0) {
        ret = LoadSumFileHeader(db);
        if (ret != DBM_OK) {
            DBM_INFO("RemoveKVStoreFile: Load sum file header flag fail.");
            return DBM_ERROR;
        }

        ret = VacuumStore(db);
        if (ret != DBM_OK) {
            DBM_INFO("RemoveKVStoreFile: vacuum database fail.");
            return DBM_ERROR;
        }
    }
    return DelSumFile(db);
}

int DBM_DeleteKVStore(const char* storeFullPath)
{
    DBM_INFO("Delete kv store start.");
    if (!IsValidPath(storeFullPath)) {
        DBM_INFO("Get KVStore invalid path.");
        return DBM_INVALID_ARGS;
    }

    DBHandle db = GetKVDBHandle(storeFullPath);
    if (db == NULL) {
        return DBM_ERROR;
    }

    db->sumFileFd = OpenSumFile(db, O_RDWR_FS);
    if (db->sumFileFd < 0) {
        DBM_INFO("DeleteKVStore: open sum file fail.");
        ReleaseKVDBHandle(db);
        return DBM_NOT_FOUND;
    }

    int sumFileLen = UtilsFileSeek(db->sumFileFd, 0UL, SEEK_END_FS);
    if (sumFileLen < 0) {
        DBM_INFO("DeleteKVStore: Get sum file length fail.");
        ReleaseKVDBHandle(db);
        return DBM_ERROR;
    }

    // Is need this version or corrupt
    if ((sumFileLen < SUM_FILE_HEADER_SIZE) && (sumFileLen != 0) && (sumFileLen != KV_SUM_INDEX)) {
        DBM_INFO("sum file invalid length!");
        ReleaseKVDBHandle(db);
        return DBM_ERROR;
    }

    int ret = RemoveKVStoreFile(db, sumFileLen);
    ReleaseKVDBHandle(db);
    DBM_INFO("Delete kv store complete[%d].", ret);
    return ret;
}

void DBM_KVStoreControl(KVStoreHandle db, int cmd, void* arg)
{
    if (db == NULL) {
        return;
    }

    switch (cmd) {
        case DBM_CTRL_SET_LOCK:
            db->lock = (void (*)(KVStoreHandle db))arg;
            break;
        case DBM_CTRL_SET_UNLOCK:
            db->unlock = (void (*)(KVStoreHandle db))arg;
            break;
        default:
            break;
    }
}

int DBM_UserInit(void)
{
    return 0;
}
