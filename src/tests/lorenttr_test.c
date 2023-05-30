/*
 * DO NOT EDIT THIS FILE. Generated by checkmk.
 * Edit the original source file "lorenttr_test.check" instead.
 */

#include <check.h>
#include <limits.h>
#include <string.h>

#include "../s21_decimal.h"

#define SET_MINUS(x) ((x) | 0x80000000)

#define SET_SCALE(x, y) ((x) | ((y) << 16))

START_TEST(equivalent_without_scale) {
  struct testcase {
    s21_decimal first, second;
  };

  struct testcase testcases[] = {
      {{{0}}, {{0}}},
      {{{1, 0, 0, 0}}, {{1, 0, 0, 0}}},
      {{{0, 0, 0, SET_MINUS(0)}}, {{0}}},
      {{{1, 0, 0, SET_MINUS(0)}}, {{1, 0, 0, SET_MINUS(0)}}},
      {{{123, 456, 789, 0}}, {{123, 456, 789, 0}}},
      {{{INT_MAX, INT_MAX, INT_MAX, 0}}, {{INT_MAX, INT_MAX, INT_MAX, 0}}},
      {{{INT_MAX, INT_MAX, INT_MAX, SET_MINUS(0)}},
       {{INT_MAX, INT_MAX, INT_MAX, SET_MINUS(0)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    ck_assert_int_eq(s21_is_equal(testcases[i].first, testcases[i].second), 1);
  }
}
END_TEST

START_TEST(equivalent_with_scale) {
  struct testcase {
    s21_decimal first, second;
  };

  struct testcase testcases[] = {
      {{{0x0, 0x0, 0x0, 0x10000}}, {{0, 0, 0, SET_MINUS(0)}}},
      {{{0, 0, 0, 0}}, {{0, 0, 0, SET_SCALE(0, 28)}}},
      {{{10000, 0, 0, SET_SCALE(0, 4)}}, {{1000000, 0, 0, SET_SCALE(0, 6)}}},
      {{{0, 0, 0, SET_MINUS(SET_SCALE(0, 14))}}, {{0, 0, 0, SET_SCALE(0, 20)}}},
      {{{123, 456, 789, SET_SCALE(0, 12)}},
       {{123, 456, 789, SET_SCALE(0, 12)}}},
      {{{1832294, 1238547, 2500000, SET_MINUS(SET_SCALE(0, 19))}},
       {{1832294, 1238547, 2500000, SET_MINUS(SET_SCALE(0, 19))}}},
      {{{~0, ~0, ~0, SET_SCALE(0, 1)}}, {{~0, ~0, ~0, SET_SCALE(0, 1)}}},
      {{{705032704, 1, 0, SET_SCALE(0, 9)}}, {{50, 0, 0, SET_SCALE(0, 1)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    ck_assert_int_eq(s21_is_equal(testcases[i].first, testcases[i].second), 1);
  }
}
END_TEST

START_TEST(different_without_scale) {
  struct testcase {
    s21_decimal first, second;
  };

  struct testcase testcases[] = {
      {{{1, 1, 0, 0}}, {{0, 0, 1, 0}}},
      {{{255, 255, 255, 0}}, {{255, 255, 255, SET_MINUS(0)}}},
      {{{~1, ~0, ~0, 0}}, {{~0, ~0, ~0, 0}}},
      {{{~0, ~1, 293, SET_MINUS(0)}}, {{~0, ~0, 293, SET_MINUS(0)}}},
      {{{~0, ~0, ~0, 0}}, {{~0, ~0, ~0, SET_MINUS(0)}}},
      {{{234, 8913250, 2831049, 0}}, {{141241, 4564363, 13919857}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    ck_assert_int_eq(s21_is_equal(testcases[i].first, testcases[i].second), 0);
  }
}
END_TEST

START_TEST(different_with_scale) {
  struct testcase {
    s21_decimal first, second;
  };

  struct testcase testcases[] = {
      {{{1, 0, 0, SET_SCALE(0, 14)}}, {{1, 0, 0, SET_SCALE(0, 28)}}},
      {{{234, 8391245, 2000000000, SET_SCALE(0, 3)}},
       {{234, 8391245, 2000000000, SET_MINUS(SET_SCALE(0, 3))}}},
      {{{0, 0, 2, SET_SCALE(0, 23)}}, {{2, 0, 0, SET_SCALE(0, 13)}}},
      {{{73812940, 1238594, ~23, SET_SCALE(0, 18)}},
       {{14895304, 4732995, ~933, SET_MINUS(SET_SCALE(0, 11))}}},
      {{{~0, ~0, ~0, SET_SCALE(0, 28)}}, {{~1, ~0, ~0, SET_SCALE(0, 28)}}},
      {{{~0, ~1, ~2, SET_SCALE(0, 27)}}, {{~2, ~2, ~3, SET_SCALE(0, 28)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    ck_assert_int_eq(s21_is_equal(testcases[i].first, testcases[i].second), 0);
  }
}
END_TEST

START_TEST(scaleless_positive_to_float) {
  struct testcase {
    s21_decimal d;
    char expected[30];
  };

  struct testcase testcases[] = {
      {{0}, "0.000000e+00"},
      {{{1, 0, 0, 0}}, "1.000000e+00"},
      {{{UINT_MAX, UINT_MAX, UINT_MAX, 0}}, "7.922816e+28"},
      {{{12319841, -2385142, 2000111333, 0}}, "3.689554e+28"},
      {{{-0, -1, 2, 0}}, "5.534023e+19"},
      {{{100000, 0, 0, 0}}, "1.000000e+05"}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    float actual = 0;
    char actual_str[30] = "";
    int response_code = s21_from_decimal_to_float(testcases[i].d, &actual);
    ck_assert_int_eq(0, response_code);
    sprintf(actual_str, "%7e", actual);
    ck_assert_str_eq(testcases[i].expected, actual_str);
  }
}
END_TEST

START_TEST(scaleless_negative_to_float) {
  struct testcase {
    s21_decimal d;
    char expected[30];
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(0)}}, "-0.000000e+00"},
      {{{1, 0, 0, SET_MINUS(0)}}, "-1.000000e+00"},
      {{{UINT_MAX, UINT_MAX, UINT_MAX, SET_MINUS(0)}}, "-7.922816e+28"},
      {{{12319841, -2385142, 2000111333, SET_MINUS(0)}}, "-3.689554e+28"},
      {{{-0, -1, 2, SET_MINUS(0)}}, "-5.534023e+19"},
      {{{100000, 0, 0, SET_MINUS(0)}}, "-1.000000e+05"}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    float actual = 0;
    char actual_str[30] = "";
    int response_code = s21_from_decimal_to_float(testcases[i].d, &actual);
    ck_assert_int_eq(0, response_code);
    sprintf(actual_str, "%7e", actual);
    ck_assert_str_eq(testcases[i].expected, actual_str);
  }
}
END_TEST

START_TEST(scaled_positive_to_float) {
  struct testcase {
    s21_decimal d;
    char expected[30];
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_SCALE(0, 28)}}, "0.000000e+00"},
      {{{1, 0, 0, SET_SCALE(0, 14)}}, "1.000000e-14"},
      {{{~0, ~0, ~0, SET_SCALE(0, 18)}}, "7.922817e+10"},
      {{{-0, -1, 2, SET_SCALE(0, 5)}}, "5.534023e+14"},
      {{{100000, 0, 0, SET_SCALE(0, 10)}}, "1.000000e-05"},
      {{{276447232, 23283, 0, SET_SCALE(0, 14)}}, "1.000000e+00"},
  };

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    float actual = 0;
    char actual_str[30] = "";
    int response_code = s21_from_decimal_to_float(testcases[i].d, &actual);
    ck_assert_int_eq(0, response_code);
    sprintf(actual_str, "%7e", actual);
    ck_assert_str_eq(testcases[i].expected, actual_str);
  }
}
END_TEST

START_TEST(scaled_negative_to_float) {
  struct testcase {
    s21_decimal d;
    char expected[30];
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(SET_SCALE(0, 22))}}, "-0.000000e+00"},
      {{{1, 0, 0, SET_MINUS(SET_SCALE(0, 13))}}, "-1.000000e-13"},
      {{{~0, ~0, ~0, SET_MINUS(SET_SCALE(0, 16))}}, "-7.922816e+12"},
      {{{123456789, 123456789, 123456789, SET_MINUS(SET_SCALE(0, 6))}},
       "-2.277376e+21"},
      {{{-400000000, -395013495, -277777777, SET_MINUS(SET_SCALE(0, 27))}},
       "-7.410406e+01"},
      {{{0, 2, 0, SET_MINUS(SET_SCALE(0, 12))}}, "-8.589935e-03"},
      {{{-2, 0, 0, SET_MINUS(SET_SCALE(0, 6))}}, "-4.294967e+03"}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    float actual = 0;
    char actual_str[30] = "";
    int response_code = s21_from_decimal_to_float(testcases[i].d, &actual);
    ck_assert_int_eq(0, response_code);
    sprintf(actual_str, "%7e", actual);
    ck_assert_str_eq(testcases[i].expected, actual_str);
  }
}
END_TEST

START_TEST(truncate_positive_integer) {
  struct testcase {
    s21_decimal d;
    s21_decimal expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, 0}}, {{0, 0, 0, 0}}},
      {{{1, 0, 0, 0}}, {{1, 0, 0, 0}}},
      {{{~0, ~0, ~0, 0}}, {{~0, ~0, ~0, 0}}},
      {{{0, 23479241, 142, 0}}, {{0, 23479241, 142, 0}}},
      {{{0, 0, 1 << 31, 0}}, {{0, 0, 1 << 31, 0}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    s21_truncate(testcases[i].d, &actual);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(truncate_negative_integer) {
  struct testcase {
    s21_decimal d;
    s21_decimal expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(0)}}, {{0, 0, 0, SET_MINUS(0)}}},
      {{{1, 0, 0, SET_MINUS(0)}}, {{1, 0, 0, SET_MINUS(0)}}},
      {{{~0, ~0, ~0, SET_MINUS(0)}}, {{~0, ~0, ~0, SET_MINUS(0)}}},
      {{{0, 23479241, 142, SET_MINUS(0)}}, {{0, 23479241, 142, SET_MINUS(0)}}},
      {{{0, 0, 1 << 31, SET_MINUS(0)}}, {{0, 0, 1 << 31, SET_MINUS(0)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    s21_truncate(testcases[i].d, &actual);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(truncate_fraction_only_positive) {
  struct testcase {
    s21_decimal d;
    s21_decimal expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_SCALE(0, 28)}}, {{0, 0, 0, 0}}},
      {{{1, 0, 0, SET_SCALE(0, 10)}}, {{0, 0, 0, 0}}},
      {{{~0, 0, 0, SET_SCALE(0, 20)}}, {{0, 0, 0, 0}}},
      {{{0, 23479241, 142, SET_SCALE(0, 22)}}, {{0, 0, 0, 0}}},
      {{{0, 0, 1 << 29, SET_SCALE(0, 28)}}, {{0, 0, 0, 0}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    s21_truncate(testcases[i].d, &actual);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(truncate_fraction_only_negative) {
  struct testcase {
    s21_decimal d;
    s21_decimal expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(SET_SCALE(0, 28))}}, {{0, 0, 0, SET_MINUS(0)}}},
      {{{1, 0, 0, SET_MINUS(SET_SCALE(0, 10))}}, {{0, 0, 0, SET_MINUS(0)}}},
      {{{1 << 31, 0, 0, SET_MINUS(SET_SCALE(0, 20))}},
       {{0, 0, 0, SET_MINUS(0)}}},
      {{{0, 23479241, 142, SET_MINUS(SET_SCALE(0, 22))}},
       {{0, 0, 0, SET_MINUS(0)}}},
      {{{0, 0, 1 << 29, SET_MINUS(SET_SCALE(0, 28))}},
       {{0, 0, 0, SET_MINUS(0)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    s21_truncate(testcases[i].d, &actual);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(truncate_positive_real_numbers) {
  struct testcase {
    s21_decimal d;
    s21_decimal expected;
  };

  struct testcase testcases[] = {
      {{{11, 0, 0, SET_SCALE(0, 1)}}, {{1, 0, 0, 0}}},
      {{{1234, 1234, 0, SET_SCALE(0, 7)}}, {{529998, 0, 0, 0}}},
      {{{9872487, 1324258, 1, SET_SCALE(0, 9)}}, {{1272562534, 4, 0, 0}}},
      {{{1 << 31, 1 << 31, 1 << 31, SET_SCALE(0, 11)}},
       {{1675134288, 92233720, 0, 0}}},
      {{{~0, ~0, ~0, SET_SCALE(0, 28)}}, {{7, 0, 0, 0}}},
      {{{1 << 31, 1 << 31, 1 << 31, SET_SCALE(0, 2)}},
       {{-1868310774, 2083059138, 21474836, 0}}},
      {{{1 << 28, 1 << 29, 1 << 30, SET_SCALE(0, 20)}},
       {{198070406, 0, 0, 0}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    s21_truncate(testcases[i].d, &actual);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(truncate_negative_real_numbers) {
  struct testcase {
    s21_decimal d;
    s21_decimal expected;
  };

  struct testcase testcases[] = {
      {{{11, 0, 0, SET_MINUS(SET_SCALE(0, 1))}}, {{1, 0, 0, SET_MINUS(0)}}},
      {{{1234, 1234, 0, SET_MINUS(SET_SCALE(0, 7))}},
       {{529998, 0, 0, SET_MINUS(0)}}},
      {{{9872487, 1324258, 1, SET_MINUS(SET_SCALE(0, 9))}},
       {{1272562534, 4, 0, SET_MINUS(0)}}},
      {{{1 << 31, 1 << 31, 1 << 31, SET_MINUS(SET_SCALE(0, 11))}},
       {{1675134288, 92233720, 0, SET_MINUS(0)}}},
      {{{~0, ~0, ~0, SET_MINUS(SET_SCALE(0, 28))}}, {{7, 0, 0, SET_MINUS(0)}}},
      {{{1 << 31, 1 << 31, 1 << 31, SET_MINUS(SET_SCALE(0, 2))}},
       {{-1868310774, 2083059138, 21474836, SET_MINUS(0)}}},
      {{{1 << 28, 1 << 29, 1 << 30, SET_MINUS(SET_SCALE(0, 20))}},
       {{198070406, 0, 0, SET_MINUS(0)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    s21_truncate(testcases[i].d, &actual);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(s21_mul_too_large_both_positive) {
  struct testcase {
    s21_decimal m1, m2;
  };

  struct testcase testcases[] = {
      {{{~0, ~0, ~0, 0}}, {{~0, ~0, ~0, 0}}},
      {{{~0, ~0, ~0, 0}}, {{1000000001, 0, 0, SET_SCALE(0, 9)}}},
      {{{0, 65536, 0, 0}}, {{0, 65536, 0, 0}}},
      {{{-1183973946, 65535998, 0, SET_SCALE(0, 10)}},
       {{-1106682409, -1290490193, 542101084, SET_SCALE(0, 6)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal product = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &product);
    ck_assert_int_eq(1, result);
  }
}
END_TEST

START_TEST(s21_mul_too_large_both_negative) {
  struct testcase {
    s21_decimal m1, m2;
  };

  struct testcase testcases[] = {
      {{{~0, ~0, ~0, SET_MINUS(0)}}, {{~0, ~0, ~0, SET_MINUS(0)}}},
      {{{~0, ~0, ~0, SET_MINUS(0)}},
       {{1000000001, 0, 0, SET_MINUS(SET_SCALE(0, 9))}}},
      {{{0, 65536, 0, SET_MINUS(0)}}, {{0, 65536, 0, SET_MINUS(0)}}},
      {{{-1183973946, 65535998, 0, SET_MINUS(SET_SCALE(0, 10))}},
       {{-1106682409, -1290490193, 542101084, SET_MINUS(SET_SCALE(0, 6))}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal product = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &product);
    ck_assert_int_eq(1, result);
  }
}
END_TEST

START_TEST(s21_mul_too_small_test) {
  struct testcase {
    s21_decimal m1, m2;
  };

  struct testcase testcases[] = {
      {{{~0, ~0, ~0, SET_MINUS(0)}}, {{~0, ~0, ~0, 0}}},
      {{{~0, ~0, ~0, SET_MINUS(0)}}, {{1000000001, 0, 0, SET_SCALE(0, 9)}}},
      {{{0, 65536, 0, SET_MINUS(0)}}, {{0, 65536, 0, 0}}},
      {{{-1183973946, 65535998, 0, SET_MINUS(SET_SCALE(0, 10))}},
       {{-1106682409, -1290490193, 542101084, SET_SCALE(0, 6)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal product = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &product);
    ck_assert_int_eq(2, result);
  }
}
END_TEST

START_TEST(s21_mul_integers_positive) {
  struct testcase {
    s21_decimal m1, m2, expected;
  };

  struct testcase testcases[] = {
      {{0}, {0}, {0}},
      {{{1, 0, 0, 0}}, {{1, 0, 0, 0}}, {{1, 0, 0, 0}}},
      {{{1, 0, 0, 0}}, {{0, 0, 0, 0}}, {{0, 0, 0, 0}}},
      {{{132, 132456, -12356, 0}}, {{1, 0, 0, 0}}, {{132, 132456, -12356, 0}}},
      {{{-153858243, -34352, 98756855, 0}},
       {{23, 0, 0, 0}},
       {{756227707, -790074, -2023559609, 0}}},
      {{{1949518561, 1596751841, 1742987178, 0}},
       {{2, 0, 0, 0}},
       {{-395930174, -1101463614, -808992940, 0}}},
      {{{337877533, 179538, 0, 0}},
       {{1038886773, 22199, 0, 0}},
       {{-1669604799, -574096166, -309358061, 0}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &actual);
    ck_assert_int_eq(0, result);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(s21_mul_integers_negative) {
  struct testcase {
    s21_decimal m1, m2, expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(0)}}, {{0, 0, 0, SET_MINUS(0)}}, {0}},
      {{{1, 0, 0, SET_MINUS(0)}}, {{1, 0, 0, SET_MINUS(0)}}, {{1, 0, 0, 0}}},
      {{{1, 0, 0, SET_MINUS(0)}}, {{0, 0, 0, SET_MINUS(0)}}, {{0, 0, 0, 0}}},
      {{{132, 132456, -12356, SET_MINUS(0)}},
       {{1, 0, 0, SET_MINUS(0)}},
       {{132, 132456, -12356, 0}}},
      {{{-153858243, -34352, 98756855, SET_MINUS(0)}},
       {{23, 0, 0, SET_MINUS(0)}},
       {{756227707, -790074, -2023559609, 0}}},
      {{{1466598063, 49532, 0, SET_MINUS(0)}},
       {{2076169216, 51648, 0, SET_MINUS(0)}},
       {{-909513728, -360855579, -1736696981, 0}}},
      {{{1798353175, 154262, 0, SET_MINUS(0)}},
       {{404678566, 25365, 0, SET_MINUS(0)}},
       {{1511036394, 2020344948, -382086511, 0}}},
      {{{726598452, 31175, 0, SET_MINUS(0)}},
       {{1339733510, 9131, 0, SET_MINUS(0)}},
       {{-160888008, 1002829430, 284670194, 0}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &actual);
    ck_assert_int_eq(0, result);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(s21_mul_integers_mixed) {
  struct testcase {
    s21_decimal m1, m2, expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(0)}}, {{0, 0, 0, 0}}, {{0, 0, 0, SET_MINUS(0)}}},
      {{{1, 0, 0, SET_MINUS(0)}}, {{1, 0, 0, 0}}, {{1, 0, 0, SET_MINUS(0)}}},
      {{{1, 0, 0, SET_MINUS(0)}}, {{0, 0, 0, 0}}, {{0, 0, 0, SET_MINUS(0)}}},
      {{{132, 132456, -12356, SET_MINUS(0)}},
       {{1, 0, 0, 0}},
       {{132, 132456, -12356, SET_MINUS(0)}}},
      {{{-153858243, -34352, 98756855, SET_MINUS(0)}},
       {{23, 0, 0, 0}},
       {{756227707, -790074, -2023559609, SET_MINUS(0)}}},
      {{{1466598063, 49532, 0, SET_MINUS(0)}},
       {{2076169216, 51648, 0, 0}},
       {{-909513728, -360855579, -1736696981, SET_MINUS(0)}}},
      {{{2067285406, 73491, 0, 0}},
       {{638442302, 21306, 0, SET_MINUS(0)}},
       {{-130742716, -1753952995, 1565820425, SET_MINUS(0)}}},
      {{{1655066706, 6040, 0, SET_MINUS(0)}},
       {{655373352, 184973, 0, 0}},
       {{-1251134256, 427664804, 1117309121, SET_MINUS(0)}}},
      {{{308453193, 63700, 0, 0}},
       {{779085263, 17078, 0, SET_MINUS(0)}},
       {{1155668231, 1573824821, 1087881381, SET_MINUS(0)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &actual);
    ck_assert_int_eq(0, result);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(s21_mul_real_numbers_positive) {
  struct testcase {
    s21_decimal m1, m2, expected;
  };

  struct testcase testcases[] = {
      {{0}, {{84820171, 1291613596, 349684731, SET_SCALE(0, 10)}}, {0}},
      {{{1, 0, 0, 0}},
       {{547961084, 729372191, 1114941508, SET_SCALE(0, 4)}},
       {{547961084, 729372191, 1114941508, SET_SCALE(0, 4)}}},
      {{{349661166, 1268731999, 1035799827, SET_SCALE(0, 27)}},
       {{1529868052, 29143101, 1580116825, SET_SCALE(0, 27)}},
       {{325868322, -1838116711, -1275816854, SET_SCALE(0, 26)}}},
      {{{1197665832, 1399128433, 1451175058, SET_SCALE(0, 17)}},
       {{598142756, 279535710, 1363083175, SET_SCALE(0, 24)}},
       {{1987257257, -209528548, -646067938, SET_SCALE(0, 13)}}},
      {{{410531689, 1690752887, 2026265109, SET_SCALE(0, 25)}},
       {{1418075727, 1574016222, 1594010991, SET_SCALE(0, 19)}},
       {{-432986778, 403444809, 595809331, SET_SCALE(0, 15)}}},
      {{{0x696FB643, 0x189773F0, 0x487F2C10, 0x1A0000}},
       {{0xCF4C962, 0x4150D5BB, 0x8C4DF77, 0x10000}},
       {{0x7B44D87, 0x448A4ED4, 0xC4BF74A3, 0x0}}},
      {{{1340952301, 843176066, 400067931, SET_SCALE(0, 18)}},
       {{1031839210, 1020206087, 857246069, SET_SCALE(0, 21)}},
       {{1843089677, 888573342, 632643376, SET_SCALE(0, 11)}}},
      {{{2022103232, 700145740, 306401761, SET_SCALE(0, 11)}},
       {{1378383756, 1480044569, 1811619385, SET_SCALE(0, 24)}},
       {{1577871507, 61454334, 1023948087, SET_SCALE(0, 7)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &actual);
    ck_assert_int_eq(0, result);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
    // for (size_t j = 0; j < SIZE_BITS; ++j) {
    //     fprintf(stderr, "%08x\t%08x\n", actual.bits[j],
    //     testcases[i].expected.bits[j]);
    // }
  }
}
END_TEST

START_TEST(s21_mul_real_numbers_negative) {
  struct testcase {
    s21_decimal m1, m2, expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(0)}},
       {{84820171, 1291613596, 349684731, SET_MINUS(SET_SCALE(0, 10))}},
       {0}},
      {{{1, 0, 0, SET_MINUS(0)}},
       {{547961084, 729372191, 1114941508, SET_MINUS(SET_SCALE(0, 4))}},
       {{547961084, 729372191, 1114941508, SET_SCALE(0, 4)}}},
      {{{587216699, 1882812493, 1930319518, SET_MINUS(SET_SCALE(0, 13))}},
       {{1558150263, 1264315772, 559483926, SET_MINUS(SET_SCALE(0, 15))}},
       {{-46022834, -1100479915, 1992216526, 0}}},
      {{{2073142480, 1203533782, 1901854035, SET_MINUS(SET_SCALE(0, 18))}},
       {{1995730700, 977688620, 1688633650, SET_MINUS(SET_SCALE(0, 14))}},
       {{-1762173841, -137796440, 592423590, SET_SCALE(0, 3)}}},
      {{{2035839223, 1610892021, 324053634, SET_MINUS(SET_SCALE(0, 16))}},
       {{395556729, 630182896, 1009169624, SET_MINUS(SET_SCALE(0, 20))}},
       {{1325174481, -981586861, 603254803, SET_SCALE(0, 8)}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &actual);
    ck_assert_int_eq(0, result);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

START_TEST(s21_mul_real_numbers_mixed) {
  struct testcase {
    s21_decimal m1, m2, expected;
  };

  struct testcase testcases[] = {
      {{{0, 0, 0, SET_MINUS(0)}},
       {{84820171, 1291613596, 349684731, SET_SCALE(0, 10)}},
       {0}},
      {{{1, 0, 0, 0}},
       {{547961084, 729372191, 1114941508, SET_MINUS(SET_SCALE(0, 4))}},
       {{547961084, 729372191, 1114941508, SET_MINUS(SET_SCALE(0, 4))}}},
      {{{383059707, 2016743972, 1990961363, SET_MINUS(SET_SCALE(0, 14))}},
       {{1236246134, 1126255921, 1051426866, SET_SCALE(0, 27)}},
       {{604218321, -1527242416, -433417633, SET_MINUS(SET_SCALE(0, 13))}}},
      {{{324141542, 1017037978, 238073690, SET_SCALE(0, 24)}},
       {{1221194580, 1354062899, 567867869, SET_MINUS(SET_SCALE(0, 18))}},
       {{-1151598054, -2086451725, -1801070814, SET_MINUS(SET_SCALE(0, 15))}}},
      {{{353635367, 772936516, 570596361, SET_MINUS(SET_SCALE(0, 26))}},
       {{1181260420, 1053299168, 978507572, SET_SCALE(0, 23)}},
       {{-1683548855, -689488375, 1029942337, SET_MINUS(SET_SCALE(0, 21))}}}};

  size_t n = sizeof(testcases) / sizeof(testcases[0]);

  for (size_t i = 0; i < n; ++i) {
    s21_decimal actual = {0};
    int result = s21_mul(testcases[i].m1, testcases[i].m2, &actual);
    ck_assert_int_eq(0, result);
    ck_assert_int_eq(1, s21_is_equal(testcases[i].expected, actual));
  }
}
END_TEST

int lorenttr_test(void) {
  Suite *s1 = suite_create("test_s21_is_equal");
  TCase *tc1_1 = tcase_create("equivalent_values");
  TCase *tc1_2 = tcase_create("different_values");
  Suite *s2 = suite_create("test_s21_from_decimal_to_float");
  TCase *tc2_1 = tcase_create("to_float_without_scale");
  TCase *tc2_2 = tcase_create("to_float_with_scale");
  Suite *s3 = suite_create("test_s21_truncate");
  TCase *tc3_1 = tcase_create("truncate_integer_decimals");
  TCase *tc3_2 = tcase_create("truncate_fraction_only_decimals");
  TCase *tc3_3 = tcase_create("truncate_real_numbers");
  Suite *s4 = suite_create("test_s21_mul");
  TCase *tc4_1 = tcase_create("s21_mul_too_large");
  TCase *tc4_2 = tcase_create("s21_mul_too_small");
  TCase *tc4_3 = tcase_create("s21_mul_ok_integers");
  TCase *tc4_4 = tcase_create("s21_mul_ok_real_numbers");
  SRunner *sr = srunner_create(s1);
  int nf;

  suite_add_tcase(s1, tc1_1);
  tcase_add_test(tc1_1, equivalent_without_scale);
  tcase_add_test(tc1_1, equivalent_with_scale);
  suite_add_tcase(s1, tc1_2);
  tcase_add_test(tc1_2, different_without_scale);
  tcase_add_test(tc1_2, different_with_scale);
  suite_add_tcase(s2, tc2_1);
  tcase_add_test(tc2_1, scaleless_positive_to_float);
  tcase_add_test(tc2_1, scaleless_negative_to_float);
  suite_add_tcase(s2, tc2_2);
  tcase_add_test(tc2_2, scaled_positive_to_float);
  tcase_add_test(tc2_2, scaled_negative_to_float);
  suite_add_tcase(s3, tc3_1);
  tcase_add_test(tc3_1, truncate_positive_integer);
  tcase_add_test(tc3_1, truncate_negative_integer);
  suite_add_tcase(s3, tc3_2);
  tcase_add_test(tc3_2, truncate_fraction_only_positive);
  tcase_add_test(tc3_2, truncate_fraction_only_negative);
  suite_add_tcase(s3, tc3_3);
  tcase_add_test(tc3_3, truncate_positive_real_numbers);
  tcase_add_test(tc3_3, truncate_negative_real_numbers);
  suite_add_tcase(s4, tc4_1);
  tcase_add_test(tc4_1, s21_mul_too_large_both_positive);
  tcase_add_test(tc4_1, s21_mul_too_large_both_negative);
  suite_add_tcase(s4, tc4_2);
  tcase_add_test(tc4_2, s21_mul_too_small_test);
  suite_add_tcase(s4, tc4_3);
  tcase_add_test(tc4_3, s21_mul_integers_positive);
  tcase_add_test(tc4_3, s21_mul_integers_negative);
  tcase_add_test(tc4_3, s21_mul_integers_mixed);
  suite_add_tcase(s4, tc4_4);
  tcase_add_test(tc4_4, s21_mul_real_numbers_positive);
  tcase_add_test(tc4_4, s21_mul_real_numbers_negative);
  tcase_add_test(tc4_4, s21_mul_real_numbers_mixed);

  srunner_add_suite(sr, s2);
  srunner_add_suite(sr, s3);
  srunner_add_suite(sr, s4);

  srunner_run_all(sr, CK_VERBOSE);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf == 0 ? 0 : 1;
}
