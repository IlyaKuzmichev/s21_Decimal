#include "s21_decimal_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../s21_decimal.h"
#include "s21_decimal_helpers.h"

/*  ======= Decimal Helpers =======   */

void s21_init_decimal(s21_decimal *src) {
  for (int i = 0; i < SIZE_BITS; i++) {
    src->bits[i] = 0;
  }
}

int s21_getBit(s21_decimal d, int i) {
  return !!(d.bits[i / 32] & 1U << (i % 32));
}

void s21_setBit(s21_decimal *d, int i, unsigned value) {
  d->bits[i / 32] = (d->bits[i / 32] & ~(1U << (i % 32))) | value << (i % 32);
}

int s21_getSignBit(s21_decimal d) { return s21_getBit(d, 127); }

void s21_setSignBit(s21_decimal *d, int sign_bit) {
  s21_setBit(d, 127, sign_bit);
}

int s21_decimal_cmp(s21_decimal a, s21_decimal b) {
  s21_big_decimal ba = {0}, bb = {0};
  s21_convert_to_big(a, &ba);
  s21_convert_to_big(b, &bb);
  s21_big_align_scales(&ba, &bb);
  int result = s21_cmp_big(ba, bb);
  if (ba.sign && bb.sign) {
    result *= -1;
  }
  return result;
}

int s21_is_zero_decimal(s21_decimal src) {
  int result = 1;
  for (int i = 0; i < SIZE_BITS - 1 && result; i++) {
    if (src.bits[i] != 0) {
      result = 0;
    }
  }
  return result;
}

/*  ======= Big Decimal Helpers =======  */

void s21_convert_to_big(s21_decimal src, s21_big_decimal *tgt) {
  int bits_counter = 0;
  *tgt = (s21_big_decimal){0};

  while (bits_counter < SIZE_BITS - 1) {
    tgt->bits[bits_counter] = src.bits[bits_counter];
    bits_counter++;
  }
  while (bits_counter < BIG_SIZE_BITS - 1) {
    tgt->bits[bits_counter] = 0;
    bits_counter++;
  }
  s21_decimal_mask msrc = {.orig = src};
  tgt->sign = msrc.mask.sign;
  tgt->scale = msrc.mask.scale;
}

void s21_convert_to_dec(s21_big_decimal src, s21_decimal *tgt) {
  for (int i = 0; i < SIZE_BITS - 1; i++) {
    tgt->bits[i] = src.bits[i];
  }
  s21_decimal_mask mtgt = {.orig = *tgt};
  mtgt.mask.sign = src.sign;
  mtgt.mask.scale = src.scale;
  *tgt = mtgt.orig;
}

int s21_big_getbit(s21_big_decimal d, int i) {
  return !!(d.bits[i / 32] & 1U << (i % 32));
}

int s21_cmp_big(s21_big_decimal a, s21_big_decimal b) {
  int result = 0;
  int both_zero = 1;
  for (int i = BIG_SIZE_BITS - 1; i >= 0 && !result; --i) {
    if (a.bits[i] > b.bits[i]) {
      result = 1;
    } else if (a.bits[i] < b.bits[i]) {
      result = -1;
    }
    if (a.bits[i] || b.bits[i]) {
      both_zero = 0;
    }
  }
  if (!both_zero && a.sign != b.sign) {
    result = a.sign > b.sign ? -1 : 1;
  }
  return result;
}

void s21_big_align_scales(s21_big_decimal *a, s21_big_decimal *b) {
  int scale_diff = (int)a->scale - (int)b->scale;
  s21_big_decimal *rescalable = scale_diff > 0 ? b : a;
  s21_big_increase_scale(rescalable, abs(scale_diff));
}

void s21_big_setbit(s21_big_decimal *d, int i, unsigned value) {
  d->bits[i / 32] = (d->bits[i / 32] & ~(1U << (i % 32))) | value << (i % 32);
}

