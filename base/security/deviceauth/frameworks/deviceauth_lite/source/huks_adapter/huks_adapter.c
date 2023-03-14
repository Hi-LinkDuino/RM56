/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "huks_adapter.h"
#include <stdio.h>
#include "securec.h"
#include "commonutil.h"
#include "hks_api.h"
#include "hks_param.h"
#include "log.h"
#include "mem_stat.h"

#define X25519_KEY_LEN 256
#define ED25519_KEY_LEN 256
#define X25519_KEY_PARAM_SET_SIZE 128 /* priv key size: 32, pub key size: 32, add two tag, no larger than 128 */
#define DEFAULT_PARAM_SET_OUT_SIZE 1024
#define HC_PARAM_CHAIN_LEN 255
#define HC_PARAM_KEY_LEN 256
#define BITS_PER_BYTE 8
#define HC_CCM_NONCE_LEN 7

#if (defined(_SUPPORT_SEC_CLONE_) || defined(_SUPPORT_SEC_CLONE_SERVER_))
static const uint8_t g_factor[] = "hichain_key_enc_key";
static const int32_t g_cert_chain_cnt = 4;
#endif

union huks_key_type_union {
    struct huks_key_type type_struct;
    uint32_t key_type;
};

#define CREATE_STRUCT(T) \
    struct T *create_struct_##T(void) \
    { \
        struct T *val = (struct T *)MALLOC(sizeof(struct T)); \
        if (val == NULL) { \
            return NULL; \
        } \
        (void)memset_s(val, sizeof(*val), 0, sizeof(*val)); \
        return val; \
    }

#define CONVERT_TO_BLOB(T, field_name) \
    struct HksBlob convert_to_blob_from_##T(struct T *val) \
    { \
        struct HksBlob hks_blob_val; \
        (void)memset_s(&hks_blob_val, sizeof(hks_blob_val), 0, sizeof(hks_blob_val)); \
        check_ptr_return_val(val->field_name, hks_blob_val); \
        check_num_return_val(val->length, hks_blob_val); \
        hks_blob_val.data = val->field_name; \
        hks_blob_val.size = val->length; \
        return hks_blob_val; \
    }

CREATE_STRUCT(hc_key_alias)
CREATE_STRUCT(sha256_value)
CREATE_STRUCT(uint8_buff)

CONVERT_TO_BLOB(hc_key_alias, key_alias)
CONVERT_TO_BLOB(ltpk, ltpk)
CONVERT_TO_BLOB(sha256_value, sha256_value)
CONVERT_TO_BLOB(sha512_value, sha512_value)
CONVERT_TO_BLOB(signature, signature)
CONVERT_TO_BLOB(stpk, stpk)
CONVERT_TO_BLOB(stsk, stsk)
CONVERT_TO_BLOB(hc_auth_id, auth_id)

static const uint8_t g_key_type_pairs[HC_MAX_KEY_TYPE_NUM][HC_KEY_TYPE_PAIR_LEN] = {
    { 0x00, 0x00 }, /* ACCESSOR_PK */
    { 0x00, 0x01 }, /* CONTROLLER_PK */
    { 0x00, 0x02 }, /* ed25519 KEYPAIR */
    { 0x00, 0x03 }, /* KEK, key encryption key, used only by DeviceAuthService */
    { 0x00, 0x04 }, /* DEK, data encryption key, used only by upper apps */
    { 0x00, 0x05 }, /* key tmp */
    { 0x00, 0x06 }  /* PSK, preshared key index */
};

static const char *g_large_prime_number_hex_384 =
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74"\
    "020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F1437"\
    "4FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"\
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF05"\
    "98DA48361C55D39A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB"\
    "9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"\
    "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"\
    "3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D04507A33"\
    "A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7"\
    "ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6BF12FFA06D98A0864"\
    "D87602733EC86A64521F2B18177B200CBBE117577A615D6C770988C0BAD946E2"\
    "08E24FA074E5AB3143DB5BFCE0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF";

static const char *g_large_prime_number_hex_256 =
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74"\
    "020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F1437"\
    "4FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"\
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF05"\
    "98DA48361C55D39A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB"\
    "9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"\
    "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"\
    "3995497CEA956AE515D2261898FA051015728E5A8AACAA68FFFFFFFFFFFFFFFF";

static int32_t hks_hex_string_to_byte(const char *src, uint8_t *dst, uint32_t dst_size)
{
    size_t length = strlen(src);
    if (length % 2 != 0) { /* odd number or not */
        LOGE("Invalid hex_string length: %zu, even num is expected", length);
        return ERROR_CODE_FAILED;
    }

    uint32_t hex_length = length / BYTE_TO_HEX_OPER_LENGTH; /* Calculate hexadecimal length */
    if (dst_size < hex_length) {
        LOGE("Invalid dst_size: %u is smaller than hex_length: %u", dst_size, hex_length);
        return ERROR_CODE_FAILED;
    }

    uint8_t nibble[2]; /* create an array of two elements */
    for (uint32_t i = 0; i < hex_length; i++) {
        nibble[0] = src[i * BYTE_TO_HEX_OPER_LENGTH];
        nibble[1] = src[i * BYTE_TO_HEX_OPER_LENGTH + 1];
        for (uint32_t j = 0; j < BYTE_TO_HEX_OPER_LENGTH; j++) { /* iterate through array */
            if ((nibble[j] <= 'F') && (nibble[j] >= 'A')) {
                nibble[j] = nibble[j] - 'A' + 10; /* hex conversion */
            } else if ((nibble[j] <= 'f') && (nibble[j] >= 'a')) {
                nibble[j] = nibble[j] - 'a' + 10; /* hex conversion */
            } else if ((nibble[j] >= '0') && (nibble[j] <= '9')) {
                nibble[j] = nibble[j] - '0';
            } else {
                LOGE("Invalid char: [%c] in hex string, [0-9]|[A-F]|[a-f] expected", (char)nibble[j]);
                return ERROR_CODE_FAILED;
            }
        }
        dst[i] = nibble[0] << 4; /* shift left for filling */
        dst[i] |= nibble[1];
    }
    return ERROR_CODE_SUCCESS;
}

static int32_t construct_param_set(struct HksParamSet **out, const struct HksParam *in_param,
    const uint32_t in_param_num)
{
    struct HksParamSet *param_set = NULL;
    int32_t status = HksInitParamSet(&param_set);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("init param set failed, status=%d", status);
        return ERROR_CODE_INIT_PARAM_SET;
    }

    status = HksAddParams(param_set, in_param, in_param_num);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("add digest param failed, status=%d", status);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_ADD_PARAM;
    }

    status = HksBuildParamSet(&param_set);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("build param set failed, status=%d", status);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_BUILD_PARAM_SET;
    }

    *out = param_set;
    return ERROR_CODE_SUCCESS;
}

static struct sha256_value sha256(const struct uint8_buff *message)
{
    struct sha256_value sha256_value;
    (void)memset_s(&sha256_value, sizeof(sha256_value), 0, sizeof(sha256_value));

    struct HksBlob src_data = { message->length, message->val };

    struct HksBlob hash = { 0, NULL };
    hash.data = (uint8_t *)MALLOC(HC_SHA256_LEN * sizeof(uint8_t));
    if (hash.data == NULL) {
        LOGE("SHA256 malloc failed");
        return sha256_value;
    }
    hash.size = HC_SHA256_LEN;

    struct HksParamSet *param_set = NULL;
    struct HksParam digest_param[] = {
        {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }
    };
    int32_t status = construct_param_set(&param_set, digest_param, array_size(digest_param));
    if (status != ERROR_CODE_SUCCESS) {
        safe_free(hash.data);
        LOGE("construct param set in the sha256 failed, status=%d", status);
        return sha256_value;
    }
    status = HksHash(param_set, &src_data, &hash);
    if ((status == 0) && (hash.size == HC_SHA256_LEN)) {
        (void)memcpy_s(sha256_value.sha256_value, sizeof(sha256_value.sha256_value), hash.data, HC_SHA256_LEN);
        sha256_value.length = HC_SHA256_LEN;
    } else {
        LOGE("SHA256 failed, status=%d", status);
        sha256_value.length = 0;
    }
    safe_free(hash.data);
    HksFreeParamSet(&param_set);
    return sha256_value;
}

static int32_t Compare(const uint8_t *a, uint32_t lenA, const uint8_t *b, uint32_t lenB)
{
    const uint8_t *tmpA = a;
    const uint8_t *tmpB = b;
    uint32_t len = lenA;
    if (lenA < lenB) {
        for (uint32_t i = 0; i < lenB - lenA; i++) {
            if (b[i] > 0) {
                return 1; // a < b: 1
            }
        }
        tmpA = a;
        tmpB = b + lenB - lenA;
        len = lenA;
    }
    if (lenA > lenB) {
        for (uint32_t i = 0; i < lenA - lenB; i++) {
            if (a[i] > 0) {
                return -1; // a > b: -1
            }
        }
        tmpA = a + lenA - lenB;
        tmpB = b;
        len = lenB;
    }
    for (uint32_t i = 0; i < len; i++) {
        if (*(tmpA + i) > *(tmpB + i)) {
            return -1;
        }
        if (*(tmpA + i) < *(tmpB + i)) {
            return 1;
        }
    }
    return 0;
}

