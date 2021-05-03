/*
  RhinoServo.h - Library to deal with Rhino DC Servo RMCS220x
  Copyright (c) 2018 RoboManipal. All right reserved
  File created by : Shashank Goyal
*/

// Guard code to prevent multiple imports
#ifndef RHINO_SERVO__H
#define  RHINO_SERVO__H

// Main arduino code
#include "Arduino.h"
// Versatile for serial
#include "Stream.h"
// For I2C Communication
#include "Wire.h"
// Debugger
#include "DebuggerSerial/src/DebuggerSerial.h"

// Operation Mode
#define MODE_SERIAL 0
#define MODE_I2C 1

// Macro to Convert Degrees to Encoder Count.
#define DEGtoCOUNT(x) 5*x 

class RhinoServo
{
	// ------------------------- Private Data Members ---------------------
	// Define Connection as I2C or Serial Connection.
	int connection_mode;
	// Serial line to which the Rhino is attached.
	Stream *Rhino_Serial;
	// I2C Address to which the Rhino is attached.
	uint8_t Rhino_Address;

	// ------------------------- Private Member Functions -----------------
	// Read Function to recieve values from the Rhino.
	int read();
	// Converts integer Values to corresponding Byte Format.
	void toBytes(int val,int arr[]);

	// ------------------ Public Members and Functions --------------------
	public:

	// ######################### Debugger Object ##########################	
	DebuggerSerial debugger;

	// ######################### Constructors #############################
	// Empty Constructor
	RhinoServo();
	// Define Connection Mode
	RhinoServo(int connection_mode);
	// Attach Serial Line.
	RhinoServo(Stream *AttachedSerial);
	// Attach I2C Address.
	RhinoServo(uint8_t Rhino_Address);

	// ######################## Setter Functions ##########################
	// Attach Serial Line.
	void AttachRhino_Serial(Stream *AttachedSerial);
	// Attach I2C Address.
	void AttachRhino_Address(uint8_t Rhino_Address);
	// Set Direction and Speed.
	void Set_Dir_Speed(int val);
	// Set Address (usable in Serial Mode Only).
	void SetI2C_Address(int val);
	// Set Encoder Value to a Specific/Zero Value.
	void Reset_Reference(int val=0);
	// Set Proportional Gain for PID Correction.
	void Set_P_Gain(int val);
	// Set Integral Gain for PID Correction.
	void Set_I_Gain(int val);
	// Reset Rhino to All Default Settings.
	void Reset();

	// ###################### Actuator Functions ##########################
	// Move to Encoder Position wrt Encoder Zero Position.
	void Moveto(int val);
	// Move to Position Relative to Current Position.
	void MovetoRel(int val);

	// ##################### Getter Functions ############################# 
	// Get Position.
	int GetCurrent_Position();
	// Display Address of the Rhino (usable in Serial Mode Only).
	int GetI2C_Address();
	// Get Proportional Gain for PID Correction.
	int Get_P_Gain();
	// Get Integral Gain for PID Correction.
	int Get_I_Gain();
};

// End of Guard Code
#endif