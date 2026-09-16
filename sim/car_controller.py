"""
Obstacle-Avoiding Robot Car: Software-in-the-Loop (SIL) Controller
Digital-twin of obstacle_avoiding_car.ino logic
"""

from enum import Enum
from typing import List, Optional

class MotionState(Enum):
    FORWARD = "FORWARD"
    BACKWARD = "BACKWARD"
    TURN_LEFT = "TURN_LEFT"
    TURN_RIGHT = "TURN_RIGHT"
    STOPPED = "STOPPED"
    EMERGENCY_BRAKE = "EMERGENCY_BRAKE"

class CarNavigationController:
    SAFE_DISTANCE_CM = 35       # Move forward only above this clearance
    CRITICAL_DISTANCE_CM = 15   # Emergency stop threshold
    MIN_TURN_CLEARANCE_CM = 25  # Minimum clearance to turn without reversing

    @staticmethod
    def apply_median_filter(raw_readings: List[float]) -> Optional[float]:
        """
        Takes raw ultrasonic samples, filters out timeouts (-1 or None),
        and returns the median of the valid readings.
        """
        valid = [r for r in raw_readings if r is not None and r >= 0]
        if not valid:
            return None
        valid.sort()
        mid = len(valid) // 2
        return valid[mid]

    def evaluate_navigation(
        self,
        forward_distance: Optional[float],
        left_clearance: Optional[float] = None,
        right_clearance: Optional[float] = None
    ) -> MotionState:
        """
        Decides motor actuation command based on sensor readings.
        """
        # 1. Sensor failure / timeout: Fail-safe stop
        if forward_distance is None or forward_distance < 0:
            return MotionState.STOPPED

        # 2. Critical Emergency Brake: Obstacle directly ahead inside safety bubble
        if forward_distance <= self.CRITICAL_DISTANCE_CM:
            return MotionState.EMERGENCY_BRAKE

        # 3. Path Clear: Cruise forward
        if forward_distance >= self.SAFE_DISTANCE_CM:
            return MotionState.FORWARD

        # 4. Obstacle Ahead in Buffer Zone [15cm, 35cm]: Evaluate scanning look-ahead
        if left_clearance is None or right_clearance is None:
            return MotionState.STOPPED

        left_val = left_clearance if left_clearance is not None else 0
        right_val = right_clearance if right_clearance is not None else 0

        # Trap detection: Both directions blocked
        if left_val < self.MIN_TURN_CLEARANCE_CM and right_val < self.MIN_TURN_CLEARANCE_CM:
            return MotionState.BACKWARD

        # Choose the path with greater clearance
        if left_val > right_val:
            return MotionState.TURN_LEFT
        else:
            return MotionState.TURN_RIGHT
