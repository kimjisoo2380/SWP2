const int pwmPin = 7;

int period_us = 1000;
int duty_percent = 0;

void setup() {
  pinMode(pwmPin, OUTPUT);
  
  set_period(10000); // 이 부분을 10ms인 경우에는 10000, 1ms인 경우는 1000, 0.1ms인 경우는 100. 
}

void set_period(int period) {
  if (period < 100) period = 100;
  if (period > 10000) period = 10000;
  period_us = period;
}

void set_duty(int duty) {
  if (duty < 0) duty = 0;
  if (duty > 100) duty = 100;
  duty_percent = duty;
}

void pwm_output(int pin) {
  int on_time = (period_us * duty_percent) / 100;
  int off_time = period_us - on_time;
  
  if (on_time > 0) {
    digitalWrite(pin, LOW);
    delayMicroseconds(on_time);
  }
  
  if (off_time > 0) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(off_time);
  }
}

void loop() {
  const int step_delay_ms = 5; 
  
  for (int d = 0; d <= 100; d++) {
    set_duty(d);
    
    long start_time = millis();
    while (millis() - start_time < step_delay_ms) {
      pwm_output(pwmPin);
    }
  }

  for (int d = 100; d >= 0; d--) {
    set_duty(d);
    
    long start_time = millis();
    while (millis() - start_time < step_delay_ms) {
      pwm_output(pwmPin);
    }
  }
}
