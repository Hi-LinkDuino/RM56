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

/*
 * Description: 计算文件SHA256校验和的基本方法
 */

#include "checksum_sha256.h"

#include <memory.h>
#include <stdlib.h>

static const unsigned int G_DEFAULT_KEY[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

enum WordIndex {
    A = 0, B, C, D, E, F, G, H
};

enum NumIndex {
    ZERO = 0, ONE, TWO, THREE, FOUR
};

/**
 * 右旋转
 */
unsigned int RotateRight(unsigned int w, int n)
{
    int len = 32;
    return (w >> n) | ((w)<<(len - n));
}

/**
 * 左旋转
 */
unsigned int RotateLeft(unsigned int w, int n)
{
    int len = 32;
    return (w<<n) | ((w)>>(len - n));
}

/**
 * 右移位
 */
unsigned int ShiftRight(unsigned int w, int n)
{
    return w >> n;
}

/**
 * 计算SHA256中的参数Sigma0
 */
unsigned int CalcSigma0(unsigned int x)
{
    unsigned int shift[] = {7, 18, 3};
    return RotateRight(x, shift[ZERO]) ^ RotateRight(x, shift[ONE]) ^ ShiftRight(x, shift[TWO]);
}

/**
 * 计算SHA256中的参数Sigma1
 */
unsigned int CalcSigma1(unsigned int x)
{
    unsigned int shift[] = {17, 19, 10};
    return RotateRight(x, shift[ZERO]) ^ RotateRight(x, shift[ONE]) ^ ShiftRight(x, shift[TWO]);
}

/**
 * 计算SH256中的参数Ch
 */
unsigned int CalcChValue(unsigned int x, unsigned int y, unsigned int z)
{
    return (x & y) ^ (~(x) & z);
}

/**
 * 计算SH256中的参数MAJ
 */
unsigned int CalcMajValue(unsigned int x, unsigned int y, unsigned int z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

/**
 * 计算SHA256中的参数Ep0
 */
unsigned int CalcEp0Value(unsigned int x)
{
    unsigned int shift[] = {2, 13, 22};
    return RotateRight(x, shift[ZERO]) ^ RotateRight(x, shift[ONE]) ^ RotateRight(x, shift[TWO]);
}

/**
 * 计算SHA256中的参数Ep1
 */
unsigned int CalcEp1Value(unsigned int x)
{
    unsigned int shift[] = {6, 11, 25};
    return RotateRight(x, shift[ZERO]) ^ RotateRight(x, shift[ONE]) ^ RotateRight(x, shift[TWO]);
}

/**
 * 初始化SHA256方法
 */
void InitSha256(MesgDigest *md)
{
    int len = 8;
    unsigned int hashInit[] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    md->dataLen = 0;
    md->bitLen = 0;

    for (int i = 0; i < len; i++) {
        md->hash[i] = hashInit[i];
    }
}

/**
 * 计算字符数组的SHA256校验和
 */
void CalcSha256(MesgDigest *md, unsigned char* mesg)
{
    int len = 8;
    int wordLen = 64;
    unsigned int t1, t2, t3, t4;
    unsigned int word[wordLen], hash[len];
    unsigned int hashT1, hashT2;
    int j = 0;
    int lenList[] = {4, 8, 16, 24};
    int pos[] = {15, 2, 7, 16};

    for (int i = 0; i < wordLen; i++) {
        if (i < lenList[TWO]) {
            t1 = mesg[j] << lenList[THREE];
            t2 = mesg[j + ONE] << lenList[TWO];
            t3 = mesg[j + TWO] << lenList[ONE];
            t4 = mesg[j + THREE];
            word[i] = t1 | t2 | t3 | t4;
            j += lenList[ZERO];
        } else {
            unsigned int s0 = CalcSigma0(word[i - pos[ZERO]]);
            unsigned int s1 = CalcSigma1(word[i - pos[ONE]]);
            word[i] = s1 + word[i - pos[TWO]] + s0 + word[i - pos[THREE]];
        }
    }

    for (int i = 0; i < len; i++) {
        hash[i] = md->hash[i];
    }

    for (int i = 0; i < wordLen; ++i) {
        hashT1 = hash[H] + CalcEp1Value(hash[E]) + CalcChValue(hash[E], hash[F], hash[G]) + G_DEFAULT_KEY[i] + word[i];
        hashT2 = CalcEp0Value(hash[A]) + CalcMajValue(hash[A], hash[B], hash[C]);
        hash[H] = hash[G];
        hash[G] = hash[F];
        hash[F] = hash[E];
        hash[E] = hash[D] + hashT1;
        hash[D] = hash[C];
        hash[C] = hash[B];
        hash[B] = hash[A];
        hash[A] = hashT1 + hashT2;
    }

    for (int i = 0; i < len; i++) {
        md->hash[i] += hash[i];
    }
}