int32_t CheckDlSpekePublicKey(const struct var_buffer *key, uint32_t bigNumLen)
{
    if (key == NULL) {
        LOGE("Param is null.");
        return HC_INPUT_PTR_NULL;
    }
    const char *primeHex = NULL;
    if (bigNumLen == HC_BIG_PRIME_MAX_LEN_384) {
        primeHex = g_large_prime_number_hex_384;
    } else {
        primeHex = g_large_prime_number_hex_256;
    }
    uint8_t min = 1;
    uint32_t primeLen = strlen(primeHex) / BYTE_TO_HEX_OPER_LENGTH;
    if (key->length > primeLen) {
        LOGE("key->length > primeLen.");
        return HC_INPUT_ERROR;
    }
    uint8_t *primeVal = (uint8_t *)MALLOC(primeLen);
    if (primeVal == NULL) {
        LOGE("Malloc primeVal failed.");
        return HC_MALLOC_FAILED;
    }
    if (hex_string_to_byte(primeHex, strlen(primeHex), primeVal) != ERROR_CODE_SUCCESS) {
        LOGE("hex_string_to_byte for prime num failed");
        FREE(primeVal);
        return HC_INPUT_ERROR;
    }
    /*
     * P - 1, since the last byte of large prime number must be greater than 1,
     * do not need to think about borrowing forward
     */
    primeVal[primeLen - 1] -= 1;
    if (Compare(key->data, key->length, &min, sizeof(uint8_t)) >= 0) {
        LOGE("key <= 1, invalid.");
        FREE(primeVal);
        return HC_MEMCPY_ERROR;
    }
    if (Compare(key->data, key->length, primeVal, primeLen) <= 0) {
        LOGE("key >= p - 1, invalid.");
        FREE(primeVal);
        return HC_MEMCPY_ERROR;
    }
    FREE(primeVal);
    return HC_OK;
}

int32_t cal_bignum_exp(struct var_buffer *base, struct var_buffer *exp,
    const uint32_t big_num_len, struct big_num *out_result)
{
    check_ptr_return_val(base, HC_INPUT_ERROR);
    check_ptr_return_val(exp, HC_INPUT_ERROR);
    check_ptr_return_val(out_result, HC_INPUT_ERROR);

    if ((big_num_len != HC_BIG_PRIME_MAX_LEN_384) && (big_num_len != HC_BIG_PRIME_MAX_LEN_256)) {
        LOGE("Not support big number len %d", big_num_len);
        return HC_LARGE_PRIME_NUMBER_LEN_UNSUPPORT;
    }

    struct HksBlob big_num_a = { base->length, base->data };
    struct HksBlob big_num_e = { exp->length, exp->data };

    uint8_t *large_num = (uint8_t *)MALLOC(big_num_len);
    if (large_num == NULL) {
        LOGE("Malloc big num buff fail");
        return ERROR_CODE_FAILED;
    }

    (void)memset_s(large_num, big_num_len, 0, big_num_len);
    int32_t status;

    if (big_num_len == HC_BIG_PRIME_MAX_LEN_384) {
        status = hks_hex_string_to_byte(g_large_prime_number_hex_384, large_num, big_num_len);
    } else {
        status = hks_hex_string_to_byte(g_large_prime_number_hex_256, large_num, big_num_len);
    }
    if (status != ERROR_CODE_SUCCESS) {
        FREE(large_num);
        return ERROR_CODE_FAILED;
    }

    struct HksBlob big_num_n = { big_num_len, large_num };
    struct HksBlob big_num_x = { big_num_len, out_result->big_num };
    if (big_num_len > sizeof(out_result->big_num)) {
        LOGE("The big num array is shorter than the expected output len.");
        FREE(large_num);
        return ERROR_CODE_FAILED;
    }

    status = HksBnExpMod(&big_num_x, &big_num_a, &big_num_e, &big_num_n);
    FREE(large_num);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Huks bn exp mod error, status=%d", status);
        return ERROR_CODE_FAILED;
    }
    out_result->length = big_num_x.size;

    return ERROR_CODE_SUCCESS;
}

struct random_value generate_random(uint32_t length)
{
    struct random_value rand;
    (void)memset_s(&rand, sizeof(rand), 0, sizeof(rand));
    if ((length == 0) || (length > HC_RAMDOM_MAX_LEN)) {
        LOGE("Generate random failed, invalid param length: %d", length);
        return rand;
    }

    struct HksBlob hks_rand = { length, rand.random_value };
    int32_t status = HksGenerateRandom(NULL, &hks_rand);
    if (status == ERROR_CODE_SUCCESS) {
        rand.length = hks_rand.size;
    } else {
        LOGE("Huks generate random failed, status: %d", status);
    }

    return rand;
}

int32_t compute_hmac(struct var_buffer *key, const struct uint8_buff *message, struct hmac *out_hmac)
{
    check_ptr_return_val(key, HC_INPUT_ERROR);
    check_ptr_return_val(message, HC_INPUT_ERROR);
    check_ptr_return_val(out_hmac, HC_INPUT_ERROR);

    struct HksBlob hks_key = { key->length, key->data };
    struct HksBlob src_data = { message->length, message->val };
    struct HksBlob output = { HC_HMAC_LEN, out_hmac->hmac };
    struct HksParamSet *param_set = NULL;

    struct HksParam hmac_param[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_MAC
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS, /* temporary key, is_key_alias is set to false determined using REE for MAC */
            .boolParam = false
        }
    };
    int32_t status = construct_param_set(&param_set, hmac_param, array_size(hmac_param));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct HMAC param set failed, status=%d", status);
        return ERROR_CODE_BUILD_PARAM_SET;
    }

    /* make hmac */
    status = HksMac(&hks_key, param_set, &src_data, &output);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Huks hmac failed, status: %d", status);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_FAILED;
    }
    out_hmac->length = output.size;
    HksFreeParamSet(&param_set);

    return ERROR_CODE_SUCCESS;
}

int32_t compute_hkdf(struct var_buffer *shared_secret, struct hc_salt *salt,
    char *key_info, uint32_t hkdf_len, struct var_buffer *out_hkdf)
{
    check_ptr_return_val(shared_secret, HC_INPUT_ERROR);
    check_ptr_return_val(salt, HC_INPUT_ERROR);
    check_ptr_return_val(out_hkdf, HC_INPUT_ERROR);
    check_ptr_return_val(key_info, HC_INPUT_ERROR);

    struct HksBlob derived_key = { hkdf_len, out_hkdf->data };
    struct HksBlob hks_salt = { salt->length, salt->salt };
    struct HksBlob hks_key_info = { (uint32_t)strlen(key_info), (uint8_t *)key_info };

    /* original key */
    struct HksBlob kdf_key = { shared_secret->length, shared_secret->data };

    /* derived key param */
    struct HksParamSet *param_set = NULL;
    struct HksParam hkdf_param[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_DERIVE
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_HKDF
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_SALT,
            .blob = hks_salt
        }, {
            .tag = HKS_TAG_INFO,
            .blob = hks_key_info
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = false
        }
    };
    int32_t status = construct_param_set(&param_set, hkdf_param, array_size(hkdf_param));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct hkdf param set failed, status=%d", status);
        return ERROR_CODE_BUILD_PARAM_SET;
    }

    /* make hkdf */
    status = HksDeriveKey(param_set, &kdf_key, &derived_key);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Huks key derivation failed, status: %d", status);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_FAILED;
    }
    out_hkdf->length = derived_key.size;

    HksFreeParamSet(&param_set);
    return ERROR_CODE_SUCCESS;
}

static int32_t init_aes_gcm_encrypt_param_set(struct HksParamSet **param_set,
    struct random_value *nonce, struct aes_aad *aad, uint32_t key_byte_size)
{
    struct HksParam encrypt_param[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_ENCRYPT
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES
        }, {
            .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_GCM
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_NONCE,
            .blob = { nonce->length, nonce->random_value }
        }, {
            .tag = HKS_TAG_ASSOCIATED_DATA,
            .blob = { aad->length, aad->aad }
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = false
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = key_byte_size * BITS_PER_BYTE
        }
    };

    return construct_param_set(param_set, encrypt_param, array_size(encrypt_param));
}

int32_t aes_gcm_encrypt(struct var_buffer *key, const struct uint8_buff *plain,
    struct aes_aad *aad, struct uint8_buff *out_cipher)
{
    check_ptr_return_val(key, HC_INPUT_ERROR);
    check_ptr_return_val(plain, HC_INPUT_ERROR);
    check_ptr_return_val(aad, HC_INPUT_ERROR);
    check_ptr_return_val(out_cipher, HC_INPUT_ERROR);
    struct random_value nonce = generate_random(HC_AES_GCM_NONCE_LEN);
    if (nonce.length == 0) {
        LOGE("Generate random to make nonce failed");
        return HC_GEN_RANDOM_FAILED;
    }

    struct HksBlob hks_key = { key->length, key->data };
    struct HksBlob hks_plain_text = { plain->length, plain->val };

    if (memcpy_s(out_cipher->val, out_cipher->size, nonce.random_value, nonce.length) != EOK) {
        LOGE("memcpy nonce fail");
        return ERROR_CODE_FAILED;
    }

