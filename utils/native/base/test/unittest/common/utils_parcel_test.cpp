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

#include <gtest/gtest.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "directory_ex.h"
#include "parcel.h"
#include "refbase.h"
#include "securec.h"

using namespace testing::ext;
using namespace OHOS;
using namespace std;

const int MAX_PARCEL_SIZE = 1000;
char g_data[MAX_PARCEL_SIZE];
class UtilsParcelTest : public testing::Test {
public:
    static void TearDownTestCase(void);
};

void UtilsParcelTest::TearDownTestCase(void)
{
    for (int i = 0; i < MAX_PARCEL_SIZE; i++) {
        g_data[i] = 0;
    }
}

/*-------------------------------base data------------------------------------*/

struct TestData {
    bool booltest;
    int8_t int8test;
    int16_t int16test;
    int32_t int32test;
    uint8_t uint8test;
    uint16_t uint16test;
    uint32_t uint32test;
};

void WriteTestData(Parcel &parcel, const struct TestData &data)
{
    bool result = false;

    result = parcel.WriteBool(data.booltest);
    EXPECT_EQ(result, true);

    result = parcel.WriteInt8(data.int8test);
    EXPECT_EQ(result, true);

    result = parcel.WriteInt16(data.int16test);
    EXPECT_EQ(result, true);

    result = parcel.WriteInt32(data.int32test);
    EXPECT_EQ(result, true);

    result = parcel.WriteUint8(data.uint8test);
    EXPECT_EQ(result, true);

    result = parcel.WriteUint16(data.uint16test);
    EXPECT_EQ(result, true);

    result = parcel.WriteUint32(data.uint32test);
    EXPECT_EQ(result, true);
}

/**
 * Here to simulate the scenario of ipc sending data, the buffer will be released when the Parcel object is destructed.
*/
bool SendData(void *&buffer, size_t size, const uint8_t *data)
{
    if (size <= 0) {
        return false;
    }
    buffer = malloc(size);
    if (buffer == nullptr) {
        return false;
    }
    if (memcpy_s(buffer, size, data, size) != EOK) {
        return false;
    }
    return true;
}

