/*
 Name:		arduino-ultra.ino
 Created:	06.02.2017 14:40:44
 Author:	gutek
*/


//------------------Defines-------------------
//#define serialDebug //Debug output over Serial connection to PC
//#define serialDebug_continuous //reads the 16 Sensors continuous


//------------------Params--------------------
namespace param
{
	int8_t ident = 85; //Identifier: 85 "U"
	int8_t ident_ask = 105;  // define ident_ask Message: 105 "i"
	int8_t call = 99; //define call Message: 99 "c"
	int8_t ask = 97;  // define ask Message: 97 "a"
}

long conversionfactor = 29.1;
unsigned long measuredelay = 19500;
long measuretimeout = 19000;
long measurecompletetime = 0;
int8_t mram = 0;	//Message storage

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
	int sen7_t = 18;
	int sen7_e = 19;
	int sen8_t = 20;
	int sen8_e = 21;
	int sen9_t = 22;
	int sen9_e = 23;
}


//-------------------Init-----------------------

int sensor[10][2] =				// Array sensor [Sensornummer][Trigger,Echo]
{
{ pin::sen0_t , pin::sen0_e }, //Line 0 : Sensor 0 [T,E]
{ pin::sen1_t , pin::sen1_e }, //Line 1 : Sensor 1 [T,E]
{ pin::sen2_t , pin::sen2_e }, //Line 2 : Sensor 2 [T,E]
{ pin::sen3_t , pin::sen3_e }, //Line 3 : Sensor 3 [T,E]
{ pin::sen4_t , pin::sen4_e }, //Line 4 : Sensor 4 [T,E]
{ pin::sen5_t , pin::sen5_e }, //Line 5 : Sensor 5 [T,E]
{ pin::sen6_t , pin::sen6_e }, //Line 6 : Sensor 6 [T,E]
{ pin::sen7_t , pin::sen7_e }, //Line 7 : Sensor 7 [T,E]
{ pin::sen8_t , pin::sen8_e }, //Line 8 : Sensor 8 [T,E]
{ pin::sen9_t , pin::sen9_e }  //Line 9 : Sensor 9 [T,E]
};

long ranges[10] = { 0 };		//Rangevector ; Line = Sensornumber
int measurecount = 0;

//-------------------Setup-----------------------

void setup() 
{
	Serial.begin(115200);
	
	while (!Serial) 
	{
		; // wait for serial port to connect. Needed for native USB port only
	}




	//Pinmode Sensors
	for (int n = 0; n <= 9; n++)
	{
		pinMode(sensor[n][0], OUTPUT); //Sensor n Trigger -> Output
		pinMode(sensor[n][1], INPUT_PULLUP);  //Sensor n Echo    -> Output
	}
}

//-------------------Functions------------------

//get Ranges of all Sensors
long getrange(int sensornumber)
{
	long range = 0;
	long time = 0;
	
	unsigned long measuretime = micros();
	digitalWrite(sensor[sensornumber][0], LOW); //trigger -> LOW
	delayMicroseconds(3);
	noInterrupts();
	digitalWrite(sensor[sensornumber][0], HIGH); //Trigger Impuls 10 us
	delayMicroseconds(10);
	digitalWrite(sensor[sensornumber][0], LOW);
	time = pulseIn(sensor[sensornumber][1], HIGH,measuretimeout); // Echo-Zeit messen
	interrupts();
	time = (time / 2); // Zeit halbieren
	range = time / conversionfactor; // Zeit in Zentimeter umrechnen
	if (micros() > measuretime)		//Check if micros overflowed
	{
		while (micros() <= measuretime + measuredelay)
		{
		}
	}
	return(range);
}

//get all ranges
void getallranges()
{
	for (int n = 0; n <= 9; n++)
	{
		ranges[n] = getrange(n);
	}
}

//Send Solution vector to Node
void sendsolution()
{
#ifdef serialDebug
	Serial.println("Solution:");
#endif // serialDebug
	for (int i = 0; i <= 9; i++)
	{
		Serial.write(lowByte(ranges[i]));
		Serial.write(highByte(ranges[i]));
	}

#ifdef serialDebug
	Serial.println("");
	Serial.println("ASCII:");
	for (int i = 0; i <= 9; i++)
	{
		Serial.print(ranges[i]);
		Serial.print(" ; ");
	}
	Serial.println("");
#endif // serialDebug
}

//-------------------Loop--------------------

void loop() 
{

#ifdef serialDebug
	Serial.println("waiting...");
#endif // serialDebug

#ifndef serialDebug_continuous
	while (!Serial.available()) {} //waits for a Message from Node
	mram = Serial.read(); //reads Message from Node
#endif //serialDebug_continuous

#ifdef serialDebug
	Serial.println("message received");
#endif // serialDebug


	if (mram == param::call)//call message received
	{
#ifdef serialDebug
		Serial.println(String("..call message.."));
#endif // serialDebug
		getallranges();
	}
	//-----------------------------------------------------------------
	else if (mram == param::ask)//ask message recieved
	{
#ifdef serialDebug
		Serial.println(String("..ask message.."));
#endif // serialDebug
		sendsolution();
	}
	//----------------------------------------------------------------
	else if (mram == param::ident_ask)//ident_ask message recieved
	{
#ifdef serialDebug
		Serial.println(String("Ident:"));
#endif // serialDebug

		Serial.write(param::ident);

#ifdef serialDebug
		Serial.println(String(" "));
#endif // serialDebug
	}
	//----------------------------------------------------------------
	else//anny other message -> do nothing
	{
#ifdef serialDebug
		Serial.println(String("!!unidentified message!!"));
#endif // serialDebug
	}



	#ifdef serialDebug_continuous
	measurecompletetime = micros();
	for (int n = 0; n <= 9; n++)
	{
		ranges[n] = getrange(n);
	}
	measurecompletetime = micros() - measurecompletetime;

	for (int i = 0; i <= 9; i++)
	{
		Serial.println(ranges[i]);
	}

	Serial.println(String(measurecount) + ("|m-time:") + (measurecompletetime)+("|s-time:"));
	#endif // serialDebug_continuous
	measurecount++;
}
