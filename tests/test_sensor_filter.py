from pathlib import Path
import unittest


SKETCH = (Path(__file__).parents[1] / "obstacle_avoiding_car.ino").read_text(
    encoding="utf-8"
)
INVALID_DISTANCE = -1
MIN_VALID_READINGS = 2


def conservative_distance(readings: list[int]) -> int:
    valid_readings = sorted(reading for reading in readings if reading > 0)
    if len(valid_readings) < MIN_VALID_READINGS:
        return INVALID_DISTANCE
    return valid_readings[(len(valid_readings) - 1) // 2]


class SensorFilterSafetyTests(unittest.TestCase):
    def test_sketch_requires_a_majority_quorum(self) -> None:
        self.assertIn(
            "#define MIN_VALID_READINGS (NUM_READINGS / 2 + 1)",
            SKETCH,
        )
        self.assertIn(
            "if (validReadings < MIN_VALID_READINGS) return INVALID_DISTANCE;",
            SKETCH,
        )

    def test_sketch_selects_the_lower_median(self) -> None:
        self.assertIn(
            "return readings[(validReadings - 1) / 2];",
            SKETCH,
        )

    def test_two_valid_pings_choose_the_smaller_distance(self) -> None:
        self.assertEqual(conservative_distance([80, INVALID_DISTANCE, 20]), 20)

    def test_missing_quorum_is_invalid(self) -> None:
        self.assertEqual(
            conservative_distance([80, INVALID_DISTANCE, INVALID_DISTANCE]),
            INVALID_DISTANCE,
        )

    def test_three_valid_pings_keep_the_median_filter(self) -> None:
        self.assertEqual(conservative_distance([80, 20, 50]), 50)


if __name__ == "__main__":
    unittest.main()
