# s21_Decimal

### Own Implementation of the decimal.h library in "C# style".

### This is a team project developed in collaboration with tabathae, ivarstet and lorenttr, also students of School 21 in Moscow.

* The program was developed and implemented on MacOs Big Sur Version 11.6.6, Processor Intel Core i5 and repeats the behavior of `decimal type` functions in C# language.
* You may build static library by running Makefile with `make` or `make s21_decimal.a` and use in your own projects in C language
* To run unit tests check.h library needs to be installed, target `make test`, it builds another library for testing with flags of coverage
* You may check test's coverage, for `make gcov_report` you must have the `lcov` utility installed, for `make gcovr_report` you must have the `gcovr` utility installed, i prefer the second option, which has better interface.
