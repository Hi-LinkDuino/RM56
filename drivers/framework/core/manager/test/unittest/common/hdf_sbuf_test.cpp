/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <cstring>
#include <gtest/gtest.h>
#include <hdf_sbuf.h>
using namespace testing::ext;

static const int DEFAULT_SBUF_SIZE = 256;
static const int DEFAULT_LOOP_COUNT = 500;
static const int DEFAULT_BIG_LOOP_COUNT = 1000;
static const int DATA_MOD = 26;

class HdfSBufTest : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override
    {
        dataTable.clear();
    }

    void GenDataTable()
    {
        dataTable[UINT8] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint8_t)));
        dataTable[UINT16] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint16_t)));
        dataTable[UINT32] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint32_t)));
        dataTable[UINT64] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint64_t)));
        dataTable[INT8] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint8_t)));
        dataTable[INT16] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint16_t)));
        dataTable[INT32] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint32_t)));
        dataTable[INT64] = std::unique_ptr<SbufData>(new SbufData(sizeof(uint64_t)));
        dataTable[BUFFER] = std::unique_ptr<SbufData>(new SbufData((rd() % UINT8_MAX) + 1));
        dataTable[STRING] = std::unique_ptr<SbufData>(new SbufData((rd() % UINT8_MAX) + 1));
        dataTable[STRING]->data[dataTable[STRING]->size - 1] = '\0';
    }

    void GenDataSequence(int seqSize)
    {
        dataSequence.clear();
        std::random_device rnd;
        for (int i = 0; i < seqSize; ++i) {
            dataSequence.push_back(rnd() % TYPE_MAX);
        }
    }

    bool PushDataSequence(HdfSBuf *sbuf)
    {
        for (auto type : dataSequence) {
            bool ret = false;
            switch (type) {
                case UINT8:
                    ret = HdfSbufWriteUint8(sbuf, *(uint8_t *)dataTable[type]->data);
                    break;
                case UINT16:
                    ret = HdfSbufWriteUint16(sbuf, *(uint16_t *)dataTable[type]->data);
                    break;
                case UINT32:
                    ret = HdfSbufWriteUint32(sbuf, *(uint32_t *)dataTable[type]->data);
                    break;
                case UINT64:
                    ret = HdfSbufWriteUint64(sbuf, *(uint64_t *)dataTable[type]->data);
                    break;
                case INT8:
                    ret = HdfSbufWriteUint8(sbuf, *(int8_t *)dataTable[type]->data);
                    break;
                case INT16:
                    ret = HdfSbufWriteUint16(sbuf, *(uint16_t *)dataTable[type]->data);
                    break;
                case INT32:
                    ret = HdfSbufWriteUint32(sbuf, *(uint32_t *)dataTable[type]->data);
                    break;
                case INT64:
                    ret = HdfSbufWriteUint64(sbuf, *(uint64_t *)dataTable[type]->data);
                    break;
                case BUFFER:
                    ret = HdfSbufWriteBuffer(sbuf, (uint8_t *)dataTable[type]->data, dataTable[type]->size);
                    break;
                case STRING:
                    ret = HdfSbufWriteString(sbuf, (char *)dataTable[type]->data);
                    break;
                default:
                    break;
            }
            if (!ret) {
                std::cout << "write type " << type << " fail" << std::endl;
                return ret;
            }
        }
        return true;
    }

    static bool DataCompare(const void *s, const void *t, uint32_t size)
    {
        return memcmp(s, t, size) == 0;
    }

    bool PullDataSequence(HdfSBuf *sbuf)
    {
        for (auto type : dataSequence) {
            bool ret = false;
            switch (type) {
                case UINT8: {
                    uint8_t val = 0;
                    ret = HdfSbufReadUint8(sbuf, &val);
                    EXPECT_EQ(true, DataCompare(&val, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case UINT16: {
                    uint16_t val = 0;
                    ret = HdfSbufReadUint16(sbuf, &val);
                    EXPECT_EQ(true, DataCompare(&val, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case UINT32: {
                    uint32_t val = 0;
                    ret = HdfSbufReadUint32(sbuf, &val);
                    EXPECT_EQ(true, DataCompare(&val, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case UINT64: {
                    uint64_t val = 0;
                    ret = HdfSbufReadUint64(sbuf, &val);
                    EXPECT_EQ(true, DataCompare(&val, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case INT8: {
                    int8_t val = 0;
                    ret = HdfSbufReadInt8(sbuf, &val);
                    EXPECT_EQ(true, ret);
                    ret = DataCompare(&val, dataTable[type]->data, dataTable[type]->size);
                    EXPECT_EQ(true, ret);
                    break;
                }
                case INT16: {
                    int16_t val = 0;
                    ret = HdfSbufReadInt16(sbuf, &val);
                    EXPECT_EQ(true, DataCompare(&val, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case INT32: {
                    int32_t val = 0;
                    ret = HdfSbufReadInt32(sbuf, &val);
                    EXPECT_EQ(true, DataCompare(&val, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case INT64: {
                    int64_t val = 0;
                    ret = HdfSbufReadInt64(sbuf, &val);
                    EXPECT_EQ(true, DataCompare(&val, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case BUFFER: {
                    const char *buf = nullptr;
                    uint32_t readSize = 0;
                    ret = HdfSbufReadBuffer(sbuf, reinterpret_cast<const void **>(&buf), &readSize);
                    EXPECT_EQ(true, DataCompare(buf, dataTable[type]->data, dataTable[type]->size));
                    EXPECT_EQ(true, ret);
                    break;
                }
                case STRING: {
                    auto val = HdfSbufReadString(sbuf);
                    EXPECT_NE(nullptr, val);
                    if (val == nullptr) {
                        return false;
                    }
                    EXPECT_EQ(strlen(val), dataTable[type]->size - 1);
                    EXPECT_EQ(true, DataCompare(val, dataTable[type]->data, dataTable[type]->size));
                    ret = true;
                    break;
                }
                default:
                    break;
            }
            if (!ret) {
                std::cout << "write type " << type << " fail" << std::endl;
                return ret;
            }
        }
        return true;
    }

    enum SbufDataType {
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        INT8,
        INT16,
        INT32,
        INT64,
        BUFFER,
        STRING,
        TYPE_MAX,
    };

    class SbufData {
    public:
        explicit SbufData(uint32_t dataSize) : data(nullptr), size(dataSize)
        {
            std::random_device rnd;
            data = new uint8_t[dataSize];
            for (uint32_t i = 0; i < dataSize; ++i) {
                data[i] = 'A' + rnd() % DATA_MOD;
            }
        }

        ~SbufData()
        {
            if (data != nullptr) {
                delete[] data;
            }
        }
        uint8_t *data;
        uint32_t size;
    };

    std::map<int, std::unique_ptr<SbufData>> dataTable;
    std::vector<int> dataSequence;
    std::random_device rd;
};

/**
  * @tc.name: SbufTestObtain001
  * @tc.desc: obtain sbuf
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestObtain001, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtain(DEFAULT_SBUF_SIZE);
    ASSERT_NE(sBuf, nullptr);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestWriteUint64002
  * @tc.desc: write uint64 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestWriteUint64002, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    auto ret = HdfSbufWriteInt64(sBuf, INT64_MAX);
    ASSERT_EQ(ret, true);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestWriteUint64Loop003
  * @tc.desc: write uint64 data sequence
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestWriteUint64Loop003, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    for (int i = 0; i < DEFAULT_BIG_LOOP_COUNT; ++i) {
        auto ret = HdfSbufWriteInt64(sBuf, INT64_MAX);
        ASSERT_EQ(ret, true);
    }
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestReadUint64Loop004
  * @tc.desc: read after write uint64 data sequence
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestReadUint64Loop004, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    int loop = DEFAULT_BIG_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        auto ret = HdfSbufWriteInt64(sBuf, INT64_MAX);
        ASSERT_EQ(ret, true);
    }
    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);
    uint64_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        auto ret = HdfSbufReadUint64(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, (uint64_t)INT64_MAX);
    }
    auto ret = HdfSbufReadUint64(readBuf, &val);
    ASSERT_EQ(ret, false);
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestInt8005
  * @tc.desc: read write int8 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestInt8005, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteInt8(sBuf, INT8_MAX);
        ASSERT_EQ(ret, true);
    }
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_EQ(dataSize, loop * sizeof(uint32_t));

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    int8_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        ret = HdfSbufReadInt8(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, INT8_MAX);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestInt16006
  * @tc.desc: read write int16 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestInt16006, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteInt16(sBuf, INT16_MAX);
        ASSERT_EQ(ret, true);
    }
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_EQ(dataSize, loop * sizeof(uint32_t));

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    int16_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        ret = HdfSbufReadInt16(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, INT16_MAX);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestInt32007
  * @tc.desc: read write int32 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestInt32007, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteInt32(sBuf, INT32_MAX);
        ASSERT_EQ(ret, true);
    }
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_EQ(dataSize, loop * sizeof(uint32_t));

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    int32_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        ret = HdfSbufReadInt32(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, INT32_MAX);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestInt64008
  * @tc.desc: read write int64 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestInt64008, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteInt64(sBuf, INT64_MAX);
        ASSERT_EQ(ret, true);
    }
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_EQ(dataSize, loop * sizeof(uint64_t));

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    int64_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        ret = HdfSbufReadInt64(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, INT64_MAX);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestUInt32009
  * @tc.desc: read write uint32 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestUInt32009, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteUint32(sBuf, UINT32_MAX);
        ASSERT_EQ(ret, true);
    }
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_EQ(dataSize, loop * sizeof(uint32_t));

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    uint32_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        ret = HdfSbufReadUint32(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, UINT32_MAX);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestUInt16010
  * @tc.desc: read write uint16 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestUInt16010, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteUint16(sBuf, UINT16_MAX);
        ASSERT_EQ(ret, true);
    }
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_EQ(dataSize, loop * sizeof(uint32_t));

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    uint16_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        ret = HdfSbufReadUint16(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, UINT16_MAX);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestUInt8011
  * @tc.desc: read write uint8 data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestUInt8011, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteUint8(sBuf, UINT8_MAX);
        ASSERT_EQ(ret, true);
    }
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_EQ(dataSize, loop * sizeof(uint32_t));

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    uint8_t val = 0;
    for (int j = 0; j < loop; ++j) {
        val = 0;
        ret = HdfSbufReadUint8(readBuf, &val);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(val, UINT8_MAX);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestString012
  * @tc.desc: read write string data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestString012, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = DEFAULT_LOOP_COUNT;
    std::string str("test");
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteString(sBuf, str.data());
        ASSERT_EQ(ret, true);
    }

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    for (int j = 0; j < loop; ++j) {
        const char *readStr = HdfSbufReadString(readBuf);
        ASSERT_EQ(std::string(readStr), str);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestNullString013
  * @tc.desc: read write bullptr string data test
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestNullString013, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    auto ret = HdfSbufWriteString(sBuf, nullptr);
    ASSERT_EQ(true, ret);
    ret = HdfSbufWriteInt32(sBuf, INT32_MIN);
    ASSERT_EQ(true, ret);
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_NE((size_t)0, dataSize);
    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);
    auto val = HdfSbufReadString(readBuf);
    ASSERT_EQ(nullptr, val);
    int32_t intVal = 0;
    ret = HdfSbufReadInt32(readBuf, &intVal);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(INT32_MIN, intVal);
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestBuffer014
  * @tc.desc: read write buffer test
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestBuffer014, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    bool ret;
    int loop = 1;
    std::string str("test");
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteBuffer(sBuf, str.data(), str.size() + 1);
        ASSERT_EQ(ret, true);
    }

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    for (int j = 0; j < loop; ++j) {
        const char *data = nullptr;
        uint32_t readSize = 0;
        ret = HdfSbufReadBuffer(readBuf, (const void **)(&data), &readSize);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(readSize, str.size() + 1);
        ASSERT_EQ(std::string(data), str);
    }
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestNullBuffer015
  * @tc.desc: read write nullptr buffer data
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestNullBuffer015, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);
    auto ret = HdfSbufWriteBuffer(sBuf, nullptr, 0);
    ASSERT_EQ(true, ret);
    ret = HdfSbufWriteInt16(sBuf, INT16_MIN);
    ASSERT_EQ(true, ret);
    size_t dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_NE((size_t)0, dataSize);
    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);
    const uint8_t *buffVal = nullptr;
    uint32_t buffLen = 0;
    ret = HdfSbufReadBuffer(readBuf, (const void **)(&buffVal), &buffLen);
    ASSERT_EQ(true, ret);
    ASSERT_EQ((uint32_t)0, buffLen);
    dataSize = HdfSbufGetDataSize(sBuf);
    ASSERT_NE((size_t)0, dataSize);
    int16_t intVal = 0;
    ret = HdfSbufReadInt16(readBuf, &intVal);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(INT16_MIN, intVal);
    HdfSbufRecycle(sBuf);
    HdfSbufRecycle(readBuf);
}

/**
  * @tc.name: SbufTestRandomDataSeq016
  * @tc.desc: read write random data test
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestRandomDataSeq016, TestSize.Level0)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);

    GenDataTable();
    GenDataSequence(100);
    bool ret = PushDataSequence(sBuf);
    ASSERT_EQ(true, ret);

    HdfSBuf *readBuf = HdfSbufBind((uintptr_t)HdfSbufGetData(sBuf), HdfSbufGetDataSize(sBuf));
    ASSERT_NE(readBuf, nullptr);

    ret = PullDataSequence(readBuf);
    ASSERT_EQ(true, ret);
    HdfSbufRecycle(sBuf);
    HdfSbufRecycle(readBuf);
}

/**
  * @tc.name: SbufTestRandomRWDataSeq017
  * @tc.desc: read write random data sequence test
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestRandomRWDataSeq017, TestSize.Level0)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);

    GenDataTable();
    GenDataSequence(100);
    bool ret = PushDataSequence(sBuf);
    ASSERT_EQ(true, ret);

    ret = PullDataSequence(sBuf);
    ASSERT_EQ(true, ret);

    ret = PushDataSequence(sBuf);
    ASSERT_EQ(true, ret);

    ret = PullDataSequence(sBuf);
    ASSERT_EQ(true, ret);

    uint8_t val = 0;
    ret = HdfSbufReadUint8(sBuf, &val);
    ASSERT_EQ(false, ret);
    ASSERT_EQ(0, val);

    HdfSbufRecycle(sBuf);
}

/**
  * @tc.name: SbufTestSbufMove018
  * @tc.desc: move sbuf test
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestSbufMove018, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);

    GenDataTable();
    GenDataSequence(100);

    bool ret;
    int loop = 1;
    std::string str("test");
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteBuffer(sBuf, str.data(), str.size() + 1);
        ASSERT_EQ(ret, true);
    }

    HdfSBuf *readBuf = HdfSbufMove(sBuf);
    ASSERT_NE(readBuf, nullptr);

    for (int j = 0; j < loop; ++j) {
        const char *data = nullptr;
        uint32_t readSize = 0;
        ret = HdfSbufReadBuffer(readBuf, (const void **)(&data), &readSize);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(readSize, str.size() + 1);
        ASSERT_EQ(std::string(data), str);
    }
    HdfSbufRecycle(sBuf);
    HdfSbufRecycle(readBuf);
}

/**
  * @tc.name: SbufTestSbufMoveHalf019
  * @tc.desc: move sbuf after read test
  * @tc.type: FUNC
  * @tc.require: AR000F869B
  */
HWTEST_F(HdfSBufTest, SbufTestSbufMoveHalf019, TestSize.Level1)
{
    HdfSBuf *sBuf = HdfSbufObtainDefaultSize();
    ASSERT_NE(sBuf, nullptr);

    bool ret;
    int loop = 1;
    std::string str("test");
    for (int i = 0; i < loop; ++i) {
        ret = HdfSbufWriteBuffer(sBuf, str.data(), str.size() + 1);
        ASSERT_EQ(ret, true);
    }
    for (int j = 0; j < loop/2; ++j) {
        const char *data = nullptr;
        uint32_t readSize = 0;
        ret = HdfSbufReadBuffer(sBuf, (const void **)(&data), &readSize);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(readSize, str.size() + 1);
        ASSERT_EQ(std::string(data), str);
    }

    HdfSBuf *readBuf = HdfSbufMove(sBuf);
    ASSERT_NE(readBuf, nullptr);

    for (int j = 0; j < loop; ++j) {
        const char *data = nullptr;
        uint32_t readSize = 0;
        ret = HdfSbufReadBuffer(readBuf, (const void **)(&data), &readSize);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(readSize, str.size() + 1);
        ASSERT_EQ(std::string(data), str);
    }

    const char *data = nullptr;
    uint32_t readSize = 0;
    ret = HdfSbufReadBuffer(readBuf, (const void **)(&data), &readSize);
    ASSERT_EQ(ret, false);

    HdfSbufRecycle(sBuf);
    HdfSbufRecycle(readBuf);
}
