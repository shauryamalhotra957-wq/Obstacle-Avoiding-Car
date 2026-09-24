"""
Obstacle Avoidance Decision Latency Benchmark.
"""
import time
from sim.safety_supervisor import CollisionSafetySupervisor
from sim.resilience_navigation import DeadlockEscapeEngine

def run_benchmark():
    engine = DeadlockEscapeEngine()
    cycles = 100_000
    start = time.perf_counter()
    for i in range(cycles):
        dist = 10.0 + (i % 60)
        status = CollisionSafetySupervisor.evaluate_distance(dist)
        if status == "EMERGENCY_STOP":
            engine.record_turn("LEFT" if i % 2 == 0 else "RIGHT")
    end = time.perf_counter()
    elapsed = end - start
    print(f"Benchmark: {cycles} navigation evaluations in {elapsed:.4f}s ({cycles/elapsed:,.0f} eval/sec)")

if __name__ == '__main__':
    run_benchmark()
