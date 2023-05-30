#include "ivarstet_test.c"
#include "lorenttr_test.c"
#include "tabathae_test.c"
#include "wilmerno_test.c"

int main() {
  lorenttr_test();
  tabathae_test();
  wilmerno_test();
  ivarstet_test();
}