# Obstacle-Avoiding Robot Car: System Architecture

An autonomous robotic vehicle featuring median-filtered ultrasonic ranging, dynamic servo field-of-view scanning, and differential dual-motor steering.

## 1. System Topology

```mermaid
flowchart TD
    subgraph Perception["Perception Subsystem"]
        US["HC-SR04 Ultrasonic Distance Sensor"]
        Servo["SG90 Pan Servo (0° - 180°)"]
    end

    subgraph Decision["Control & Navigation Core"]
        Filter["Rolling 5-Point Median Filter"]
        SafetySupervisor["Emergency Distance Arbiter (< 15cm)"]
        PathSolver["Multi-Sector Corridor Evaluator"]
        DeadlockDetector["Stall & Oscillation Watchdog"]
    end

    subgraph Actuation["Mobility Subsystem"]
        HBridge["L298N Dual H-Bridge Driver"]
        MotorL["Left DC Motor Gearbox"]
        MotorR["Right DC Motor Gearbox"]
    end

    US --> Filter
    Servo -.-> US
    Filter --> SafetySupervisor
    SafetySupervisor --> PathSolver
    DeadlockDetector --> PathSolver
    PathSolver --> HBridge
    HBridge --> MotorL
    HBridge --> MotorR
```

## 2. Obstacle Evasion Sequence

```mermaid
sequenceDiagram
    autonumber
    participant Sensor as HC-SR04 Ranging
    participant Brain as Navigation Logic
    participant Servo as Scanning Servo
    participant Motors as L298N H-Bridge

    loop Forward Cruise
        Brain->>Sensor: Ping Forward Distance
        Sensor-->>Brain: 45 cm (Clear)
        Brain->>Motors: DRIVE_FORWARD (PWM 200)
    end

    Note over Brain,Sensor: Obstacle Detected at 12 cm!
    Brain->>Motors: EMERGENCY_BRAKE (Active Low)
    Brain->>Motors: REVERSE_MOMENTARY (300ms)
    Brain->>Servo: Sweep Left (150°)
    Servo-->>Brain: Left Distance = 10 cm
    Brain->>Servo: Sweep Right (30°)
    Servo-->>Brain: Right Distance = 55 cm
    Brain->>Servo: Center Servo (90°)
    Brain->>Motors: PIVOT_RIGHT (Turn Verification)
    Brain->>Motors: RESUME_FORWARD
```

## 3. Navigation State Machine

```mermaid
stateDiagram-v2
    [*] --> ForwardCruising
    ForwardCruising --> EmergencyStop: Distance < 15cm
    EmergencyStop --> ReverseEscape: Reverse 250ms
    ReverseEscape --> PanoramicScan: Stop & Sweep
    PanoramicScan --> PivotTurn: Select Clear Corridor (Left/Right)
    PivotTurn --> VerificationScan: Pan Forward
    VerificationScan --> ForwardCruising: Path Clear > 30cm
    VerificationScan --> EmergencyStop: Path Blocked (Deadlock Detected)
```
