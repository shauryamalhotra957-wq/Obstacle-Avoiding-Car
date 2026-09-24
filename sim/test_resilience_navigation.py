import pytest
from sim.safety_supervisor import CollisionSafetySupervisor
from sim.resilience_navigation import DeadlockEscapeEngine

def test_collision_zones():
    assert CollisionSafetySupervisor.evaluate_distance(10.0) == "EMERGENCY_STOP"
    assert CollisionSafetySupervisor.evaluate_distance(25.0) == "CAUTION_REDUCE_SPEED"
    assert CollisionSafetySupervisor.evaluate_distance(120.0) == "CLEAR_PATH"
    assert CollisionSafetySupervisor.evaluate_distance(1.0) == "OUT_OF_BOUNDS_FAULT"
    assert CollisionSafetySupervisor.evaluate_distance(450.0) == "OUT_OF_BOUNDS_FAULT"

def test_deadlock_oscillation_detection():
    engine = DeadlockEscapeEngine(max_consecutive_turns=4)
    assert engine.record_turn('LEFT') is False
    assert engine.record_turn('RIGHT') is False
    assert engine.record_turn('LEFT') is False
    # Alternating 4th turn trips deadlock!
    assert engine.record_turn('RIGHT') is True
    assert engine.deadlock_active is True

def test_escape_maneuver_execution():
    engine = DeadlockEscapeEngine()
    engine.deadlock_active = True
    escape = engine.compute_escape_maneuver()
    assert escape["action"] == "ESCAPE_MANEUVER"
    assert escape["reverse_duration_ms"] >= 400
    assert escape["pivot_direction"] in ['LEFT', 'RIGHT']
    assert engine.deadlock_active is False
