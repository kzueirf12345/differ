#include <stdio.h>
#include <math.h>

#include "op_math.h"
#include "tree/operation/operation.h"
#include "logger/liblogger.h"
#include "utils/utils.h"

operand_t math_SUM        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");

    return first + second;
}
operand_t math_SUB        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");

    return first - second;
}
operand_t math_MUL        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");

    return first * second;
}
operand_t math_DIV        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    if (fabs(second) < __DBL_EPSILON__)
        return NAN;

    return first / second;
}
operand_t math_POW        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");

    return pow(first, second);
}
operand_t math_SIN        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    return sin(first);
}
operand_t math_COS        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    return cos(first);
}
operand_t math_TG         (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    if (fabs(cos(first)) < __DBL_EPSILON__)
        return NAN;

    return tan(first);
}
operand_t math_CTG        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    if (fabs(sin(first)) < __DBL_EPSILON__)
        return NAN;

    return tan(M_PI_2 - first);
}
operand_t math_ARCSIN     (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    if (first < -1 && 1 < first)
        return NAN;

    return asin(first);
}
operand_t math_ARCCOS     (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    if (first < -1 && 1 < first)
        return NAN;

    return acos(first);
}
operand_t math_ARCTG      (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    return atan(first);
}
operand_t math_ARCCTG     (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    return M_PI_2 - atan(first);
}
operand_t math_SH         (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    return sinh(first);
}
operand_t math_CH         (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    return cosh(first);
}
operand_t math_TH         (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    if (fabs(cosh(first)) < __DBL_EPSILON__)
        return NAN;

    return tanh(first);
}
operand_t math_CTH        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    if (fabs(sinh(first)) < __DBL_EPSILON__)
        return NAN;

    return 1 / tanh(first);
}
operand_t math_LOG        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");

    if (first < 0 || fabs(first - 1) < __DBL_EPSILON__ || second < 0)
        return NAN;

    return log(first) / log(second);
}
operand_t math_LN        (const operand_t first, const operand_t second, FILE* out)
{
    lassert(!is_invalid_ptr(out), "");
    (void)second;

    if (first < 0)
        return NAN;

    return log(first);
}