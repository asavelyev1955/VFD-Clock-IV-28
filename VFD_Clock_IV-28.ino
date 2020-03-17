
//4 Digit 7 Segment VFD Arduino Based Clock w/o LED Seconds Circle
//by Frank Zheng, REV 1.1 - based on niq_ro
//Alle Pins ausser Shift-Register-Steuerpins sind Active Low!
// Для динамической индикации, схема - FZL92KGIBNA3HA7.LARGE
// Схема с транзисторами, Ардуино Нано и ИВ-28

#include <Wire.h>
#include "RTClib.h"
//#include "TimerOne.h" 

// Назначения контактов сетки
int digit1 = 6; //Arduino Pin D6~, PWM, Pin 12, T9
int digit2 = 9; //Arduino Pin D9~, PWM, Pin 15, T10
int digit3 = 10; //Arduino Pin D10~, PWM, Pin 16, T11
int digit4 = 11; //Arduino Pin D11~, PWM, Pin 17, T12


// Сегменты 
int segA = 2; //Arduino Pin D2, Pin 4, T1
int segB = 3; //Arduino Pin D3, Pin 5, T2
int segC = 4; //Arduino Pin D4, Pin 6, T3
int segD = 5; //Arduino Pin D5, Pin 11, T4
int segE = 7; //Arduino Pin D7, Pin 13, T5
int segF = 8; //Arduino Pin D8, Pin 14, T6
int segG = 12; //Arduino Pin D12, Pin 18, T7
int segDP = 13; //Arduino Pin D13, Pin 19, T8


// Кнопки регулировки времени
byte SW0 = A0;
byte SW1 = A1;
byte SW2 = A2;

RTC_DS1307 RTC;

//Глобальные переменные секунда
//int alle_x_sekunden = 1;
//int secondreset = 0;
//Определяем текущую секунду
//int sekaktuell = 0;

//Hex-Dec преобразование (niq_ro)
int zh, uh, ore;
int zm, um, miniti;

void setup() {
  
	//Инициализация RTC
	//Serial wird fuer Shift-Register verwendet
	//Serial.begin(9600);
	Wire.begin();
	RTC.begin();
	//RTC.adjust(DateTime(__DATE__, __TIME__));

	//LED-Blink / ШИМ-модуляции
	Wire.beginTransmission(0x68);
	Wire.write(0x07); //Указатель перенесен, ШИМ
	//Wire.write(0x00); //PWM Ausschalten
	Wire.write(0x10); //0x10 (h) 00010000 (b) an Kontrollregister: 1Hz Blink
	//Wire.write(0x13); //0x13 (h) 00010011 (b) 32kHz

	Wire.endTransmission();

	//RTC-Aktiv?
	if (! RTC.isrunning()) Serial.println("RTC is not running!");
		
		//Nein -> Schreibe Systemzeit in RTC
	//	RTC.adjust(DateTime(__DATE__, __TIME__));

        
  
	// dht.begin();
	
	//Назначение выводов I/O
	
	pinMode(segA, OUTPUT);
	pinMode(segB, OUTPUT);
	pinMode(segC, OUTPUT);
	pinMode(segD, OUTPUT);
	pinMode(segE, OUTPUT);
	pinMode(segF, OUTPUT);
	pinMode(segG, OUTPUT);
	pinMode(segDP, OUTPUT);

	pinMode(digit1, OUTPUT);
	pinMode(digit2, OUTPUT);
	pinMode(digit3, OUTPUT);
	pinMode(digit4, OUTPUT);
	
	pinMode(SW0, INPUT);  
	pinMode(SW1, INPUT);  
	pinMode(SW2, INPUT); 

	
	//Digital-Pull-Up
	
	digitalWrite(SW0, HIGH);
	digitalWrite(SW1, HIGH);
	digitalWrite(SW2, HIGH);
	
	// Serial.begin(9600);
	// Serial.println("HARDWARECOP VFD Clock - Welcome");

}