int s21_big_int_div(s21_big_decimal *value, int divisor) {
  int remainder = 0;
  s21_big_decimal result = {{0}, 0, 0};

  result.scale = value->scale;
  result.sign = value->sign;
  if (divisor < 0) {
    result.sign ^= 1;
    divisor *= -1;
  }
  for (int i = 191; i >= 0; --i) {
    remainder *= 2;
    remainder += s21_big_getbit(*value, i);
    if (remainder / divisor >= 1) {
      s21_big_setbit(&result, i, 1);
      remainder -= divisor;
    }
  }
  *value = result;
  return remainder;
}

int s21_big_lshift(s21_big_decimal *d, unsigned i) {
  int lost_bit = 0;

  if (i < 192)
    while (--i < 192) {
      if (s21_big_getbit(*d, 191)) lost_bit = 1;
      for (size_t j = 191; j > 0; --j)
        s21_big_setbit(d, j, s21_big_getbit(*d, j - 1));
      s21_big_setbit(d, 0, 0);
    }
  else {
    d->bits[0] = d->bits[1] = d->bits[2] = d->bits[3] = d->bits[4] =
        d->bits[5] = 0;
    lost_bit = 1;
  }
  return lost_bit;
}

void s21_big_int_mul(s21_big_decimal *value, int multiplier) {
  s21_big_decimal result = {{0}, 0, 0};

  result.scale = value->scale;
  result.sign = value->sign;
  if (multiplier < 0) {
    result.sign ^= 1;
    multiplier *= -1;
  }
  for (int i = 0; i < BIG_SIZE_BITS; ++i) {
    uint64_t partial_product = (uint64_t)value->bits[i] * multiplier;
    uint64_t partial_sum = result.bits[i] + partial_product;
    int j = i;
    while (partial_sum >> 32) {
      result.bits[j] = partial_sum & (~(uint64_t)0 >> 32);
      partial_sum = result.bits[j + 1] + (partial_sum >> 32);
      ++j;
    }
    result.bits[j] = partial_sum;
  }
  *value = result;
}

void s21_big_int_add(s21_big_decimal *value, int addend) {
  int i = 0;
  uint64_t partial_sum = (uint64_t)value->bits[i] + (uint64_t)addend;
  while (partial_sum >> 32) {
    value->bits[i] = partial_sum & (~(uint64_t)0 >> 32);
    partial_sum = value->bits[i + 1] + (partial_sum >> 32);
    ++i;
  }
  value->bits[i] = partial_sum;
}

double pow10(double exp) {
  int result = 1;
  for (int i = 0; i < exp; ++i) {
    result *= 10;
  }
  return result;
}

void s21_big_increase_scale(s21_big_decimal *d, int increment) {  // 14
  // UINT64_MAX / UINT32_MAX = 2^64 / 2^32 = 2^32 = 4294967296 =>
  // => max pow of 10 for scaling 32 bit parts is 9
  const int LIMIT_INCREMENT = 10;
  while (increment > 0) {
    int delta = increment;
    if (increment > LIMIT_INCREMENT - 1) {
      delta = LIMIT_INCREMENT - 1;
    }

    int multiplier = pow10(delta);  // 4
    s21_big_int_mul(d, multiplier);
    d->scale += delta;
    increment -= delta;
  }
}

void s21_big_decrease_scale(s21_big_decimal *d, int *first_remainder,
                            int *last_remainder, int decrement) {
  for (int i = 0; i < decrement; ++i) {
    if (*last_remainder && !(*first_remainder)) {
      *first_remainder = *last_remainder;
    }
    *last_remainder = s21_big_int_div(d, 10);
    --d->scale;
  }
}

