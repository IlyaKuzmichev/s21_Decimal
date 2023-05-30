#include <limits.h>

#include "../s21_decimal.h"
#include "s21_decimal_helpers.h"

enum arithmetic_errors {
  OK = 0,
  TOO_BIG = 1,
  TOO_SMALL = 2,
  DIV_BY_ZERO = 3,
};

int s21_big_dec_add(s21_big_decimal value_1, s21_big_decimal value_2,
                    s21_big_decimal *result) {
  int error = OK;
  unsigned overflow = 0;
  result->scale = value_1.scale;

  for (int i = 0; i < BIG_SIZE_BITS; i++) {
    uint64_t temp_res = (uint64_t)value_1.bits[i] + (uint64_t)value_2.bits[i] +
                        (uint64_t)overflow;

    if (temp_res > (uint64_t)UINT32_MAX) {
      overflow = 1;
      result->bits[i] = temp_res - (uint64_t)UINT32_MAX - 1;
    } else {
      overflow = 0;
      result->bits[i] = temp_res;
    }
  }

  if (overflow) {
    error = TOO_BIG;
  }
  return error;
}

int add_main(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = OK;
  int first_remainder = 0, last_remainder = 0;

  s21_big_decimal big_val_1 = {0}, big_val_2 = {0}, big_result = {0};
  s21_convert_to_big(value_1, &big_val_1);
  s21_convert_to_big(value_2, &big_val_2);

  s21_big_align_scales(&big_val_1, &big_val_2);  // выравнивание
  if (s21_big_dec_add(big_val_1, big_val_2, &big_result)) {
    error = TOO_BIG;
  }  // сложение выравненных

  s21_big_shrink_to_decimal(&big_result, &first_remainder, &last_remainder);
  s21_big_banking_rounding(&big_result, first_remainder, last_remainder);
  if (s21_more_than_dec(big_result)) {
    error = TOO_BIG;
  }

  s21_convert_to_dec(big_result, result);
  return error;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = OK;
  int sign_1 = s21_getSignBit(value_1);
  int sign_2 = s21_getSignBit(value_2);
  *result = (s21_decimal){0};

  if (sign_1 == sign_2) {
    error = add_main(value_1, value_2, result);
    if (sign_1) {
      s21_setSignBit(result, 1);
    }
    if (error && sign_1) {
      error = TOO_SMALL;
    }
  } else {
    if (sign_2) {
      s21_setSignBit(&value_2, 0);
      error = s21_sub(value_1, value_2, result);
    } else {
      s21_setSignBit(&value_1, 0);
      error = s21_sub(value_2, value_1, result);
    }
  }
  return error;
}

int s21_actual_mul(s21_decimal d1, s21_decimal d2, s21_decimal *result) {
  int fail = OK;

  s21_big_decimal bd1 = {0}, bd2 = {0}, res = {0};
  s21_convert_to_big(d1, &bd1);
  s21_convert_to_big(d2, &bd2);
  s21_big_mul(bd1, bd2, &res);
  fail = s21_safe_fit_to_dec(&res);
  if (!fail) s21_convert_to_dec(res, result);

  return fail;
}

void s21_big_mul(s21_big_decimal bd1, s21_big_decimal bd2,
                 s21_big_decimal *res) {
  for (unsigned i = 0; i < 96; ++i) {
    if (s21_big_getbit(bd1, i)) s21_big_dec_add(bd2, *res, res);

    s21_big_lshift(&bd2, 1);
  }

  res->scale = bd1.scale + bd2.scale;
  res->sign = bd1.sign ^ bd2.sign;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int fail = OK;
  *result = (s21_decimal){0};

  if (!s21_is_zero_decimal(value_1) && !s21_is_zero_decimal(value_1))
    fail = s21_actual_mul(value_1, value_2, result);

  return fail;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = OK;
  *result = (s21_decimal){0};

  s21_decimal_mask a = {.orig = value_1}, b = {.orig = value_2};
  if (a.mask.sign != b.mask.sign) {
    s21_negate(value_2, &value_2);
    error = s21_add(value_1, value_2, result);
  } else {
    error = s21_true_sub(value_1, value_2, result);
  }
  return error;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_big_decimal big_value_1 = {{0}, 0, 0}, big_value_2 = {{0}, 0, 0},
                  big_result = {{0}, 0, 0};
  int return_code = OK;
  *result = (s21_decimal){0};

  if (s21_is_zero_decimal(value_2)) {
    return DIV_BY_ZERO;
  } else if (s21_is_zero_decimal(value_1)) {
    s21_setSignBit(result, s21_getSignBit(value_1) ^ s21_getSignBit(value_2));
    return OK;
  }

  s21_convert_to_big(value_1, &big_value_1);
  s21_convert_to_big(value_2, &big_value_2);
  s21_big_div(big_value_1, big_value_2, &big_result);
  return_code = s21_safe_fit_to_dec(&big_result);
  s21_convert_to_dec(big_result, result);
  return return_code;
}

