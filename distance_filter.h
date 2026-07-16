#pragma once

// Assumes the valid distance readings have already been sorted ascending.
// For an even count, select the lower middle reading so obstacle detection
// remains conservative when a sensor sample is missing.
constexpr int conservativeMedianIndex(int validCount) {
  return (validCount - 1) / 2;
}
