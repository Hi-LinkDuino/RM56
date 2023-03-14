/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hks_hkdf_derive_part1_test.h"
#include "hks_hkdf_derive_test_common.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::HkdfDerive {
class HksHkdfDerivePart1Test : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksHkdfDerivePart1Test::SetUpTestCase(void)
{
}

void HksHkdfDerivePart1Test::TearDownTestCase(void)
{
}

void HksHkdfDerivePart1Test::SetUp()
{
}

void HksHkdfDerivePart1Test::TearDown()
{
}
static struct HksParam g_genParams001[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_128
    }
};
static struct HksParam g_hkdfParams001[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_hkdfFinishParams001[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest001"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest001"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_32
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};
static struct HksParam g_genParams002[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_128
    }
};
static struct HksParam g_hkdfParams002[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_48
    }
};
static struct HksParam g_hkdfFinishParams002[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest002"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest002"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_48
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_genParams003[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_128
    }
};
static struct HksParam g_hkdfParams003[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_64
    }
};
static struct HksParam g_hkdfFinishParams003[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest003"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest003"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_64
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_genParams004[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_192
    }
};
static struct HksParam g_hkdfParams004[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_hkdfFinishParams004[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest004"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest004"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_32
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};
static struct HksParam g_genParams005[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_192
    }
};
static struct HksParam g_hkdfParams005[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_48
    }
};
static struct HksParam g_hkdfFinishParams005[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest005"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest005"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_48
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_genParams006[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_192
    }
};
static struct HksParam g_hkdfParams006[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_64
    }
};
static struct HksParam g_hkdfFinishParams006[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest006"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest006"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_64
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_genParams007[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }
};
static struct HksParam g_hkdfParams007[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_hkdfFinishParams007[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest007"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest007"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_32
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};
static struct HksParam g_genParams008[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }
};
static struct HksParam g_hkdfParams008[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_48
    }
};
static struct HksParam g_hkdfFinishParams008[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest008"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest008"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_48
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_genParams009[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }
};
static struct HksParam g_hkdfParams009[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_64
    }
};
static struct HksParam g_hkdfFinishParams009[] = {
    {
        .tag =  HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksHKDFDeriveKeyAliasFinalTest009"),
            (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest009"
        }
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::HkdfDerive::DERIVE_KEY_SIZE_64
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};

/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive001
 * @tc.desc: alg-HKDF pur-Derive dig-SHA256 KEY_SIZE-128
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive001, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest001"), (uint8_t *)"HksHKDFDeriveKeyAliasTest001" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams001, sizeof(g_genParams001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams001, sizeof(g_hkdfParams001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams001, sizeof(g_hkdfFinishParams001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest001"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest001"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive002
 * @tc.desc: alg-HKDF pur-Derive dig-SHA384 KEY_SIZE-128
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive002, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest002"), (uint8_t *)"HksHKDFDeriveKeyAliasTest002" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams002, sizeof(g_genParams002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams002, sizeof(g_hkdfParams002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams002, sizeof(g_hkdfFinishParams002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest002"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest002"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive003
 * @tc.desc: alg-HKDF pur-Derive dig-SHA384 KEY_SIZE-192
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive003, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest003"), (uint8_t *)"HksHKDFDeriveKeyAliasTest003" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams003, sizeof(g_genParams003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams003, sizeof(g_hkdfParams003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams003, sizeof(g_hkdfFinishParams003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest003"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest003"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive004
 * @tc.desc: alg-HMAC pur-MAC dig-SHA512.
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive004, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest004"), (uint8_t *)"HksHKDFDeriveKeyAliasTest004" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams004, sizeof(g_genParams004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams004, sizeof(g_hkdfParams004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams004, sizeof(g_hkdfFinishParams004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest004"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest004"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive005
 * @tc.desc: alg-HKDF pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive005, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest005"), (uint8_t *)"HksHKDFDeriveKeyAliasTest005" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams005, sizeof(g_genParams005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams005, sizeof(g_hkdfParams005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams005, sizeof(g_hkdfFinishParams005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest005"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest005"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive006
 * @tc.desc: alg-HKDF pur-Derive dig-SHA384.
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive006, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest006"), (uint8_t *)"HksHKDFDeriveKeyAliasTest006" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams006, sizeof(g_genParams006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams006, sizeof(g_hkdfParams006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams006, sizeof(g_hkdfFinishParams006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest006"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest006"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive007
 * @tc.desc: alg-HKDF pur-Derive dig-SHA512.
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive007, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest007"), (uint8_t *)"HksHKDFDeriveKeyAliasTest007" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams007, sizeof(g_genParams007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams007, sizeof(g_hkdfParams007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams007, sizeof(g_hkdfFinishParams007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest007"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest007"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive008
 * @tc.desc: alg-HKDF pur-Derive dig-SHA512.
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive008, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest008"), (uint8_t *)"HksHKDFDeriveKeyAliasTest008" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams008, sizeof(g_genParams008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams008, sizeof(g_hkdfParams008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams008, sizeof(g_hkdfFinishParams008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest008"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest008"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
/**
 * @tc.name: HksHkdfDerivePart1Test.HksHKDFDerive009
 * @tc.desc: alg-HKDF pur-Derive dig-SHA512.
 * @tc.type: FUNC
 */
HWTEST_F(HksHkdfDerivePart1Test, HksHKDFDerive009, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksHKDFDeriveKeyAliasTest009"), (uint8_t *)"HksHKDFDeriveKeyAliasTest009" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams009, sizeof(g_genParams009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. HKDF Three Stage */
    struct HksParamSet *hkdfParamSet = nullptr;
    struct HksParamSet *hkdfFinishParamSet = nullptr;
    ret = InitParamSet(&hkdfParamSet, g_hkdfParams009, sizeof(g_hkdfParams009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // finish paramset
    ret = InitParamSet(&hkdfFinishParamSet, g_hkdfFinishParams009, sizeof(g_hkdfFinishParams009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // Init-Update-final
    HksHkdfDeriveTestNormalCase(keyAlias, genParamSet, hkdfParamSet, hkdfFinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksHKDFDeriveKeyAliasFinalTest009"),
        .data = (uint8_t *)"HksHKDFDeriveKeyAliasFinalTest009"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hkdfParamSet);
    HksFreeParamSet(&hkdfFinishParamSet);
}
} // namespace Unittest::HkdfDerive