/*
 Name:		arduino-ultra.ino
 Created:	06.02.2017 14:40:44
 Author:	gutek
*/


//----------------------Defines----------------------------
//#define serialDebug //Debug output over Serial connection to PC
//#define serialDebug_continuous //reads the 16 Sensors continuous


//----------------------Params-----------------------------

namespace param
{
	int8_t ident = 85;		//Identifier: 85 "U"
	int8_t ident_ask = 105; // define ident_ask Message: 105 "i"
	int8_t call = 99;		//define call Message: 99 "c"
	int8_t ask = 97;			// define ask Message: 97 "a"
}

long conversionfactor = 29.1;		//conversioncactor microseconds -> cm
unsigned long measuredelay = 19500; //standard time for one measurement
long measuretimeout = 19000;			//actual measurement timeout
int8_t mram = 0;						//Message storage


//----------------------Pinset-----------------------------

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


//----------------------Init-------------------------------

// Array sensor [Sensornummer][Trigger,Echo]
int sensor[10][2] =				
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

int16_t ranges[10] = { 0 };	//Rangevector ; Line = Sensornumber

int measurecount = 0;		//stores the number of measures done
long measuretime = 0;		//Storage variable for Time measurement


//----------------------Setup------------------------------

void setup() 
{
	//starting serial connection to Host
	Serial.begin(115200);
	
	while (!Serial) 
	{
		; // wait for serial port to connect. Needed for native USB port only
	}

#ifdef serialDebug //sends debug message
	Serial.println("Serial Connection established");
	Serial.println(String("identifier : ") + (param::ident));
	Serial.println(String("call Message : ") + (param::call));
	Serial.println(String("ask Message : ") + (param::ask));
#endif // serialDebug


	//Pinmode Sensors
	for (int n = 0; n <= 9; n++)
	{
		pinMode(sensor[n][0], OUTPUT);		//Sensor n Trigger -> Output
		pinMode(sensor[n][1], INPUT_PULLUP);//Sensor n Echo    -> Output

#ifdef serialDebug
		Serial.println(String("Sensor ") + (i)+("-> OUTPUT"));
#endif // serialDebug

	}
}

//----------------------Functions--------------------------


//get Ranges of a Sensor
int16_t getrange(int sensornumber)
{
	long range = 0;	//return value
	long time = 0;	//time storage
	
	unsigned long measuretime = micros(); //set beginning of measure

	digitalWrite(sensor[sensornumber][0], LOW); //trigger -> LOW
	delayMicroseconds(3);
	//noInterrupts();//disable all interrupts
	digitalWrite(sensor[sensornumber][0], HIGH); //Trigger Impuls 10 us
	delayMicroseconds(10);
	digitalWrite(sensor[sensornumber][0], LOW);
	time = pulseIn(sensor[sensornumber][1], HIGH,measuretimeout); //measure echo duration
	//interrupts();
	time = (time / 2.0);					//travel time is half the measured
	range = time / conversionfactor;		//convert travel tim in cm
	if (micros() > measuretime)			//Check if micros overflowed
	{
		while (micros() <= measuretime + measuredelay)//wait echo to fade away
		{
		}
	}
	return((int16_t)range); //return measured range
}

//get all ranges
void getallranges()
{
	for (int n = 0; n <= 9; n++)
	{
		ranges[n] = getrange(n);//measure sensor [n]
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
		Serial.write(lowByte(ranges[i]));	//Send low 8-Bit of 16-Bit
		Serial.write(highByte(ranges[i]));	//Send high 8-Bit of 16-Bit
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


//----------------------Loop-------------------------------

void loop() 
{

#ifdef serialDebug
	Serial.println("waiting...");
#endif // serialDebug

#ifndef serialDebug_continuous
	while (!Serial.available()) {}	//waits for a Message from Node
	mram = Serial.read();			//reads Message from Node
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
	//--------------------------------------------------------
	else if (mram == param::ask)//ask message recieved
	{
#ifdef serialDebug
		Serial.println(String("..ask message.."));
#endif // serialDebug
		sendsolution();
	}
	//--------------------------------------------------------
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
	//--------------------------------------------------------
	else//anny other message -> do nothing
	{
#ifdef serialDebug
		Serial.println(String("!!unidentified message!!"));
#endif // serialDebug
	}

	Serial.flush();


	#ifdef serialDebug_continuous
	measuretime = micros();
	for (int n = 0; n <= 9; n++)
	{
		ranges[n] = getrange(n);
	}
	measuretime = micros() - measuretime;

	for (int i = 0; i <= 9; i++)
	{
		Serial.println(ranges[i]);
	}

	Serial.println(String(measurecount) + ("|m-time:") + (measuretime)+("|s-time:"));
	#endif // serialDebug_continuous
	measurecount++;
}
