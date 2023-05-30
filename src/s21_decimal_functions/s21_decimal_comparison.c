#include "../s21_decimal.h"
#include "s21_decimal_helpers.h"

enum comparison_result {
  FALSE = 0,
  TRUE = 1,
};

int s21_is_less(s21_decimal a, s21_decimal b) {
  return s21_decimal_cmp(a, b) < 0;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  return s21_decimal_cmp(a, b) <= 0;
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  return !s21_decimal_cmp(a, b);
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_equal(a, b);
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  return s21_decimal_cmp(a, b) > 0;
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  return s21_decimal_cmp(a, b) >= 0;
}
