#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buttons
const int setBtn  = 8;
const int nextBtn = 9;
const int incBtn  = 10;

// Outputs
const int buzzer = 11;
const int led1 = 12;
const int led2 = 13;
const int led3 = A0;

// Alarms
int alarmH[3] = {8, 13, 20};
int alarmM[3] = {0, 0, 0};

int lastMinute = -1;

void setup() {
  pinMode(setBtn, INPUT_PULLUP);
  pinMode(nextBtn, INPUT_PULLUP);
  pinMode(incBtn, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  Wire.begin();
  rtc.begin();

  lcd.init();
  lcd.backlight();

  // ⭐ IMPORTANT FIX (NO BATTERY CASE)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  loadAlarm();

  lcd.setCursor(0,0);
  lcd.print("Medicine Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  // TIME DISPLAY
  lcd.setCursor(0,0);
  lcd.print("T:");
  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());

  // DATE DISPLAY
  lcd.setCursor(0,1);
  lcd.print("D:");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());

  checkAlarm(now.hour(), now.minute());

  if (digitalRead(setBtn) == LOW) {
    delay(300);
    setAlarm();
  }

  delay(500);
}

void setAlarm() {
  for (int i = 0; i < 3; i++) {

    // SET HOUR
    while (true) {
      lcd.clear();
      lcd.print("Alarm ");
      lcd.print(i+1);
      lcd.print(" Hour");

      lcd.setCursor(0,1);
      lcd.print(alarmH[i]);

      if (digitalRead(incBtn) == LOW) {
        delay(200);
        while (digitalRead(incBtn) == LOW);
        alarmH[i]++;
        if (alarmH[i] > 23) alarmH[i] = 0;
      }

      if (digitalRead(nextBtn) == LOW) {
        delay(200);
        while (digitalRead(nextBtn) == LOW);
        break;
      }
    }

    // SET MINUTE
    while (true) {
      lcd.clear();
      lcd.print("Alarm ");
      lcd.print(i+1);
      lcd.print(" Min");

      lcd.setCursor(0,1);
      lcd.print(alarmM[i]);

      if (digitalRead(incBtn) == LOW) {
        delay(200);
        while (digitalRead(incBtn) == LOW);
        alarmM[i]++;
        if (alarmM[i] > 59) alarmM[i] = 0;
      }

      if (digitalRead(nextBtn) == LOW) {
        delay(200);
        while (digitalRead(nextBtn) == LOW);
        break;
      }
    }
  }

  saveAlarm();

  lcd.clear();
  lcd.print("Saved!");
  delay(1500);
  lcd.clear();
}

void checkAlarm(int h, int m) {

  if (m == lastMinute) return;

  for (int i = 0; i < 3; i++) {
    if (h == alarmH[i] && m == alarmM[i]) {
      lastMinute = m;
      alert(i + 1);
    }
  }
}

void alert(int group) {

  lcd.clear();
  lcd.print("Take Medicine");
  lcd.setCursor(0,1);
  lcd.print("Group ");
  lcd.print(group);

  tone(buzzer, 1000);

  if (group == 1) digitalWrite(led1, HIGH);
  if (group == 2) digitalWrite(led2, HIGH);
  if (group == 3) digitalWrite(led3, HIGH);

  delay(5000);

  noTone(buzzer);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

  lcd.clear();
}

void saveAlarm() {
  for (int i = 0; i < 3; i++) {
    EEPROM.write(i*2, alarmH[i]);
    EEPROM.write(i*2+1, alarmM[i]);
  }
}

void loadAlarm() {
  for (int i = 0; i < 3; i++) {
    alarmH[i] = EEPROM.read(i*2);
    alarmM[i] = EEPROM.read(i*2+1);

    if (alarmH[i] > 23) alarmH[i] = 8 + i;
    if (alarmM[i] > 59) alarmM[i] = 0;
  }
}