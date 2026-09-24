# Obstacle-Avoiding Car: Robotics Safety & Threat Analysis

Autonomous ground vehicles must guarantee physical safety, fail-safe stopping, and acoustic anomaly handling.

| Risk Category | Threat Description | Severity | Safety Mitigation |
|---|---|---|---|
| **Acoustic Blindness** | Sound-absorbing materials (foam/fur) failing to reflect HC-SR04 ultrasonic echo | High | Speed-limited cruising + multi-sample echo timeout fallback |
| **Motor Stall Burnout** | Wheels trapped against barrier drawing high stall current (>2A) | High | Deadlock timeout: If forward velocity zero for >1.5s, trigger reverse-pivot |
| **Brownout Glitch** | Motor inrush current dropping 5V rail to MCU causing spurious resets | Critical | Independent motor battery bus + bulk 470uF decoupling capacitors |
| **Servo Jitter** | High-frequency pulse variations shaking distance sensor during scan | Low | Deadband timing delay (200ms settling time after servo sweep) |
| **Corner Trap Oscillations** | Alternating left-right pivots infinitely inside acute angle | Medium | Randomized pivot bias angle upon 3 consecutive pivot events |
