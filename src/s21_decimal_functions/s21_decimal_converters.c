#include "../s21_decimal.h"
#include "s21_decimal_helpers.h"

enum converters_errors {
  SUCCESS = 0,
  CONVERTION_ERROR = 1,
};

#define INTMIN 2147483648.

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int fail = 0;
  union {
    s21_decimal d;
    s21_dec prts;
  } u = {src};
  *dst = 0;

  if (dst) {
    float res = 0;
    for (int i = 0; i < 96; ++i)
      if (s21_getBit(src, i)) res += pow(2, i);

    res /= pow(10.0, u.prts.scale);
    res *= u.prts.sign ? -1. : 1.;

    *dst = res;
  } else
    fail = 1;

  return fail;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error = SUCCESS;
  *dst = (s21_decimal){0};

  if (dst == NULL || isinf(src) || isnan(src)) {
    error = CONVERTION_ERROR;
    return error;
  }

  if (float_is_zero(src)) {
    return SUCCESS;
  }

  info_float info = {0};
  if (parse_float(src, &info)) {
    error = CONVERTION_ERROR;
  }
  if (error == SUCCESS) {
    need_increment(&info);
    unsigned value = info_to_int(info);
    error = put_float_to_decimal(dst, &info, value);
  }

  return error;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  s21_decimal tgt = {0};
  int error = SUCCESS;
  *dst = 0;

  error = s21_truncate(src, &tgt);
  if (!tgt.bits[1] && !tgt.bits[2] && tgt.bits[0] == (unsigned)INTMIN &&
      s21_getSignBit(tgt)) {
    *dst = INT_MIN;
  } else if (tgt.bits[1] || tgt.bits[2] || s21_getBit(tgt, 31)) {
    error = CONVERTION_ERROR;
  } else if (!tgt.bits[1] && !tgt.bits[2]) {
    *dst = tgt.bits[0];
    if (s21_getSignBit(tgt)) {
      *dst *= -1;
    }
  }
  return error;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  s21_decimal_mask result = {0};
  unsigned usrc = 0;
  *dst = (s21_decimal){0};

  if (src < 0) {
    result.mask.sign = 1;
    usrc = (unsigned)(-src);
  } else {
    usrc = (unsigned)src;
  }
  result.mask.number[0] = usrc;
  *dst = result.orig;
  return SUCCESS;
}
