/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "mbedtls_hash_to_point.h"
#include <stdio.h>
#include <mbedtls/bignum.h>
#include "securec.h"
#include "alg_defs.h"
#include "hal_error.h"
#include "hc_log.h"

#define LEN_HALF_DIVISOR 2
#define PARAM_A_INDEX 2
#define PARAM_U_INDEX 4
#define PARAM_MINUS_A_INDEX 3
#define PARAM_ONE_INDEX 5
#define HASH_TO_POINT_PARA_NUMS 6

#define LOG_AND_GOTO_CLEANUP_IF_FAIL(ret, fmt, ...) \
do { \
    if ((ret) != 0) { \
        LOGE(fmt, ##__VA_ARGS__); \
        goto CLEAN_UP; \
    } \
} while (0)

static uint8_t g_hash2pointParas[HASH_TO_POINT_PARA_NUMS][BYTE_LENGTH_CURVE_25519] = {
    { 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    /* 0:p */
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xed },
    { 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    /* 1:divide_minus_p_1_2 */
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf6 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 2:A */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x6d, 0x06 },
    { 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,    /* 3:-A */
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x92, 0xe7 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 4:u = 2 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    /* 5:1 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
};

static void FreeCleanFiveBns(mbedtls_mpi *bn1, mbedtls_mpi *bn2, mbedtls_mpi *bn3,
                             mbedtls_mpi *bn4, mbedtls_mpi *bn5)
{
    mbedtls_mpi_free(bn1);
    mbedtls_mpi_free(bn2);
    mbedtls_mpi_free(bn3);
    mbedtls_mpi_free(bn4);
    mbedtls_mpi_free(bn5);
}

static void SwapEndian(uint8_t *pubKey, int len)
{
    if ((pubKey == NULL) || (len <= 0)) {
        return;
    }
    for (int i = 0; i < len / LEN_HALF_DIVISOR; ++i) {
        uint8_t tmp = pubKey[i];
        pubKey[i] = pubKey[len - i - 1];
        pubKey[len - i - 1] = tmp;
    }
}

static bool IsInvalidBlob(const Uint8Buff *blob)
{
    return blob == NULL || blob->val == NULL || blob->length == 0;
}

static int32_t CalTmpParaX(mbedtls_mpi *tmpY, const mbedtls_mpi *tmpX, const mbedtls_mpi *modP)
{
    int32_t status;
    mbedtls_mpi tmpBnA;
    mbedtls_mpi tmpBnB;
    mbedtls_mpi tmpBnC;
    mbedtls_mpi tmpBnE;
    mbedtls_mpi paraBnA;
    mbedtls_mpi paraBnU;

    mbedtls_mpi_init(&tmpBnA);
    mbedtls_mpi_init(&tmpBnB);
    mbedtls_mpi_init(&tmpBnC);
    mbedtls_mpi_init(&tmpBnE);
    mbedtls_mpi_init(&paraBnA);
    mbedtls_mpi_init(&paraBnU);

    status = mbedtls_mpi_read_binary(&paraBnA, g_hash2pointParas[PARAM_A_INDEX], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error1");

    status = mbedtls_mpi_read_binary(&paraBnU, g_hash2pointParas[PARAM_U_INDEX], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error2");

    status = mbedtls_mpi_copy(&tmpBnB, tmpX);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error3");
    /* a := b ^ 3 + A * b ^ 2 + b */

    status = mbedtls_mpi_exp_mod(&tmpBnE, &tmpBnB, &paraBnU, modP, NULL);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error4"); // b^2

    status = mbedtls_mpi_mul_mpi(&tmpBnC, &tmpBnE, &tmpBnB);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error5");
    status = mbedtls_mpi_mod_mpi(&tmpBnC, &tmpBnC, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error6"); // b^3

    status = mbedtls_mpi_mul_mpi(&tmpBnA, &tmpBnE, &paraBnA); // A*b^2
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error7");
    status = mbedtls_mpi_mod_mpi(&tmpBnA, &tmpBnA, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error8");

    status = mbedtls_mpi_add_mpi(&tmpBnE, &tmpBnC, &tmpBnA); // b^3 + A*b^2
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error9");
    status = mbedtls_mpi_mod_mpi(&tmpBnE, &tmpBnE, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error10");

    status = mbedtls_mpi_add_mpi(tmpY, &tmpBnE, &tmpBnB); // b^3 + A*b^2 + b
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error11");
    status = mbedtls_mpi_mod_mpi(tmpY, tmpY, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParaX error12");

CLEAN_UP:
    FreeCleanFiveBns(&paraBnA, &tmpBnA, &tmpBnB, &tmpBnC, &tmpBnE);
    mbedtls_mpi_free(&paraBnU);
    return status;
}

static int32_t CalTmpParab(mbedtls_mpi *tmpX, const mbedtls_mpi *modP, const uint8_t *hash, uint32_t hashLen)
{
    int32_t status;
    mbedtls_mpi paraBnNegA;
    mbedtls_mpi paraBNOne;
    mbedtls_mpi paraBnU;
    mbedtls_mpi tmpBnA;
    mbedtls_mpi tmpBnB;

    mbedtls_mpi_init(&paraBnNegA);
    mbedtls_mpi_init(&paraBNOne);
    mbedtls_mpi_init(&paraBnU);
    mbedtls_mpi_init(&tmpBnA);
    mbedtls_mpi_init(&tmpBnB);

    status = mbedtls_mpi_read_binary(&paraBnNegA, g_hash2pointParas[PARAM_MINUS_A_INDEX], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error1");
    status = mbedtls_mpi_read_binary(&paraBNOne, g_hash2pointParas[PARAM_ONE_INDEX], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error2");
    status = mbedtls_mpi_read_binary(&paraBnU, g_hash2pointParas[PARAM_U_INDEX], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error3");
    status = mbedtls_mpi_read_binary(&tmpBnA, hash, hashLen);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error4");

    /* b := -A / (1 + u * a ^ 2) */
    status = mbedtls_mpi_exp_mod(&tmpBnB, &tmpBnA, &paraBnU, modP, NULL);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error5");

    status = mbedtls_mpi_mul_mpi(&tmpBnA, &tmpBnB, &paraBnU);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error6");
    status = mbedtls_mpi_mod_mpi(&tmpBnA, &tmpBnA, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error7");

    status = mbedtls_mpi_add_mpi(&tmpBnB, &tmpBnA, &paraBNOne);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error8");
    status = mbedtls_mpi_mod_mpi(&tmpBnB, &tmpBnB, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error9");

    status = mbedtls_mpi_inv_mod(&tmpBnA, &tmpBnB, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error10");

    status = mbedtls_mpi_mul_mpi(tmpX, &tmpBnA, &paraBnNegA);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error11");
    status = mbedtls_mpi_mod_mpi(tmpX, tmpX, modP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "CalTmpParab error12");
CLEAN_UP:
    FreeCleanFiveBns(&paraBnNegA, &paraBNOne, &paraBnU, &tmpBnA, &tmpBnB);
    return status;
}

/*
 * hash2point function, use BoringSSL big number algorithm library;
 * p_point(little endian): the output pointer of Curve25519 point;
 * p_hash (little endian): the input pointer of string;
 */
static int32_t Elligator(unsigned char *point, int pointLength, const unsigned char *hash, int hashLength)
{
    mbedtls_mpi paraBnP;
    mbedtls_mpi paraBnSquare;
    mbedtls_mpi paraBnNegA;
    mbedtls_mpi tmpBnA;
    mbedtls_mpi tmpBnB;
    mbedtls_mpi tmpBnC;
    mbedtls_mpi tmpBnE;

    mbedtls_mpi_init(&paraBnP);
    mbedtls_mpi_init(&paraBnSquare);
    mbedtls_mpi_init(&paraBnNegA);
    mbedtls_mpi_init(&tmpBnA);
    mbedtls_mpi_init(&tmpBnB);
    mbedtls_mpi_init(&tmpBnC);
    mbedtls_mpi_init(&tmpBnE);

    int32_t status = mbedtls_mpi_read_binary(&paraBnP, g_hash2pointParas[0], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error1");
    status = mbedtls_mpi_read_binary(&paraBnSquare, g_hash2pointParas[1], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error2");
    status = mbedtls_mpi_read_binary(&paraBnNegA, g_hash2pointParas[PARAM_MINUS_A_INDEX], BYTE_LENGTH_CURVE_25519);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error3");

    status = CalTmpParab(&tmpBnB, &paraBnP, hash, hashLength);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error4");
    status = CalTmpParaX(&tmpBnA, &tmpBnB, &paraBnP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error5");

    status = mbedtls_mpi_sub_mpi(&tmpBnC, &paraBnP, &tmpBnB);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error6");
    status = mbedtls_mpi_mod_mpi(&tmpBnC, &tmpBnC, &paraBnP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error7");

    status = mbedtls_mpi_add_mpi(&tmpBnC, &tmpBnC, &paraBnNegA);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error8");
    status = mbedtls_mpi_mod_mpi(&tmpBnC, &tmpBnC, &paraBnP);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error9");

    status = mbedtls_mpi_exp_mod(&tmpBnE, &tmpBnA, &paraBnSquare, &paraBnP, NULL);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error10");

    uint8_t swap = (mbedtls_mpi_cmp_mpi(&paraBnSquare, &tmpBnE) == 1);
    status = mbedtls_mpi_safe_cond_swap(&tmpBnB, &tmpBnC, swap);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error11");

    status = mbedtls_mpi_write_binary(&tmpBnC, point, pointLength);
    LOG_AND_GOTO_CLEANUP_IF_FAIL(status, "Elligator error12");

    SwapEndian(point, pointLength);
CLEAN_UP:
    mbedtls_mpi_free(&paraBnP);
    mbedtls_mpi_free(&paraBnSquare);
    FreeCleanFiveBns(&paraBnNegA, &tmpBnA, &tmpBnB, &tmpBnC, &tmpBnE);
    return status;
}

int32_t MbedtlsHashToPoint(const Uint8Buff *hash, Uint8Buff *outEcPoint)
{
    if (IsInvalidBlob(hash) || IsInvalidBlob(outEcPoint)) {
        return HAL_ERR_INVALID_PARAM;
    }
    if (hash->length != BYTE_LENGTH_CURVE_25519 || outEcPoint->length != BYTE_LENGTH_CURVE_25519) {
        LOGE("MbedtlsHashToPoint invalid length.");
        return HAL_ERR_INVALID_PARAM;
    }
    uint8_t hashTmp[BYTE_LENGTH_CURVE_25519] = {0};
    (void)memcpy_s(hashTmp, BYTE_LENGTH_CURVE_25519, hash->val, BYTE_LENGTH_CURVE_25519);

    hashTmp[BYTE_LENGTH_CURVE_25519 - 1] &= ~HASH2POINT_PARA_PREPRO;
    SwapEndian(hashTmp, BYTE_LENGTH_CURVE_25519);
    int status = Elligator(outEcPoint->val, BYTE_LENGTH_CURVE_25519, hashTmp, BYTE_LENGTH_CURVE_25519);
    if (status != 0) {
        LOGE("Elligator failed, status:%d", status);
    }
    return status;
}
