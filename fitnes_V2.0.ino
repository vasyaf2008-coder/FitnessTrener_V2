#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA A4// дисплей
#define SCL A5// дисплей
#define TRIG 9 // датчик расстояния
#define ECHO 10 // датчик расст.
#define LED 12 //индикатор препятствия
#define CLK 2 // потенциометр
#define DT 3 // потенциометр
#define batton1 4 // кнопка на потенциометре
#define buzzer 11 // пищалка

LiquidCrystal_I2C lcd(0x27, 16, 2);

boolean led_flag = 0;
unsigned int upquantity = 0;
int seriilimit = 60;
int serialquantity = 0;

int lastStateCLK; // для потенциометра

unsigned long previousMillis = 0;
unsigned long upMillis = 0;
const unsigned long interval = 50; // частота измерения датчика
const unsigned long upinterval = 400; // время пролёта руки у датчика


void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(batton1, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  lcd.init();           // Инициализация
  lcd.backlight();      // Включить подсветку
  lcd.setCursor(0, 0);  
  lcd.print("                 ");
  lcd.print(serialquantity);
  lcd.setCursor(0, 1);
  lcd.print("                 ");
  lcd.setCursor(0, 1);
  lcd.print(upquantity/2);
  lcd.print("/");
  lcd.print(seriilimit);
  
  lastStateCLK = digitalRead(CLK);

}


void loop() {
  unsigned long currentMillis = millis();
  int currentStateCLK = digitalRead(CLK);

   if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH);
  float distance = duration * 0.034 / 2; // скорость звука ~0.034 см/мкс
  // Serial.print("Расстояние: ");
  // Serial.print(distance);
  // Serial.println(" см");
  if (distance <= 25) { // условие для датчика, отсчёт 1x
    digitalWrite(LED, HIGH);
    led_flag = 1;
    if (currentMillis - upMillis >= upinterval) {
      upMillis = currentMillis;
      upquantity++;
      Serial.println(upquantity);
      lcd.setCursor(0, 1);
      lcd.print("                 ");
      lcd.setCursor(0, 1);
      lcd.print(upquantity/2);
      lcd.print("/");
      lcd.print(seriilimit);
    }
  } else {
    digitalWrite(LED, LOW);
    led_flag = 0;
  }
  if (upquantity/2 > seriilimit-1) { // пересчёт кол-ва сделаных серий
    serialquantity++;
    upquantity = 0;
    tone(buzzer, 1000);  // 1000 Гц
    delay(500);
    noTone(buzzer);
    delay(500);
    lcd.setCursor(0, 0);
    lcd.print("                 ");
    lcd.setCursor(0, 0);
    lcd.print(serialquantity);
  }

  // delay(50);
   }
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(DT) != currentStateCLK) {
      seriilimit++;
    } else {
      if (seriilimit > 1){
      seriilimit--;
      }
    }
    lcd.setCursor(0, 1);
    lcd.print("                 ");
    lcd.setCursor(0, 1);
    lcd.print(upquantity/2);
    lcd.print("/");
    lcd.print(seriilimit);
  }
   
  lastStateCLK = currentStateCLK;

  if (digitalRead(batton1) == LOW) {
    upquantity = 0;
     lcd.setCursor(0, 1);
      lcd.print("                 ");
      lcd.setCursor(0, 1);
      lcd.print(upquantity/2);
      lcd.print("/");
      lcd.print(seriilimit);
  }
}