    struct HksBlob tag_cipher = { out_cipher->size - nonce.length, out_cipher->val + nonce.length };
    struct HksParamSet *param_set = NULL;
    int32_t status = init_aes_gcm_encrypt_param_set(&param_set, &nonce, aad, hks_key.size);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("init encrypt param set failed, status=%d", status);
        return ERROR_CODE_BUILD_PARAM_SET;
    }

    status = HksEncrypt(&hks_key, param_set, &hks_plain_text, &tag_cipher);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Huks aead encrypt error, status: %d", status);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_FAILED;
    }

    out_cipher->length = tag_cipher.size + nonce.length;
    HksFreeParamSet(&param_set);
    return ERROR_CODE_SUCCESS;
}

static int32_t init_aes_gcm_decrypt_param_set(struct HksParamSet **param_set,
    const struct uint8_buff *cipher, struct aes_aad *aad, uint32_t key_byte_size)
{
    struct HksParam decrypt_param[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_DECRYPT
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES
        }, {
            .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_GCM
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_NONCE,
            .blob = { HC_AES_GCM_NONCE_LEN, cipher->val }
        }, {
            .tag = HKS_TAG_ASSOCIATED_DATA,
            .blob = { aad->length, aad->aad }
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = false
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = key_byte_size * BITS_PER_BYTE
        }
    };

    return construct_param_set(param_set, decrypt_param, array_size(decrypt_param));
}

int32_t aes_gcm_decrypt(struct var_buffer *key, const struct uint8_buff *cipher,
    struct aes_aad *aad, struct uint8_buff *out_plain)
{
    check_ptr_return_val(key, HC_INPUT_ERROR);
    check_ptr_return_val(cipher, HC_INPUT_ERROR);
    check_ptr_return_val(aad, HC_INPUT_ERROR);
    check_ptr_return_val(out_plain, HC_INPUT_ERROR);

    if (cipher->length < HC_AES_GCM_NONCE_LEN) {
        LOGE("Cipher length is short than nonce max length");
        return ERROR_CODE_FAILED;
    }

    struct HksBlob hks_key = { key->length, key->data };
    struct HksBlob nonce_blob = { HC_AES_GCM_NONCE_LEN, cipher->val };
    struct HksBlob cipher_text_with_tag = { cipher->length - nonce_blob.size, cipher->val + nonce_blob.size };
    struct HksBlob plain_text = { 0, NULL };

    plain_text.data = (uint8_t *)MALLOC(cipher_text_with_tag.size - HKS_AE_TAG_LEN);
    check_ptr_return_val(plain_text.data, ERROR_CODE_FAILED);
    plain_text.size = cipher_text_with_tag.size - HKS_AE_TAG_LEN;

    struct HksParamSet *param_set = NULL;
    int32_t status = init_aes_gcm_decrypt_param_set(&param_set, cipher, aad, hks_key.size);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("init encrypt param set failed, status=%d", status);
        safe_free(plain_text.data);
        return ERROR_CODE_BUILD_PARAM_SET;
    }

    status = HksDecrypt(&hks_key, param_set, &cipher_text_with_tag, &plain_text);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Huks aead decrypt failed, status: %d", status);
        safe_free(plain_text.data);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_FAILED;
    }

    if (memcpy_s(out_plain->val, out_plain->size, plain_text.data, plain_text.size) != EOK) {
        safe_free(plain_text.data);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_FAILED;
    }
    out_plain->length = plain_text.size;

    safe_free(plain_text.data);
    HksFreeParamSet(&param_set);
    return status;
}

struct service_id generate_service_id(const struct session_identity *identity)
{
    struct service_id service_id;
    (void)memset_s(&service_id, sizeof(service_id), 0, sizeof(service_id));
    if (identity == NULL) {
        LOGE("Input is null");
        return service_id;
    }

    uint32_t pack_name_len = identity->package_name.length;
    if ((pack_name_len == 0) || (pack_name_len > HC_PACKAGE_NAME_BUFF_LEN)) {
        LOGE("Pack name length is: %u", pack_name_len);
        return service_id;
    }
    uint32_t type_len = identity->service_type.length;
    if ((type_len == 0) || (type_len > HC_SERVICE_TYPE_BUFF_LEN)) {
        LOGE("Service type length is: %u", type_len);
        return service_id;
    }
    uint32_t srv_len = type_len + pack_name_len;
    struct uint8_buff id_buff = {
        .length = srv_len,
        .size = srv_len
    };

    id_buff.val = (uint8_t *)MALLOC(srv_len);
    if (id_buff.val == NULL) {
        LOGE("Malloc mem failed");
        return service_id;
    }

    if (memcpy_s(id_buff.val, srv_len, identity->package_name.name, pack_name_len) != EOK) {
        LOGE("Copy service id buff failed");
        safe_free(id_buff.val);
        return service_id;
    }
    if (memcpy_s(id_buff.val + pack_name_len, srv_len - pack_name_len, identity->service_type.type, type_len) != EOK) {
        LOGE("Copy service id buff failed");
        safe_free(id_buff.val);
        return service_id;
    }

    struct sha256_value srv_sha256 = sha256(&id_buff);
    safe_free(id_buff.val);
    id_buff.val = NULL;

    if (srv_sha256.length > 0) {
        if (memcpy_s(service_id.service_id, HC_SERVICE_ID_BUFF_LEN, srv_sha256.sha256_value, HC_SHA256_LEN) == EOK) {
            service_id.length = srv_sha256.length;
        } else {
            LOGE("Copy hash value failed");
        }
    }

    return service_id;
}

static int32_t convert_byte_to_hex_string(const uint8_t *byte_array, uint32_t byte_array_size,
    uint8_t *hex_string, uint32_t out_size)
{
    int32_t length = 0;

    check_num_return_val(byte_array_size, length);
    if (out_size < (byte_array_size * BYTE_TO_HEX_OPER_LENGTH)) { /* doubleword, length*2 */
        return length;
    }
    uint32_t hex_str_arr_len = (uint32_t)(out_size + 1);
    char *hex_str_arr = (char *)MALLOC(hex_str_arr_len);
    if (hex_str_arr == NULL) {
        LOGE("Copy hex arr to string failed");
        return 0;
    }
    (void)memset_s(hex_str_arr, hex_str_arr_len, 0, hex_str_arr_len);
    for (uint32_t i = 0; i < byte_array_size; i++) {
        if (sprintf_s((char *)(hex_str_arr + length), hex_str_arr_len - length, "%02x", byte_array[i]) < 0) {
            LOGE("What happened was that the probability was zero"); /* caller perceives memery error, no return */
        }
        length += BYTE_TO_HEX_OPER_LENGTH; /* doubleword, length increases 2 each time */
    }
    if (memcpy_s(hex_string, out_size, hex_str_arr, length) != EOK) {
        LOGE("Copy hex arr to string failed");
        length = 0;
    }

    safe_free(hex_str_arr);
    hex_str_arr = NULL;
    return length;
}

struct hc_key_alias generate_key_alias(const struct service_id *service_id,
    const struct hc_auth_id *auth_id, enum huks_key_alias_type key_type)
{
    struct hc_key_alias temp_alias;
    (void)memset_s(&temp_alias, sizeof(temp_alias), 0, sizeof(temp_alias));
    check_ptr_return_val(service_id, temp_alias);
    check_ptr_return_val(auth_id, temp_alias);
    if (key_type >= HC_MAX_KEY_TYPE_NUM) {
        LOGE("Invalid user type");
        return temp_alias;
    }
    if (service_id->length > HC_SERVICE_ID_BUFF_LEN) {
        LOGE("service_id length is error");
        return temp_alias;
    }
    if (auth_id->length > HC_AUTH_ID_BUFF_LEN) {
        LOGE("auth_id length is error");
        return temp_alias;
    }

    uint32_t key_type_pair_size = HC_KEY_TYPE_PAIR_LEN;
    const uint8_t *key_type_pair = g_key_type_pairs[key_type];
    uint32_t total_len = service_id->length + auth_id->length + key_type_pair_size;
    struct uint8_buff key_alias_buff;

    (void)memset_s(&key_alias_buff, sizeof(key_alias_buff), 0, sizeof(key_alias_buff));
    key_alias_buff.val = (uint8_t *)MALLOC(total_len + 1);
    if (key_alias_buff.val == NULL) {
        LOGE("Malloc key alias buff failed");
        return temp_alias;
    }

    key_alias_buff.size = total_len + 1; /* one character longer for terminator */
    key_alias_buff.length = total_len;
    (void)memset_s(key_alias_buff.val, key_alias_buff.size, 0, key_alias_buff.size);
    (void)memcpy_s(key_alias_buff.val, key_alias_buff.size, service_id->service_id, service_id->length);
    (void)memcpy_s(key_alias_buff.val + service_id->length, key_alias_buff.size - service_id->length,
                   key_type_pair, key_type_pair_size);
    (void)memcpy_s(key_alias_buff.val + service_id->length + key_type_pair_size,
                   key_alias_buff.size - service_id->length - key_type_pair_size,
                   auth_id->auth_id, auth_id->length);

    struct hc_key_alias key_alias;
    (void)memset_s(&key_alias, sizeof(key_alias), 0, sizeof(key_alias));
    struct sha256_value alias_sha256 = sha256(&key_alias_buff);

    safe_free(key_alias_buff.val);
    key_alias_buff.val = NULL;
    if (alias_sha256.length > 0) {
        int32_t length = convert_byte_to_hex_string(alias_sha256.sha256_value, alias_sha256.length,
                                                    key_alias.key_alias, HC_KEY_ALIAS_MAX_LEN);
        key_alias.length = length;
    }
    return key_alias;
}