/**
 * @tc.name: test_parcel_WriteAndRead_001
 * @tc.desc: test parcel primary type read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_001, TestSize.Level0)
{
    Parcel parcel(nullptr);
    struct TestData data = { true, -0x34, 0x5634, -0x12345678, 0x34, 0x5634, 0x12345678 };
    WriteTestData(parcel, data);

    bool readbool = parcel.ReadBool();
    EXPECT_EQ(readbool, data.booltest);

    int8_t readint8 = parcel.ReadInt8();
    EXPECT_EQ(readint8, data.int8test);

    int16_t readint16 = parcel.ReadInt16();
    EXPECT_EQ(readint16, data.int16test);

    int32_t readint32 = parcel.ReadInt32();
    EXPECT_EQ(readint32, data.int32test);

    uint8_t readuint8 = parcel.ReadUint8();
    EXPECT_EQ(readuint8, data.uint8test);

    uint16_t readuint16 = parcel.ReadUint16();
    EXPECT_EQ(readuint16, data.uint16test);

    uint32_t readuint32 = parcel.ReadUint32();
    EXPECT_EQ(readuint32, data.uint32test);
}

/**
 * @tc.name: test_parcel_WriteAndRead_002
 * @tc.desc: test parcel primary type read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_002, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    Parcel parcel2(nullptr);
    struct TestData data = { true, -0x34, 0x5634, -0x12345678, 0x34, 0x5634, 0x12345678 };
    WriteTestData(parcel1, data);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    bool result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());
    EXPECT_EQ(result, true);

    bool readbool = parcel2.ReadBool();
    EXPECT_EQ(readbool, data.booltest);

    int8_t readint8 = parcel2.ReadInt8();
    EXPECT_EQ(readint8, data.int8test);

    int16_t readint16 = parcel2.ReadInt16();
    EXPECT_EQ(readint16, data.int16test);

    int32_t readint32 = parcel2.ReadInt32();
    EXPECT_EQ(readint32, data.int32test);

    uint8_t readuint8 = parcel2.ReadUint8();
    EXPECT_EQ(readuint8, data.uint8test);

    uint16_t readuint16 = parcel2.ReadUint16();
    EXPECT_EQ(readuint16, data.uint16test);

    uint32_t readuint32 = parcel2.ReadUint32();
    EXPECT_EQ(readuint32, data.uint32test);
}

/**
 * @tc.name: test_parcel_WriteAndRead_003
 * @tc.desc: test parcel primary type read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_003, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    Parcel parcel2(nullptr);
    struct TestData data = { true, -0x34, 0x5634, -0x12345678, 0x34, 0x5634, 0x12345678 };
    WriteTestData(parcel1, data);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    bool result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());
    EXPECT_EQ(result, true);

    bool boolVal = true;
    result = parcel2.ReadBool(boolVal);
    EXPECT_EQ(result, true);
    EXPECT_EQ(boolVal, data.booltest);

    int8_t int8Val;
    result = parcel2.ReadInt8(int8Val);
    EXPECT_EQ(result, true);
    EXPECT_EQ(int8Val, data.int8test);

    int16_t int16Val;
    result = parcel2.ReadInt16(int16Val);
    EXPECT_EQ(result, true);
    EXPECT_EQ(int16Val, data.int16test);

    int32_t int32Val;
    result = parcel2.ReadInt32(int32Val);
    EXPECT_EQ(result, true);
    EXPECT_EQ(int32Val, data.int32test);

    uint8_t uint8Val;
    result = parcel2.ReadUint8(uint8Val);
    EXPECT_EQ(result, true);
    EXPECT_EQ(uint8Val, data.uint8test);

    uint16_t uint16Val;
    result = parcel2.ReadUint16(uint16Val);
    EXPECT_EQ(result, true);
    EXPECT_EQ(uint16Val, data.uint16test);

    uint32_t uint32Val;
    result = parcel2.ReadUint32(uint32Val);
    EXPECT_EQ(result, true);
    EXPECT_EQ(uint32Val, data.uint32test);
}

/**
 * @tc.name: test_parcel_WriteAndRead_004
 * @tc.desc: test parcel primary type read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_004, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    bool result;

    int64_t int64test = -0x1234567887654321;
    result = parcel1.WriteInt64(int64test);
    EXPECT_EQ(result, true);

    uint64_t uint64test = 0x1234567887654321;
    result = parcel1.WriteUint64(uint64test);
    EXPECT_EQ(result, true);

    int64_t readint64 = parcel1.ReadInt64();
    EXPECT_EQ(readint64, int64test);

    uint64_t readuint64 = parcel1.ReadUint64();
    EXPECT_EQ(readuint64, uint64test);

    Parcel parcel2(nullptr);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());

    readint64 = parcel2.ReadInt64();
    EXPECT_EQ(readint64, int64test);

    readuint64 = parcel2.ReadUint64();
    EXPECT_EQ(readuint64, uint64test);
}

/**
 * @tc.name: test_parcel_WriteAndRead_String_001
 * @tc.desc: test parcel string read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_String_001, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    bool result;

    string strwrite = "test";
    result = parcel1.WriteString(strwrite);
    EXPECT_EQ(result, true);

    string strwrite1 =
        "test for write string padded**********************************************************##################";
    result = parcel1.WriteString(strwrite1);
    EXPECT_EQ(result, true);

    string strwrite2 =
        "test for write string padded**********************************************************##################";
    result = parcel1.WriteString(strwrite2);
    EXPECT_EQ(result, true);

    string strread = parcel1.ReadString();
    string strread1 = parcel1.ReadString();
    string strread2 = parcel1.ReadString();
    EXPECT_EQ(0, strcmp(strread.c_str(), strwrite.c_str()));
    EXPECT_EQ(0, strcmp(strread1.c_str(), strwrite1.c_str()));
    EXPECT_EQ(0, strcmp(strread2.c_str(), strwrite2.c_str()));

    Parcel parcel2(nullptr);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());

    strread = parcel2.ReadString();
    strread1 = parcel2.ReadString();
    strread2 = parcel2.ReadString();
    EXPECT_EQ(0, strcmp(strread.c_str(), strwrite.c_str()));
    EXPECT_EQ(0, strcmp(strread1.c_str(), strwrite1.c_str()));
    EXPECT_EQ(0, strcmp(strread2.c_str(), strwrite2.c_str()));
}

/**
 * @tc.name: test_parcel_WriteAndRead_String_002
 * @tc.desc: test parcel string read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_String_002, TestSize.Level0)
{
    Parcel parcel1(nullptr);

    u16string str16write = u"12345";
    bool result = parcel1.WriteString16(str16write);
    EXPECT_EQ(result, true);

    u16string str16write2 = u"12345 test for write16string padded*********";
    result = parcel1.WriteString16(str16write2);
    EXPECT_EQ(result, true);

    u16string str16read = parcel1.ReadString16();
    u16string str16read2 = parcel1.ReadString16();
    EXPECT_EQ(0, str16read.compare(str16write));
    EXPECT_EQ(0, str16read2.compare(str16write2));

    Parcel parcel2(nullptr);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());

    str16read = parcel2.ReadString16();
    str16read2 = parcel2.ReadString16();
    EXPECT_EQ(0, str16read.compare(str16write));
    EXPECT_EQ(0, str16read2.compare(str16write2));
}

/**
 * @tc.name: test_parcel_WriteAndRead_String_003
 * @tc.desc: test parcel CString read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_String_003, TestSize.Level0)
{
    Parcel parcel(nullptr);
    string test1 = "12345";
    string test2 = "23456";
    string test3 = "34567";
    string test4 = "45678";
    parcel.WriteCString(test1.c_str());
    parcel.WriteCString(test2.c_str());
    parcel.WriteCString(test3.c_str());
    parcel.WriteCString(test4.c_str());

    EXPECT_EQ(0, strcmp(test1.c_str(), parcel.ReadCString()));
    EXPECT_EQ(0, strcmp(test2.c_str(), parcel.ReadCString()));
    EXPECT_EQ(0, strcmp(test3.c_str(), parcel.ReadCString()));
    EXPECT_EQ(0, strcmp(test4.c_str(), parcel.ReadCString()));
}

/**
 * @tc.name: test_parcel_WriteAndRead_String004
 * @tc.desc: test parcel CString read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_String004, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    bool result = false;

    // write from Java, read from C++
    result = parcel1.WriteString16WithLength(nullptr, 0);
    EXPECT_EQ(result, true);

    u16string str16write = u"12345";
    char16_t *value1 = str16write.data();
    result = parcel1.WriteString16WithLength(value1, str16write.length());
    EXPECT_EQ(result, true);

    u16string str16write2 = u"12345 test for write16string padded*********";
    char16_t *value2 = str16write2.data();
    result = parcel1.WriteString16WithLength(value2, str16write2.length());
    EXPECT_EQ(result, true);

    u16string str16readNull = parcel1.ReadString16();
    u16string str16read1 = parcel1.ReadString16();
    u16string str16read2 = parcel1.ReadString16();
    EXPECT_EQ(0, str16readNull.compare(std::u16string()));
    EXPECT_EQ(0, str16read1.compare(str16write));
    EXPECT_EQ(0, str16read2.compare(str16write2));

    // write from C++, read from Java
    result = parcel1.WriteString16(str16write);
    EXPECT_EQ(result, true);

    result = parcel1.WriteString16(str16write2);
    EXPECT_EQ(result, true);

    int32_t readLength1 = 0;
    u16string str16read3 = parcel1.ReadString16WithLength(readLength1);
    EXPECT_EQ(readLength1, static_cast<int32_t>(str16write.length()));

    int32_t readLength2 = 0;
    u16string str16read4 = parcel1.ReadString16WithLength(readLength2);
    EXPECT_EQ(readLength2, static_cast<int32_t>(str16write2.length()));

    EXPECT_EQ(0, str16read3.compare(str16write));
    EXPECT_EQ(0, str16read4.compare(str16write2));
}

/**
 * @tc.name: test_parcel_WriteAndRead_Float_001
 * @tc.desc: test parcel float types read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_Float_001, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    bool result;

    float floatwrite = 12.345678f;
    result = parcel1.WriteFloat(floatwrite);
    EXPECT_EQ(result, true);

    double doublewrite = 1345.7653;
    result = parcel1.WriteDouble(doublewrite);
    EXPECT_EQ(result, true);

    float floatread;
    result = parcel1.ReadFloat(floatread);
    EXPECT_EQ(result, true);
    EXPECT_EQ(floatwrite, floatread);

    double doubleread;
    doubleread = parcel1.ReadDouble();
    EXPECT_EQ(doublewrite, doubleread);

    Parcel parcel2(nullptr);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());
    result = parcel2.ReadFloat(floatread);
    EXPECT_EQ(result, true);
    EXPECT_EQ(floatwrite, floatread);

    doubleread = parcel2.ReadDouble();
    EXPECT_EQ(doublewrite, doubleread);
}

/**
 * @tc.name: test_parcel_WriteAndRead_String_005
 * @tc.desc: test parcel String type read write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndRead_String_005, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    bool result;

    string strwrite = "test";
    result = parcel1.WriteString(strwrite);
    EXPECT_EQ(result, true);

    string strwrite1 =
        "test for write string padded**********************************************************##################";
    result = parcel1.WriteString(strwrite1);
    EXPECT_EQ(result, true);

    string strwrite2 =
        "test for write string padded**********************************************************##################";
    result = parcel1.WriteString(strwrite2);
    EXPECT_EQ(result, true);

    string strread;
    string strread1;
    string strread2;
    result = parcel1.ReadString(strread);
    EXPECT_EQ(result, true);
    result = parcel1.ReadString(strread1);
    EXPECT_EQ(result, true);
    result = parcel1.ReadString(strread2);
    EXPECT_EQ(result, true);
    EXPECT_EQ(0, strcmp(strread.c_str(), strwrite.c_str()));
    EXPECT_EQ(0, strcmp(strread1.c_str(), strwrite1.c_str()));
    EXPECT_EQ(0, strcmp(strread2.c_str(), strwrite2.c_str()));

    Parcel parcel2(nullptr);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());
    EXPECT_EQ(result, true);

    result = parcel2.ReadString(strread);
    EXPECT_EQ(result, true);
    result = parcel2.ReadString(strread1);
    EXPECT_EQ(result, true);
    result = parcel2.ReadString(strread2);
    EXPECT_EQ(result, true);
    EXPECT_EQ(0, strcmp(strread.c_str(), strwrite.c_str()));
    EXPECT_EQ(0, strcmp(strread1.c_str(), strwrite1.c_str()));
    EXPECT_EQ(0, strcmp(strread2.c_str(), strwrite2.c_str()));
}

struct Padded {
    char title;
    int32_t handle;
    uint64_t cookie;
};

struct Unpadded {
    char tip;
};

void ValidatePadded(const struct Padded &left, const struct Padded &right)
{
    EXPECT_EQ(left.title, right.title);
    EXPECT_EQ(left.handle, right.handle);
    EXPECT_EQ(left.cookie, right.cookie);
}

void ValidateUnpadded(const struct Unpadded &left, const struct Unpadded &right)
{
    EXPECT_EQ(left.tip, right.tip);
}

/**
 * @tc.name: test_parcel_Data_Structure_001
 * @tc.desc: test parcel struct data related function.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_Data_Structure_001, TestSize.Level0)
{
    Parcel parcel(nullptr);
    bool result;

    const struct Padded pad = { 'p', 0x34567890, -0x2345678998765432 };
    const struct Unpadded unpad = { 'u' };

    result = parcel.WriteBuffer(static_cast<const void *>(&pad), sizeof(struct Padded));
    EXPECT_EQ(true, result);
    const struct Padded *padRead = reinterpret_cast<const struct Padded *>(parcel.ReadBuffer(sizeof(struct Padded)));
    ValidatePadded(*padRead, pad);
    EXPECT_EQ(parcel.GetWritePosition(), parcel.GetReadPosition());

    result = parcel.WriteBuffer(static_cast<const void *>(&unpad), sizeof(struct Unpadded));
    const struct Unpadded *unpadRead =
        reinterpret_cast<const struct Unpadded *>(parcel.ReadBuffer(sizeof(struct Unpadded)));
    ValidateUnpadded(*unpadRead, unpad);
    EXPECT_NE(parcel.GetWritePosition(), parcel.GetReadPosition());

    parcel.RewindRead(0);
    parcel.RewindWrite(0);
    EXPECT_EQ(parcel.GetWritePosition(), parcel.GetReadPosition());

    result = parcel.WriteUnpadBuffer(static_cast<const void *>(&pad), sizeof(struct Padded));
    EXPECT_EQ(true, result);
    const struct Padded *padReadNew =
        reinterpret_cast<const struct Padded *>(parcel.ReadUnpadBuffer(sizeof(struct Padded)));
    ValidatePadded(*padReadNew, pad);
    EXPECT_EQ(parcel.GetWritePosition(), parcel.GetReadPosition());

    result = parcel.WriteUnpadBuffer(static_cast<const void *>(&unpad), sizeof(struct Unpadded));
    EXPECT_EQ(true, result);
    const struct Unpadded *unpadReadNew =
        reinterpret_cast<const struct Unpadded *>(parcel.ReadUnpadBuffer(sizeof(struct Unpadded)));
    ValidateUnpadded(*unpadReadNew, unpad);
    EXPECT_EQ(parcel.GetWritePosition(), parcel.GetReadPosition());
}

struct VectorTestData {
    vector<bool> booltest = { false, false, true, false, true };
    vector<int8_t> int8test = { 0x01, 0x10, -0x20, 0x30, 0x40 };
    vector<int16_t> int16test = { 0x1234, -0x2345, 0x3456, -0x4567, 0x5678 };
    vector<int32_t> int32test = { 0x12345678, -0x23456789, 0x34567890, -0x45678901 };
    vector<int64_t> int64test = { 0x1234567887654321, -0x2345678998765432 };
    vector<uint8_t> uint8test = { 0x01, 0x10, 0x20, 0x30, 0x40 };
    vector<uint16_t> uint16test = { 0x1234, 0x2345, 0x3456, 0x4567, 0x5678 };
    vector<uint32_t> uint32test = { 0x12345678, 0x23456789, 0x34567890, 0x45678901 };
    vector<uint64_t> uint64test = { 0x1234567887654321, 0x2345678998765432 };
};

void WriteVectorTestData(Parcel &parcel, const VectorTestData &data)
{
    bool result = parcel.WriteBoolVector(data.booltest);
    EXPECT_EQ(result, true);
    result = parcel.WriteInt8Vector(data.int8test);
    EXPECT_EQ(result, true);
    result = parcel.WriteInt16Vector(data.int16test);
    EXPECT_EQ(result, true);
    result = parcel.WriteInt32Vector(data.int32test);
    EXPECT_EQ(result, true);
    result = parcel.WriteInt64Vector(data.int64test);
    EXPECT_EQ(result, true);
    result = parcel.WriteUInt8Vector(data.uint8test);
    EXPECT_EQ(result, true);
    result = parcel.WriteUInt16Vector(data.uint16test);
    EXPECT_EQ(result, true);
    result = parcel.WriteUInt32Vector(data.uint32test);
    EXPECT_EQ(result, true);
    result = parcel.WriteUInt64Vector(data.uint64test);
    EXPECT_EQ(result, true);
}

void ReadVectorTestData(Parcel &parcel, const VectorTestData &data)
{
    vector<bool> boolread;
    vector<int8_t> int8read;
    vector<int16_t> int16read;
    vector<int32_t> int32read;
    vector<int64_t> int64read;

    vector<uint8_t> uint8read;
    vector<uint16_t> uint16read;
    vector<uint32_t> uint32read;
    vector<uint64_t> uint64read;

    bool result = parcel.ReadBoolVector(&boolread);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.booltest.size(); i++) {
        EXPECT_EQ(data.booltest[i], boolread[i]);
    }

    result = parcel.ReadInt8Vector(&int8read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.int8test.size(); i++) {
        EXPECT_EQ(data.int8test[i], int8read[i]);
    }

    result = parcel.ReadInt16Vector(&int16read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.int16test.size(); i++) {
        EXPECT_EQ(data.int16test[i], int16read[i]);
    }

    result = parcel.ReadInt32Vector(&int32read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.int32test.size(); i++) {
        EXPECT_EQ(data.int32test[i], int32read[i]);
    }

    result = parcel.ReadInt64Vector(&int64read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.int64test.size(); i++) {
        EXPECT_EQ(data.int64test[i], int64read[i]);
    }

    result = parcel.ReadUInt8Vector(&uint8read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.uint8test.size(); i++) {
        EXPECT_EQ(data.uint8test[i], uint8read[i]);
    }

    result = parcel.ReadUInt16Vector(&uint16read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.uint16test.size(); i++) {
        EXPECT_EQ(data.uint16test[i], uint16read[i]);
    }

    result = parcel.ReadUInt32Vector(&uint32read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.uint32test.size(); i++) {
        EXPECT_EQ(data.uint32test[i], uint32read[i]);
    }

    result = parcel.ReadUInt64Vector(&uint64read);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < data.uint64test.size(); i++) {
        EXPECT_EQ(data.uint64test[i], uint64read[i]);
    }
}

/**
 * @tc.name: test_parcel_WriteAndReadVector_001
 * @tc.desc: test vector parcel read and write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndReadVector_001, TestSize.Level0)
{
    Parcel parcel(nullptr);
    struct VectorTestData data;

    WriteVectorTestData(parcel, data);
    ReadVectorTestData(parcel, data);
}

/**
 * @tc.name: test_parcel_WriteAndReadVector_002
 * @tc.desc: test vector parcel read and write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndReadVector_002, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    struct VectorTestData data;
    WriteVectorTestData(parcel1, data);

    Parcel parcel2(nullptr);

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    bool result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());
    EXPECT_EQ(result, true);
    ReadVectorTestData(parcel2, data);
}

/**
 * @tc.name: test_parcel_WriteAndReadVector_003
 * @tc.desc: test vector parcel read and write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndReadVector_003, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    Parcel parcel2(nullptr);
    bool result;
    vector<string> stringtest{ "test", "test for", "test for write", "test for write vector" };
    vector<u16string> string16test{ u"test", u"test for", u"test for write", u"test for write vector" };

    result = parcel1.WriteStringVector(stringtest);
    EXPECT_EQ(result, true);
    result = parcel1.WriteString16Vector(string16test);
    EXPECT_EQ(result, true);

    vector<string> stringread;
    result = parcel1.ReadStringVector(&stringread);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < stringtest.size(); i++) {
        EXPECT_EQ(stringtest[i], stringread[i]);
    }

    vector<u16string> u16stringread;
    result = parcel1.ReadString16Vector(&u16stringread);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < string16test.size(); i++) {
        EXPECT_EQ(0, string16test[i].compare(u16stringread[i]));
    }

    void *buffer = nullptr;
    size_t size = parcel1.GetDataSize();
    if (!SendData(buffer, size, reinterpret_cast<const uint8_t *>(parcel1.GetData()))) {
        ASSERT_FALSE(false);
    }

    result = parcel2.ParseFrom(reinterpret_cast<uintptr_t>(buffer), parcel1.GetDataSize());
    result = parcel2.ReadStringVector(&stringread);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < stringtest.size(); i++) {
        EXPECT_EQ(stringtest[i], stringread[i]);
    }

    result = parcel2.ReadString16Vector(&u16stringread);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < string16test.size(); i++) {
        EXPECT_EQ(0, string16test[i].compare(u16stringread[i]));
    }
}

/**
 * @tc.name: test_parcel_WriteAndReadVector_004
 * @tc.desc: test vector parcel read and write.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_WriteAndReadVector_004, TestSize.Level0)
{
    Parcel parcel1(nullptr);
    Parcel parcel2(nullptr);
    bool result;
    vector<float> floattest{ 11221.132313, 11221.45678 };
    vector<double> doubletest{ 1122.132313, 1122.45678 };

    result = parcel1.WriteFloatVector(floattest);
    EXPECT_EQ(result, true);
    result = parcel1.WriteDoubleVector(doubletest);
    EXPECT_EQ(result, true);

    vector<float> floatread;
    vector<double> doubleread;

    result = parcel1.ReadFloatVector(&floatread);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < floattest.size(); i++) {
        EXPECT_EQ(floattest[i], floatread[i]);
    }

    result = parcel1.ReadDoubleVector(&doubleread);
    EXPECT_EQ(result, true);
    for (size_t i = 0; i < doubletest.size(); i++) {
        EXPECT_EQ(doubletest[i], doubleread[i]);
    }
}

class TestParcelable : public virtual Parcelable {
public:
    TestParcelable() = default;
    ~TestParcelable() = default;

    bool Marshalling(Parcel &parcel) const override;
    static TestParcelable *Unmarshalling(Parcel &parcel);

public:
    int32_t int32Write_ = -0x12345678;
    int32_t int32Read_;
};

bool TestParcelable::Marshalling(Parcel &parcel) const
{
    bool result = parcel.WriteInt32(this->int32Write_);
    return result;
}

TestParcelable *TestParcelable::Unmarshalling(Parcel &parcel)
{
    auto *read = new TestParcelable();
    read->int32Read_ = parcel.ReadInt32();
    return read;
}

/**
 * @tc.name: test_parcel_parcelable_001
 * @tc.desc: test parcel read and write parcelable obj.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_parcelable_001, TestSize.Level0)
{
    Parcel parcel(nullptr);
    sptr<TestParcelable> parcelableWrite = new TestParcelable();
    bool result = false;

    result = parcel.WriteParcelable(parcelableWrite);
    EXPECT_EQ(true, result);
    EXPECT_EQ(parcel.GetWritePosition(), parcel.GetDataSize());

    sptr<TestParcelable> parcelableRead = parcel.ReadParcelable<TestParcelable>();
    EXPECT_EQ(parcelableWrite->int32Write_, parcelableRead->int32Read_);
    EXPECT_EQ(parcel.GetReadPosition(), parcel.GetDataSize());
}

/**
 * @tc.name: test_parcel_parcelable_002
 * @tc.desc: test parcel read and write parcelable obj.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_parcelable_002, TestSize.Level0)
{
    Parcel parcel(nullptr);

    bool result = parcel.WriteParcelable(nullptr);
    EXPECT_EQ(true, result);

    sptr<TestParcelable> parcelableRead = parcel.ReadParcelable<TestParcelable>();
    EXPECT_EQ(nullptr, parcelableRead);
}

/**
 * @tc.name: test_parcel_parcelable_003
 * @tc.desc: test parcel read and write parcelable obj.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_parcel_parcelable_003, TestSize.Level0)
{
    Parcel parcel(nullptr);
    sptr<TestParcelable> parcelableWriteNull;
    bool result = parcel.WriteStrongParcelable(parcelableWriteNull);
    EXPECT_EQ(true, result);

    sptr<TestParcelable> parcelableWrite = new TestParcelable();

    bool test = parcelableWrite->TestBehavior(Parcelable::BehaviorFlag::HOLD_OBJECT);
    EXPECT_EQ(false, test);
    test = parcelableWrite->TestBehavior(Parcelable::BehaviorFlag::IPC);
    EXPECT_EQ(false, test);
    test = parcelableWrite->TestBehavior(Parcelable::BehaviorFlag::RPC);
    EXPECT_EQ(false, test);

    result = parcel.WriteStrongParcelable(parcelableWrite);
    EXPECT_EQ(true, result);

    sptr<TestParcelable> parcelableReadNull = parcel.ReadParcelable<TestParcelable>();
    EXPECT_EQ(nullptr, parcelableReadNull);

    sptr<TestParcelable> parcelableRead = parcel.ReadParcelable<TestParcelable>();
    EXPECT_EQ(parcelableWrite->int32Write_, parcelableRead->int32Read_);

    test = parcelableWrite->TestBehavior(Parcelable::BehaviorFlag::HOLD_OBJECT);
    EXPECT_EQ(true, test);
    test = parcelableWrite->TestBehavior(Parcelable::BehaviorFlag::IPC);
    EXPECT_EQ(false, test);
    test = parcelableWrite->TestBehavior(Parcelable::BehaviorFlag::RPC);
    EXPECT_EQ(false, test);

    parcelableWrite->ClearBehavior(Parcelable::BehaviorFlag::HOLD_OBJECT);
    test = parcelableWrite->TestBehavior(Parcelable::BehaviorFlag::HOLD_OBJECT);
    EXPECT_EQ(false, test);
}

/**
 * @tc.name: test_SetMaxCapacity_001
 * @tc.desc: test parcel capacity function.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_SetMaxCapacity_001, TestSize.Level0)
{
    Parcel parcel(nullptr);
    char test[48] = {0};
    bool ret;
    ret = parcel.WriteBuffer(test, 48);
    EXPECT_EQ(true, ret);
    // because default maxCap is 200 * 1024, so reset it more
    parcel.SetMaxCapacity(201 * 1024);
    // test write data over max capacity: 205780 + 48 > 201 * 1024
    char test2[205780] = {0};
    ret = parcel.WriteBuffer(test2, 205780);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: test_SetMaxCapacity_002
 * @tc.desc: test parcel capacity function.
 * @tc.type: FUNC
 */
HWTEST_F(UtilsParcelTest, test_SetMaxCapacity_002, TestSize.Level0)
{
    Parcel parcel(nullptr);
    char test[48] = {0};
    bool ret;
    ret = parcel.WriteInt32(5767168);
    EXPECT_EQ(true, ret);
    ret = parcel.WriteBuffer(test, 48);
    EXPECT_EQ(true, ret);
    vector<std::u16string> val;
    ret = parcel.ReadString16Vector(&val);
    EXPECT_EQ(false, ret);
}
