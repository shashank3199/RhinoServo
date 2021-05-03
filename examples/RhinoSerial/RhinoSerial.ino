/*
	RhinoSerial - Sample Code for Serial Communication for Rhino RMCS-220X 
	Copyright (c) 2018 RoboManipal. All right reserved
	File created by : Shashank Goyal
*/

#include <RhinoServo.h>

RhinoServo R = RhinoServo(MODE_SERIAL);

void setup()
{
	// Begin Serial Communication at 9600 baudrate.
	Serial.begin(9600);
	Serial3.begin(9600);
	// Attach Serial Line to the Rhino.
	R.AttachRhino_Serial(&Serial3);
	// Enable Debugger for Rhino 1.
	R.debugger.Initialize("Rhino", &Serial, DEBUG);

	// Uncomment the following to Disable Debugger -
	// R.debugger.disableDebugger();
}

void loop()
{
	while (Serial.available())
	{
		switch (Serial.read())
		{
			case 'M':	// Move to Position
			R.Moveto(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////

			case 'm':	// Move Relative to Current Position
			R.MovetoRel(DEGtoCOUNT(Serial.parseInt()));
			break;

			///////////////////////////////////////////////////////////

			case 'R':	// Reset Encoder Value
			R.Reset_Reference(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////

			case 'r':	// Reset Encoder Value to Zero
			R.Reset_Reference();
			break;

			///////////////////////////////////////////////////////////

			case 'P':	// Read Position
			Serial.println(R.GetCurrent_Position());
			break;

			///////////////////////////////////////////////////////////

			case 'A' : 	// Get I2C Address 
			R.GetI2C_Address();
			break;

			///////////////////////////////////////////////////////////

			case 'S':	// Set I2C Address
			R.SetI2C_Address(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////			

			case 'W': // Set Constant Rotation of the Motor 
			R.Set_Dir_Speed(Serial.parseInt());
			break;

			///////////////////////////////////////////////////////////			

			case 'K': // Set P-Gain for PID Correction
			R.Set_P_Gain(Serial.parseInt());
			R.Get_P_Gain();
			break;

			///////////////////////////////////////////////////////////			

			case 'L': // Set I-Gain for PID Correction. 
			R.Set_I_Gain(Serial.parseInt());
			R.Get_I_Gain();
			break;

			///////////////////////////////////////////////////////////			

			case 'D': // Reset Rhino to Defaults.
			R.Reset();
			break;
		}
	}
}
