#include "../distance_filter.h"

static_assert(conservativeMedianIndex(1) == 0,
              "one reading must select its only sample");
static_assert(conservativeMedianIndex(2) == 0,
              "two readings must select the nearer sample");
static_assert(conservativeMedianIndex(3) == 1,
              "three readings must select the true median");

int main() {
  return 0;
}
