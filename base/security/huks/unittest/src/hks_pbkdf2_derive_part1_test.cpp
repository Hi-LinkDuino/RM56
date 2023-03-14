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

#include "hks_pbkdf2_derive_part1_test.h"
#include "hks_pbkdf2_derive_test_common.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::Pbkdf2Derive {
class HksPbkdf2DerivePart1Test : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksPbkdf2DerivePart1Test::SetUpTestCase(void)
{
}

void HksPbkdf2DerivePart1Test::TearDownTestCase(void)
{
}

void HksPbkdf2DerivePart1Test::SetUp()
{
}

void HksPbkdf2DerivePart1Test::TearDown()
{
}

uint8_t g_saltdata1[16] = {0};

static struct HksParam g_genParams001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = 256
    }
};
static struct HksParam g_pbkdf2Params001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = 256
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_pbkdf2FinishParams001[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest001_2"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest001_2"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_32
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};
static struct HksParam g_genParams002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_192
    }
};
static struct HksParam g_pbkdf2Params002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_pbkdf2FinishParams002[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest002_2"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest002_2"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_32
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};
static struct HksParam g_genParams003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }
};
static struct HksParam g_pbkdf2Params003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_pbkdf2FinishParams003[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest003_2"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest003_2"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_32
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};
static struct HksParam g_genParams004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_128
    }
};
static struct HksParam g_pbkdf2Params004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_48
    }
};
static struct HksParam g_pbkdf2FinishParams004[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest004_2"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest004_2"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_48
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_genParams005[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_192
    }
};
static struct HksParam g_pbkdf2Params005[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_48
    }
};
static struct HksParam g_pbkdf2FinishParams005[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest005_2"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest005_2"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_48
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_genParams006[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256}
};
static struct HksParam g_pbkdf2Params006[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_48
    }
};
static struct HksParam g_pbkdf2FinishParams006[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest006_2Finish"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest006_2Finish"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_48
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_genParams007[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_128
    }
};
static struct HksParam g_pbkdf2Params007[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_64
    }
};
static struct HksParam g_pbkdf2FinishParams007[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest007_2Finish"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest007_2Finish"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_64
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_genParams008[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_192
    }
};
static struct HksParam g_pbkdf2Params008[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_64
    }
};
static struct HksParam g_pbkdf2FinishParams008[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest008_2Finish"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest008_2Finish"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_64
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_genParams009[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }
};
static struct HksParam g_pbkdf2Params009[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = DERIVE_ITERATION
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(g_saltdata1),
            (uint8_t *)g_saltdata1
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_64
    }
};
static struct HksParam g_pbkdf2FinishParams009[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = {
            strlen("HksPBKDF2DeriveKeyAliasTest009_2Finish"),
            (uint8_t *)"HksPBKDF2DeriveKeyAliasTest009_2Finish"
        }
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Pbkdf2Derive::DERIVE_KEY_SIZE_64
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive001
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive001, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest001_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest001_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams001, sizeof(g_genParams001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params001, sizeof(g_pbkdf2Params001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams001,
        sizeof(g_pbkdf2FinishParams001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest001_2"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest001_2"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive002
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive002, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest002_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest002_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams002, sizeof(g_genParams002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params002, sizeof(g_pbkdf2Params002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams002,
        sizeof(g_pbkdf2FinishParams002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest002_2"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest002_2"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive003
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive003, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest003_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest003_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams003, sizeof(g_genParams003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params003, sizeof(g_pbkdf2Params003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams003,
        sizeof(g_pbkdf2FinishParams003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest003_2"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest003_2"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive004
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive004, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest004_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest004_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams004, sizeof(g_genParams004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params004, sizeof(g_pbkdf2Params004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams004,
        sizeof(g_pbkdf2FinishParams004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest004_2"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest004_2"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive005
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive005, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest005_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest005_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams005, sizeof(g_genParams005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params005, sizeof(g_pbkdf2Params005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams005,
        sizeof(g_pbkdf2FinishParams005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest005_2"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest005_2"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive006
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive006, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest006_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest006_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams006, sizeof(g_genParams006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params006, sizeof(g_pbkdf2Params006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams006,
        sizeof(g_pbkdf2FinishParams006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest006_2Finish"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest006_2Finish"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive007
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive007, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest007_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest007_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams007, sizeof(g_genParams007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params007, sizeof(g_pbkdf2Params007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams007,
        sizeof(g_pbkdf2FinishParams007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest007_2Finish"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest007_2Finish"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive008
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive008, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest008_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest008_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams008, sizeof(g_genParams008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params008, sizeof(g_pbkdf2Params008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams008,
        sizeof(g_pbkdf2FinishParams008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest008_2Finish"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest008_2Finish"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}

/**
 * @tc.name: HksPbkdf2DerivePart1Test.HksPbkdf2Derive009
 * @tc.desc: alg-PBKDF2 pur-Derive dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksPbkdf2DerivePart1Test, HksPbkdf2Derive009, TestSize.Level0)
{
    struct HksBlob keyAlias = { strlen("HksPBKDF2DeriveKeyAliasTest009_1"),
        (uint8_t *)"HksPBKDF2DeriveKeyAliasTest009_1" };
    int32_t ret = HKS_FAILURE;
    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams009, sizeof(g_genParams009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 2. PBKDF2 Three Stage */
    struct HksParamSet *pbkdf2ParamSet = nullptr;
    struct HksParamSet *pbkdf2FinishParamSet = nullptr;
    ret = InitParamSet(&pbkdf2ParamSet, g_pbkdf2Params009, sizeof(g_pbkdf2Params009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    struct HksParam *saltParam = nullptr;
    HksGetParam(pbkdf2ParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";
    // Finish paramset
    ret = InitParamSet(&pbkdf2FinishParamSet, g_pbkdf2FinishParams009,
        sizeof(g_pbkdf2FinishParams009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    // init-update-final
    HksPbkdf2DeriveTestNormalCase(keyAlias, genParamSet, pbkdf2ParamSet, pbkdf2FinishParamSet);
    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    struct HksBlob deleteKeyAlias = { .size = strlen("HksPBKDF2DeriveKeyAliasTest009_2Finish"),
        .data = (uint8_t *)"HksPBKDF2DeriveKeyAliasTest009_2Finish"};
    ret = HksDeleteKey(&deleteKeyAlias, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete Final Key failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&pbkdf2ParamSet);
    HksFreeParamSet(&pbkdf2FinishParamSet);
}
} // namespace Unittest::Pbkdf2Derive