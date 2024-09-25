#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "a.h"
#include "calc_math.h"
/*
Integer Square root taken from Hackers Delight Second Edition (Henry S. Warren Jr.)
*/
I64t isqrt(I64t x)
{
    U64t x1, x2;
    if (x < 0)
        return -1;
    unsigned m,y,b;
    x1 = x;
    m = 0x40000000;
    y = 0;
    while (m != 0) {
        b = y | m;
        y = y>>1;
        if (x>=b) {
            x = x-b;
            y = y | m;
        }
        m = m >> 2;
    }
    return y;
}