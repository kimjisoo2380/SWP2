// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24°C (m/s)
#define INTERVAL 25       // sampling interval (ms)
#define PULSE_DURATION 10 // TRIG pulse width (µs)
#define _DIST_MIN 100     // minimum distance (mm)
#define _DIST_MAX 300     // maximum distance (mm)
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL) // convert µs → mm

#define _EMA_ALPHA 0.5    // EMA weight

// 중위수 필터 관련 설정
#define N 30               // 최근 N개의 샘플 (3, 10, 30)
float samples[N];          // 샘플 저장 버퍼
int sample_index = 0;      // 현재 인덱스
bool buffer_filled = false;

// global variables
unsigned long last_sampling_time;
float dist_ema = _DIST_MAX;     // EMA 계산용 변수
float dist_median = _DIST_MAX;  // 중위수 필터 결과

// ----- 함수 선언 -----
float USS_measure(int TRIG, int ECHO);
float getMedian(float arr[], int size);

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, HIGH); // LED off initially

  Serial.begin(57600);
}

void loop() {
  float dist_raw;

  if (millis() < last_sampling_time + INTERVAL)
    return;

  // 초음파 센서 측정
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // 중위수 필터: 최근 N개의 샘플 관리 
  samples[sample_index] = dist_raw;
  sample_index = (sample_index + 1) % N;
  if (sample_index == 0) buffer_filled = true;

  if (buffer_filled)
    dist_median = getMedian(samples, N);
  else
    dist_median = dist_raw; // 초기엔 버퍼 다 안참

  // EMA 필터
  dist_ema = _EMA_ALPHA * dist_raw + (1 - _EMA_ALPHA) * dist_ema;

  // LED 제어 
  if (dist_median >= _DIST_MIN && dist_median <= _DIST_MAX)
    digitalWrite(PIN_LED, LOW);  // 범위 안 → ON
  else
    digitalWrite(PIN_LED, HIGH); // 범위 밖 → OFF

  // 시리얼 플로터 출력
  Serial.print("Min:");     Serial.print(_DIST_MIN);
  Serial.print(",raw:");    Serial.print(dist_raw);
  Serial.print(",ema:");    Serial.print(dist_ema);
  Serial.print(",median:"); Serial.print(dist_median);
  Serial.print(",Max:");    Serial.print(_DIST_MAX);
  Serial.println("");

  last_sampling_time = millis();
}

// 초음파 거리 측정 
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // mm 단위 반환
}

// 중위수 계산 함수
float getMedian(float arr[], int size) {
  float sorted[size];
  for (int i = 0; i < size; i++) sorted[i] = arr[i];

  // 간단한 버블 정렬
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (sorted[j] > sorted[j + 1]) {
        float temp = sorted[j];
        sorted[j] = sorted[j + 1];
        sorted[j + 1] = temp;
      }
    }
  }

  // 중위수 반환
  if (size % 2 == 0)
    return (sorted[size/2 - 1] + sorted[size/2]) / 2.0;
  else
    return sorted[size/2];
}