//Routine
void loop() {
  
	//Включить дисплей DP
	digitalWrite(segDP, LOW);
	
	//регулировка времени
	DateTime now = RTC.now();

        //Время = час * 100  + Minute (14h * 100 + 25 min) = 1425
        
	int timp = now.hour()*100+now.minute();
	//int timp = (now.minute(), DEC);
	//displayNumber(12);
	//int timp = 1234;


	// display parts   
	for(int i = 150 ; i >0  ; i--) {
		if (timp > 1000) displayNumber01(timp); 
                //e.g. 2142 -> timp = 2, 1, 4, 2 -> GOTO displayNumber01(2142)
		else displayNumber02(timp); 
                //e.g. 0943 -> timp = 0, 9, 4, 3 -> GOTO displayNumber02(0943)
	} 

	for(int i = 150 ; i >0  ; i--) {
		if (timp > 1000) displayNumber03(timp); 
		else displayNumber04(timp); 
	} 
	
	//Нажата кнопка регулировки?
	if (!(digitalRead(SW0))) set_time(); //Во время SW0 Low: установить время!
} 

//Установить время (niq_ro)
void set_time()   {
	byte minutes1 = 0;
	byte hours1 = 0;
	byte minutes = 0;
	byte hours = 0;

	while (!digitalRead(SW0))
	{
		minutes1=minutes;
		hours1=hours;
   
		while (!digitalRead(SW1)) // set minutes
		{ 
			minutes++;  
			// converting hexa in zecimal:
			zh = hours / 16;
			uh = hours - 16 * zh ;
			ore = 10 * zh + uh; 
			zm = minutes / 16;
			um = minutes - 16 * zm ;
			miniti = 10 * zm + um; 
    
			for(int i = 20 ; i >0  ; i--) {
				displayNumber01(ore*100+miniti); 
			}

      
			if ((minutes & 0x0f) > 9) minutes = minutes + 6;
			if (minutes > 0x59) minutes = 0;
			//Serial.print("Minutes = ");
			//if (minutes >= 9) Serial.print("0");
			//Serial.println(minutes, HEX);
			delay(150);    
		}

		while (!digitalRead(SW2)) // set hours
		{ 
			hours++;          
     
			// converting hexa in zecimal:
			zh = hours / 16;
			uh = hours - 16 * zh ;
			ore = 10 * zh + uh; 
			zm = minutes / 16;
			um = minutes - 16 * zm ;
			miniti = 10 * zm + um; 
    
			for(int i = 20 ; i >0  ; i--) {
				displayNumber01(ore*100+miniti); 
			}
   
      
			if ((hours & 0x0f) > 9) hours =  hours + 6;
			if (hours > 0x23) hours = 0;
			//Serial.print("Hours = ");
			//if (hours <= 9) Serial.print("0");
			//Serial.println(hours, HEX);
			delay(150);
		}

		Wire.beginTransmission(0x68); // activate DS1307
		Wire.write(0); // where to begin
		Wire.write(0x00);          //seconds
		Wire.write(minutes);          //minutes
		Wire.write(0x80 | hours);    //hours (24hr time)
		Wire.write(0x06);  // Day 01-07
		Wire.write(0x01);  // Date 0-31
		Wire.write(0x05);  // month 0-12
		Wire.write(0x09);  // Year 00-99
		Wire.write(0x10); // Control 0x10 produces a 1 HZ square wave on pin 7. 
		Wire.endTransmission();
  
		// converting hexa in zecimal:
		zh = hours / 16;
		uh = hours - 16 * zh ;
		ore = 10 * zh + uh; 
		zm = minutes / 16;
		um = minutes - 16 * zm ;
		miniti = 10 * zm + um; 
    
		for(int i = 20 ; i >0  ; i--) {
			displayNumber01(ore*100+miniti); 
		}
		//  delay(150);
	}
}

