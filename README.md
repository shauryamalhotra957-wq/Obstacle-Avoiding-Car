# Obstacle Avoiding Car

Arduino code for a small obstacle-avoiding robot car. The robot uses an HC-SR04 ultrasonic sensor mounted on a servo, median-filtered distance readings, emergency braking, directional scanning, turn verification, and fail-safe timeout handling to avoid obstacles more reliably.

![Obstacle avoiding car logic](docs/readme-preview.svg)

## Project Snapshot

| Area | Detail |
| --- | --- |
| Experience | Arduino autonomous robot car sketch |
| Core system | Ultrasonic sensing, median filtering, servo scan, motor control, turn verification |
| Design signal | Pin map, safety thresholds, and readable decision loop |
| Quality signal | Hardware setup notes, fail-safe sensor handling, compile CI, calibration guidance |

## What It Does

- Measures distance with an HC-SR04 ultrasonic sensor.
- Uses a median filter to reduce bad readings and echo spikes.
- Stops and reverses if an obstacle is critically close.
- Scans left and right using a servo-mounted sensor.
- Chooses the clearer direction.
- Turns, re-centers the sensor, and verifies that the path is actually clear.
- Treats a missing ultrasonic echo as a sensor failure instead of assuming the road is clear.
- Controls two DC motors through an L298N-style motor driver.

## Hardware

- Arduino Uno or compatible board
- HC-SR04 ultrasonic sensor
- Servo motor
- L298N motor driver
- Two DC motors
- Robot car chassis
- External battery supply for motors

## Pin Map

| Component | Arduino Pin |
|---|---|
| HC-SR04 trigger | `10` |
| HC-SR04 echo | `9` |
| Servo signal | `7` |
| Motor IN1 | `2` |
| Motor IN2 | `3` |
| Motor IN3 | `4` |
| Motor IN4 | `5` |
| Motor ENA | `6` |
| Motor ENB | `11` |

## Core Logic

```text
Read front distance
  -> hold position if the ultrasonic sensor times out
  -> emergency stop if too close
  -> move forward if clear
  -> reverse if blocked
  -> scan left and right
  -> treat timed-out side scans as blocked
  -> turn toward better path
  -> verify front is clear before continuing
```

## Sensor Fail-Safe

An HC-SR04 timeout means no echo pulse was received before the configured timeout. That is **not** treated as a very large distance.

- If the front reading times out, the motors stop and the robot holds position.
- If a left or right scan times out, that direction is treated as blocked for the turn decision.
- If verification after a turn times out, the robot does not accept that reading as proof that the path is clear.

This prevents a disconnected, obstructed, or malfunctioning ultrasonic sensor from being interpreted as an open road.

## Safety Thresholds

```cpp
#define SAFE_DISTANCE 35
#define CRITICAL_DISTANCE 15
#define TURN_SPEED 75
#define NUM_READINGS 3
#define SENSOR_TIMEOUT_US 30000UL
```

Tune these values for your chassis, motor speed, battery voltage, and sensor placement.

## Quick Start

1. Install the Arduino IDE.
2. Install or enable the built-in `Servo` library.
3. Open `obstacle_avoiding_car.ino`.
4. Wire the robot according to the pin map.
5. Upload the sketch.
6. Open Serial Monitor at `9600` baud for distance and decision logs.

Every push and pull request also compiles the sketch for an Arduino Uno through GitHub Actions.

## Why Median Filtering Matters

Ultrasonic sensors can occasionally return noisy spikes. This project takes multiple readings, discards timed-out samples, sorts the valid measurements, and uses their median. That makes the robot less likely to react to a single bad measurement while still failing safe if every sample times out.

## Repository Structure

```text
Obstacle-Avoiding-Car/
  obstacle_avoiding_car.ino
  README.md
  .github/
    workflows/
      ci.yml
  docs/
    readme-preview.svg
```

## Future Improvements

- Add encoder feedback for more accurate turns.
- Add PID speed balancing between left and right motors.
- Add a battery voltage monitor.
- Add a wiring diagram photo.
- Add chassis calibration notes.
- Add autonomous maze-solving behavior.

## Safety Notes

Use a separate motor power supply and common ground with the Arduino. Keep wheels lifted during first upload/testing so unexpected motor movement does not damage the robot or nearby objects.

Before driving on the floor, perform a sensor-failure bench test with the wheels lifted: disconnect or cover the HC-SR04 echo path and confirm Serial Monitor reports the timeout while the motors remain stopped.

## Operator Experience

The serial console exposes `READY`, `SENSOR`, `ACTION`, `VERIFY`, and `SAFETY` states instead of ambiguous raw messages. The [operator experience guide](docs/USER_EXPERIENCE.md) defines the setup journey, recovery model, and presentation rules for future hardware revisions.
