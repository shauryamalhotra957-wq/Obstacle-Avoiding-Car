"""
Robotic Safety & Collision Boundary Supervisor.
"""
class CollisionSafetySupervisor:
    CRITICAL_STOP_DISTANCE_CM = 15.0
    SLOW_SPEED_DISTANCE_CM = 35.0
    MAX_VALID_RANGE_CM = 400.0
    MIN_VALID_RANGE_CM = 2.0

    @classmethod
    def evaluate_distance(cls, raw_cm: float) -> str:
        """Evaluate safety zone based on physical boundary distance."""
        if raw_cm < cls.MIN_VALID_RANGE_CM or raw_cm > cls.MAX_VALID_RANGE_CM:
            return "OUT_OF_BOUNDS_FAULT"
        if raw_cm <= cls.CRITICAL_STOP_DISTANCE_CM:
            return "EMERGENCY_STOP"
        if raw_cm <= cls.SLOW_SPEED_DISTANCE_CM:
            return "CAUTION_REDUCE_SPEED"
        return "CLEAR_PATH"