//Дисплей 1:
void displayNumber01(int toDisplay) {

	#define DISPLAY_BRIGHTNESS  1500
	#define DIGIT_ON  LOW
	#define DIGIT_OFF  HIGH

	for(int digit = 4 ; digit > 0 ; digit--) {
		//PWM: Включите ненадолго
		switch(digit) {
			case 1:
				digitalWrite(digit1, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 2:
				digitalWrite(digit2, DIGIT_ON);
				digitalWrite(segDP, LOW);
			break;
			case 3:
				digitalWrite(digit3, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 4:
				digitalWrite(digit4, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
		}
		lightNumber(toDisplay % 10);
		toDisplay /= 10;
		delayMicroseconds(DISPLAY_BRIGHTNESS); 

		//Выключить сегменты
		lightNumber(10); 

		//Отключить рекламу ??
		digitalWrite(digit1, DIGIT_OFF);
		digitalWrite(digit2, DIGIT_OFF);
		digitalWrite(digit3, DIGIT_OFF);
		digitalWrite(digit4, DIGIT_OFF);
	}
} 

//Дисплей 2:
void displayNumber02(int toDisplay) {

	#define DISPLAY_BRIGHTNESS  1500
	#define DIGIT_ON  LOW
	#define DIGIT_OFF  HIGH

	for(int digit = 4 ; digit > 0 ; digit--) {
		//PWM: Включите ненадолго
		switch(digit) {
			case 1:
				digitalWrite(digit1, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 2:
				digitalWrite(digit2, DIGIT_ON);
				digitalWrite(segDP, LOW);
			break;
			case 3:
				digitalWrite(digit3, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 4:
				digitalWrite(digit4, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
		}
		lightNumber(toDisplay % 10);
		toDisplay /= 10;
		delayMicroseconds(DISPLAY_BRIGHTNESS); 

		//Выключить сегменты
		lightNumber(10); 

		//Anzeigen ausschalten
		digitalWrite(digit1, DIGIT_OFF);
		digitalWrite(digit2, DIGIT_OFF);
		digitalWrite(digit3, DIGIT_OFF);
		digitalWrite(digit4, DIGIT_OFF);
	}	
} 

//Дисплей 3:
void displayNumber03(int toDisplay) {

	#define DISPLAY_BRIGHTNESS  1500
	#define DIGIT_ON  LOW
	#define DIGIT_OFF  HIGH

	for(int digit = 4 ; digit > 0 ; digit--) {
		//PWM: Включите ненадолго
		switch(digit) {
			case 1:
				digitalWrite(digit1, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 2:
				digitalWrite(digit2, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 3:
				digitalWrite(digit3, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 4:
				digitalWrite(digit4, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
		}
		lightNumber(toDisplay % 10);
		toDisplay /= 10;
		delayMicroseconds(DISPLAY_BRIGHTNESS); 

		//Выключить сегменты
		lightNumber(10); 

		//Anzeigen ausschalten
		digitalWrite(digit1, DIGIT_OFF);
		digitalWrite(digit2, DIGIT_OFF);
		digitalWrite(digit3, DIGIT_OFF);
		digitalWrite(digit4, DIGIT_OFF);
	}
} 

//Дисплей 4:
void displayNumber04(int toDisplay) {
	#define DISPLAY_BRIGHTNESS  1500
	#define DIGIT_ON  LOW
	#define DIGIT_OFF  HIGH

	for(int digit = 4 ; digit > 0 ; digit--) {
		//PWM: Включите ненадолго
		switch(digit) {
			case 1:
				digitalWrite(digit1, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 2:
				digitalWrite(digit2, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 3:
				digitalWrite(digit3, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
			case 4:
				digitalWrite(digit4, DIGIT_ON);
				digitalWrite(segDP, HIGH);
			break;
		}
		lightNumber(toDisplay % 10);
		toDisplay /= 10;
		delayMicroseconds(DISPLAY_BRIGHTNESS); 

		//Выключить сегменты
		lightNumber(10); 

		//Отключить рекламу
		digitalWrite(digit1, DIGIT_OFF);
		digitalWrite(digit2, DIGIT_OFF);
		digitalWrite(digit3, DIGIT_OFF);
		digitalWrite(digit4, DIGIT_OFF);
	}
} 

//подписать условия:
//Fuerr case 10 дисплей включен со всеми сегментами
void lightNumber(int numberToDisplay) {

	#define SEGMENT_ON  LOW
	#define SEGMENT_OFF HIGH

	switch (numberToDisplay){

		//Номер 0
		case 0:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_OFF);
		break;

		case 1:
			digitalWrite(segA, SEGMENT_OFF);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_OFF);
		break;

		case 2:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_OFF);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_ON);
		break;

		case 3:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_ON);
		break;

		case 4:
			digitalWrite(segA, SEGMENT_OFF);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
		break;

		case 5:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_OFF);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
		break;

		case 6:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_OFF);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
		break;

		case 7:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_OFF);
		break;

		case 8:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
		break;

		case 9:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
		break;

		//Всё выключено
		case 10:
			digitalWrite(segA, SEGMENT_OFF);
			digitalWrite(segB, SEGMENT_OFF);
			digitalWrite(segC, SEGMENT_OFF);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_OFF);
		break;
  
  }
}
