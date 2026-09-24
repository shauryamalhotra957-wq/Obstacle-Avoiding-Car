import pytest
from sim.trajectory_planner import TrajectoryPlanner

def test_stopping_distance_calculation():
    planner = TrajectoryPlanner(wheelbase_cm=15.0)
    assert planner.calculate_stopping_distance_cm(0) == 0.0
    # speed = 30 cm/s, decel = 120 -> 900 / 240 = 3.75 cm
    assert planner.calculate_stopping_distance_cm(30.0) == 3.75

def test_evasion_heading_selection():
    planner = TrajectoryPlanner()
    assert planner.select_evasion_heading(left_distance_cm=45.0, right_distance_cm=20.0) == "STEER_LEFT"
    assert planner.select_evasion_heading(left_distance_cm=15.0, right_distance_cm=50.0) == "STEER_RIGHT"
    assert planner.select_evasion_heading(left_distance_cm=10.0, right_distance_cm=12.0) == "REVERSE_ESCAPE"

def test_turning_radius():
    planner = TrajectoryPlanner(wheelbase_cm=15.0, max_steering_angle_deg=45.0)
    radius_45 = planner.compute_turning_radius_cm(45.0)
    radius_30 = planner.compute_turning_radius_cm(30.0)
    assert radius_30 > radius_45
    assert radius_45 > 0
