/* Copyright (C) 2011 by Valentin Ochs
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>

typedef int (*cmpfun)(const void *, const void *);

#define MIDDLE_ONE(a, b, c)                                      \
    (*cmp)(a, b) < 0 ?                                           \
    ((*cmp)(b, c) < 0 ? (b) : ((*cmp)(a, c) < 0 ? (c) : (a))):   \
    ((*cmp)(b, c) > 0 ? (b) : ((*cmp)(a, c) < 0 ? (a) : (c)))

#define SWAPN(a, b, n)                                           \
    do {                                                         \
        register char *x = (char *)(a);                          \
        register char *y = (char *)(b);                          \
        register char tmp;                                       \
        int i = (n) / sizeof(char);                              \
        for (--i; i >= 0; i--) {                                 \
            tmp = *x;                                            \
            *x++ = *y;                                           \
            *y++ = tmp;                                          \
        }                                                        \
    } while (0)

void qsort(void *base, size_t nel, size_t width, cmpfun cmp)
{
    char *start, *end, *m, *l, *r;
    int i, j, swapflag = 0;
    char temp[width];

    if (width == 0 || base == NULL || nel == 0) {
        return;
    }

    start = (char *)base;
    end = start + (nel - 1) * width;
loop:
    if (nel < 7) { // insertion sort
insertqort:
        for (l = start + width; l <= end; l += width) {
            memcpy(temp, l, width);
            for (m = l - width; m >= start; m -= width) {
                if ((*cmp)(m, temp) > 0) {
                    memcpy((m + width), m, width);
                } else {
                    break;
                }
            }
            memcpy((m + width), temp, width);
        }
        return;
    }

    // quick sort
    m = start + (nel >> 1) * width;
    m = MIDDLE_ONE(start, m, end);
    if (m != start) {
        SWAPN(start, m, width);
        m = start;
    }
    l = start + width;
    r = end;

    while (l <= r) {
        while (l <= r && (*cmp)(l, m) < 0) {
            l += width;
        }
        while (l <= r && (*cmp)(r, m) >= 0) {
            r -= width;
        }
        if (l < r) {
            SWAPN(l, r, width);
            l += width;
            r -= width;
            swapflag = 1;
        }
    }
    SWAPN(m, l - width, width);
    m = l - width;
    if (swapflag == 0) {
        goto insertqort;
    }

    if (m - start > end - m) {
        qsort(start, (m - start) / width, width, (*cmp));
        if (m == end) {
            return;
        }
        start = m + width;
        nel = (end - start) / width + 1;
        goto loop;
    } else {
        qsort(m + width, (end - m) / width, width, (*cmp));
        if (m == start) {
            return;
        }
        end = m - width;
        nel = (end - start) / width + 1;
        goto loop;
    }
}
