# Obstacle-Avoiding Robot Car: Field Operations & Calibration Runbook

Standard operating procedures for chassis alignment, servo centering, motor balance, and sensor calibration.

## Diagnostic Calibration Checklist
1. **Servo Mechanical Zeroing**:
   - Detach servo horn.
   - Command servo to write `90°`.
   - Reattach ultrasonic sensor bracket pointing parallel to chassis forward vector.
2. **Motor Trim Adjustment**:
   - Drive car forward on flat terrain.
   - If car drifts right, increase left motor PWM offset or verify gear friction.
3. **HC-SR04 Echo Calibration**:
   - Measure 50.0 cm to flat wall using physical rule.
   - Verify serial telemetry reports between 49.0 cm and 51.0 cm.

## Emergency Playbooks
- **Wheel Lock / Motor Stall**: If car does not translate while PWM active, cut main motor battery switch immediately to prevent L298N thermal shutdown.
- **Sensor Blind Spot Recovery**: Install secondary side-facing IR sensors if encountering glass partitions or sound-absorbing drapery.
