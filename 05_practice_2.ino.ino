// 05_practice_2.ino

int ledPin = 7; // LED가 연결된 디지털 핀 번호 정의

void setup() {
  // 핀을 출력 모드로 설정합니다.
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 1. 처음 1초 동안 LED 켜기 (loop() 시작)
  digitalWrite(ledPin, HIGH); // LED 켜짐
  delay(1000); // 1000ms (1초) 대기

  // 2. 다음 1초 동안 LED 5회 깜빡이기
  // 1회 깜빡임 (꺼짐 100ms + 켜짐 100ms) = 200ms
  // 5회 반복 = 1000ms (1초)
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, LOW);  // LED 끄기
    delay(100);
    digitalWrite(ledPin, HIGH); // LED 켜기
    delay(100);
  }

  // 3. LED 끄고 무한 루프 상태에서 종료
  digitalWrite(ledPin, LOW);  // 최종적으로 LED 끄기

  // 무한 루프: loop() 함수가 다시 실행되지 않도록 여기서 멈춥니다.
  // 이 블록에 진입하면 LED는 꺼진 상태를 유지하게 됩니다.
  while (1) {
    // digitalWrite(ledPin, LOW); // 반복 명령이 필요 없지만, 명확성을 위해 유지 가능
  }
}
