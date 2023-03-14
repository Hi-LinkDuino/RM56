/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <securec.h>
#include <sstream> // 使用stringstream

#include "cstring"
#include "distributed_kv_data_manager.h"
#include "types.h"
#include "process_msg.h"

using namespace OHOS::DistributedKv;
using namespace std;

const double DEFDELTA = 0.00001;
const int MAX_DATA_LENGTH = 1024;
const int NUMTHREE = 3;
const int STR_VALUE = 23;
const int RESULR_TWO = 2;
const int RESULT_OK = 0;
const int RESULT_ERR = 1;
namespace DisKvTest {
DistributedKvDataManager manager;
std::shared_ptr<SingleKvStore> KvStorePtr;
Status statusGetKvStore;
Status statusCloseKvStore;
Status statusDeleteKvStore;
UserId userId;
AppId appId;
StoreId storeIdTest;
}; // namespace DisKvTest

void initKvstoreId(void)
{
    DisKvTest::KvStorePtr = nullptr; // declare kvstore instance.
    DisKvTest::statusGetKvStore = Status::ERROR;
    DisKvTest::statusCloseKvStore = Status::ERROR;
    DisKvTest::statusDeleteKvStore = Status::ERROR;

    DisKvTest::userId.userId = "account0";
    DisKvTest::appId.appId = "com.ohos.kvdatamanager3.test";
    DisKvTest::storeIdTest.storeId = "test3";

    // 1.创建数据库
    Options options { .createIfMissing = true,
        .encrypt = false, //   .persistant = true,
        .autoSync = false,
        .backup = false,
        .kvStoreType = KvStoreType::SINGLE_VERSION };

    DisKvTest::statusGetKvStore = DisKvTest::manager.GetSingleKvStore(
        options, { DisKvTest::appId }, { DisKvTest::storeIdTest }, DisKvTest::KvStorePtr);
}

char* getRealData(char* str, char* delims)
{
    if (str == nullptr || delims == nullptr) {
        return nullptr;
    }
    char* result = strtok(str, delims);
    char* second = nullptr;
    while (result != nullptr) {
        second = result;
        result = strtok(nullptr, delims);
    }
    return second;
}

void getParam(char* putData, char ret[][1024])
{
    char str[MAX_DATA_LENGTH] = { 0 };
    if (putData == nullptr) {
        return;
    }
    strcpy_s(str, strlen(putData) + 1, putData);
    char delims[2] = { ":" };
    char* result = strtok(str, delims);
    int i = 0;
    while (result != nullptr) {
        printf("result is \"%s\"\n", result);
        strcpy_s(ret[i], strlen(result) + 1, result);
        result = strtok(nullptr, delims);
        i++;
        if (i == NUMTHREE) {
            return;
        }
    }
    return;
}

int ProcessSoftBus(int code, char* recvData)
{
    return RESULT_OK;
}

int ProcessDataMgr(int code, char* recvData)
{
    LOG("ProcessDataMgr, begin");

    initKvstoreId();
    if (DisKvTest::KvStorePtr == nullptr) {
        std::cout << "ERR：DisKvTest::KvStorePtr == nullptr" << std::endl;
        return RESULT_ERR;
    }
    std::cout << "create status=" << static_cast<int>(DisKvTest::statusGetKvStore) << std::endl;
    if (Status::SUCCESS != DisKvTest::statusGetKvStore) {
        std::cout << "ERR：statusGetKvStore" << std::endl;
        return RESULT_ERR;
    }

    switch (code) {
        case CTRL_CODE_DATAMGR_GET_DATA:
            return processGetData(recvData);
        case CTRL_CODE_DATAMGR_GET_DATA_REPLY:
            return RESULT_OK;
        case CTRL_CODE_DATAMGR_CREATE_KV:
            return processCreateKv(recvData);
        case CTRL_CODE_DATAMGR_DELETE_KV:
            return processDeleteKv(recvData);
        case CTRL_CODE_DATAMGR_PUT_DATA:
            return processPutData(recvData);
        case CTRL_CODE_DATAMGR_DELETE_DATA:
            return processDeletetData(recvData);
        default:
            break;
    }
    return -1;
}

int processDeletetData(char* putData)
{
    LOG("LOGdisDataTest---processDeletetData,  begin");
    // 解析远端发来的数据 result[0]=code result[1]=key result[RESULR_TWO]=value
    char result[NUMTHREE][MAX_DATA_LENGTH] = { { 0 }, { 0 }, { 0 } };
    memset_s(result, NUMTHREE * MAX_DATA_LENGTH, 0, NUMTHREE * MAX_DATA_LENGTH);
    getParam(putData, result);
    for (int i = 0; i < NUMTHREE; i++) {
        LOG("LOGdisDataTest---processGetData %s", result[i]);
    }

    // put data删除数据
    Value valueInt;
    Key keyInt = result[1];
    Status status = DisKvTest::KvStorePtr->Delete(keyInt);
    if (Status::SUCCESS == status) {
        LOG("LOGdisDataTest---Delete:RESULT_OK");
        return RESULT_OK;
    }
    LOG("LOGdisDataTest---Delete:RESULT_ERR");
    return RESULT_ERR;
}

