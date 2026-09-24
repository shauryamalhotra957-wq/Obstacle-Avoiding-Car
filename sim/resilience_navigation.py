"""
Obstacle-Avoiding Car: Deadlock Escape & Resilience Navigation Engine.
Detects traps, wheel stall conditions, and executes escape maneuvers.
"""
import random

class DeadlockEscapeEngine:
    def __init__(self, max_consecutive_turns: int = 4):
        self.max_consecutive_turns = max_consecutive_turns
        self.turn_history = []
        self.deadlock_active = False

    def record_turn(self, direction: str) -> bool:
        """Record turn event. Return True if deadlock oscillation detected."""
        self.turn_history.append(direction)
        if len(self.turn_history) > self.max_consecutive_turns:
            self.turn_history.pop(0)

        # Detect alternating oscillation (e.g. L, R, L, R)
        if len(self.turn_history) >= 4:
            if self.turn_history[-4:] == ['LEFT', 'RIGHT', 'LEFT', 'RIGHT'] or                self.turn_history[-4:] == ['RIGHT', 'LEFT', 'RIGHT', 'LEFT']:
                self.deadlock_active = True
                return True
                
        # Detect trapped spinning in one direction
        if len(self.turn_history) >= self.max_consecutive_turns and len(set(self.turn_history)) == 1:
            self.deadlock_active = True
            return True
            
        return False

    def compute_escape_maneuver(self) -> dict:
        """Generate randomized high-degree reverse-pivot to escape corner traps."""
        reverse_duration_ms = random.randint(400, 700)
        pivot_direction = random.choice(['LEFT', 'RIGHT'])
        pivot_duration_ms = random.randint(600, 1000)
        self.turn_history.clear()
        self.deadlock_active = False
        return {
            "action": "ESCAPE_MANEUVER",
            "reverse_duration_ms": reverse_duration_ms,
            "pivot_direction": pivot_direction,
            "pivot_duration_ms": pivot_duration_ms
        }