void s21_big_div(s21_big_decimal big_value_1, s21_big_decimal big_value_2,
                 s21_big_decimal *big_result) {
  s21_big_decimal remainder = {{0}, 0, 0}, to_compare = {{0}, 0, 0};
  int finish_division = 1, bits_counter = 192, result_bits_counter = 0;

  big_result->sign = big_value_1.sign ^ big_value_2.sign;
  big_value_1.sign = big_value_2.sign = 0;
  while (s21_cmp_big(big_value_1, big_value_2) < 0) {
    s21_big_increase_scale(&big_value_1, 1);
  }
  while (finish_division && result_bits_counter < 160) {
    int remainder_bit_value =
        bits_counter > 0 ? s21_big_getbit(big_value_1, bits_counter - 1) : 0;
    s21_big_lshift(&remainder, 1);
    s21_big_setbit(&remainder, 0, remainder_bit_value);
    if (s21_cmp_big(remainder, big_value_2) >= 0) {
      s21_big_lshift(big_result, 1);
      s21_big_setbit(big_result, 0, 1);
      result_bits_counter++;
      remainder = s21_big_naive_sub(remainder, big_value_2);
    } else if (result_bits_counter) {
      s21_big_lshift(big_result, 1);
      result_bits_counter++;
    }
    --bits_counter;
    if (bits_counter <= 0 && !s21_cmp_big(remainder, to_compare)) {
      finish_division = 0;
    }
  }
  s21_div_rescaling(big_result, bits_counter,
                    big_value_1.scale - big_value_2.scale);
}

void s21_div_rescaling(s21_big_decimal *big_result, int bits_counter,
                       int result_temp_scale) {
  int scale = 0, first_remainder = 0, last_remainder = 0;

  big_result->scale = (-1) * bits_counter;
  while (bits_counter) {
    while (big_result->bits[5]) {
      s21_big_decrease_scale(big_result, &first_remainder, &last_remainder, 1);
    }
    s21_big_int_mul(big_result, 5);
    bits_counter++;
  }
  scale = big_result->scale + result_temp_scale;
  if (scale < 0) {
    s21_big_increase_scale(big_result, (-1) * scale);
    big_result->scale = 0;
  } else {
    big_result->scale = scale;
  }
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int return_code = OK;
  s21_big_decimal big_value_1 = {{0}, 0, 0}, big_value_2 = {{0}, 0, 0};
  *result = (s21_decimal){0};

  if (s21_is_zero_decimal(value_2)) {
    return_code = DIV_BY_ZERO;
  } else if (s21_is_zero_decimal(value_1)) {
    s21_setSignBit(result, s21_getSignBit(value_1));
  } else {
    int sign = s21_getSignBit(value_1);
    s21_setSignBit(&value_1, 0);
    s21_setSignBit(&value_2, 0);
    s21_convert_to_big(value_1, &big_value_1);
    s21_convert_to_big(value_2, &big_value_2);
    s21_big_decimal big_result, temp_result = {0};
    s21_big_div(big_value_1, big_value_2, &temp_result);
    s21_big_truncate(&temp_result);
    big_result = temp_result, temp_result = (s21_big_decimal){0};
    s21_big_mul(big_value_2, big_result, &temp_result);
    big_result = temp_result, temp_result = (s21_big_decimal){0};
    s21_big_sub(big_value_1, big_result, &temp_result);
    big_result = temp_result;
    return_code = s21_safe_fit_to_dec(&big_result);
    s21_convert_to_dec(big_result, result);
    s21_setSignBit(result, sign);
  }
  return return_code;
}