int processPutData(char* putData)
{
    LOG("LOGdisDataTest-processPutData,  begin");
    // 解析远端发来的数据 result[0]=code result[1]=key result[RESULR_TWO]=value
    char result[NUMTHREE][MAX_DATA_LENGTH] = { { 0 }, { 0 }, { 0 } };
    memset_s(result, NUMTHREE * MAX_DATA_LENGTH, 0, NUMTHREE * MAX_DATA_LENGTH);
    getParam(putData, result);
    for (int i = 0; i < NUMTHREE; i++) {
        LOG("LOGdisDataTest-processPutData %s", result[i]);
    }

    // put data修改数据
    Value valueInt;
    Key keyInt = result[1];
    if (strcmp(result[1], "math_score_int") == 0) {
        int numInt = atoi(result[RESULR_TWO]);
        valueInt = Value(TransferTypeToByteArray<int>(numInt));
    } else if (strcmp(result[1], "math_score_float") == 0) {
        float f2 = atof(result[RESULR_TWO]);
        valueInt = Value(TransferTypeToByteArray<float>(f2));
    } else if (strcmp(result[1], "math_score_double") == 0) {
        double f2 = atof(result[RESULR_TWO]);
        valueInt = Value(TransferTypeToByteArray<double>(f2));
    } else if (strcmp(result[1], "math_score_int64_t") == 0) {
        int64_t numInt = atoi(result[RESULR_TWO]);
        valueInt = Value(TransferTypeToByteArray<int64_t>(numInt));
    } else if (strcmp(result[1], "math_score_size_t") == 0) {
        size_t numSize = atoi(result[RESULR_TWO]);
        valueInt = Value(TransferTypeToByteArray<size_t>(numSize));
    }

    else if (strcmp(result[1], "math_score_string") == 0) {
        // result[RESULR_TWO]解析的不完整
        string ss;
        string ss2;
        ss = putData;
        ss2 = ss.substr(STR_VALUE);
        cout << "LOGdisDataTest2--ss = " << ss << endl;
        cout << "LOGdisDataTest2--ss2 = " << ss2 << endl;
        LOG("LOGdisDataTest2--ss2.c_str() =  %s", ss2.c_str());
        valueInt = Value(ss2);

    } else if (strcmp(result[1], "math_score_vector") == 0) {
        std::string str = result[RESULR_TWO];
        std::vector<uint8_t> vect;
        vect.assign(str.begin(), str.end());
        valueInt = Value(vect);
    }
    Status status = DisKvTest::KvStorePtr->Put(keyInt, valueInt);
    if (Status::SUCCESS == status) {
        LOG("LOGdisDataTest2--putData:RESULT_OK");
        return RESULT_OK;
    }
    LOG("LOGdisDataTest2--putData:RESULT_ERR");
    return RESULT_ERR;
}

