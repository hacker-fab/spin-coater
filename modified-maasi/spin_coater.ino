#include <LiquidCrystal.h>
#include <Servo.h>


#define PIN_RS 10
#define PIN_RW 11
#define PIN_E 12
#define PIN_D4 2
#define PIN_D5 3
#define PIN_D6 4
#define PIN_D7 5

#define PIN_RPM_UP    15
#define PIN_RPM_DOWN  14
#define PIN_TIME_UP   21
#define PIN_TIME_DOWN 20

#define PIN_START 17

#define PIN_MOTOR 9


void setup() {
  Serial.begin(115200);
  pinMode(PIN_RPM_UP, INPUT_PULLUP);
  pinMode(PIN_RPM_DOWN, INPUT_PULLUP);
  pinMode(PIN_TIME_UP, INPUT_PULLUP);
  pinMode(PIN_TIME_DOWN, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);
}

void loop() {
  
  auto lcd = LiquidCrystal(PIN_RS, PIN_RW, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

  Servo servo;

  servo.attach(PIN_MOTOR);
  servo.writeMicroseconds(1000);

  lcd.begin(16, 2);
  lcd.clear();

  bool prev_spinning = false;
  long long prev_rpm = 3000;
  long long prev_duration = 30;
  long long prev_progress = 0;

  long long rpm = 3000;
  long long duration = 30;
  long long progress = 0;
  long long period = 1000;

  lcd.setCursor(0, 0); lcd.print("RPM: "); lcd.print(rpm);
  lcd.setCursor(0, 1); lcd.print("Duration: "); lcd.print(duration);

  bool prev_button_states[5] = { false, false, false, false };
  bool button_states[5] = { false, false, false, false };

  long long start_time = 0;

  bool spinning = false;

  while (true) {
    button_states[0] = digitalRead(PIN_RPM_UP);
    button_states[1] = digitalRead(PIN_RPM_DOWN);
    button_states[2] = digitalRead(PIN_TIME_UP);
    button_states[3] = digitalRead(PIN_TIME_DOWN);
    button_states[4] = digitalRead(PIN_START);

    bool changed = false;

    bool pushed[5] = { false, false, false, false };
    for (int i = 0; i < 5; ++i) {
      pushed[i] = !button_states[i] && prev_button_states[i];
      if (pushed[i]) {
        changed = true;
        delay(100);
      }
    }

    if (spinning) {
      progress = (millis() - start_time) / 1000;
      if ((millis() - start_time) > duration * 1000) {
        spinning = false;
      }
      if (pushed[4]) {
        spinning = false;
        delay(100);
      }
    } 
    else {
      if (pushed[0]) {
        rpm += 100;
        delay(100);
      } 
      else if (pushed[1]) {
        rpm -= 100;
        delay(100);
      }

      if (pushed[2]) {
        duration += 1;
        delay(100);
      }
      else if (pushed[3]) {
        duration -= 1;
        delay(100);
      }

      if (pushed[4]) {
        start_time = millis();
        spinning = true;
        delay(100);
      }
    }

    memcpy(prev_button_states, button_states, sizeof(button_states));

    if (prev_spinning != spinning || prev_progress != progress || prev_rpm != rpm || prev_duration != duration) {
      prev_spinning = spinning;
      prev_rpm = rpm;
      prev_duration = duration;
      prev_progress = progress;

      if (spinning) {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Spinning...");
        lcd.setCursor(0, 1); lcd.print(progress); lcd.print(" / "); lcd.print(duration); lcd.print(" s");
        period = map(rpm, 0, 12000, 1000, 2000);
        servo.writeMicroseconds(period);
      } 
      else {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("RPM: "); lcd.print(rpm);
        lcd.setCursor(0, 1); lcd.print("Duration: "); lcd.print(duration);
        servo.writeMicroseconds(1000);
      }
    }
  }
}