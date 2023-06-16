#ifndef SCATK_FIX64_H
#define SCATK_FIX64_H

#include "defs.h"

// struct for a.b fixed point representation
typedef struct _fix64 {
    scatk::i64 a;
    scatk::u64 b;
} fix64;

// a.b to double
namespace scatk{
    double fix64_to_double(fix64 x);
    fix64 double_to_fix64(double x);

    fix64 fix64_add(fix64 x, fix64 y);
    fix64 fix64_sub(fix64 x, fix64 y);
    fix64 fix64_mul(fix64 x, fix64 y);
    fix64 fix64_div(fix64 x, fix64 y);
    fix64 fix64_neg(fix64 x);
    fix64 fix64_abs(fix64 x);
}


#endif //SCATK_FIX64_H