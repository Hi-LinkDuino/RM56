/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef HAP_SIGNING_BLOCK_UTILS_H
#define HAP_SIGNING_BLOCK_UTILS_H

#include <string>
#include <vector>
#include "openssl/evp.h"

#include "common/data_source.h"
#include "common/export_define.h"
#include "common/hap_byte_buffer.h"
#include "common/random_access_file.h"
#include "util/hap_verify_openssl_utils.h"
#include "util/signature_info.h"

namespace OHOS {
namespace Security {
namespace Verify {
constexpr int ZIP_CHUNK_DIGEST_PRIFIX_LEN = 5;

enum HapBlobType {
    HAP_SIGN_BLOB = 0x20000000,
    PROOF_ROTATION_BLOB = 0x20000001,
    PROFILE_BLOB = 0x20000002,
    PROPERTY_BLOB = 0x20000003,
};

struct HapSignBlockHead {
    int version;
    int blockCount;
    long long hapSignBlockSize;
    long long hapSignBlockMagicLo;
    long long hapSignBlockMagicHi;
};

struct HapSubSignBlockHead {
    unsigned int type;
    unsigned int length;
    unsigned int offset;
};

class HapSigningBlockUtils {
public:
    DLL_EXPORT static bool FindHapSignature(RandomAccessFile& hapFile, SignatureInfo& signInfo);
    DLL_EXPORT static bool GetOptionalBlockIndex(std::vector<OptionalBlock>& optionBlocks, int type, int& index);
    DLL_EXPORT static bool VerifyHapIntegrity(Pkcs7Context& digestInfo, RandomAccessFile& hapFile,
        SignatureInfo& signInfo);

private:
    DLL_EXPORT static const long long HAP_SIG_BLOCK_MAGIC_HIGH_OLD;
    DLL_EXPORT static const long long HAP_SIG_BLOCK_MAGIC_LOW_OLD;
    DLL_EXPORT static const long long HAP_SIG_BLOCK_MAGIC_HIGH;
    DLL_EXPORT static const long long HAP_SIG_BLOCK_MAGIC_LOW;
    DLL_EXPORT static const int ZIP_HEAD_OF_SIGNING_BLOCK_LENGTH;
    DLL_EXPORT static const int ZIP_EOCD_SEGMENT_FLAG;
    static const long long CHUNK_SIZE;
    static const int HAP_SIG_BLOCK_MIN_SIZE;
    static const int ZIP_EOCD_SEG_MIN_SIZE;
    static const int ZIP_EOCD_COMMENT_LENGTH_OFFSET;
    static const int ZIP_CD_OFFSET_IN_EOCD;
    static const int ZIP_CD_SIZE_OFFSET_IN_EOCD;
    static const int ZIP_BLOCKS_NUM_NEED_DIGEST;
    static const char ZIP_FIRST_LEVEL_CHUNK_PREFIX;
    static const char ZIP_SECOND_LEVEL_CHUNK_PREFIX;
    /* the specifications of hap sign block */
    static constexpr long long MAX_HAP_SIGN_BLOCK_SIZE = 1024 * 1024 * 10LL; // 10MB
    static constexpr int MAX_BLOCK_COUNT = 10;
    static constexpr int VERSION_FOR_NEW_MAGIC_NUM = 3;

private:
    DLL_EXPORT static bool FindEocdInHap(RandomAccessFile& hapFile, std::pair<HapByteBuffer, long long>& eocd);
    DLL_EXPORT static bool FindEocdInHap(RandomAccessFile& hapFile, unsigned short maxCommentSize,
        std::pair<HapByteBuffer, long long>& eocd);
    DLL_EXPORT static bool FindEocdInSearchBuffer(HapByteBuffer& zipContents, int& offset);
    DLL_EXPORT static bool GetCentralDirectoryOffset(HapByteBuffer& eocd, long long eocdOffset,
        long long& centralDirectoryOffset);
    static bool FindHapSigningBlock(RandomAccessFile& hapFile, long long centralDirOffset,
        SignatureInfo& signInfo);
    static bool FindHapSubSigningBlock(RandomAccessFile& hapFile, int blockCount,
        long long blockArrayLen, long long hapSignBlockOffset, SignatureInfo& signInfo);
    DLL_EXPORT static bool ClassifyHapSubSigningBlock(SignatureInfo& signInfo,
        const HapByteBuffer& subBlock, unsigned int type);
    DLL_EXPORT static bool SetUnsignedInt32(HapByteBuffer& buffer, int offset, long long value);
    DLL_EXPORT static bool ComputeDigestsWithOptionalBlock(const DigestParameter& digestParam,
        const std::vector<OptionalBlock>& optionalBlocks, const HapByteBuffer& chunkDigest,
        HapByteBuffer& finalDigest);
    static bool ComputeDigestsForEachChunk(const DigestParameter& digestParam, DataSource* contents[],
        int len, HapByteBuffer& result);
    static int GetChunkCount(long long inputSize, long long chunkSize);
    static bool InitDigestPrefix(const DigestParameter& digestParam,
        unsigned char (&chunkContentPrefix)[ZIP_CHUNK_DIGEST_PRIFIX_LEN], int chunkLen);
    DLL_EXPORT static DigestParameter GetDigestParameter(int nId);
    DLL_EXPORT static bool GetSumOfChunkDigestLen(DataSource* contents[], int len, int chunkDigestLen,
        int& chunkCount, int& sumOfChunkDigestLen);
    static bool ParseSignBlockHead(HapSignBlockHead& hapSignBlockHead, HapByteBuffer& hapBlockHead);
    static inline bool CheckSignBlockHead(const HapSignBlockHead& hapSignBlockHead);
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_SIGNING_BLOCK_UTILS_H