static int32_t init_x25519_generate_key_input_param_set(struct HksParamSet **input_param_set)
{
    struct HksParam key_param[] = {
        {
            .tag = HKS_TAG_KEY_STORAGE_FLAG,
            .uint32Param = HKS_STORAGE_TEMP
        }, {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_X25519
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = X25519_KEY_LEN
        }, {
            .tag = HKS_TAG_IS_ALLOWED_WRAP,
            .boolParam = true
        }
    };

    int32_t status = construct_param_set(input_param_set, key_param, array_size(key_param));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct encrypt param set failed, status=%d", status);
        return ERROR_CODE_BUILD_PARAM_SET;
    }

    return ERROR_CODE_SUCCESS;
}

static int32_t parse_x25519_output_param_set(struct HksParamSet *output_param_set,
    struct st_key_pair *out_key_pair)
{
    int32_t status = HksFreshParamSet(output_param_set, false); /* false means fresh by local, not though IPC */
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("fresh param set failed, status:%d", status);
        return ERROR_CODE_FRESH_PARAM_SET;
    }

    struct HksParam *pub_key_param = NULL;
    status = HksGetParam(output_param_set, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pub_key_param);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("get pub key from param set failed, status:%d", status);
        return ERROR_CODE_GET_PUB_KEY_FROM_PARAM_SET;
    }

    struct HksParam *priv_key_param = NULL;
    status = HksGetParam(output_param_set, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priv_key_param);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("get priv key from param set failed, status:%d", status);
        return ERROR_CODE_GET_PRIV_KEY_FROM_PARAM_SET;
    }

    if (memcpy_s(out_key_pair->st_public_key.stpk, HC_ST_PUBLIC_KEY_LEN,
        pub_key_param->blob.data, pub_key_param->blob.size) != EOK) {
        LOGE("parse x25519 output param set memcpy public key failed!");
        return ERROR_CODE_FAILED;
    }
    out_key_pair->st_public_key.length = pub_key_param->blob.size;

    if (memcpy_s(out_key_pair->st_private_key.stsk, HC_ST_PRIVATE_KEY_LEN,
        priv_key_param->blob.data, priv_key_param->blob.size) != EOK) {
        LOGE("parse x25519 output param set memcpy private key failed!");
        return ERROR_CODE_FAILED;
    }
    out_key_pair->st_private_key.length = priv_key_param->blob.size;

    return ERROR_CODE_SUCCESS;
}

int32_t generate_st_key_pair(struct st_key_pair *out_key_pair)
{
    check_ptr_return_val(out_key_pair, HC_INPUT_ERROR);
    (void)memset_s(out_key_pair, sizeof(*out_key_pair), 0, sizeof(*out_key_pair));

    struct HksParamSet *input_param_set = NULL;
    int32_t status = init_x25519_generate_key_input_param_set(&input_param_set);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("init x25519 generate key input param set failed! status:%d", status);
        return status;
    }

    struct HksParamSet *output_param_set = (struct HksParamSet *)MALLOC(X25519_KEY_PARAM_SET_SIZE);
    if (output_param_set == NULL) {
        LOGE("allocate buffer for output param set failed");
        HksFreeParamSet(&input_param_set);
        return ERROR_CODE_FAILED;
    }

    (void)memset_s(output_param_set, X25519_KEY_PARAM_SET_SIZE, 0, X25519_KEY_PARAM_SET_SIZE);
    output_param_set->paramSetSize = X25519_KEY_PARAM_SET_SIZE;

    do {
        status = HksGenerateKey(NULL, input_param_set, output_param_set);
        if (status != ERROR_CODE_SUCCESS) {
            LOGE("generate x25519 key failed! status:%d", status);
            status = ERROR_CODE_GENERATE_KEY;
            break;
        }

        status = parse_x25519_output_param_set(output_param_set, out_key_pair);
        if (status != ERROR_CODE_SUCCESS) {
            LOGE("parse x25519 output param set failed! status:%d", status);
            break;
        }
    } while (0);

    HksFreeParamSet(&input_param_set);
    safe_free(output_param_set);
    return status;
}

int32_t generate_lt_key_pair(struct hc_key_alias *key_alias, const struct hc_auth_id *auth_id)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_ptr_return_val(auth_id, HC_INPUT_ERROR);

    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    check_num_return_val(key_alias_blob.size, ERROR_CODE_FAILED);

    struct hc_auth_id tmp_id = *auth_id;
    struct HksParamSet *param_set = NULL;
    struct HksParam key_param[] = {
        {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_ED25519
        }, {
            .tag = HKS_TAG_KEY_STORAGE_FLAG,
            .uint32Param = HKS_STORAGE_PERSISTENT
        }, {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = ED25519_KEY_LEN
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = convert_to_blob_from_hc_auth_id(&tmp_id)
        }, {
            .tag = HKS_TAG_IS_ALLOWED_WRAP,
            .boolParam = true
        }
    };

    int32_t status = construct_param_set(&param_set, key_param, array_size(key_param));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct encrypt param set failed, status=%d", status);
        return ERROR_CODE_BUILD_PARAM_SET;
    }

    status = HksGenerateKey(&key_alias_blob, param_set, NULL);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Hks generate failed, status=%d", status);
        HksFreeParamSet(&param_set);
        return ERROR_CODE_GENERATE_KEY;
    }

    HksFreeParamSet(&param_set);
    return ERROR_CODE_SUCCESS;
}

int32_t export_lt_public_key(struct hc_key_alias *key_alias, struct ltpk *out_public_key)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_ptr_return_val(out_public_key, HC_INPUT_ERROR);

    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    check_num_return_val(key_alias_blob.size, ERROR_CODE_FAILED);

    struct HksBlob key = { HC_LT_PUBLIC_KEY_LEN, out_public_key->ltpk };
    int32_t hks_status = HksExportPublicKey(&key_alias_blob, NULL, &key);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("Export public key failed, status=%d", hks_status);
        return ERROR_CODE_FAILED;
    }
    out_public_key->length = key.size;

    return ERROR_CODE_SUCCESS;
}

int32_t delete_key(struct hc_key_alias *key_alias)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);

    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    check_num_return_val(key_alias_blob.size, ERROR_CODE_FAILED);

    int32_t hks_status = HksDeleteKey(&key_alias_blob, NULL);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("Delete key failed, status=%d", hks_status);
        return ERROR_CODE_FAILED;
    }

    return ERROR_CODE_SUCCESS;
}

/*
 * delete long time public key
 *
 * @key_alias: long time public key alias
 * @return 0 -- success, others -- failed
 */
int32_t delete_lt_public_key(struct hc_key_alias *key_alias)
{
    int32_t hks_status = delete_key(key_alias);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("Delete lt public key failed, status=%d", hks_status);
    }

    return hks_status;
}

static int32_t init_import_lt_public_key_param_set(struct HksParamSet **param_set,
    const int32_t user_type, const int32_t pair_type, struct hc_auth_id *auth_id)
{
#if !(defined(_SUPPORT_SEC_CLONE_) || defined(_SUPPORT_SEC_CLONE_SERVER_))
    union huks_key_type_union huks_key_type;
    huks_key_type.type_struct.user_type = (uint8_t)user_type;
    huks_key_type.type_struct.pair_type = (uint8_t)pair_type;
    huks_key_type.type_struct.reserved1 = (uint8_t)0;
    huks_key_type.type_struct.reserved2 = (uint8_t)0;
#endif

    (void)pair_type;
    struct HksParam key_param[] = {
        {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_ED25519
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = ED25519_KEY_LEN
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = convert_to_blob_from_hc_auth_id(auth_id)
        }, {
            .tag = HKS_TAG_IS_ALLOWED_WRAP,
            .boolParam = true
        },
#if (defined(_SUPPORT_SEC_CLONE_) || defined(_SUPPORT_SEC_CLONE_SERVER_))
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_KEY_ROLE,
            .uint32Param = user_type
        }
#else
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_KEY_ROLE,
            .uint32Param = (uint32_t)huks_key_type.key_type
        }
#endif
    };

    return construct_param_set(param_set, key_param, array_size(key_param));
}

int32_t import_lt_public_key(struct hc_key_alias *key_alias, struct ltpk *peer_public_key,
    const int32_t user_type, const int32_t pair_type, struct hc_auth_id *auth_id)
{
    if ((pair_type < 0) || (pair_type >= HC_MAX_PAIR_TYPE_NUM)) {
        return HC_INPUT_ERROR;
    }
    if ((user_type < 0) || (user_type >= HC_MAX_KEY_TYPE_NUM)) {
        return HC_INPUT_ERROR;
    }
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_ptr_return_val(peer_public_key, HC_INPUT_ERROR);
    check_ptr_return_val(auth_id, HC_INPUT_ERROR);

    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    if (key_alias_blob.size == 0) {
        LOGE("Convert key alias to blob failed");
        return ERROR_CODE_FAILED;
    }

    struct HksBlob ltpk_key_blob = convert_to_blob_from_ltpk(peer_public_key);
    if (ltpk_key_blob.size == 0) {
        LOGE("Convert ltpk key to blob failed");
        return ERROR_CODE_FAILED;
    }

    struct HksParamSet *param_set = NULL;
    int32_t status = init_import_lt_public_key_param_set(&param_set, user_type, pair_type, auth_id);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("init import lt public key input param set failed! status:%d", status);
        return status;
    }

    status = HksImportKey(&key_alias_blob, param_set, &ltpk_key_blob);

    HksFreeParamSet(&param_set);
    return status;
}

