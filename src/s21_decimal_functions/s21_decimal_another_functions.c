#include "../s21_decimal.h"
#include "s21_decimal_helpers.h"

enum another_errors {
  OK = 0,
  CALC_ERR = 1,
};

int s21_negate(s21_decimal value, s21_decimal *result) {
  int error = OK;

  s21_setSignBit(&value, s21_getSignBit(value) ^ 1);
  *result = value;

  return error;
}

void s21_big_truncate(s21_big_decimal *b) {
  int exp = b->scale;
  b->scale = 0;

  while (exp-- > 0) s21_big_int_div(b, 10);
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int error = OK;
  *result = (s21_decimal){0};
  s21_big_decimal b = {{0}, 0, 0};

  s21_convert_to_big(value, &b);
  s21_big_truncate(&b);

  s21_convert_to_dec(b, result);

  return error;
}

void s21_big_round(s21_big_decimal *b) {
  int first_fraction_digit = 0;
  while (b->scale > 0) {
    first_fraction_digit = s21_big_int_div(b, 10);
    --b->scale;
  }
  if (first_fraction_digit >= 5) {
    s21_big_int_add(b, 1);
  }
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int error = OK;
  *result = (s21_decimal){0};
  s21_big_decimal b = {0};

  s21_convert_to_big(value, &b);
  s21_big_round(&b);

  s21_convert_to_dec(b, result);

  return error;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int error = OK;
  *result = (s21_decimal){0};
  s21_big_decimal big_val = {0};

  s21_convert_to_big(value, &big_val);
  int sign = big_val.sign;
  int is_fraction = 0;

  while (big_val.scale) {
    s21_big_int_div(&big_val, 10);
    --big_val.scale;
    is_fraction = 1;
  }

  if (sign && is_fraction) {
    s21_big_decimal one = {{1, 0, 0, 0, 0, 0}, 0, 0};
    s21_big_decimal res = {0};
    s21_big_dec_add(big_val, one, &res);
    big_val = res;
    big_val.sign = 1;
  }

  s21_convert_to_dec(big_val, result);
  return error;
}
