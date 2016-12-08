/*

   @file Arduino_32x16_LED_Matrix_Clock.ino
   @brief Ardino 32 * 16 Led Matrix Clock
      DS3231:  SDA pin   -> Arduino Analog 4
               SCL pin   -> Arduino Analog 5

             Download the library here:
             http://www.rinkydinkelectronics.com/library.php?id=73

      32 * 16 LED MATRIX AD-501-C:
               1:VCC    pin -> Arduino Degital 6
               2:SIN1   pin -> Arduino Degital 7
               3:SIN2   pin -> Arduino Degital 8
               4:SIN3   pin -> Arduino Degital 9
               5:CLOCK  pin -> Arduino Degital 10
               6:LATCH  pin -> Arduino Degital 11
               7:ENABLE pin -> Arduino Degital 12
               8:VLED   pin  -> 5V
               9:GND    pin  -> GND
              10:GND    pin  -> GND

   @author Kei Takagi
   @date 2016.12.4

   Copyright (c) 2016 Kei Takagi
   Released under the MIT license
   http://opensource.org/licenses/mit-license.php

*/

#include <DS3231.h>
#include <MsTimer2.h>

#define VCC    6
#define SIN1   7
#define SIN2   8
#define SIN3   9
#define CLOCK  10
#define LATCH  11
#define ENABLE 12

DS3231  rtc(SDA, SCL);
Time  t;
word DisplayBuffer[32];
byte Displayi = 0;
byte anime[4][32];
byte anime_wk[4];
int i, j;

short HourH, HourL, MinH, MinL, Sec;
boolean  HourHF, HourLF, MinHF;
unsigned long Millisecond = 0;
int Anime_cnt;
int Anime_no;
int in_Year;
int in_Month;
int in_Day;
int in_Hour;
int in_Minute;
int in_Second;
int buffer_length;
char buff[30];

byte font[10][16] = {{
    0b01111100,
    0b11111110,
    0b11000110,
    0b11000110,
    0b11001110,
    0b11001110,
    0b11011110,
    0b11010110,
    0b11110110,
    0b11100110,
    0b11100110,
    0b11000110,
    0b11000110,
    0b11111110,
    0b01111100,
    0b00000000
  }, {
    0b00011000,
    0b00111000,
    0b00111000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00111100,
    0b00111100,
    0b00000000
  }, {
    0b01111000,
    0b11111100,
    0b11001110,
    0b11000110,
    0b00000110,
    0b00001110,
    0b00011100,
    0b00011000,
    0b00110000,
    0b01110000,
    0b01100000,
    0b11100000,
    0b11000000,
    0b11111110,
    0b11111110,
    0b00000000
  }, {
    0b11111100,
    0b11111110,
    0b00000110,
    0b00000110,
    0b00000110,
    0b00001110,
    0b11111100,
    0b11111100,
    0b00001110,
    0b00000110,
    0b00000110,
    0b00000110,
    0b00000110,
    0b11111110,
    0b11111100,
    0b00000000
  }, {
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11111110,
    0b01111110,
    0b00001100,
    0b00001100,
    0b00001100,
    0b00001100,
    0b00001100,
    0b00000000
  }, {
    0b11111110,
    0b11111110,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11111100,
    0b11111110,
    0b00001110,
    0b00000110,
    0b00000110,
    0b00001110,
    0b11111110,
    0b11111100,
    0b00000000
  }, {
    0b01111100,
    0b11111100,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11111100,
    0b11111110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11111110,
    0b01111100,
    0b00000000
  }, {
    0b11111110,
    0b11111110,
    0b11000110,
    0b11000110,
    0b00000110,
    0b00001100,
    0b00001100,
    0b00001100,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000
  }, {
    0b01111100,
    0b11111110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11111110,
    0b01111100,
    0b11111110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11111110,
    0b01111100,
    0b00000000
  }, {
    0b01111100,
    0b11111110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11111110,
    0b01111110,
    0b00000110,
    0b00000110,
    0b00000110,
    0b00000110,
    0b00000110,
    0b01111110,
    0b01111100,
    0b00000000
  }
};

void setup()
{
  rtc.begin();
  Serial.begin(9600);

  Led32x16Init();
  MsTimer2::set(1, Led32x16Display);     // 1ms毎にLed32x16Display( )割込み関数を呼び出す
  MsTimer2::start();                     // タイマー割り込み開始
  digitalWrite(ENABLE, LOW);
  Millisecond = 0;

  Serial.println("Time adjustment");
  Serial.println("Year,Nonth,Day,Hour,Minute,Second[Enter]");
  Serial.println("Ex)");
  Serial.println("  2016,4,9,0,20,0[Enter]");
}