int32_t check_lt_public_key_exist(struct hc_key_alias *key_alias)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_num_return_val(key_alias->length, HC_INPUT_ERROR);
    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    int32_t hks_status = HksKeyExist(&key_alias_blob, NULL);
    if (hks_status == 0) {
        return ERROR_CODE_SUCCESS;
    } else {
        LOGI("Check lt public key exist failed, status = %d", hks_status);
        return ERROR_CODE_FAILED;
    }
}

int32_t check_key_exist(struct hc_key_alias *key_alias)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_num_return_val(key_alias->length, HC_INPUT_ERROR);
    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    int32_t hks_status = HksKeyExist(&key_alias_blob, NULL);
    if (hks_status == 0) {
        return ERROR_CODE_SUCCESS;
    } else {
        LOGI("Check key exist failed, status = %d", hks_status);
        return ERROR_CODE_FAILED;
    }
}

static int32_t init_key_info_list(struct HksKeyInfo *key_info_list, int32_t len)
{
    (void)memset_s(key_info_list, sizeof(struct HksKeyInfo) * len, 0, sizeof(struct HksKeyInfo) * len);
    for (int32_t i = 0; i < len; ++i) {
        struct HksKeyInfo key_info_tmp;

        key_info_tmp.alias.data = (uint8_t *)MALLOC(HC_KEY_ALIAS_MAX_LEN);
        if (key_info_tmp.alias.data == NULL) {
            LOGE("allocate space for key info alias data failed");
            return ERROR_CODE_NO_SPACE;
        }
        (void)memset_s(key_info_tmp.alias.data, HC_KEY_ALIAS_MAX_LEN, 0, HC_KEY_ALIAS_MAX_LEN);
        key_info_tmp.alias.size = HC_KEY_ALIAS_MAX_LEN;

        key_info_tmp.paramSet = (struct HksParamSet *)MALLOC(DEFAULT_PARAM_SET_OUT_SIZE);
        if (key_info_tmp.paramSet == NULL) {
            safe_free(key_info_tmp.alias.data);
            key_info_tmp.alias.data = NULL;
            LOGE("allocate space for key param set failed");
            return ERROR_CODE_NO_SPACE;
        }
        (void)memset_s(key_info_tmp.paramSet, DEFAULT_PARAM_SET_OUT_SIZE, 0, DEFAULT_PARAM_SET_OUT_SIZE);
        key_info_tmp.paramSet->paramSetSize = DEFAULT_PARAM_SET_OUT_SIZE;

        key_info_list[i] = key_info_tmp;
    }
    return 0;
}

static int32_t inner_get_lt_info_by_key_info(struct HksKeyInfo *key_info,
    struct huks_key_type *out_key_type, struct hc_auth_id *out_auth_id)
{
    union huks_key_type_union key_type_union;
    struct HksParam *key_role = NULL;
    int32_t status = HksGetParam(key_info->paramSet, HKS_TAG_KEY_ROLE, &key_role);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("get key role from param set failed, status:%d", status);
        return ERROR_CODE_FAILED;
    }

    key_type_union.key_type = key_role->uint32Param;
    out_key_type->user_type = key_type_union.type_struct.user_type;
    out_key_type->pair_type = key_type_union.type_struct.pair_type;
    out_key_type->reserved1 = key_type_union.type_struct.reserved1;
    out_key_type->reserved2 = key_type_union.type_struct.reserved2;

    struct HksParam *auth_id = NULL;
    status = HksGetParam(key_info->paramSet, HKS_TAG_KEY_AUTH_ID, &auth_id);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("get auth id from param set failed, status:%d", status);
        return ERROR_CODE_FAILED;
    }

    if (memcpy_s(out_auth_id->auth_id, HC_AUTH_ID_BUFF_LEN,
        auth_id->blob.data, auth_id->blob.size) != EOK) {
        LOGE("Copy key param failed");
        return ERROR_CODE_FAILED;
    }
    out_auth_id->length = auth_id->blob.size;

    return status;
}

static int32_t inner_get_lt_info_by_key_alias(struct HksBlob *key_alias,
    struct huks_key_type *out_key_type, struct hc_auth_id *out_auth_id)
{
    struct HksParamSet *output_param_set = (struct HksParamSet *)MALLOC(DEFAULT_PARAM_SET_OUT_SIZE);
    if (output_param_set == NULL) {
        LOGE("allocate space for param set out failed");
        return ERROR_CODE_FAILED;
    }
    (void)memset_s(output_param_set, DEFAULT_PARAM_SET_OUT_SIZE, 0, DEFAULT_PARAM_SET_OUT_SIZE);
    output_param_set->paramSetSize = DEFAULT_PARAM_SET_OUT_SIZE;

    int32_t status = HksGetKeyParamSet(key_alias, NULL, output_param_set);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Get huks key param set failed");
        goto get_key_info_free;
    }

    status = HksFreshParamSet(output_param_set, false);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("fresh param set failed, status:%d", status);
        goto get_key_info_free;
    }

    struct HksParam *key_role = NULL;
    status = HksGetParam(output_param_set, HKS_TAG_KEY_ROLE, &key_role);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("get key role from param set failed, status:%d", status);
        goto get_key_info_free;
    }

    union huks_key_type_union key_type_union;
    key_type_union.key_type = key_role->uint32Param;
    out_key_type->user_type = key_type_union.type_struct.user_type;
    out_key_type->pair_type = key_type_union.type_struct.pair_type;
    out_key_type->reserved1 = key_type_union.type_struct.reserved1;
    out_key_type->reserved2 = key_type_union.type_struct.reserved2;

    struct HksParam *auth_id = NULL;
    status = HksGetParam(output_param_set, HKS_TAG_KEY_AUTH_ID, &auth_id);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("get auth id from param set failed, status:%d", status);
        goto get_key_info_free;
    }

    if (memcpy_s(out_auth_id->auth_id, HC_AUTH_ID_BUFF_LEN, auth_id->blob.data, auth_id->blob.size) != EOK) {
        LOGE("Copy key param failed");
        goto get_key_info_free;
    }
    out_auth_id->length = auth_id->blob.size;

get_key_info_free:
    safe_free(output_param_set);
    return status;
}

int32_t get_lt_key_info(struct hc_key_alias *alias, struct huks_key_type *out_key_type, struct hc_auth_id *out_auth_id)
{
    check_ptr_return_val(alias, HC_INPUT_ERROR);
    check_ptr_return_val(out_key_type, HC_INPUT_ERROR);
    check_ptr_return_val(out_auth_id, HC_INPUT_ERROR);

    struct HksBlob alias_blob = convert_to_blob_from_hc_key_alias(alias);
    return inner_get_lt_info_by_key_alias(&alias_blob, out_key_type, out_auth_id);
}

int32_t check_key_alias_is_owner(struct hc_key_alias *key_alias)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_num_return_val(key_alias->length, HC_INPUT_ERROR);

    int32_t error_code = check_lt_public_key_exist(key_alias);
    if (error_code != ERROR_CODE_SUCCESS) {
        LOGE("Key is not exist");
        return error_code;
    }

    struct huks_key_type key_type;
    struct hc_auth_id auth_id;

    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    error_code = inner_get_lt_info_by_key_alias(&key_alias_blob, &key_type, &auth_id);
    if (error_code != ERROR_CODE_SUCCESS) {
        LOGE("Get key info failed");
        return error_code;
    }

    if (key_type.user_type != (uint8_t)HC_USER_TYPE_CONTROLLER) {
        return ERROR_CODE_FAILED;
    }
    if (key_type.pair_type == (uint8_t)HC_PAIR_TYPE_BIND) {
        return ERROR_CODE_SUCCESS;
    } else {
        return ERROR_CODE_FAILED;
    }
}

static uint32_t load_lt_public_key_list(const struct hc_auth_id *owner_id, int32_t trust_user_type,
    struct HksKeyInfo *key_info_list, uint32_t list_count, struct hc_auth_id *out_auth_list)
{
    uint8_t pair_type = owner_id == NULL ? (uint8_t)HC_PAIR_TYPE_BIND : (uint8_t)HC_PAIR_TYPE_AUTH;
    uint8_t user_type = (uint8_t)trust_user_type;
    int32_t err_code;
    struct huks_key_type key_type;
    struct hc_auth_id auth_id;
    uint32_t effect_count = 0;

