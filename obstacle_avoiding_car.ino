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
#define NUM_READINGS 3         // Median filter samples
#define MIN_VALID_READINGS (NUM_READINGS / 2 + 1)
#define SENSOR_TIMEOUT_US 30000UL
#define INVALID_DISTANCE -1L

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
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, SENSOR_TIMEOUT_US);
  if (duration == 0) return INVALID_DISTANCE;
  return duration * 0.034 / 2;
}

bool isValidDistance(long distance) {
  return distance > 0;
}

// Requires a majority of valid pings. For an even valid set, the lower median
// wins so one larger reading can never make the car assume extra clearance.
long getDistance() {
  long readings[NUM_READINGS];
  int validReadings = 0;

  for (int i = 0; i < NUM_READINGS; i++) {
    long reading = getSingleDistance();
    if (isValidDistance(reading)) {
      readings[validReadings] = reading;
      validReadings++;
    }
    delay(15); // small gap between pings prevents echo overlap
  }

  if (validReadings < MIN_VALID_READINGS) return INVALID_DISTANCE;

  // Simple sort for median
  for (int i = 0; i < validReadings - 1; i++)
    for (int j = i + 1; j < validReadings; j++)
      if (readings[i] > readings[j]) {
        long tmp = readings[i];
        readings[i] = readings[j];
        readings[j] = tmp;
      }
  return readings[(validReadings - 1) / 2];
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
bool turnUntilClear(bool goLeft, int maxAttempts = 5) {
  for (int i = 0; i < maxAttempts; i++) {
    if (goLeft) turnLeft(); else turnRight();
    delay(600);
    stopMotors();
    delay(150);

    scanner.write(90);
    delay(300);
    long check = getDistance();
    Serial.print("[VERIFY] Forward clearance (cm): "); Serial.println(check);

    if (!isValidDistance(check)) {
      Serial.println("[SENSOR] Verification timed out; stopping instead of turning blind");
      stopMotors();
      return false;
    }
    if (check > SAFE_DISTANCE) return true; // Path is clear, done!
    // Still blocked — turn a bit more
  }
  Serial.println("[SAFETY] Turn limit reached; holding position");
  stopMotors();
  return false;
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
  Serial.println("[READY] Obstacle car initialized; scanner centered");
  Serial.println("[STATE] Motors stopped until the first verified reading");
  stopMotors();
}

// ---------------- MAIN LOOP ----------------
void loop() {
  scanner.write(90);
  delay(250);
  long frontDistance = getDistance();

  Serial.print("[SENSOR] Front clearance (cm): "); Serial.println(frontDistance);

  if (!isValidDistance(frontDistance)) {
    Serial.println("[SENSOR] Front sensor timed out; holding position for a fresh reading");
    stopMotors();
    delay(250);
    return;
  }

  // Emergency: too close, stop immediately
  if (frontDistance < CRITICAL_DISTANCE) {
    Serial.println("[SAFETY] Critical clearance; stopping and reversing");
    stopMotors();
    delay(100);
    moveBackward();
    delay(600);
    stopMotors();
    delay(200);
    return;
  }

  // Path is clear
  if (frontDistance > SAFE_DISTANCE) {
    Serial.println("[ACTION] Path clear; moving forward");
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

  Serial.print("[SENSOR] Left clearance (cm): ");  Serial.println(leftDistance);
  Serial.print("[SENSOR] Right clearance (cm): "); Serial.println(rightDistance);

  bool leftValid = isValidDistance(leftDistance);
  bool rightValid = isValidDistance(rightDistance);
  if (!leftValid && !rightValid) {
    Serial.println("[SENSOR] Both side scans timed out; holding position");
    stopMotors();
    delay(250);
    return;
  }

  // --- Decide direction with verification ---
  if (leftValid && leftDistance > SAFE_DISTANCE && (!rightValid || leftDistance > rightDistance)) {
    Serial.println("[ACTION] Turning left");
    turnUntilClear(true);
  }
  else if (rightValid && rightDistance > SAFE_DISTANCE) {
    Serial.println("[ACTION] Turning right");
    turnUntilClear(false);
  }
  else {
    // Dead end: back up more and do a wider turn
    Serial.println("[SAFETY] Dead end; reversing before a wider turn");
    moveBackward();
    delay(700);
    stopMotors();
    delay(200);

    // Pick the slightly better side even if both bad
    if (leftValid && (!rightValid || leftDistance >= rightDistance)) turnUntilClear(true, 8);
    else if (rightValid) turnUntilClear(false, 8);
  }

  stopMotors();
  delay(200);
}
