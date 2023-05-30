#pragma once

#include "../s21_decimal.h"

#define BIG_SIZE_BITS 6
#define MAX_DECIMAL pow(2, 96) - 1

typedef struct {
  unsigned int bits[BIG_SIZE_BITS];
  unsigned scale : 7;
  unsigned sign : 1;
} s21_big_decimal;

typedef struct {
  uint32_t number[3];
  int trash : 16;
  unsigned scale : 8;
  int garbage : 7;
  unsigned sign : 1;
} s21_dec;

typedef union {
  s21_decimal orig;
  s21_dec mask;
} s21_decimal_mask;

typedef struct {
  unsigned number : 23;
  unsigned exp : 8;
  unsigned sign : 1;
} s21_mask_float;

typedef union {
  float orig;
  s21_mask_float mask;
} s21_float;

#define MAX_PRECISION 8

typedef struct {
  char mantissa[MAX_PRECISION];
  int sign;
  int scale;
  int need_to_increment;
  int len;
} info_float;

/* Helpers */
void s21_init_decimal(s21_decimal *src);
int s21_getBit(s21_decimal d, int i);
void s21_setBit(s21_decimal *d, int i, unsigned value);
int s21_getSignBit(s21_decimal d);
void s21_setSignBit(s21_decimal *d, int sign_bit);
int s21_decimal_cmp(s21_decimal a, s21_decimal b);
int s21_is_zero_decimal(s21_decimal src);
int s21_is_valid_decimal(s21_decimal src);

/* Helpers for Big Decimal */

/* converters */
void s21_convert_to_big(s21_decimal src, s21_big_decimal *tgt);
void s21_convert_to_dec(s21_big_decimal src, s21_decimal *tgt);

/* bit operations */
int s21_big_getbit(s21_big_decimal d, int i);
void s21_big_setbit(s21_big_decimal *d, int i, unsigned value);
int s21_big_lshift(s21_big_decimal *d, unsigned i);

/* arithmetic operations for int as 2nd operand */
int s21_big_int_div(s21_big_decimal *value, int divisor);
void s21_big_int_mul(s21_big_decimal *value, int multiplier);
void s21_big_int_add(s21_big_decimal *value, int addend);

/* arithmetic auxiliary functions */
int s21_big_dec_add(s21_big_decimal value_1, s21_big_decimal value_2,
                    s21_big_decimal *result);
int s21_big_sub(s21_big_decimal a, s21_big_decimal b, s21_big_decimal *result);
int s21_true_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void s21_big_sub_borrow(s21_big_decimal *a, int starting_digit);
s21_big_decimal s21_big_naive_sub(s21_big_decimal value_1,
                                  s21_big_decimal value_2);
void s21_big_mul(s21_big_decimal bd1, s21_big_decimal bd2,
                 s21_big_decimal *res);
void s21_big_div(s21_big_decimal big_value_1, s21_big_decimal big_value_2,
                 s21_big_decimal *big_result);
void s21_big_truncate(s21_big_decimal *b);

/* scale modifying functions */
void s21_div_rescaling(s21_big_decimal *big_result, int bits_counter,
                       int result_temp_scale);
void s21_big_align_scales(s21_big_decimal *a, s21_big_decimal *b);
void s21_big_increase_scale(s21_big_decimal *d, int increment);
void s21_big_decrease_scale(s21_big_decimal *d, int *first_remainder,
                            int *last_reminder, int decrement);

/* rounding functions */
void s21_big_shrink_to_decimal(s21_big_decimal *value, int *first_remainder,
                               int *last_reminder);
int s21_safe_fit_to_dec(s21_big_decimal *d);
void s21_big_banking_rounding(s21_big_decimal *value, int first_remainder,
                              int last_remainder);

/* compare functions */
int s21_cmp_big(s21_big_decimal a, s21_big_decimal b);
int s21_more_than_dec(s21_big_decimal value);

// float functions

int float_is_zero(float value);
int is_even(char c);
void need_increment(info_float *info);
int parse_float(float value, info_float *result);
unsigned info_to_int(info_float info);
int put_float_to_decimal(s21_decimal *dec, info_float *info, unsigned value);

/* ==========================================================================*/
/* Auxiliary functions for decimal, that not used in library


// init
void s21_init_big_decimal(s21_big_decimal *src);

void s21_print(s21_decimal d);
int s21_is_valid_decimal(s21_decimal src);
int s21_lshift(s21_decimal *d, unsigned i);
int s21_rshift(s21_decimal *d, unsigned i);
int s21_big_rshift(s21_big_decimal *d, unsigned i);
void s21_big_print(s21_big_decimal d);
void nice_print_big_dec(s21_big_decimal dec, int bin);
void nice_print_float(float value);

 */
/* ==========================================================================*/