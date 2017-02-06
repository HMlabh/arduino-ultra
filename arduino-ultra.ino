/*
 Name:		arduino-ultra.ino
 Created:	06.02.2017 14:40:44
 Author:	gutek
*/


//------------------Defines-------------------
#define serialDebug //Debug output over Serial connection to PC




//------------------Pinset--------------------

namespace pin
{
	int sen0_t = 1;
	int sen0_e = 0;
	int sen1_t = 2;
	int sen1_e = 3;
	int sen2_t = 4;
	int sen2_e = 5;
	int sen3_t = 6;
	int sen3_e = 7;
	int sen4_t = 8;
	int sen4_e = 9;
	int sen5_t = 10;
	int sen5_e = 11;
	int sen6_t = 12;
	int sen6_e = 13;
	int sen7_t = 14;
	int sen7_e = 15;
	int sen8_t = 16;
	int sen8_e = 17;
	int sen9_t = 18;
	int sen9_e = 19;
}


//-------------------Init-----------------------

int sensor[10][2] =				// Array sensor [Sensornummer][Trigger,Echo]
{
{ pin::sen0_t , pin::sen0_e }, //Zeile 0 : Sensor 0 [T,E]
{ pin::sen1_t , pin::sen1_e }, //Zeile 1 : Sensor 1 [T,E]
{ pin::sen2_t , pin::sen2_e }, //Zeile 2 : Sensor 2 [T,E]
{ pin::sen3_t , pin::sen3_e }, //Zeile 3 : Sensor 3 [T,E]
{ pin::sen4_t , pin::sen4_e }, //Zeile 4 : Sensor 4 [T,E]
{ pin::sen5_t , pin::sen5_e }, //Zeile 5 : Sensor 5 [T,E]
{ pin::sen6_t , pin::sen6_e }, //Zeile 6 : Sensor 6 [T,E]
{ pin::sen7_t , pin::sen7_e }, //Zeile 7 : Sensor 7 [T,E]
{ pin::sen8_t , pin::sen8_e }, //Zeile 8 : Sensor 8 [T,E]
{ pin::sen9_t , pin::sen9_e }  //Zeile 9 : Sensor 9 [T,E]
};


//-------------------Setup-----------------------

void setup() 
{
	
}

//-------------------Functions------------------

//-------------------Loop--------------------

void loop() 
{
	

}
