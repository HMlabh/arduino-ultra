/*
 Name:		PingIt.ino
 Created:	08.09.2016 14:40:44
 Author:	gutek
*/


#include <Adafruit_ADS1015.h>
String version="V0.2.1";
//--------------------------------------------------
int sendelay = 5;
int measuredelay = 50;
int measurecount = 10;
int sensornr = 1;//|1=HC-SR04|2=SHARP ZLF|3=Sharp K0F|
//--------------------------------------------------

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_TLC59711.h>

//------------------Defines-------------------

//LCD
#define pin_lcd_sda 2
#define pin_lcd_scl 3
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

long dist_sen1 = 0;
long dist_sen2 = 0;
uint32_t mtimestart = 0;
uint32_t mtime = 0;
float mcount = 0;

//Ultraschall Sensor HC SR-04
#define pin_senhc1_trig A0
#define pin_senhc1_echo A1
#define pin_senhc2_trig A2
#define pin_senhc2_echo A3
long time_sen1 = 0;
long time_sen2 = 0;

//IR-Sensor Sharp GP2Y0A60SZLF 10-150cm, 5V
#define pin_senSZLF1_en A0
#define pin_senSZLF1_out A1
#define pin_senSZLF2_en A2
#define pin_senSZLF2_out A3
long measure_sen1 = 0;
long measure_sen2 = 0;

//IR-Sensor Sharp GP2Y0A21YK0F 10-80cm, 5V
#define pin_senSK0F1_out A1
#define pin_senSK0F2_out A3
void setup() 
{
	switch (sensornr)
	{
	case 1:
		hcinit();
		break;
	case 2:
		szlfinit();
		break;
	case 3:
		sk0finit();
		break;
	}

	//LCD
	lcd.begin(20, 4);             // initialize the lcd 
	lcd.backlight();			//Licht an
	lcd.home();                   // go home
	lcd.print("Ping It");
	lcd.setCursor(12, 0);        
	lcd.print(version);
	lcd.setCursor(0, 1);        // go to the next line
	lcd.print("Autor:");
	lcd.setCursor(0, 2);        // go to the next line
	lcd.print("Markus Gutekunst");
	lcd.setCursor(0, 3);        // go to the next line
	lcd.print("Lab. Prof. Haller");
	delay(2000);
	lcd.clear();
}

//Initialisierung HC
void hcinit()
{
	//Ultraschall Sensor HC SR-04
	pinMode(pin_senhc1_trig, OUTPUT);
	pinMode(pin_senhc1_echo, INPUT);
	pinMode(pin_senhc2_trig, OUTPUT);
	pinMode(pin_senhc2_echo, INPUT);
	digitalWrite(pin_senhc1_trig, LOW);
	digitalWrite(pin_senhc2_trig, LOW);
}
//Messung HC
void hcmeasure()
{
	//Messung Sensor 1
	noInterrupts();
	digitalWrite(pin_senhc1_trig, HIGH); //Trigger Impuls 10 us
	delayMicroseconds(10);
	digitalWrite(pin_senhc1_trig, LOW);
	time_sen1 = pulseIn(pin_senhc1_echo, HIGH); // Echo-Zeit messen
	interrupts();
	time_sen1 = (time_sen1 / 2); // Zeit halbieren
	dist_sen1 = time_sen1 / 29.1; // Zeit in Zentimeter umrechnen
	delay(sendelay);
	//Messung Sensor 2
	noInterrupts();
	digitalWrite(pin_senhc2_trig, HIGH); //Trigger Impuls 10 us
	delayMicroseconds(10);
	digitalWrite(pin_senhc2_trig, LOW);
	time_sen2 = pulseIn(pin_senhc2_echo, HIGH); // Echo-Zeit messen
	interrupts();
	time_sen2 = (time_sen2 / 2); // Zeit halbieren
	dist_sen2 = time_sen2 / 29.1; // Zeit in Zentimeter umrechnen
}

//Initialisierung Sharp ZLF
void szlfinit()
{
	pinMode(pin_senSZLF1_en, OUTPUT);
	pinMode(pin_senSZLF1_out, INPUT);
	pinMode(pin_senSZLF2_en, OUTPUT);
	pinMode(pin_senSZLF2_out, INPUT);
	digitalWrite(pin_senSZLF1_en, HIGH);	//Sensor an
	digitalWrite(pin_senSZLF2_en, HIGH);	//Sensor an
}

//Messung Sharp ZLF
void szlfmeasure()
{
	//Messung Sensor 1
	measure_sen1 = 0;
	for (int i = 0; i < measurecount+1; i++)
	{
		delay(measuredelay);
		measure_sen1 = measure_sen1 + analogRead(pin_senSZLF1_out);
	}
	measure_sen1 = measure_sen1 / measurecount;
	dist_sen1 = (187754 * pow(measure_sen1, -1.51));

	delay(sendelay);

	//Messung Sensor 2
	measure_sen2 = 0;
	for (int i = 0; i < measurecount + 1; i++)
	{
		delay(measuredelay);
		measure_sen2 = measure_sen2 + analogRead(pin_senSZLF2_out);
	}
	measure_sen2 = measure_sen2 / measurecount;
	dist_sen2 = (187754 * pow(measure_sen2, -1.51));
}

//Initialisierung Sharp K0F
void sk0finit()
{
	pinMode(pin_senSK0F1_out, INPUT);
	pinMode(pin_senSK0F2_out, INPUT);
}

//Messung Sharp K0F
void sk0fmeasure()
{
	//Messung Sensor 1
	measure_sen1 = 0;
	for (int i = 0; i < measurecount+1; i++)
	{
		delay(measuredelay);
		measure_sen1 = measure_sen1 + analogRead(pin_senSK0F1_out);
	}
	measure_sen1 = measure_sen1 / measurecount;
	dist_sen1 = (4*12343.85 * pow(measure_sen1, -1.15));
	delay(sendelay);

	//Messung Sensor 2
	measure_sen2 = 0;
	for (int i = 0; i < measurecount + 1; i++)
	{
		delay(measuredelay);
		measure_sen2 = measure_sen2 + analogRead(pin_senSK0F2_out);
	}
	measure_sen2 = measure_sen2 / measurecount;
	dist_sen2 = (4*12343.85 * pow(measure_sen2, -1.15));
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	mtimestart = millis();
	switch (sensornr)
	{
	case 1:
		hcmeasure();
		break;
	case 2:
		szlfmeasure();
		break;
	case 3:
		sk0fmeasure();
		break;
	}
	
	mtime = (millis() - mtimestart);
	mcount = 1 / (mtime*0.001);

	//Ergebnis anzeigen
	lcd.home();
	lcd.print(String("Sen1:  ")+(dist_sen1) + ("cm     "));
	lcd.setCursor(0, 1);
	lcd.print(String("Sen2:  ")+(dist_sen2) + ("cm     "));
	lcd.setCursor(0, 2);
	lcd.print(String("Messzeit:  ")+ (mtime)+("ms     "));
	lcd.setCursor(0, 3);
	lcd.print(String("Messungen/s:  ")+(mcount)+(" "));
	delay(500);
	
}