int processGetData(char* putData)
{
    LOG("LOGdisDataTest-processGetData,  begin");

    // 解析远端发来的数据 result[0]=code result[1]=key result[RESULR_TWO]=value
    char result[NUMTHREE][MAX_DATA_LENGTH] = { { 0 }, { 0 }, { 0 } };
    memset_s(result, NUMTHREE * MAX_DATA_LENGTH, 0, NUMTHREE * MAX_DATA_LENGTH);
    getParam(putData, result);
    for (int i = 0; i < NUMTHREE; i++) {
        LOG("for result[i] %s", result[i]);
    }

    // 获取到本端数据
    Value valueRetInt;
    Key keyInt = result[1];
    Status status = DisKvTest::KvStorePtr->Get(keyInt, valueRetInt);
    if (Status::SUCCESS != status) {
        LOG("LOGdisDataTest--ERROR: Get(keyInt, valueRetInt)");
        return RESULT_ERR;
    }

    std::string ret = valueRetInt.ToString();
    LOG("LOGdisDataTest--result[RESULR_TWO]=  %s", result[RESULR_TWO]);
    LOG("LOGdisDataTest--ret.c_str() =  %s", ret.c_str());

    if (strcmp(result[1], "math_score_int") == 0) {
        LOG("LOGdisDataTest--math_score_int ");
        int iaaa = TransferByteArrayToType<int>(valueRetInt.Data());
        int i2 = atoi(result[RESULR_TWO]);
        LOG("LOGdisDataTest--iaaa=  %d", iaaa);
        LOG("LOGdisDataTest--i2 =  %d", i2);
        if (iaaa == i2) {
            return RESULT_OK;
        }

    } else if (strcmp(result[1], "math_score_float") == 0) {
        LOG("LOGdisDataTest--math_score_float ");
        float faaa = TransferByteArrayToType<float>(valueRetInt.Data());
        float f2 = atof(result[RESULR_TWO]);
        float fdelta = f2 - faaa;
        LOG("LOGdisDataTest--faaa=  %f", faaa);
        LOG("LOGdisDataTest--f2 =  %f", f2);
        LOG("LOGdisDataTest--fdelta =  %f", fdelta);
        if (std::abs(fdelta) <= DEFDELTA) {
            return RESULT_OK;
        }
    } else if (strcmp(result[1], "math_score_double") == 0) {
        LOG("LOGdisDataTest--math_score_double ");
        double daaa = TransferByteArrayToType<double>(valueRetInt.Data());
        double d2 = atof(result[RESULR_TWO]);
        double delta = d2 - daaa;
        LOG("LOGdisDataTest--daaa=  %f", daaa);
        LOG("LOGdisDataTest--d2 =  %f", d2);
        LOG("LOGdisDataTest--delta =  %f", delta);
        if (std::abs(delta) <= DEFDELTA) {
            return RESULT_OK;
        }
    } else if (strcmp(result[1], "math_score_int64_t") == 0) {
        LOG("LOGdisDataTest--math_score_int64_t ");
        int64_t iaaa_t = TransferByteArrayToType<int64_t>(valueRetInt.Data());
        int64_t i2_t = atoi(result[RESULR_TWO]);
        if (iaaa_t == i2_t) {
            return RESULT_OK;
        }
    } else if (strcmp(result[1], "math_score_size_t") == 0) {
        LOG("LOGdisDataTest--math_score_size_t ");
        size_t sizeaaa = TransferByteArrayToType<size_t>(valueRetInt.Data());
        size_t size2 = atoi(result[RESULR_TWO]);
        if (sizeaaa == size2) {
            return RESULT_OK;
        }
    } else if (strcmp(result[1], "math_score_string") == 0) {
        // result[RESULR_TWO]解析的不完整
        string ss;
        string ss2;
        ss = putData;
        ss2 = ss.substr(STR_VALUE);
        cout << "LOGdisDataTest--ss = " << ss << endl;
        cout << "LOGdisDataTest--ss2 = " << ss2 << endl;
        LOG("LOGdisDataTest--ss2.c_str() =  %s", ss2.c_str());
        LOG("LOGdisDataTest--ret.c_str() =  %s", ret.c_str());
        if (strcmp(ss2.c_str(), ret.c_str()) == 0) {
            return RESULT_OK;
        }
    } else if (strcmp(result[1], "math_score_vector") == 0) {
        LOG("LOGdisDataTest--result[RESULR_TWO]=  %s", result[RESULR_TWO]);
        LOG("LOGdisDataTest--ret.c_str() =  %s", ret.c_str());
        if (strcmp(result[RESULR_TWO], ret.c_str()) == 0) {
            return RESULT_OK;
        }
    }
    return RESULT_ERR;
}

int processCreateKv(char* putData)
{
    LOG("processCreateKv,  begin");
    std::cout << "create status=" << static_cast<int>(DisKvTest::statusGetKvStore) << std::endl;
    if (Status::SUCCESS == DisKvTest::statusGetKvStore) {
        std::cout << "SUCCESS：statusGetKvStore" << std::endl;
        return RESULT_OK;
    } else {
        std::cout << "ERR：statusGetKvStore" << std::endl;
        return RESULT_ERR;
    }
}

int processDeleteKv(char* putData)
{
    LOG("processDeleteKv,  begin");
    DisKvTest::statusCloseKvStore = DisKvTest::manager.CloseAllKvStore(DisKvTest::appId);
    DisKvTest::statusDeleteKvStore = DisKvTest::manager.DeleteAllKvStore(DisKvTest::appId);
    if ((Status::SUCCESS == DisKvTest::statusCloseKvStore) && (Status::SUCCESS == DisKvTest::statusDeleteKvStore)) {
        std::cout << "SUCCESS：statusDeleteKvStore" << std::endl;
        return RESULT_OK;
    } else {
        std::cout << "ERR：statusDeleteKvStore" << std::endl;
        return RESULT_ERR;
    }
}

int ProcessDP(int code, char* recvData)
{
    return RESULT_OK;
}

int ProcessDM(int code, char* recvData)
{
    return RESULT_OK;
}

int ProcessFileMgr(int code, char* recvData)
{
    return RESULT_OK;
}

int ProcessSecMgr(int code, char* recvData)
{
    return RESULT_OK;
}

int ProcessMediaMgr(int code, char* recvData)
{
    return RESULT_OK;
}