int s21_true_sub(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  s21_big_decimal a = {{0}, 0, 0}, b = {{0}, 0, 0};

  s21_convert_to_big(value_1, &a);
  s21_convert_to_big(value_2, &b);
  s21_big_align_scales(&a, &b);
  int cmp_result = s21_cmp_big(a, b);
  s21_big_decimal big_result = {{0}, 0, 0}, *minuend = NULL, *subtrahend = NULL;
  if (cmp_result < 0) {
    big_result.sign = ~a.sign;
    minuend = &b;
    subtrahend = &a;
  } else {
    big_result.sign = a.sign;
    minuend = &a;
    subtrahend = &b;
  }
  int error = s21_big_sub(*minuend, *subtrahend, &big_result);
  s21_convert_to_dec(big_result, result);
  return error;
}

int s21_big_sub(s21_big_decimal a, s21_big_decimal b, s21_big_decimal *result) {
  int error = 0;  // OK
  int first_remainder = 0, last_remainder = 0;

  s21_big_align_scales(&a, &b);
  result->scale = a.scale;
  for (int i = 0; i < BIG_SIZE_BITS; ++i) {
    uint64_t partial_minuend = a.bits[i], partial_subtrahend = b.bits[i];
    if (a.bits[i] < b.bits[i]) {
      s21_big_sub_borrow(&a, i + 1);
      partial_minuend += (uint64_t)1 << 32;
    }
    result->bits[i] = partial_minuend - partial_subtrahend;
  }
  s21_big_shrink_to_decimal(result, &first_remainder, &last_remainder);
  s21_big_banking_rounding(result, first_remainder, last_remainder);
  return error;
}

void s21_big_sub_borrow(s21_big_decimal *a, int starting_digit) {
  int current_digit = starting_digit;
  while (!a->bits[current_digit]) {
    ++current_digit;
  }
  --a->bits[current_digit];
  --current_digit;
  while (current_digit >= starting_digit) {
    a->bits[current_digit] = UINT_MAX;
    --current_digit;
  }
}

void s21_big_shrink_to_decimal(s21_big_decimal *value, int *first_remainder,
                               int *last_remainder) {
  while (s21_more_than_dec(*value) && value->scale) {
    if (*last_remainder && !(*first_remainder)) {
      *first_remainder = *last_remainder;
    }
    *last_remainder = s21_big_int_div(value, 10);
    --value->scale;
  }
}

void s21_big_banking_rounding(s21_big_decimal *value, int first_remainder,
                              int last_remainder) {
  if (last_remainder > 5 || (5 == last_remainder && first_remainder) ||
      (5 == last_remainder && (value->bits[0] & 1UL))) {
    s21_big_int_add(value, 1);
  }
}

int s21_more_than_dec(s21_big_decimal value) {
  return value.bits[3] || value.bits[4] || value.bits[5];
}

int s21_safe_fit_to_dec(s21_big_decimal *d) {
  int fail = 0;  // OK
  int too_small = 0;
  int first_remainder = 0, last_remainder = 0;

  s21_big_shrink_to_decimal(d, &first_remainder, &last_remainder);
  if (s21_more_than_dec(*d)) fail = d->sign ? 2 : 1;

  if (!fail && d->scale > 28) {  // result is almost 0
    s21_big_decrease_scale(d, &first_remainder, &last_remainder, d->scale - 28);
  }
  if (!fail && !(d->bits[0] || d->bits[1] || d->bits[2]) && d->scale) {
    *d = (s21_big_decimal){0};
    too_small = 1;
  }
  if (!fail && !too_small) {
    s21_big_banking_rounding(d, first_remainder, last_remainder);
    if (d->bits[3] && d->scale) {
      s21_big_decrease_scale(d, &first_remainder, &last_remainder, 1);
      s21_big_banking_rounding(d, first_remainder, last_remainder);
    } else if (d->bits[3]) {
      fail = d->sign ? 2 : 1;
    }
  }
  return fail;
}