    if ((trust_user_type < 0) || (trust_user_type >= HC_MAX_KEY_TYPE_NUM)) {
        return effect_count;
    }
    for (uint32_t i = 0; i < list_count; i++) {
        struct HksParam *key_flag_param = NULL;
        int32_t status = HksGetParam(key_info_list[i].paramSet, HKS_TAG_KEY_FLAG, &key_flag_param);
        if (status != ERROR_CODE_SUCCESS) {
            LOGE("get key flag from param set failed, status:%d", status);
            return ERROR_CODE_FAILED;
        }
        if (key_flag_param->uint32Param == HKS_KEY_FLAG_GENERATE_KEY) {
            continue;
        }
        err_code = inner_get_lt_info_by_key_info(&key_info_list[i], &key_type, &auth_id);
        if (err_code != ERROR_CODE_SUCCESS) {
            continue;
        }
        if (key_type.user_type != user_type) {
            continue;
        }
        if (user_type == (uint8_t)HC_USER_TYPE_CONTROLLER) {
            if (key_type.pair_type != pair_type) {
                continue;
            }
        }
        if (memcpy_s(out_auth_list[effect_count].auth_id, HC_AUTH_ID_BUFF_LEN,
                     auth_id.auth_id, auth_id.length) != EOK) {
            LOGE("Copy from temp hc_auth_id to out_auth_list failed");
            continue;
        }
        out_auth_list[effect_count].length = auth_id.length;
        effect_count++;
    }
    return effect_count;
}

int32_t get_lt_public_key_list(const struct hc_auth_id *owner_auth_id, int32_t trust_user_type,
    struct hc_auth_id *out_auth_list, uint32_t *out_count)
{
    check_ptr_return_val(out_auth_list, HC_INPUT_ERROR);
    check_ptr_return_val(out_count, HC_INPUT_ERROR);

    int32_t error_code = ERROR_CODE_SUCCESS;
    struct HksKeyInfo key_info_list[HC_PUB_KEY_ALIAS_MAX_NUM];
    int32_t status = init_key_info_list(key_info_list, HC_PUB_KEY_ALIAS_MAX_NUM);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Init key info list failed, status=%d", status);
        error_code = ERROR_CODE_FAILED;
        goto exit;
    }

    uint32_t list_count = HC_PUB_KEY_ALIAS_MAX_NUM;
    status = HksGetKeyInfoList(NULL, key_info_list, &list_count);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Huks get pub key info list failed, status=%d", status);
        error_code = ERROR_CODE_FAILED;
        goto exit;
    }

    /* filter with trust_user_type */
    uint32_t effect_count = load_lt_public_key_list(owner_auth_id, trust_user_type, key_info_list,
                                                    list_count, out_auth_list);
    /* output param */
    *out_count = effect_count;

exit:
    for (int32_t i = 0; i < HC_PUB_KEY_ALIAS_MAX_NUM; ++i) {
        safe_free(key_info_list[i].alias.data);
        safe_free(key_info_list[i].paramSet);
    }
    return error_code;
}

static int32_t gen_sign_key_param_set(struct HksParamSet **param_set)
{
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN /* correspond to old key usage */
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_ED25519 /* alg, correspond to old key type */
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for sign failed, status:%d", status);
    }

    return status;
}

int32_t sign(struct hc_key_alias *key_alias, const struct uint8_buff *message, struct signature *out_signature)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_ptr_return_val(message, HC_INPUT_ERROR);
    check_ptr_return_val(out_signature, HC_INPUT_ERROR);
    check_num_return_val(key_alias->length, HC_INPUT_ERROR);

    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    if (key_alias_blob.size == 0) {
        LOGE("Convert hks key alias to blob failed");
        return ERROR_CODE_FAILED;
    }

    struct sha256_value sha256_value = sha256(message);
    if (sha256_value.length == 0) {
        LOGE("Get sha256 hash failed");
        return ERROR_CODE_FAILED;
    }

    struct HksBlob hash = convert_to_blob_from_sha256_value(&sha256_value);
    if (hash.size == 0) {
        LOGE("Convert sha256 hash to blob failed");
        return ERROR_CODE_FAILED;
    }

    struct HksParamSet *key_param_set = NULL;
    int32_t hks_status = gen_sign_key_param_set(&key_param_set);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("gen sign key param set failed, status:%d", hks_status);
        return ERROR_CODE_FAILED;
    }

    struct HksBlob signature = { HC_SIGNATURE_LEN, out_signature->signature };
    hks_status = HksSign(&key_alias_blob, key_param_set, &hash, &signature);
    if ((hks_status == ERROR_CODE_SUCCESS) && (signature.size == HC_SIGNATURE_LEN)) {
        out_signature->length = HC_SIGNATURE_LEN;
    } else {
        LOGE("Sign failed, status=%d", hks_status);
        hks_status = ERROR_CODE_FAILED;
    }

    HksFreeParamSet(&key_param_set);
    return hks_status;
}

static int gen_verify_key_param_set(const bool is_keyalias, const uint32_t key_size,
    const int32_t user_type, struct HksParamSet **param_set)
{
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_ED25519
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_KEY_ROLE,
            .uint32Param = (uint32_t)user_type
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = is_keyalias
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = (is_keyalias ? 0 : key_size)
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for verify failed, status:%d", status);
    }

    return status;
}

int32_t verify(struct hc_key_alias *key_alias, const int32_t user_type,
    const struct uint8_buff *message, struct signature *signature)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_ptr_return_val(message, HC_INPUT_ERROR);
    check_ptr_return_val(signature, HC_INPUT_ERROR);
    check_num_return_val(key_alias->length, HC_INPUT_ERROR);

    int32_t error_code = ERROR_CODE_FAILED;
    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    if (key_alias_blob.size == 0) {
        LOGE("Convert hks key alias to blob failed");
        return error_code;
    }

    struct HksBlob signature_blob = convert_to_blob_from_signature(signature);
    if (signature_blob.size == 0) {
        LOGE("Convert hks signature to blob failed");
        return error_code;
    }

    struct sha256_value sha256_value = sha256(message);
    if (sha256_value.length == 0) {
        LOGE("Get sha256 hash failed. message val:%s, message length:%d", message->val, message->length);
        return error_code;
    }

    struct HksBlob hash = convert_to_blob_from_sha256_value(&sha256_value);
    if (hash.size == 0) {
        LOGE("Convert sha256 hash to blob failed");
        return error_code;
    }

    /* true: is key alias, 0: key alias have not key size */
    struct HksParamSet *key_param_set = NULL;
    int32_t hks_status = gen_verify_key_param_set(true, 0, user_type, &key_param_set);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("failed to gen verify key param set, status:%d", hks_status);
        return error_code;
    }

    hks_status = HksVerify(&key_alias_blob, key_param_set, &hash, &signature_blob);
    if (hks_status == 0) {
        error_code = ERROR_CODE_SUCCESS;
    } else {
        LOGE("Verify failed. status=%d", hks_status);
        if (check_lt_public_key_exist(key_alias) != ERROR_CODE_SUCCESS) {
            error_code = ERROR_CODE_NO_PEER_PUBLIC_KEY;
        }
    }

    HksFreeParamSet(&key_param_set);
    return error_code;
}

int32_t verify_with_public_key(const int32_t user_type, const struct uint8_buff *message,
    struct var_buffer *public_key, struct signature *signature)
{
    check_ptr_return_val(message, HC_INPUT_ERROR);
    check_ptr_return_val(public_key, HC_INPUT_ERROR);
    check_ptr_return_val(signature, HC_INPUT_ERROR);

    int32_t error_code = ERROR_CODE_FAILED;
    struct sha256_value sha256_value = sha256(message);
    if (sha256_value.length == 0) {
        LOGE("Get sha256 hash failed");
        return error_code;
    }

    struct HksBlob hash = convert_to_blob_from_sha256_value(&sha256_value);
    if (hash.size == 0) {
        LOGE("Convert sha256 hash to blob failed");
        return error_code;
    }

    struct HksBlob signature_blob = convert_to_blob_from_signature(signature);
    if (signature_blob.size == 0) {
        LOGE("Convert hks signature to blob failed");
        return error_code;
    }

    struct HksParamSet *key_param_set = NULL;
    int32_t hks_status = gen_verify_key_param_set(false, /* false: is public key */
        public_key->length * BITS_PER_BYTE, user_type, &key_param_set);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("gen verify with public key param set failed, status:%d", hks_status);
        return error_code;
    }

    struct HksBlob public_key_blob = { public_key->length, public_key->data };
    hks_status = HksVerify(&public_key_blob, key_param_set, &hash, &signature_blob);
    if (hks_status == 0) {
        error_code = ERROR_CODE_SUCCESS;
    } else {
        LOGE("Verify failed, status=%d", hks_status);
        error_code = ERROR_CODE_FAILED;
    }

    HksFreeParamSet(&key_param_set);
    return error_code;
}

static int32_t gen_agreed_key_param_set(struct HksParamSet **param_set)
{
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_DERIVE
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_X25519
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = HC_ST_PUBLIC_KEY_LEN * BITS_PER_BYTE
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = false
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for agreed key failed, status:%d", status);
    }

    return status;
}

int32_t compute_sts_shared_secret(struct stsk *self_private_key, struct stpk *peer_public_key,
    struct sts_shared_secret *out_shared_key)
{
    check_ptr_return_val(self_private_key, HC_INPUT_ERROR);
    check_num_return_val(self_private_key->length, HC_INPUT_ERROR);
    check_ptr_return_val(peer_public_key, HC_INPUT_ERROR);
    check_num_return_val(peer_public_key->length, HC_INPUT_ERROR);
    check_ptr_return_val(out_shared_key, HC_INPUT_ERROR);

    (void)memset_s(out_shared_key, sizeof(*out_shared_key), 0, sizeof(*out_shared_key));
    int32_t error_code = ERROR_CODE_FAILED;
    struct HksBlob self_private_key_blob = convert_to_blob_from_stsk(self_private_key);
    if (self_private_key_blob.size == 0) {
        LOGE("Convert key alias for private key to blob failed");
        return error_code;
    }

