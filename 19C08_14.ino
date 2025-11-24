// 아두이노용 적외선 거리 센서 노이즈 필터 함수 (O(n^2) 버전)


// IR 센서가 연결된 아날로그 핀 번호
const int IR_SENSOR_PIN = A0; 


void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int ir_sensor_filtered(int n, float position, int verbose) {

    unsigned long start_time = 0;
    if (verbose == 2) {
        start_time = micros(); 
    }

    // 1. n개의 측정값을 저장할 배열 동적 할당
    int* vals = new int[n]; 

    // 2. n회 측정 및 배열에 저장
    for (int i = 0; i < n; i++) {
        vals[i] = analogRead(IR_SENSOR_PIN);
        delay(100); // 100ms 대기
    }

    // 3. n개 값 오름차순 정렬
    bubbleSort(vals, n); 

    // 4. position 위치의 인덱스 계산
    int ret_idx = (int)((n - 1) * position);

    // 5. verbose 옵션에 따른 출력 처리
    if (verbose == 1) {
        Serial.print("Sorted vals = [");
        for (int i = 0; i < n; i++) {
            Serial.print(vals[i]);
            if (i < n - 1) Serial.print(", ");
        }
        Serial.println("]");
        Serial.print("Return index = ");
        Serial.println(ret_idx);
        
    } else if (verbose == 2) {
        unsigned long end_time = micros();
        float elapsed_time = (end_time - start_time) / 1000.0;
        Serial.print("Elapsed time: ");
        Serial.print(elapsed_time);
        Serial.println(" ms");
    }

    // 6. 최종 값 저장 및 메모리 해제
    int result = vals[ret_idx];
    delete[] vals; 

    // 7. 최종 값 반환
    return result;
}

void setup() {
    Serial.begin(9600); 
    while (!Serial) {
        ; 
    }
    Serial.println("Setup complete. Sensor test starts.");
}

void loop() {
    
    Serial.println("--- [Test] verbose=1 ---");
    int val = ir_sensor_filtered(5, 0.5, 1); // n=5, position=0.5, verbose=1
    Serial.print("==> Filtered Value: ");
    Serial.println(val);
    
    delay(1000); 
}
