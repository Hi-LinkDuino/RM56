#ifndef COMPLEX_OP_H
#define COMPLEX_OP_H

#include <math.h>
#include <stdbool.h>
#include "ae_math.h"

typedef struct
{
    float r;
    float i;
} Complex;

Complex cpx_create(float r, float i)
{
    return (Complex) { r, i };
}

Complex cpx_add(Complex a, Complex b)
{
    float r = a.r + b.r;
    float i = a.i + b.i;
    return cpx_create(r, i);
}

Complex cpx_sub(Complex a, Complex b)
{
    float r = a.r - b.r;
    float i = a.i - b.i;
    return cpx_create(r, i);
}

Complex cpx_mult(Complex a, Complex b)
{
    float r = a.r * b.r - a.i * b.i;
    float i = a.r * b.i + a.i * b.r;
    return cpx_create(r, i);
}

float cpx_power(Complex a)
{
    return (a.r * a.r + a.i * a.i);
}

Complex cpx_div(Complex a, Complex b)
{
    float norm = cpx_power(b);
    float r = (a.r * b.r + a.i * b.i) / norm;
    float i = (a.i * b.r - a.r * b.i) / norm;
    return cpx_create(r, i);
}

Complex cpx_scale(Complex a, float b)
{
    float r = a.r * b;
    float i = a.i * b;
    return cpx_create(r, i);
}

Complex cpx_conj(Complex a)
{
    float r = a.r;
    float i = -a.i;
    return cpx_create(r, i);
}

float cpx_abs(Complex a)
{
    return sqrtf(cpx_power(a));
}

float cpx_angle(Complex a)
{
    return atan2f(a.i, a.r);
}

bool cpx_is_equal(Complex a, Complex b)
{
    return (float_is_equal(a.r, b.r) && float_is_equal(a.i, b.i));
}

#ifdef CMATRIX_DEBUG
void cpx_print(FILE *fd, Complex a)
{
    if (a.i >= 0)
        fprintf(fd, "%f + %fi", a.r, a.i);
    else
        fprintf(fd, "%f - %fi", a.r, -a.i);
}
#endif

#endif