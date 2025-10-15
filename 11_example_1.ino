#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9   // LED active-low 
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10  // servo motor

// Configurable parameters
#define SND_VEL 346.0     // sound velocity (m/sec)
#define INTERVAL 25       // sampling interval (msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (usec)
#define _DIST_MIN 180.0   // 18cm (unit: mm) 
#define _DIST_MAX 360.0   // 36cm (unit: mm) 
#define _EMA_ALPHA 0.3    // EMA filter weight [cite: 68]

// Duty duration for servo motor (NEEDS TUNING for your specific servo)
#define _DUTY_MIN 600 // servo 0 degree position
#define _DUTY_MAX 2300 // servo 180 degree position

// Calculated constants
#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // duration to distance conversion factor

// Global variables
float dist_prev_raw = _DIST_MAX;     // Stores the last valid raw distance for the range filter
float dist_ema_prev = _DIST_MAX;     // Stores the last EMA value for the next calculation
unsigned long last_sampling_time;    // Stores the last measurement time

Servo myservo; // Create servo object

void setup() {
  // Initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, HIGH);  // Turn off LED initially (active-low)

  // Attach servo and set to a neutral position
  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds((_DUTY_MIN + _DUTY_MAX) / 2); // Start at 90 degrees

  // Initialize serial port for monitoring
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered, dist_ema;

  // 1. TIMING: Check if it's time for the next sample
  if (millis() < last_sampling_time + INTERVAL)
    return;
  last_sampling_time = millis(); // Update sampling time

  // 2. MEASUREMENT: Get a distance reading from the sensor
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // 3. RANGE FILTER: Prevent servo from jumping on failed readings 
  if (dist_raw < _DIST_MIN || dist_raw > _DIST_MAX) {
    dist_filtered = dist_prev_raw; // If out of range, use the last good value
  } else {
    dist_filtered = dist_raw;      // If in range, use the current value
    dist_prev_raw = dist_raw;      // and update the last good value
  }

  // 4. EMA FILTER: Smooth out the distance to prevent jitter 
  dist_ema = _EMA_ALPHA * dist_filtered + (1.0 - _EMA_ALPHA) * dist_ema_prev;
  dist_ema_prev = dist_ema; // Save the current EMA value for the next loop

  // 5. SERVO CONTROL: Control the servo based on the smoothed distance
  int servo_us;
  if (dist_ema <= _DIST_MIN) {
    servo_us = _DUTY_MIN; // Distance <= 18cm: 0 degrees [cite: 60]
  }
  else if (dist_ema >= _DIST_MAX) {
    servo_us = _DUTY_MAX; // Distance >= 36cm: 180 degrees 
  }
  else {
    // Proportional control between 18cm and 36cm 
    servo_us = map(dist_ema, _DIST_MIN, _DIST_MAX, _DUTY_MIN, _DUTY_MAX);
  }
  myservo.writeMicroseconds(servo_us);

  // 6. LED CONTROL: Turn LED on/off based on the raw, unfiltered distance 
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    digitalWrite(PIN_LED, LOW); // Turn ON if within range
  } else {
    digitalWrite(PIN_LED, HIGH); // Turn OFF if out of range or measurement failed
  }

  // 7. SERIAL PLOTTER OUTPUT (for debugging and visualization)
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",dist:"); Serial.print(dist_raw);
  Serial.print(",ema:");  Serial.print(dist_ema);
  Serial.print(",Servo:");Serial.print(myservo.read());
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");
}

// Function to measure distance using the ultrasonic sensor
// Returns distance in millimeters (mm)
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