void loop()
{
  if (Serial.available()) {
    buff[buffer_length] = Serial.read();
    if (buffer_length > 30 || buff[buffer_length] == '\n') {
      buff[buffer_length] = '\0';
      in_Year = atoi(strtok(buff, ","));
      in_Month = atoi(strtok(NULL, ","));
      in_Day = atoi(strtok(NULL, ","));
      in_Hour = atoi(strtok(NULL, ","));
      in_Minute = atoi(strtok(NULL, ","));
      in_Second = atoi(strtok(NULL, ","));

      Serial.print("Setting complete :");
      Serial.print(in_Year);
      Serial.print("/");
      Serial.print(in_Month);
      Serial.print("/");
      Serial.print(in_Day);
      Serial.print(" ");
      Serial.print(in_Hour);
      Serial.print(":");
      Serial.print(in_Minute);
      Serial.print(":");
      Serial.println(in_Second);

      rtc.setDate(in_Day, in_Month, in_Year);
      rtc.setTime(in_Hour, in_Minute, in_Second);
      buffer_length = 0;
    } else {
      buffer_length++;
    }
  }

  if (  Millisecond % 100 != 0) return;

  t = rtc.getTime();
  HourH = t.hour / 10;
  HourL = t.hour % 10;
  MinH  = t.min / 10;
  MinL  = t.min % 10;
  Sec   = t.sec;
  if ( Sec == 0)Anime_cnt = 0;

  switch ( Sec % 2 ) {
    case 0:
      *(DisplayBuffer + 15) = 0x1010;
      *(DisplayBuffer + 16) = 0x0808;
      break;
    case 1:
      *(DisplayBuffer + 15) = 0x0808;
      *(DisplayBuffer + 16) = 0x1010;
      break;
  }

  if ( Sec >= 57 & Anime_cnt == 0) {
    Millisecond = 0;
    HourHF = false;
    HourLF = false;
    MinHF = false;
    Anime_cnt = 1;

    for (i = 0; i < 16; i++) {
      anime[0][i] = font[HourH][i];
      anime[1][i] = font[HourL][i];
      anime[2][i] = font[MinH][i];
      anime[3][i] = font[MinL][i];
      anime[0][i + 15] = 0x00;
      anime[1][i + 15] = 0x00;
      anime[2][i + 15] = 0x00;
      anime[3][i + 15] = 0x00;
    }

    for (i = 0; i < 15; i++)anime[3][i + 16] = font[(MinL + 1) % 10][i];

    if (MinL == 9) {
      MinHF = true;
      for (i = 0; i < 15; i++)anime[2][i + 16] = font[(MinH + 1) % 6][i];
    }

    if (MinH == 5 && MinL == 9) {
      HourLF = true;
      for (i = 0; i < 15; i++) anime[1][i + 16] = font[(HourL + 1) % 10][i];
    }

    if (HourL == 9 && MinH == 5 && MinL == 9) {
      HourHF = true;
      for (i = 0; i < 15; i++) anime[0][i + 16] = font[(HourH + 1) % 3][i];
    }

    if (HourH == 2 && HourL == 3 && MinH == 5 && MinL == 9) {
      HourHF = true;
      for (i = 0; i < 15; i++) {
        anime[3][i + 16] = font[0][i];
        anime[2][i + 16] = font[0][i];
        anime[1][i + 16] = font[0][i];
        anime[0][i + 16] = font[0][i];
      }
    }
  }
  if (Anime_cnt > 0) {
    for ( i = 0 ; i < 15 ; i++ ) {
      Anime_no = ( Millisecond / (3000 / 16)) + i;
      anime_wk[0] = HourHF ? anime[0][Anime_no] : font[HourH][i];
      anime_wk[1] = HourLF ? anime[1][Anime_no] : font[HourL][i];
      anime_wk[2] = MinHF ? anime[2][Anime_no] : font[MinH][i];
      anime_wk[3] = anime[3][Anime_no];

      *(DisplayBuffer + i)      = anime_wk[0] << 8 | anime_wk[1] >> 1;
      *(DisplayBuffer + i + 17) = anime_wk[2] << 8 | anime_wk[3] >> 1 ;
    }
    Anime_cnt += 1;
  } else {
    for ( i = 0 ; i < 15 ; i++ ) {
      *(DisplayBuffer + i)      = font[HourH][i] << 8 | font[HourL][i] >> 1;
      *(DisplayBuffer + i + 17) = font[MinH][i]  << 8 | font[MinL][i] >> 1 ;
    }
  }
}

void Led32x16Init() {
  pinMode(VCC,    OUTPUT);
  pinMode(SIN1,   OUTPUT);
  pinMode(SIN2,   OUTPUT);
  pinMode(SIN3,   OUTPUT);
  pinMode(CLOCK,  OUTPUT);
  pinMode(LATCH,  OUTPUT);
  pinMode(ENABLE, OUTPUT);
  digitalWrite(VCC, HIGH);
}

void Led32x16Display() {
  byte j;
  Millisecond++;
  Displayi < 16 ? Displayi++ : Displayi = 0;
  for (j = 0; j < 16; j++) {
    digitalWrite(SIN1, ( 0x8000 >> Displayi >> j ) & 0x0001);
    digitalWrite(SIN2, (*(DisplayBuffer + 15 - j) >> 15 - Displayi) & 0x0001 );
    digitalWrite(SIN3, (*(DisplayBuffer + 31 - j) >> 15 - Displayi) & 0x0001 );
    digitalWrite(CLOCK, LOW);
    digitalWrite(CLOCK, HIGH);
  }
  digitalWrite(LATCH, HIGH);
  digitalWrite(LATCH, LOW);
}
