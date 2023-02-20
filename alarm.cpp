#include "DS1302.h" // Библиотека для работы с DS1302
#include <stdio.h> // Для форматированного вывода
#include <LiquidCrystal.h>
#include <SPI.h>//Библиотека для работы с интерфейсом SPI
#include <MFRC522.h>//Библ. для работы RFID-считывателем
// Номера выводов к которым подключен RFID-считыватель.
// Выбирать куда подключить можно только RST и SS(SDA),
// остальные должны быть подключены следующим образом:
// MOSI - 11, MISO - 12, SCK - 13
#define RST_PIN 9 // Номер вывода RST
#define SS_PIN 10 // Номер вывода SS(SDA)
// Создаём объект MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int rstPin = 16; // Номер вывода RST (он же CE)
const int datPin = 17; // Номер вывода DAT (он же I/O)
const int clkPin = 18; // Номер вывода CLK (он же SCLK)

DS1302 rtc(rstPin, datPin, clkPin);

unsigned char j=2; //сирена
unsigned char z=200;
unsigned char f=5; //коэф для ускорения сирены
const int SIR1_pin =14;
const int SIR2_pin =15;
int sensorPin = 5; 
const int PIR_pin =8; 

void setup() {
analogReference(INTERNAL); // Используем ИОН 1.1В
  SPI.begin(); // Инициализируем шину SPI
mfrc522.PCD_Init(); // Инициализируем MFRC522
  lcd.begin(16, 2);
  //Serial.begin(9600);
  pinMode(PIR_pin, INPUT); // назначаем PIR_pin входом
  digitalWrite(PIR_pin, LOW);
  pinMode(SIR1_pin,OUTPUT);
  pinMode(SIR2_pin,OUTPUT);
  digitalWrite(SIR1_pin, LOW);
  digitalWrite(SIR2_pin, LOW); 

 
 pinMode(1,OUTPUT);
 digitalWrite(1, HIGH); 
}
bool signaling= true;
bool alert= false;
void loop() {
  
  
  if(signaling==false && alert== false)
  {
     lcd.setCursor(0, 1);
     lcd.print(temperature()); lcd.print(" C ");
    
   
     lcd.print(" SigOFF       ");
    //TODO time 
      lcd.setCursor(0, 0);
     lcd.print(timeOut());
      digitalWrite(1, HIGH); 
  }
  
  
  if(signaling==true && alert== false)
  {
    lcd.setCursor(0, 1);
    lcd.print(temperature()); lcd.print(" C ");
    
   
     lcd.print(" SigON       ");
       lcd.setCursor(0, 0);
       lcd.print("              ");
       digitalWrite(1, LOW); 
  }   
  
   if (digitalRead(PIR_pin) == HIGH && signaling==true)
     {
      signaling= false;
      alert = true;
       digitalWrite(1, HIGH); 
     }
  
   if(alert==true)
   {
      lcd.setCursor(0, 0);
        lcd.print("Alert             ");
      lcd.setCursor(0, 1);
      lcd.print((int)temperature()); lcd.print(" C ");
      lcd.print(" Alert   ");
        siren();  
        
    // return;
   }
   if(mfrc522.PICC_IsNewCardPresent() && 
                mfrc522.PICC_ReadCardSerial() )///колдонуть с условием 
        {
          String UID = "";
          byte uidSize = mfrc522.uid.size; // длина UID
           for (int i = 0; i < uidSize; i++)
           {
             byte B = mfrc522.uid.uidByte[i];
             String Bstr = String(B, 16);
             UID += Bstr + " ";
           }
          if(UID=="c3 15 d1 19 ")///добавить uidroot
            {
              signaling=signaling==true?false:true;
              alert= false;
              delay(1000);
            }
                    
        }
  
    
}

  void siren()
    {
         for(int i=1000;i>300;i-=f){
            digitalWrite(SIR1_pin, HIGH);
            digitalWrite(SIR2_pin, LOW);
            delayMicroseconds(z);
            digitalWrite(SIR1_pin, LOW);
            digitalWrite(SIR2_pin, LOW);
            delayMicroseconds((i*j-z)/2);
            digitalWrite(SIR1_pin, LOW);
            digitalWrite(SIR2_pin, HIGH);
            delayMicroseconds(z);
            digitalWrite(SIR1_pin, LOW);
            digitalWrite(SIR2_pin, LOW);
            delayMicroseconds((i*j-z)/2);
        }
       for(int i=300;i<1000;i+=f){
           digitalWrite(SIR1_pin, HIGH);
           digitalWrite(SIR2_pin, LOW);
           delayMicroseconds(z);
           digitalWrite(SIR1_pin, LOW);
           digitalWrite(SIR2_pin, LOW);
           delayMicroseconds((i*j-z)/2);
           digitalWrite(SIR1_pin, LOW);
           digitalWrite(SIR2_pin, HIGH);
           delayMicroseconds(z);
           digitalWrite(SIR1_pin, LOW);
           digitalWrite(SIR2_pin, LOW);
           delayMicroseconds((i*j-z)/2);
       }
       
   }
 
    float temperature()
    {  
    int n = analogRead(sensorPin);
   float tempC = n / 9.3;//Рассчитываем температуру
   // float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
   // Serial.print(temperatureF); Serial.println(" degrees F");
   /// delay(1000); //ждем секунду
      return tempC;
    }
     String timeOut()
    {  
   Time t = rtc.time();
   int day = t.day; // Получаем номер дня недели
      // (0-ВС, 1-ПН, 2-ВТ, 3-СР, 4-ЧТ, 5-ПТ, 6-СБ)
      String dayString = ""; // Название дня недели
      // Расшифровываем день недели
      switch (day)
      {
      case Time::kSunday: dayString = "Sun";
      case Time::kMonday: dayString = "Mon";
      case Time::kTuesday: dayString = "Tue";
      case Time::kWednesday: dayString = "Wen";
      case Time::kThursday: dayString = "Thu";
      case Time::kFriday: dayString = "Fri";
      case Time::kSaturday: dayString = "Sat";
      }
      char buf[50];
      snprintf(buf, sizeof(buf),
      "%s %02d.%02d.%04d ", /*%02d:%02d:%02d*/
      dayString.c_str(),
      t.date, t.mon, t.yr,
      t.hr, t.min, t.sec);
      Serial.println(buf);
      return buf;
    }
 