s21_big_decimal s21_big_naive_sub(s21_big_decimal value_1,
                                  s21_big_decimal value_2) {
  s21_big_decimal result = {{0}, 0, 0};

  for (int i = 0; i < 191; i++) {
    int bit_1 = s21_big_getbit(value_1, i);
    int bit_2 = s21_big_getbit(value_2, i);
    if (bit_2 > bit_1) {
      s21_big_setbit(&result, i, 1);
      int j = i + 1;
      while (!s21_big_getbit(value_1, j)) {
        s21_big_setbit(&value_1, j, 1);
        j++;
      }
      s21_big_setbit(&value_1, j, 0);
    } else {
      s21_big_setbit(&result, i, bit_1 - bit_2);
    }
  }
  return result;
}

int float_is_zero(float value) {
  s21_float val = {.orig = value};
  if (val.mask.exp == 0 && val.mask.number == 0) {
    return 1;
  }
  return 0;
}

enum {
  SUCCESS = 0,
  ERROR = 1,
};

int is_even(char c) {
  int even = 0;
  if (c == '0' || c == '2' || c == '4' || c == '6' || c == '8') {
    even = 1;
  }
  return even;
}

void need_increment(info_float *info) {
  char remainder = info->mantissa[info->len];
  if (remainder == '6' || remainder == '7' || remainder == '8' ||
      remainder == '9' ||
      (remainder == '5' && !is_even(info->mantissa[info->len - 1]))) {
    info->need_to_increment = 1;
  }
}

int parse_float(float value, info_float *result) {
  int error = SUCCESS;
  char *str_to_parse = (char *)calloc(sizeof(char), 80);
  //    char str_to_parse[80];  //// do dinamic
  sprintf(str_to_parse, "%+.29f", value);
  result->sign = (str_to_parse[0] == '+') ? 0 : 1;
  int is_integer = 1;
  int cur_number = 0;
  int is_first = 1;
  int i = 1;

  for (; i < 80 && result->scale <= 29 && cur_number < 8 &&
         str_to_parse[i] != '\0';
       i++) {
    if (str_to_parse[i] == '.') {
      is_integer = 0;
      i++;
    }

    if (is_first && str_to_parse[i] == '0') {
      if (!is_integer) {
        result->scale++;
      }
      continue;
    }

    is_first = 0;
    result->mantissa[cur_number++] = str_to_parse[i];
    if (!is_integer) {
      result->scale++;
    }
  }

  if (is_integer) {
    while (str_to_parse[i] != '.') {
      result->scale--;
      i++;
    }
  }

  result->len = cur_number - 1;
  if (result->scale != 0) {
    result->scale--;
  }
  free(str_to_parse);
  if (result->scale > 28 || is_first) {
    error = ERROR;
  }
  return error;
}

unsigned info_to_int(info_float info) {
  int result = 0;
  for (int i = info.len - 1; i >= 0; i--) {
    result += (int)(info.mantissa[i] - '0') *
              pow((double)10, (double)(info.len - 1 - i));
  }
  if (info.need_to_increment) {
    result++;
  }
  return result;
}

int put_float_to_decimal(s21_decimal *dec, info_float *info, unsigned value) {
  int error = SUCCESS;

  s21_big_decimal big = {0};
  big.bits[0] = value;

  while (info->scale + (int)big.scale < 0 && error == SUCCESS) {
    s21_big_int_mul(&big, 10);
    info->scale++;
    if (s21_more_than_dec(big)) {
      error = ERROR;
    }
  }

  big.scale += info->scale;
  big.sign = info->sign;

  if (error == SUCCESS) {
    s21_convert_to_dec(big, dec);
  }

  return error;
}

