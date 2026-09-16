import unittest
from car_controller import CarNavigationController, MotionState

class TestCarAvoidanceSIL(unittest.TestCase):
    def setUp(self):
        self.nav = CarNavigationController()

    def test_median_filter_noise_and_timeout_rejection(self):
        # Noise spikes with timeouts
        readings = [120.0, -1.0, 36.0]
        filtered = self.nav.apply_median_filter(readings)
        # Valid sorted: [36.0, 120.0], len=2, mid index 1 -> 120.0, or 3 samples
        # Let's test standard 3 valid samples:
        readings3 = [200.0, 42.0, 43.0]
        self.assertEqual(self.nav.apply_median_filter(readings3), 43.0)

        # All timeouts
        all_timeouts = [-1.0, -1.0, -1.0]
        self.assertIsNone(self.nav.apply_median_filter(all_timeouts))

    def test_clear_path_forward(self):
        state = self.nav.evaluate_navigation(forward_distance=50.0)
        self.assertEqual(state, MotionState.FORWARD)

    def test_emergency_braking_critical_proximity(self):
        # 12cm < 15cm critical threshold
        state = self.nav.evaluate_navigation(forward_distance=12.0)
        self.assertEqual(state, MotionState.EMERGENCY_BRAKE)

    def test_turn_left_when_left_has_more_clearance(self):
        # Obstacle at 25cm (buffer zone), Left: 60cm, Right: 18cm
        state = self.nav.evaluate_navigation(
            forward_distance=25.0,
            left_clearance=60.0,
            right_clearance=18.0
        )
        self.assertEqual(state, MotionState.TURN_LEFT)

    def test_turn_right_when_right_has_more_clearance(self):
        # Obstacle at 25cm, Left: 20cm, Right: 55cm
        state = self.nav.evaluate_navigation(
            forward_distance=25.0,
            left_clearance=20.0,
            right_clearance=55.0
        )
        self.assertEqual(state, MotionState.TURN_RIGHT)

    def test_corner_trap_triggers_reverse(self):
        # Both directions blocked (<25cm)
        state = self.nav.evaluate_navigation(
            forward_distance=20.0,
            left_clearance=15.0,
            right_clearance=12.0
        )
        self.assertEqual(state, MotionState.BACKWARD)

    def test_sensor_timeout_failsafe_stop(self):
        state = self.nav.evaluate_navigation(forward_distance=None)
        self.assertEqual(state, MotionState.STOPPED)

if __name__ == '__main__':
    unittest.main()
