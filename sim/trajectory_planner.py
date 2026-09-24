"""
Kinematic Trajectory Planner & Safety Buffer Calculator.
Calculates steering angle arc curvature and dynamic stopping distances
based on ultrasonic proximity readings and vehicle linear velocity.
"""
import math

class TrajectoryPlanner:
    def __init__(self, wheelbase_cm: float = 15.0, max_steering_angle_deg: float = 45.0):
        self.wheelbase_cm = wheelbase_cm
        self.max_steering_angle_deg = max_steering_angle_deg

    def calculate_stopping_distance_cm(self, speed_cm_per_sec: float, deceleration_cm_per_sec2: float = 120.0) -> float:
        if speed_cm_per_sec <= 0:
            return 0.0
        return round((speed_cm_per_sec ** 2) / (2 * deceleration_cm_per_sec2), 2)

    def select_evasion_heading(self, left_distance_cm: float, right_distance_cm: float, min_clearance_cm: float = 25.0) -> str:
        """
        Determines the optimal lateral avoidance heading based on obstacle field geometry.
        """
        if left_distance_cm < min_clearance_cm and right_distance_cm < min_clearance_cm:
            return "REVERSE_ESCAPE"
        if right_distance_cm > left_distance_cm:
            return "STEER_RIGHT"
        return "STEER_LEFT"

    def compute_turning_radius_cm(self, steering_angle_deg: float) -> float:
        clamped_angle = max(1.0, min(self.max_steering_angle_deg, abs(steering_angle_deg)))
        rad = math.radians(clamped_angle)
        return round(self.wheelbase_cm / math.sin(rad), 2)
