#include <Servo.h>

// ---------------- PINS ----------------
#define TRIG_PIN 10
#define ECHO_PIN 9
#define SERVO_PIN 7
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define ENA 6
#define ENB 11

// ---------------- SETTINGS ----------------
#define SAFE_DISTANCE 35        // Reduced: react sooner
#define CRITICAL_DISTANCE 15   // Emergency stop threshold
#define TURN_SPEED 75          // Higher turn speed = more reliable turns
#define NUM_READINGS 3          // Median filter samples

int leftMotorSpeed = 78;
int rightMotorSpeed = 70;

Servo scanner;

// ---------------- RELIABLE DISTANCE (Median Filter) ----------------
long getSingleDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

// Takes multiple readings and returns the median — eliminates bad spikes
long getDistance() {
  long readings[NUM_READINGS];
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = getSingleDistance();
    delay(15); // small gap between pings prevents echo overlap
  }
  // Simple sort for median
  for (int i = 0; i < NUM_READINGS - 1; i++)
    for (int j = i + 1; j < NUM_READINGS; j++)
      if (readings[i] > readings[j]) {
        long tmp = readings[i];
        readings[i] = readings[j];
        readings[j] = tmp;
      }
  return readings[NUM_READINGS / 2];
}

// ---------------- MOTOR CONTROLS ----------------
void moveForward() {
  analogWrite(ENA, leftMotorSpeed);
  analogWrite(ENB, rightMotorSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  analogWrite(ENA, leftMotorSpeed);
  analogWrite(ENB, rightMotorSpeed);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void turnLeft() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); // Left backward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // Right forward
}

void turnRight() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // Left forward
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); // Right backward
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ---------------- TURN WITH VERIFICATION ----------------
// Turns for given ms, then re-checks front is actually clear
void turnUntilClear(bool goLeft, int maxAttempts = 5) {
  for (int i = 0; i < maxAttempts; i++) {
    if (goLeft) turnLeft(); else turnRight();
    delay(600);
    stopMotors();
    delay(150);

    scanner.write(90);
    delay(300);
    long check = getDistance();
    Serial.print("Post-turn check: "); Serial.println(check);

    if (check > SAFE_DISTANCE) return; // Path is clear, done!
    // Still blocked — turn a bit more
  }
}

// ---------------- SETUP ----------------
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  scanner.attach(SERVO_PIN);
  scanner.write(90);
  Serial.begin(9600);
  delay(1500); // Let servo settle
}

// ---------------- MAIN LOOP ----------------
void loop() {
  scanner.write(90);
  delay(250);
  long frontDistance = getDistance();

  Serial.print("Front: "); Serial.println(frontDistance);

  // Emergency: too close, stop immediately
  if (frontDistance < CRITICAL_DISTANCE) {
    stopMotors();
    delay(100);
    moveBackward();
    delay(600);
    stopMotors();
    delay(200);
  }

  // Path is clear
  if (frontDistance > SAFE_DISTANCE) {
    moveForward();
    return;
  }

  // --- Obstacle Detected ---
  stopMotors();
  delay(200);

  // Back up
  moveBackward();
  delay(450);
  stopMotors();
  delay(200);

  // Scan LEFT
  scanner.write(150); // wider angle = better view
  delay(500);
  long leftDistance = getDistance();

  // Scan RIGHT
  scanner.write(30);
  delay(500);
  long rightDistance = getDistance();

  // Re-center
  scanner.write(90);
  delay(300);

  Serial.print("Left: ");  Serial.println(leftDistance);
  Serial.print("Right: "); Serial.println(rightDistance);

  // --- Decide direction with verification ---
  if (leftDistance > rightDistance && leftDistance > SAFE_DISTANCE) {
    Serial.println("→ TURN LEFT");
    turnUntilClear(true);
  }
  else if (rightDistance >= leftDistance && rightDistance > SAFE_DISTANCE) {
    Serial.println("→ TURN RIGHT");
    turnUntilClear(false);
  }
  else {
    // Dead end: back up more and do a wider turn
    Serial.println("→ DEAD END – reversing + turning");
    moveBackward();
    delay(700);
    stopMotors();
    delay(200);

    // Pick the slightly better side even if both bad
    if (leftDistance >= rightDistance) turnUntilClear(true, 8);
    else turnUntilClear(false, 8);
  }

  stopMotors();
  delay(200);
}