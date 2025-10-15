#include <Servo.h>
#include <math.h>

#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_SERVO 10

#define SOUND_SPEED 346.0
#define PULSE_DURATION 10
#define INTERVAL 100
#define SCALE (0.001 * 0.5 * SOUND_SPEED)

#define THRESHOLD 200    // 차량 감지 거리(mm)
#define SERVO_MIN 90
#define SERVO_MAX 0

#define MODE_SIGMOID 1
#define MODE_EASE 2

int MODE = MODE_EASE;  // Sigmoid: SIGMOID / Ease-in-out: EASE 로 바꾸면 됨. 

Servo gate;
bool car_detected = false;
unsigned long last_update = 0;

// Sigmoid Function
float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

// Ease-In-Out (cosine) Function
float easeInOut(float x) {
  return (1.0 - cos(M_PI * x)) / 2.0;
}

// 초음파 거리 측정
float measure_distance() {
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(PIN_TRIG, LOW);
  float distance = pulseIn(PIN_ECHO, HIGH, 30000) * SCALE; // mm
  return distance;
}


void smoothMove(int fromAngle, int toAngle) {
  int steps = 50;
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;
    float curve;

    if (MODE == MODE_SIGMOID) {
      float z = (t - 0.5) * 12.0;   
      curve = sigmoid(z);
    } else {
      curve = easeInOut(t);
    }

    int angle = fromAngle + (toAngle - fromAngle) * curve;
    gate.write(angle);
    delay(30);
  }
}

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  Serial.begin(57600);

  gate.attach(PIN_SERVO);
  gate.write(SERVO_MIN);  // 시작은 닫힌 상태
  Serial.println("Parking Gate System Ready!");
}

void loop() {
  if (millis() - last_update < INTERVAL) return;
  last_update = millis();

  float dist = measure_distance();
  Serial.print("Distance(mm): "); Serial.println(dist);

  // 차량 감지 로직 (올바른 방향)
  if (dist > 50 && dist <= THRESHOLD) {
    // 가까이 있음 → 차단기 열림
    if (!car_detected) {
      Serial.println("Car detected → Gate Opening");
      smoothMove(SERVO_MIN, SERVO_MAX);
      car_detected = true;
    }
  } 
  else if (dist > THRESHOLD || dist == 0) {
    // 멀어짐 → 차단기 닫힘
    if (car_detected) {
      Serial.println("Car passed → Gate Closing");
      smoothMove(SERVO_MAX, SERVO_MIN);
      car_detected = false;
    }
  }
}