    struct HksBlob peer_public_key_blob = convert_to_blob_from_stpk(peer_public_key);
    if (peer_public_key_blob.size == 0) {
        LOGE("Convert key alias for peer public key to blob failed");
        return error_code;
    }

    struct HksParamSet *param_set = NULL;
    int32_t hks_status = gen_agreed_key_param_set(&param_set);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("gen agreed key param set failed! status:%d", hks_status);
        return error_code;
    }

    struct HksBlob key_alias_for_agreed_key = { HC_STS_SHARED_SECRET_LENGTH, out_shared_key->sts_shared_secret };
    hks_status = HksAgreeKey(param_set, &self_private_key_blob, &peer_public_key_blob, &key_alias_for_agreed_key);
    if ((hks_status == 0) && (key_alias_for_agreed_key.size == HC_STS_SHARED_SECRET_LENGTH)) {
        out_shared_key->length = key_alias_for_agreed_key.size;
        error_code = ERROR_CODE_SUCCESS;
    } else {
        LOGE("Key agreement by alias failed, status:%d", hks_status);
    }
    return error_code;
}

int32_t key_info_init(void)
{
    int32_t ret = HksInitialize();
    if (ret == HKS_SUCCESS) {
        return ERROR_CODE_SUCCESS;
    }

    if ((ret != HKS_ERROR_INVALID_KEY_FILE) && (ret != HKS_ERROR_CRYPTO_ENGINE_ERROR) &&
        (ret != HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL)) {
        LOGE("Hks: Init hks failed, ret: %d", ret);
        return ERROR_CODE_FAILED;
    }

    DBG_OUT("Hks: The local hks file needs to be refreshed!");
    LOGI("Start to delete local database file!");
    ret = HksRefreshKeyInfo();
    if (ret != HKS_SUCCESS) {
        LOGE("Hks: HksRefreshKeyInfo failed, ret:%d", ret);
        return ERROR_CODE_FAILED;
    }
    ret = HksInitialize();
    if (ret != HKS_SUCCESS) {
        LOGE("Hks: Init hks failed, ret:%d", ret);
        return ERROR_CODE_FAILED;
    }
    return ERROR_CODE_SUCCESS;
}

#if (defined(_SUPPORT_SEC_CLONE_) || defined(_SUPPORT_SEC_CLONE_SERVER_))
static int32_t init_aes_ccm_decrypt_key_params(struct HksParamSet **param_set,
    const struct uint8_buff *cipher, const struct aes_aad *aad)
{
    struct HksBlob hks_nonce = { HC_CCM_NONCE_LEN, cipher->val };
    struct uint8_buff aad_data = { (uint8_t *)aad->aad, aad->length, aad->length };
    struct sha256_value aad_sha = sha256(&aad_data);
    struct HksBlob hks_aad = { aad_sha.length, aad_sha.sha256_value };

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_DECRYPT
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES
        }, {
            .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_CCM
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = HKS_AES_KEY_SIZE_256
        }, {
            .tag = HKS_TAG_NONCE,
            .blob = hks_nonce
        }, {
            .tag = HKS_TAG_ASSOCIATED_DATA,
            .blob = hks_aad
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for AES CCM decrypt key failed");
    }

    return status;
}

int32_t aes_ccm_decrypt(struct var_buffer *key, const struct uint8_buff *cipher,
    struct aes_aad *aad, struct uint8_buff *out_plain)
{
    check_ptr_return_val(key, HC_INPUT_ERROR);
    check_ptr_return_val(cipher, HC_INPUT_ERROR);
    check_ptr_return_val(aad, HC_INPUT_ERROR);
    check_ptr_return_val(out_plain, HC_INPUT_ERROR);
    if (cipher->length < HC_AES_GCM_NONCE_LEN) {
        LOGE("cipher length is no larger than salt size");
        return ERROR_CODE_FAILED;
    }

    struct HksParamSet *key_param_set = NULL;
    int32_t hks_status = init_aes_ccm_decrypt_key_params(&key_param_set, cipher, aad);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("init aes ccm decrypt key param set failed, status:%d", hks_status);
        return ERROR_CODE_FAILED;
    }

    struct HksBlob hks_key = { key->length, key->data };
    struct HksBlob cipher_text_with_tag = { cipher->length - HC_CCM_NONCE_LEN, cipher->val + HC_CCM_NONCE_LEN };
    struct HksBlob plain_text = { out_plain->size, out_plain->val };

    hks_status = HksDecrypt(&hks_key, key_param_set, &cipher_text_with_tag, &plain_text);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("Huks aead decrypt failed, status:%d", hks_status);
        HksFreeParamSet(&key_param_set);
        return ERROR_CODE_FAILED;
    }
    out_plain->length = plain_text.size;

    HksFreeParamSet(&key_param_set);
    return hks_status;
}

static int32_t gen_lt_x25519_key_param_set(struct HksParamSet **param_set, const struct hc_auth_id *auth_id)
{
    struct hc_auth_id tmp_id = *auth_id;
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_KEY_STORAGE_FLAG,
            .uint32Param = HKS_STORAGE_PERSISTENT
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_X25519
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = HC_PARAM_KEY_LEN
        }, {
            .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = convert_to_blob_from_hc_auth_id(&tmp_id)
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for lt x25519 key failed");
    }

    return status;
}

int32_t generate_lt_X25519_key_pair(struct hc_key_alias *key_alias, const struct hc_auth_id *auth_id)
{
    check_ptr_return_val(key_alias, HC_INPUT_ERROR);
    check_ptr_return_val(auth_id, HC_INPUT_ERROR);

    struct HksBlob key_alias_blob = convert_to_blob_from_hc_key_alias(key_alias);
    check_num_return_val(key_alias_blob.size, ERROR_CODE_FAILED);

    struct HksParamSet *key_param_set = NULL;
    int32_t status = gen_lt_x25519_key_param_set(&key_param_set, auth_id);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("init lt x25519 key pair param set failed, status:%d", status);
        return ERROR_CODE_FAILED;
    }

    status = HksGenerateKey(&key_alias_blob, key_param_set, NULL);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Hks generate failed, status:%d", status);
    }

    if (HksKeyExist(&key_alias_blob, NULL) == 0) {
        LOGI("Generate key success, key exist");
    }

    HksFreeParamSet(&key_param_set);
    return status;
}

static int32_t gen_key_attestation_param_set(struct HksParamSet **param_set,
    const struct uint8_buff *challenge, struct HksBlob *key_alias_blob)
{
    struct HksBlob hks_challenge = { challenge->length, challenge->val };
    struct HksBlob temp_key_alias = { key_alias_blob->size, key_alias_blob->data };
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_X25519
        }, {
            .tag = HKS_TAG_PADDING, /* interface regulations */
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = HC_PARAM_CHAIN_LEN
        }, {
            .tag = HKS_TAG_ATTESTATION_CHALLENGE,
            .blob = hks_challenge
        }, {
            .tag = HKS_TAG_KEY_STORAGE_FLAG,
            .uint32Param = HKS_STORAGE_PERSISTENT
        }, {
            .tag = HKS_TAG_ATTESTATION_ID_ALIAS,
            .blob = temp_key_alias
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for attestation key failed");
    }

    return status;
}

int32_t get_key_attestation(const struct uint8_buff *challenge, struct hc_key_alias *sk_alias,
    struct uint8_buff *out_cert_chain)
{
    int32_t error_code = HC_INPUT_ERROR;

    check_ptr_return_val(challenge, error_code);
    check_ptr_return_val(sk_alias, error_code);
    check_ptr_return_val(out_cert_chain, error_code);

    struct HksBlob sk_alias_blob = { sk_alias->length, sk_alias->key_alias };
    struct HksParamSet *key_param_set = NULL;
    int32_t hks_status = gen_key_attestation_param_set(&key_param_set, challenge, &sk_alias_blob);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("gen key attestation param set failed, status:%d", hks_status);
        return ERROR_CODE_FAILED;
    }

    struct HksBlob cert[g_cert_chain_cnt];
    for (int32_t i = 0; i < g_cert_chain_cnt; ++i) {
        struct HksBlob certBlob;
        certBlob.data = out_cert_chain[i].val;
        certBlob.size = out_cert_chain[i].length;
        cert[i] = certBlob;
    }

    struct HksCertChain hks_cert_chains = { cert, g_cert_chain_cnt };

    hks_status = HksAttestKey(&sk_alias_blob, key_param_set, &hks_cert_chains);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("Key attestaion failed, errCode is %d", hks_status);
    }

    LOGI("hks_key_attestation finish");

    for (int32_t i = 0; i < g_cert_chain_cnt; ++i) {
        out_cert_chain[i].length = hks_cert_chains.certs[i].size;
    }

    HksFreeParamSet(&key_param_set);
    return hks_status;
}

static int32_t gen_get_cert_chain_param_set(struct HksParamSet **param_set)
{
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_ECC
        }, {
            .tag = HKS_TAG_PADDING, /* interface regulations */
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = HC_PARAM_CHAIN_LEN
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for attestation key failed");
    }

    return status;
}

int32_t get_cert_chain(const struct uint8_buff *challenge,
    struct hc_key_alias *sk_alias, struct uint8_buff *out_cert_chain)
{
    int32_t error_code = HC_INPUT_ERROR;

