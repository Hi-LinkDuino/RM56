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

#include "ComplexTest.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <complex.h>
#include "log.h"
#include "MathUtils.h"

#ifdef __cplusplus
extern "C" {
#endif


int TestCabs()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 1.0 + 3.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 6.5555555 - 5.0 * I;
    double expected[] = {3.16227766016837952279, 3.0, 8.24471393764394};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        double d = cabs(testZ[i]);
        if (!Equal(d, expected[i])) {
            LOG("cabs of %lf+%lfi failed, expected:%.20lf, actual:%.20lf\n",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestCabsf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 6.555 - 5.0 * I;
    float expected[] = {5.0, 3.0, 8.244272};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        float d = cabsf(testZ[i]);
        if (!Equalf(d, expected[i])) {
            LOG("cabsf of %f+%fi failed, expected:%.10f, actual:%.10f",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestCabsl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789123456789 - 2.0 * I;
    long double expected[] = {5.0, 3.0, 2.35035271354479216};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        long double d = cabsl(testZ[i]);
        if (!Equall(d, expected[i])) {
            LOG("cabsl of %Lf+%Lfi failed, expected:%.20Lf, actual:%.20Lf",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestCacos()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.123456 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = 0.93681246115571925603 + (-2.30550903124346850959) * I;
    expected[1] = 1.57079632679489655800 + 1.81844645923206682525 * I;
    expected[2] = 1.09661331246844784282 + 1.54935939824075052940 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cacos(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ctanl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCacosf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.123456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = 0.9368131757 + -2.3055081367 * I;
    expected[1] = 1.5707963705 + 1.8184465170 * I;
    expected[2] = 1.0966134071 + 1.5493593216 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cacosf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cacosf of %f+%fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCacosh()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.123456 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = 2.30550903124346850959 + 0.93681246115571925603 * I;
    expected[1] = 1.81844645923206682525 + -1.57079632679489655800 * I;
    expected[2] = 1.54935939824075052940 + -1.09661331246844784282 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cacosh(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cocosh of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCacoshf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.123456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = 2.3055081367 + 0.9368131757 * I;
    expected[1] = 1.8184465170 + -1.5707963705 * I;
    expected[2] = 1.5493593216 + -1.0966134071 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cacoshf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cacoshf of %f+%fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCacoshl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.123456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    expected[0] = 2.30550903124346850959 + 0.93681246115571925603 * I;
    expected[1] = 1.81844645923206682525 + -1.57079632679489655800 * I;
    expected[2] = 1.54935939824075052940 + -1.09661331246844784282 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cacoshl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cacoshl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCacosl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.123456 - 2.0 * I;
    long _Complex expected[testCount];
    long _Complex resultval[testCount];

    expected[0] = 0.936812 - 2.305509 * I;
    expected[1] = 1.570796 + 1.818446 * I;
    expected[2] = 1.096613 + 1.549359 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cacosl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cacosl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf+%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCarg()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double expected[] = {0.92729521800161218703, -1.57079632679489655800, -1.01776488671168796607};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        double resultval = carg(testZ[i]);
        if (!Equal(resultval, expected[i])) {
            LOG("carg of %Lf+%Lfi failed, expected:%.20Lf, actual:%.20Lf\n",
                creal(testZ[i]), cimag(testZ[i]), expected[i], resultval);
            sum++;
        }
    }
    return sum;
}

int TestCargf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float expected[] = {0.9272952180, -1.5707963268, -1.0177677870};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        float resultval = cargf(testZ[i]);
        if (!Equalf(resultval, expected[i])) {
            LOG("cargf of %f+%fi failed, \n expected:%.10f,\n actual:%.10f\n",
                creal(testZ[i]), cimag(testZ[i]), expected[i], resultval);
            sum++;
        }
    }
    return sum;
}

int TestCargl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double expected[] = {0.92729521800161218703, -1.57079632679489655800, -1.01776774326071883792};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        long double resultval = cargl(testZ[i]);
        if (!Equall(resultval, expected[i])) {
            LOG("cargl of %Lf+%Lfi failed, expected:%.20Lf, actual:%.20Lf\n",
                creal(testZ[i]), cimag(testZ[i]), expected[i], resultval);
            sum++;
        }
    }
    return sum;
}

int TestCasin()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = 0.63398400000000000000 + 2.30550900000000000000 * I;
    expected[1] = 0.00000000000000000000 - 1.81844600000000000000 * I;
    expected[2] = 0.51505965148270682619 + -1.56954262407246969424 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = casin(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("casin of %.20Lf+%.20Lfi failed,\n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf+%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCasinf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.123456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = 0.6339831948 + 2.3055081367 * I;
    expected[1] = 0.0000000000 - 1.8184460000 * I;
    expected[2] = 0.4741830000 - 1.5493590000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = casinf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("casinf of %.10f+%.10fi failed,\n expected:%.10f+%.10fi,\n actual:%.10f+%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCasinh()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = 2.29991404087926865074 + 0.91761685335147724629 * I;
    expected[1] = 1.76274717403908720570 + -1.57079632679489655800 * I;
    expected[2] = 1.52957806263130779989 + -0.97493960889397524028 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = casinh(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("casinh of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCasinhf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = 2.2999134064 + 0.9176163673 * I;
    expected[1] = 1.7627469301 + -1.5707963705 * I;
    expected[2] = 1.5295758247 + -0.9749425054 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = casinhf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("casinhf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCasinhl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = 2.29991404087926865074 + 0.91761685335147724629 * I;
    expected[1] = 1.76274717403908720570 + -1.57079632679489655800 * I;
    expected[2] = 1.52957598661739924140 + -0.97494239607734123343 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = casinhl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("casinhl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCasinl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = 0.63398400000000000000 + 2.30550900000000000000 * I;
    expected[1] = 0.00000000000000000000 - 1.81844600000000000000 * I;
    expected[2] = 0.51505680174746037991 + -1.56954114502494923578 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = casinl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("casinl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCatan()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = 1.44830699523146444463 + 0.15899719167999917802 * I;
    expected[1] = -1.57079632679489655800 + -0.34657359027997264311 * I;
    expected[2] = 1.32100260208930864181 + -0.35694146805942778000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = catan(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("catan of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCatanf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = 1.4483070000 + 0.1589970000 * I;
    expected[1] = -1.5707962513 + -0.3465735912 * I;
    expected[2] = 1.3210030794 + -0.3569429517 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = catanf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("catanf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCatanh()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = 0.11750100000000000000 + 1.40992100000000000000 * I;
    expected[1] = 0.00000000000000000000 - 1.24904600000000000000 * I;
    expected[2] = 0.19913075343655456395 + -1.20880401646302515495 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = catanh(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("catanh of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCatanhf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = 0.1175010000 + 1.4099210000 * I;
    expected[1] = 0.0000000000 - 1.2490460000 * I;
    expected[2] = 0.1991299987 + -1.2088029385 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = catanhf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("catanhf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCatanhl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = 0.11750100000000000000 + 1.40992100000000000000 * I;
    expected[1] = 0.00000000000000000000 - 1.24904600000000000000 * I;
    expected[2] = 0.19913000142015302441 + -1.20880294804886800897 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = catanhl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("catanhl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCatanl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = 1.44830699523146444463 + 0.15899719167999917802 * I;
    expected[1] = -1.57079632679489655800 + -0.34657359027997264311 * I;
    expected[2] = 1.32100304043625027539 + -0.35694293478475969117 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = catanl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("catanl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCcos()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = -27.03494560 - 3.85115333 * I;
    expected[1] = 10.06766200;
    expected[2] = 1.24125772 + 3.42377699 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ccos(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ccos of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCcosf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = -27.0349445343 + -3.8511533737 * I;
    expected[1] = 10.0676620000;
    expected[2] = 1.2412856817 + 3.4237675667 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ccosf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ccosf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCcosh()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = -6.58066304 - 7.58155274 * I;
    expected[1] = -0.98999249;
    expected[2] = -0.77566723 - 1.43029421 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ccosh(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ccosh of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCcoshf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = -6.5806630000 - 7.5815530000 * I;
    expected[1] = -0.9899920000;
    expected[2] = -0.7756620646 + -1.4302809238 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ccoshf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ccoshf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCcoshl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long _Complex expected[testCount];
    long _Complex resultval[testCount];

    expected[0] = -6.580663 - 7.581553 * I;
    expected[1] = -0.989992;
    expected[2] = -0.775667 - 1.430294 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ccoshl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ccoshl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCcosl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = -27.03494560307422389656 + -3.85115333481177701813 * I;
    expected[1] = 10.06766200000000000000;
    expected[2] = 1.24128574667119084829 + 3.42376754746658606976 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ccosl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ccosl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCexp()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = -13.12878308146215822205 + -15.20078446306795427745 * I;
    expected[1] = -0.989992000000000000000 - 0.141120000000000000000 * I;
    expected[2] = -1.43025218442476775849 + -3.12515803748498344206 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cexp(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cexp of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCexpf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = -13.1287830000 - 15.2007840000 * I;
    expected[1] = -0.9899920000 - 0.1411200000 * I;
    expected[2] = -1.4302409887 + -3.1251332760 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cexpf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cexpf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCexpl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = -13.12878308146215822205 + -15.20078446306795427745 * I;
    expected[1] = -0.98999200000000000000 - 0.14112000000000000000 * I;
    expected[2] = -1.43024089977955104480 + -3.12513338008534180545 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cexpl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cexpl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCimag()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double expected[] = {4.000000, -3.000000, -2.000000};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        double d = cimag(testZ[i]);
        if (!Equal(d, expected[i])) {
            LOG("cimag of %.20Lf+%.20Lfi failed, expected:%.20Lf, actual:%.20Lf",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestCimagf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[] = {4.000000, -3.000000, -2.000000};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        float  d = cimagf(testZ[i]);
        if (!Equalf(d, expected[i])) {
            LOG("cimagf of %.10f+%.10fi failed, expected:%.10f, actual:%.10f",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestCimagl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double  expected[testCount];

    expected[0] = 4.000000;
    expected[1] = -3.000000;
    expected[2] = -2.000000;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        long double  d = cimagl(testZ[i]);
        if (!Equall(d, expected[i])) {
            LOG("cimagl of %.20Lf+%.20Lfi failed, expected:%.20Lf, actual:%.20Lf",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestClog()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];

    expected[0] = 1.60943791243410028180 + 0.92729521800161218703 * I;
    expected[1] = 1.09861228866810978211 + -1.57079632679489655800 * I;
    expected[2] = 0.85456540748777654048 + -1.01776488671168796607 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = clog(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("clog of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestClogf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = 1.6094380000 + 0.9272950000 * I;
    expected[1] = 1.0986123085 + -1.5707963705 * I;
    expected[2] = 0.8545635939 + -1.0177677870 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = clogf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("clogf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestClogl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = 1.60943791243410028180 + 0.92729521800161218703 * I;
    expected[1] = 1.09861228866810978211 + -1.57079632679489655800 * I;
    expected[2] = 0.85456364419155605994 + -1.01776774326071883792 * I;
    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = clogl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("clogl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestConj()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    expected[0] = 4.000000 - 3.000000 * I;
    expected[1] = 0.000000 - 3.000000 * I;
    expected[2] = 2.256000 - 1.030000 * I;
    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = conj(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("conj of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestConjf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];

    expected[0] = 4.000000 - 3.000000 * I;
    expected[1] = 0.000000 - 3.000000 * I;
    expected[2] = 2.256000 - 1.030000 * I;
    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = conjf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("conjf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestConjl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];

    expected[0] = 4.000000 - 3.000000 * I;
    expected[1] = 0.000000 - 3.000000 * I;
    expected[2] = 2.256000 - 1.030000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = conjl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("conjl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCpow()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    double _Complex power[testCount];

    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;

    power[0] = 2.0 + 3.0 * I;
    power[1] = 2.0 - 2.0 * I;
    power[2] = 0.0 - 3.0 * I;

    expected[0] = 1.42600947539257538743 + 0.60243463019053899998 * I;
    expected[1] = 0.22800935530979718480 + 0.31507871264107617515 * I;
    expected[2] = -0.03953800026646088484 + -0.02578534206891676742 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cpow(testZ[i], power[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equal(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cpow of %.20Lf+%.20Lfi,%.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(power[i]), cimag(power[i]),
                creal(expected[i]), cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCpowf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    float _Complex power[testCount];

    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;

    power[0] = 2.0 + 3.0 * I;
    power[1] = 2.0 - 2.0 * I;
    power[2] = 0.0 - 3.0 * I;

    expected[0] = 1.4260095358 + 0.6024347544 * I;
    expected[1] = 0.2280094773 + 0.3150786161 * I;
    expected[2] = -0.0395375155 + -0.0257853400 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cpowf(testZ[i], power[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cpowf of %.10f+%.10fi,%.20Lf+%.20Lfi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(power[i]), cimag(power[i]),
                creal(expected[i]), cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCpowl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    long double _Complex power[testCount];

    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;

    power[0] = 2.0 + 3.0 * I;
    power[1] = 2.0 - 2.0 * I;
    power[2] = 0.0 - 3.0 * I;

    expected[0] = 1.42600947539257538743 + 0.60243463019053899998 * I;
    expected[1] = 0.22800935530979718480 + 0.31507871264107617515 * I;
    expected[2] = -0.03953752504023075320 + -0.02578533024804808421 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cpowl(testZ[i], power[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cpowl of %.20Lf+%.20Lfi,%.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(power[i]), cimag(power[i]),
                creal(expected[i]), cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCproj()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    expected[0] = 4.000000 + 3.000000 * I;
    expected[1] = 0.000000 + 3.000000 * I;
    expected[2] = 2.256000 + 1.030000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cproj(testZ[i]);
        if ((!Equal (creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cprojof %.20f+%.20fi failed, \n expected:%.20f+%.20fi,\n actual:%.20f +%.20fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCprojf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = 4.000000 + 3.000000 * I;
    expected[1] = 0.000000 + 3.000000 * I;
    expected[2] = 2.256000 + 1.030000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cprojf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cprojf of %f+%fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCprojl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    expected[0] = 4.000000 + 3.000000 * I;
    expected[1] = 0.000000 + 3.000000 * I;
    expected[2] = 2.256000 + 1.030000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = cprojl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("cprojl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCreal()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456789 - 2.0 * I;
    double expected[] = {3.00000000000000000000, 0.00000000000000000000, 1.23456788999999989009};
    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        double d = creal(testZ[i]);
        if (!Equal(d, expected[i])) {
            LOG("creal of %.20Lf+%.20Lfi failed, expected:%.20Lf, actual:%.20Lf",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestCrealf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    float expected[] = {3.000000, 0.000000, 1.234560};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        float  d = crealf(testZ[i]);
        if (!Equalf(d, expected[i])) {
            LOG("crealf of %.10f+%.10fi failed, expected:%.10f, actual:%.10f",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}


int TestCreall()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 3.0 + 4.0 * I;
    testZ[1] = 0.0 - 3.0 * I;
    testZ[2] = 1.23456 - 2.0 * I;
    long double  expected[] = {3.000000, 0.000000, 1.234560};

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        long double  d = creall(testZ[i]);
        if (!Equall(d, expected[i])) {
            LOG("creall of %.20Lf+%.20Lfi failed, expected:%.20Lf, actual:%.20Lf",
                creal(testZ[i]), cimag(testZ[i]), expected[i], d);
            sum++;
        }
    }
    return sum;
}

int TestCsin()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    expected[0] = -9.22649862501591755404 + -6.18447589812529496811 * I;
    expected[1] = 0.00000000000000000000 + 0.995055000000000000000 * I;
    expected[2] = 1.22263236469252678518 + -0.77333777683149840243 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csin(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equal(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csin of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsinf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = -7.6192321777 + -6.5481195450 * I;
    expected[1] = 0.0000000000 - 0.142547000000 * I;
    expected[2] = 1.2226322889 + -0.7733377218 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csinf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csinf of %f+%fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f+%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsinh()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    expected[0] = -30.74488615260046486810 + 1.28017476495887727594 * I;
    expected[1] = 0.00000000000000000000 + 0.99505500000000000000 * I;
    expected[2] = 2.42996158979828669189 + 4.13629706118861140141 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csinh(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equal(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csinh of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsinhf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = -27.0168151855 + 3.8537375927 * I;
    expected[1] = 0.0000000000 - 0.1425470000 * I;
    expected[2] = 2.4299616814 + 4.1362972260 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csinhf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csinhf of %f+%fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f+%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsinhl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    expected[0] = -30.74488615260046486810 + 1.28017476495887727594 * I;
    expected[1] = 0.00000000000000000000 + 0.995055000000000000000 * I;
    expected[2] = 2.42996158979828669189 + 4.13629706118861140141 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csinhl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csinhl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsinl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    expected[0] = -9.22649862501591755404 + -6.18447589812529496811 * I;
    expected[1] = 0.00000000000000000000 + 0.995055000000000000000 * I;
    expected[2] = 1.22263236469252678518 + -0.77333777683149840243 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csinl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csinl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsqrt()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    expected[0] = 2.15360235493437635057 + 0.71972432443185685447 * I;
    expected[1] = 1.22474487139158894067 + 1.22474487139158916271 * I;
    expected[2] = 1.53883190408405168270 + 0.33466943246575064919 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csqrt(testZ[i]);
        if ((!Equal(creal(expected[i]), creal(resultval[i]))) && (!Equal(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csqrt of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsqrtf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = 2.1213202477 + 0.7071067691 * I;
    expected[1] = 1.2247449160 + 1.2247449160 * I;
    expected[2] = 1.5388319492 + 0.3346694112 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csqrtf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csqrtf of %f+%fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f+%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCsqrtl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    expected[0] = 2.15360235493437635057 + 0.71972432443185685447 * I;
    expected[1] = 1.22474487139158894067 + 1.22474487139158916271 * I;
    expected[2] = 1.53883190408405168270 + 0.33466943246575064919 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = csqrtl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("csqrtl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCtan()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 1.1 + 2.12 * I;
    testZ[1] = 1.0 + 3.12 * I;
    testZ[2] = 0.0 + 1.03 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    expected[0] = 0.02369384883396526489 + 1.01682442264852812563 * I;
    expected[1] = 0.00355174767798756632 + 1.00161786795187368604 * I;
    expected[2] = 0.00000000000000000000 + 0.77390833985584200000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ctan(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ctan of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCtanf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 1.1 + 2.12 * I;
    testZ[1] = 1.0 + 3.12 * I;
    testZ[2] = 0.0 + 1.03 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = 0.0236938488 + 1.0168244226 * I;
    expected[1] = 0.0035517477 + 1.0016178679 * I;
    expected[2] = 0.0000000000 + 0.7739083399 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ctanf(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ctanf of %.10f+%.10fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f +%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCtanh()
{
    const int testCount = 3;
    double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    double _Complex expected[testCount];
    double _Complex resultval[testCount];
    expected[0] = 0.99947419378454971373 + -0.00004382891481422311 * I;
    expected[1] = 0.00000000000000000000 + 0.995055000000000000000 * I;
    expected[2] = 1.01017902431788741957 + 0.01957765681621519149 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ctanh(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ctanh of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCtanhf()
{
    const int testCount = 3;
    float _Complex testZ[testCount];
    testZ[0] = 4.0 + 3.0 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    float _Complex expected[testCount];
    float _Complex resultval[testCount];
    expected[0] = 0.999356 + -0.000187 * I;
    expected[1] = 0.000000 - 0.142547 * I;
    expected[2] = 1.010179 + 0.019578 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ctanhf(testZ[i]);
        if ((!Equalf(creal(expected[i]), creal(resultval[i]))) && (!Equalf(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ctanhf of %f+%fi failed, \n expected:%.10f+%.10fi,\n actual:%.10f+%.10fi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCtanhl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 4.12 + 3.1 * I;
    testZ[1] = 0.0 + 3.0 * I;
    testZ[2] = 2.256 + 1.03 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    expected[0] = 0.99947419378454971373 + -0.00004382891481422311 * I;
    expected[1] = 0.00000000000000000000 + 0.995055000000000000000 * I;
    expected[2] = 1.01017902431788741957 + 0.01957765681621519149 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ctanhl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ctanhl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

int TestCtanl()
{
    const int testCount = 3;
    long double _Complex testZ[testCount];
    testZ[0] = 1.1 + 2.12 * I;
    testZ[1] = 1.0 + 3.12 * I;
    testZ[2] = 0.0 + 1.03 * I;
    long double _Complex expected[testCount];
    long double _Complex resultval[testCount];
    expected[0] = 0.02369384883396526489 + 1.01682442264852812563 * I;
    expected[1] = 0.00355174767798756632 + 1.00161786795187368604 * I;
    expected[2] = 0.00000000000000000000 + 0.77390833985584200000 * I;

    int sum = 0;
    for (int i = 0; i < testCount; ++i) {
        resultval[i] = ctanl(testZ[i]);
        if ((!Equall(creal(expected[i]), creal(resultval[i]))) && (!Equall(cimag(expected[i]), cimag(resultval[i])))) {
            LOG("ctanl of %.20Lf+%.20Lfi failed, \n expected:%.20Lf+%.20Lfi,\n actual:%.20Lf +%.20Lfi\n",
                creal(testZ[i]), cimag(testZ[i]), creal(expected[i]),
                cimag(expected[i]), creal(resultval[i]), cimag(resultval[i]));
            sum++;
        }
    }
    return sum;
}

#ifdef __cplusplus
}
#endif