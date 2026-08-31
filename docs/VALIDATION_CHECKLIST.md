# Hardware validation checklist

Use this checklist after a wiring change, firmware change, or chassis rebuild. Keep the wheels lifted until the fail-safe checks pass.

## Before power-on

- Verify the Arduino ground and motor-driver ground are common.
- Confirm the motor supply is separate from the Arduino's logic supply and that the driver is rated for the expected load.
- Check the HC-SR04 and servo connectors against the pin map in the README.
- Ensure the servo can sweep without pulling on wires or hitting the chassis.
- Place the car away from stairs, traffic, people, pets, and water.

## Bench checks

1. Upload the sketch and open Serial Monitor at `9600` baud.
2. With the wheels raised, verify that a clear front target produces stable distance readings.
3. Place an object inside `SAFE_DISTANCE` and confirm that the car stops/reverses rather than driving forward.
4. Cover or disconnect the ultrasonic echo path. Confirm that the console reports a timeout and the motors remain stopped.
5. Restore the sensor, place a nearer obstacle to one side, and confirm that the scan selects the clearer side.
6. After a turn, confirm that the servo re-centres and the robot checks the new forward path before moving.

## Floor test

- Start at low motor speed in an open, flat area.
- Test one obstacle at a time before testing multiple obstacles.
- Record the battery voltage, surface type, and tuned values for `SAFE_DISTANCE`, `CRITICAL_DISTANCE`, and `TURN_SPEED`.
- Repeat the timeout test with the wheels on the floor; sensor failure must still leave the car stopped.

Do not treat a successful bench test as permission to run unattended. This prototype should always have an operator ready to remove motor power.