/* ==========================================================================*/
/* Auxiliary functions for decimal, that not used in library

void s21_init_big_decimal(s21_big_decimal *src) {
  for (int i = 0; i < BIG_SIZE_BITS; i++) {
    src->bits[i] = 0;
  }
  src->scale = 0;
  src->sign = 0;
}

void s21_print(s21_decimal d) {
  char str[29] = {'\0'};
  s21_big_decimal b = {{0}, 0, 0};
  s21_convert_to_big(d, &b);

  for (int i = 0; i < 29; ++i) str[i] = s21_big_int_div(&b, 10) + '0';

  if (b.sign) printf("-");
  for (int i = 28; i >= 0; --i) {
    printf("%c", str[i]);
    if (i == b.scale)
      printf(".");
    else if (i % 3 == 0 && i)
      printf(",");
  }
  printf("\n");

  //     BINARY PRINT
  // for (int i = 0; i < 128; ++i) {
  //   printf("%d", s21_getBit(d, i));
  //   if ((i + 1) % 32 == 0 && i) printf(" ");
  // }
  // printf("\n");
}

int s21_is_valid_decimal(s21_decimal src) {
  int valid = 1;
  union {
    s21_decimal base;
    s21_dec parts;
  } uni;

  uni.base = src;
  if (uni.parts.garbage != 0 || uni.parts.trash != 0) {
    valid = 0;
  }
  return valid;
}

int s21_lshift(s21_decimal *d, unsigned i) {
  int lost_bit = 0;

  if (i < 96)
    while (--i < 96) {
      if (s21_getBit(*d, 95)) lost_bit = 1;
      for (size_t j = 95; j > 0; --j) s21_setBit(d, j, s21_getBit(*d, j - 1));
      s21_setBit(d, 0, 0);
    }
  else
    d->bits[0] = d->bits[1] = d->bits[2] = 0;

  return lost_bit;
}

int s21_rshift(s21_decimal *d, unsigned i) {
  int lost_bit = 0;

  if (i < 96)
    while (--i < 96) {
      if (s21_getBit(*d, 0)) lost_bit = 1;
      for (size_t j = 0; j < 95; ++j) s21_setBit(d, j, s21_getBit(*d, j + 1));
      s21_setBit(d, 95, 0);
    }
  else
    d->bits[0] = d->bits[1] = d->bits[2] = 0;

  return lost_bit;
}

int s21_big_rshift(s21_big_decimal *d, unsigned i) {
  int lost_bit = 0;

  if (i < 192)
    while (--i < 192) {
      if (s21_big_getbit(*d, 0)) lost_bit = 1;
      for (size_t j = 0; j < 191; ++j)
        s21_big_setbit(d, j, s21_big_getbit(*d, j + 1));
      s21_big_setbit(d, 191, 0);
    }
  else {
    d->bits[0] = d->bits[1] = d->bits[2] = d->bits[3] = d->bits[4] =
        d->bits[5] = 0;
    lost_bit = 1;
  }
  return lost_bit;
}

void s21_big_print(s21_big_decimal d) {
  for (int i = 0; i < 224; ++i) {
    printf("%d", s21_big_getbit(d, i));
    if ((i + 1) % 32 == 0 && i) printf(" ");
  }
  printf("\n");
}

void nice_print_float(float value) {
  unsigned aboba = *(unsigned *)&value;

  printf("float:  %f\n", value);

  for (int i = 31; i >= 0; i--) {
    printf("%u ", !!(aboba & (1U << i)));
  }
  printf("\n");

  s21_float abo = {.orig = value};
  printf("sign: %u\n", abo.mask.sign);
  printf("exp: %u\n", abo.mask.exp - 127);
}

void nice_print_big_dec(s21_big_decimal dec, int bin) {
  if (dec.sign) {
    printf("- ");
  }
  for (int i = BIG_SIZE_BITS - 1; i >= 0; i--) {
    if (bin) {
      for (int j = 31; j >= 0; j--) {
        printf("%d", s21_big_getbit(dec, i * 32 + j));
      }
    } else {
      printf("%u", dec.bits[i]);
    }
    printf(" ");
  }
  printf("scale: %u\n", dec.scale);
}

 */
/* ==========================================================================*/
