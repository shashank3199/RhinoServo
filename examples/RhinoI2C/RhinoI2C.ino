/*
	RhinoI2C - Sample Code for I2C Communication for Rhino RMCS-220X 
	Copyright (c) 2018 RoboManipal. All right reserved
	File created by : Shashank Goyal
*/

/*****************************************************

1. Set Rhino I2C Addresses for each of the Rhinos.
2. Open Serial Monitor with Baud rate 9600.
3. Give Commands in the Following Format -

	<Command><motor_number>#<parameter_value>

Eg -	"M1#200" : Moves Motor 1 to Encoder Position 200.
		"m2#180" : Moves Motor 2 by 180 Deg Relative to Current Position.

*****************************************************/

#include <RhinoServo.h>

RhinoServo R1 = RhinoServo(MODE_I2C);
RhinoServo R2 = RhinoServo(MODE_I2C);
int num;

void setup()
{
	// Begin I2C Communication.
	Wire.begin();
	// Begin Serial Communication at 9600 baudrate.
	Serial.begin(9600);
	// Enable Debugger for Rhino 1.
	R1.debugger.Initialize("Rhino 1", &Serial, DEBUG);
	// Attach Rhino 1 at 0x01 Address.
	R1.AttachRhino_Address(0x01);
	// Enable Debugger for Rhino 2.
	R2.debugger.Initialize("Rhino 2", &Serial, DEBUG);
	// Attach Rhino 2 at 0x02 Address.
	R2.AttachRhino_Address(0x02);

	// Uncomment the following to Disable Debuggers -

	// R1.debugger.disableDebugger();
	// R2.debugger.disableDebugger();
}

void loop()
{
	while (Serial.available())
	{
		switch (Serial.read())
		{
			case 'M':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			// Read Position parameter.
			if(num==1)
				R1.Moveto(Serial.parseInt());
			else
				R2.Moveto(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////

			case 'm':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			// Read Relative Position.
			if(num==1)
				R1.MovetoRel(DEGtoCOUNT(Serial.parseInt()));
			else
				R2.MovetoRel(DEGtoCOUNT(Serial.parseInt()));
			break;

			///////////////////////////////////////////////////////////

			case 'R':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			// Read New Encoder Position.
			if(num==1)
				R1.Reset_Reference(Serial.parseInt());
			else
				R2.Reset_Reference(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////

			case 'r':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			if(num==1)
				R1.Reset_Reference();
			else
				R2.Reset_Reference();
			break;

			///////////////////////////////////////////////////////////

			case 'S':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			// Read Speed for Rotation.
			if(num==1)
				R1.Set_Dir_Speed(Serial.parseInt());
			else
				R2.Set_Dir_Speed(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////

			case 'P':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			if(num==1)
				R1.GetCurrent_Position();
			else
				R2.GetCurrent_Position();
			break;			

			///////////////////////////////////////////////////////////

			case 'K':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			// Read P - Gain for PID Correction. 
			if(num==1)
				R1.Set_P_Gain(Serial.parseInt());
			else
				R2.Set_P_Gain(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////

			case 'L':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			// Read I - Gain for PID Correction. 
			if(num==1)
				R1.Set_I_Gain(Serial.parseInt());
			else
				R2.Set_I_Gain(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////

			case 'D':
			// Read Motor Number.
			num = Serial.parseInt();
			// Remove the #.
			Serial.read();
			if(num==1)
				R1.Reset();
			else
				R2.Reset();
			break;			
		}
	}
}