    check_ptr_return_val(challenge, error_code);
    check_ptr_return_val(sk_alias, error_code);
    check_ptr_return_val(out_cert_chain, error_code);

    struct HksBlob hks_key_alias = { sk_alias->length, sk_alias->key_alias };
    struct HksParamSet *key_param_set = NULL;
    int32_t hks_status = gen_get_cert_chain_param_set(&key_param_set);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("gen get cert chain param set failed, status:%d", hks_status);
        return ERROR_CODE_FAILED;
    }

    int8_t malloc_flag = 1; /* malloc ok */
    struct HksBlob cert[g_cert_chain_cnt];
    (void)memset_s(cert, sizeof(cert), 0, sizeof(cert));
    const uint32_t cert_buff_size = 8192; /* each certificate buff max size */
    for (int32_t i = 0; i < g_cert_chain_cnt; ++i) {
        cert[i].data = (uint8_t *)MALLOC(cert_buff_size);
        if (cert[i].data == NULL) {
            malloc_flag = 0;
            break;
        }
        (void)memset_s(cert[i].data, cert_buff_size, 0, cert_buff_size);
        cert[i].size = cert_buff_size;
    }

    if (malloc_flag == 0) {
        for (int32_t i = 0; i < g_cert_chain_cnt; ++i) {
            if (cert[i].data != NULL) {
                FREE(cert[i].data);
            }
        }
        HksFreeParamSet(&key_param_set);
        return HC_MALLOC_FAILED;
    }

    struct HksCertChain hks_cert_chains = { cert, g_cert_chain_cnt };

    hks_status = HksGetCertificateChain(&hks_key_alias, key_param_set, &hks_cert_chains);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("Get cert chain failed, errCode is %d", hks_status);
    }

    out_cert_chain->length = hks_cert_chains.certs[0].size;
    (void)memset_s(out_cert_chain->val, out_cert_chain->length, 0, out_cert_chain->length);
    if (memcpy_s(out_cert_chain->val, out_cert_chain->length, hks_cert_chains.certs[0].data,
        hks_cert_chains.certs[0].size) != EOK) {
        LOGE("Copy cert chain failed!");
    }

    for (int32_t i = 0; i < g_cert_chain_cnt; ++i) {
        FREE(hks_cert_chains.certs[i].data);
    }

    HksFreeParamSet(&key_param_set);
    return hks_status;
}

static int32_t gen_asset_unwrap_param_set(struct HksParamSet **param_set, struct HksBlob *aad)
{
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_UNWRAP
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_KEY_TYPE_AES
        }, {
            .tag = HKS_TAG_PADDING, /* interface regulations */
            .uint32Param = HKS_PADDING_PKCS7
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = HC_PARAM_KEY_LEN
        }, {
            .tag = HKS_TAG_ASSOCIATED_DATA,
            .blob = *aad
        }
    };

    int32_t status = construct_param_set(param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for attestation key failed");
    }

    return status;
}

static int32_t get_asset_aad_wrap(struct uint8_buff *sec_data, struct HksBlob *aad, struct HksBlob *wrap_data)
{
    /* sec_data = asset_len + asset_data + wrap_data_len + wrap_data */
    if (sec_data->length < sizeof(unsigned int)) {
        return ERROR_CODE_FAILED;
    }

    unsigned int asset_len = *(uint32_t *)(sec_data->val);

    uint32_t offset = sizeof(asset_len);
    if (sec_data->length < (offset + asset_len)) {
        return ERROR_CODE_FAILED;
    }

    struct uint8_buff asset_data = { sec_data->val + offset, asset_len, asset_len };
    struct sha256_value asset_sha = sha256(&asset_data);
    if (memcpy_s(aad->data, aad->size, asset_sha.sha256_value, asset_sha.length) != EOK) {
        return ERROR_CODE_FAILED;
    }
    aad->size = asset_sha.length;

    offset += asset_len;
    if (sec_data->length < (offset + sizeof(unsigned int))) {
        return ERROR_CODE_FAILED;
    }

    unsigned int wrap_len = *(uint32_t *)(sec_data->val + offset);

    offset += sizeof(wrap_len);
    if (sec_data->length < (offset + wrap_len)) {
        return ERROR_CODE_FAILED;
    }

    wrap_data->size = wrap_len;
    wrap_data->data = sec_data->val + offset;

    return ERROR_CODE_SUCCESS;
}

int32_t asset_unwrap(struct uint8_buff *sec_data, struct hc_key_alias *dec_alias,
    struct hc_key_alias *target_alias)
{
    int32_t error_code = HC_INPUT_ERROR;

    check_ptr_return_val(sec_data, error_code);
    check_ptr_return_val(dec_alias, error_code);
    check_ptr_return_val(target_alias, error_code);

    struct HksBlob aad = { HC_SHA256_LEN, NULL };
    struct HksBlob wrap_data = { 0, NULL };

    aad.data = (uint8_t *)MALLOC(HC_SHA256_LEN);
    if (aad.data == NULL) {
        return ERROR_CODE_FAILED;
    }
    (void)memset_s(aad.data, HC_SHA256_LEN, 0, HC_SHA256_LEN);

    int32_t status = get_asset_aad_wrap(sec_data, &aad, &wrap_data);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("get asset aad and wrap data failed, status:%d", status);
        safe_free(aad.data);
        return ERROR_CODE_FAILED;
    }

    struct HksBlob hks_dec_alias = { dec_alias->length, dec_alias->key_alias };
    struct HksBlob hks_target_alias = { target_alias->length, target_alias->key_alias };
    struct HksParamSet *key_param_set = NULL;
    status = gen_asset_unwrap_param_set(&key_param_set, &aad);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("gen asset unwrap param set failed, status:%d", status);
        safe_free(aad.data);
        HksFreeParamSet(&key_param_set);
        return ERROR_CODE_FAILED;
    }

    status = HksUnwrapKey(&hks_dec_alias, &hks_target_alias, &wrap_data, key_param_set);
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("Hks unwrap failed, errCode is %d", status);
    }

    safe_free(aad.data);
    HksFreeParamSet(&key_param_set);
    return status;
}

static int32_t gen_derived_key_param_set(struct HksParamSet **hks_param_set,
    struct hc_key_alias *base_alias, struct HksBlob derive_factor)
{
    struct hc_auth_id temp_auth_id = { HC_AUTH_ID_BUFF_LEN, { 0 } };

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES
        }, {
            .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_CCM
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = HC_PARAM_KEY_LEN
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = { temp_auth_id.length, temp_auth_id.auth_id }
        }, {
            .tag = HKS_TAG_KEY_STORAGE_FLAG,
            .uint32Param = HKS_STORAGE_PERSISTENT
        }, {
            .tag = HKS_TAG_DERIVE_MAIN_KEY,
            .blob = { base_alias->length, base_alias->key_alias }
        }, {
            .tag = HKS_TAG_DERIVE_FACTOR,
            .blob = derive_factor
        }, {
            .tag = HKS_TAG_KEY_GENERATE_TYPE,
            .uint32Param = HKS_KEY_GENERATE_TYPE_DERIVE
        }
    };

    int32_t status = construct_param_set(hks_param_set, params, array_size(params));
    if (status != ERROR_CODE_SUCCESS) {
        LOGE("construct param set for attestation key failed");
    }

    return status;
}

int32_t gen_derived_key(struct hc_key_alias *base_alias, struct hc_key_alias *to_save_alias)
{
    check_ptr_return_val(base_alias, HC_INPUT_ERROR);
    check_num_return_val(to_save_alias, HC_INPUT_ERROR);
    if (check_lt_public_key_exist(base_alias) != HC_OK) {
        LOGE("Base key is not exist");
        return HC_INPUT_ERROR;
    }

    if (check_lt_public_key_exist(to_save_alias) == HC_OK) {
        LOGW("Derive key exists, will be genetate again");
    }

    struct HksBlob hks_target_alias = { to_save_alias->length, to_save_alias->key_alias };

    struct HksParamSet *key_param_set = NULL;

    uint32_t factor_size = strlen((char *)g_factor);
    struct HksBlob derive_factor = { factor_size, NULL };

    derive_factor.data = (uint8_t *)MALLOC(factor_size + 1);
    if (derive_factor.data == NULL) {
        return HC_MALLOC_FAILED;
    }
    if (memcpy_s(derive_factor.data, factor_size, g_factor, factor_size) != EOK) {
        safe_free(derive_factor.data);
        return memory_copy_error(__func__, __LINE__);
    }

    derive_factor.data[factor_size] = '\0';

    int32_t hks_status = gen_derived_key_param_set(&key_param_set, base_alias, derive_factor);
    if (hks_status != ERROR_CODE_SUCCESS) {
        HksFreeParamSet(&key_param_set);
        safe_free(derive_factor.data);
        LOGE("gen asset unwrap param set failed, status:%d", hks_status);
        return hks_status;
    }

    hks_status = HksGenerateKey(&hks_target_alias, key_param_set, NULL);
    if (hks_status != ERROR_CODE_SUCCESS) {
        LOGE("generate symmetric key failed, status:%d", hks_status);
    }

    HksFreeParamSet(&key_param_set);
    safe_free(derive_factor.data);
    return hks_status;
}
#endif
