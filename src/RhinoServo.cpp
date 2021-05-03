/*
  RhinoServo.cpp - Library to deal with Rhino DC Servo RMCS220x
  Copyright (c) 2018 RoboManipal. All right reserved
  File created by : Shashank Goyal
*/

// Include library
#include"RhinoServo.h"

// ######################### Private Members ##########################

// Used to Read Values from Rhino.
int RhinoServo :: read()
{
	// Variable for reading data.
	int position = 0;
	bool neg = false;
	String inString = "";
	
	// For Serial Mode Connection.
	if(connection_mode == MODE_SERIAL)
	{
		while (Rhino_Serial->available())
		{
			Rhino_Serial->flush();
			// Read incoming Characters
			int inChar = Rhino_Serial->read();
			// If Number then append digits to the string.
			if (isDigit(inChar))
				inString += (char)inChar;
			// If negative sign.
			if (inChar == '-')
				neg = true;
			// End of Communication marked by Carriage Return.
			if (inChar == '\r')
			{
				// Convert the String to Int.
				position = inString.toInt();
				if (neg)
					position *= -1;
			}
		}
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Wait for data to be available.
		while (Wire.available())
		{
			int inChar[4];
			// Recieve the data in the form of a Array. 
			for(int i=0;i<4;i++)
				inChar[i] = Wire.read();

			// Check MSB for Sign Bit of the Data.
			if(inChar[3] == 255)
			{
				// If True, then negate all the other Bytes to get the Magnitude.
				for(int i=0;i<4;i++)
					inChar[i] = 255 - inChar[i];
				// Set Negative state as True.
				neg = true;
			}

			// Convert Hexadecimal to Decimal value.
			position = inChar[0];
			for(int i=1;i<4;i++)
			{
				for(int j=0;j<i;j++)
					inChar[i]*=256;
				position += inChar[i];
			}

			if(neg)
				position*=-1;
		}
	}

	else
	{
		// Debugger message (level: ERROR)
		// Connection Mode not defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return -1; 
	}

	// Return Position Value.
	return position;	
}

// Converts Data into Bytes for I2C Communication.
void RhinoServo :: toBytes(int val,int arr[])
{
	// Set Sign Flag. 
	bool flag=val>=0?0:1;
	// Consider only Magnitude.
	if(val<0)
		val*=-1;

	// Convert Decimal to Hexadecimal Value.
	for(int i=0;i<4;i++)
	{
		arr[i]=val%256;
		val/=256;
		// if Negative Value, Negate all the Bytes.
		if(flag == true)
			arr[i]=255-arr[i];
	}
}


// ####################### Public Member Functions ###################
// ========================= Constructors =============================

RhinoServo :: RhinoServo()
{
	// Empty Constructor.
}

RhinoServo :: RhinoServo(int connection_mode)
{
	// Define Connection Mode.
	this->connection_mode = connection_mode;
}

RhinoServo :: RhinoServo(Stream *AttachedSerial)
{
	// Connect the Serial Line.
	this->AttachRhino_Serial(AttachedSerial);
}

RhinoServo :: RhinoServo(uint8_t Rhino_Address)
{
	// Define I2C Address
	this->AttachRhino_Address(Rhino_Address);
}

// ########################## Member Functions ########################

void RhinoServo :: AttachRhino_Serial(Stream *AttachedSerial)
{
	// Attach Serial Line to Rhino.
	this->Rhino_Serial = AttachedSerial;
	// Set Mode as Serial.
	this->connection_mode = MODE_SERIAL;
	// Debugger message (level: INFO)
    // Rhino Serial attached.
	this->debugger.print("Rhino Serial attached", INFO);
}

void RhinoServo :: AttachRhino_Address(uint8_t Rhino_Address)
{
	// Attach I2C Address Value to Rhino.
	this->Rhino_Address = Rhino_Address;
	// Set Mode as I2C.
	this->connection_mode = MODE_I2C;
	// Debugger message (level: INFO)
    // Rhino attached on I2C Address: <Address>
	String msg = "Rhino Attached on I2C Rhino Address: ";
	msg.concat(Rhino_Address);
	this->debugger.print(msg, INFO);
}

// Set Constant Rotation and its Speed.
void RhinoServo :: Set_Dir_Speed(int val)
{
	// Speed Constraints.
	if(val>255 || val<-255)
	{
		// Debugger message (level: ERROR)
		// Error: Invalid Speed
		this->debugger.print("Invalid Speed", ERROR);
		return;
	}

	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		// Command Variable to Set Speed for Constant Rotation.
		String cmd = "S";
		// Write Speed Value.
		cmd.concat(val);
		// Marks end of Command.
		cmd.concat("\n\r");
		// Write Command to the Serial Line.
		Rhino_Serial->print(cmd);
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Begin Transmission to the Relevant Rhino.
		Wire.beginTransmission(Rhino_Address);
		// Write the 'Set Speed' Command Value: '1'.
		Wire.write(1);
		// For Positive Values.
		if(val>=0)
		{
			// Write LSB.
			Wire.write(val%256); 
			// Write MSB.
			Wire.write(val/256);
		}
		// For Negative Values.
		else
		{
			// Write LSB.
			Wire.write(255-val%256); 
			// Write MSB.
			Wire.write(255-val/256);	
		}
		// End Transmission.
		Wire.endTransmission();
		delay(500);
	}

	else
	{
		// Debugger message: (level: ERROR)
		// Connection Mode not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "Motor Direction and Speed changed to: ";
	msg.concat(val);
	// Debugger message: (level: DEBUG)
	// Motor Direction and Speed Changed to: <Speed Value>
	this->debugger.print(msg,DEBUG);
}

// Assign an I2C Address via Serial Mode Only.
void RhinoServo :: SetI2C_Address(int val)
{
	if(connection_mode != MODE_SERIAL)
	{
		// Debugger message: (level:ERROR)
		// Invalid Connection Mode. I2C Address can only be set in Serial Mode.
		this->debugger.print("Invalid Connection Mode. I2C Address can only be set in Serial Mode.",ERROR);
		return ;
	}

	// To Incorporate the Right Shift of Data which sets the Address Value by half.
	val*=2;
	// Command Variable to Set I2C Address.
	String cmd = "E";
	// Write Address value.
	cmd.concat(val);
	// Marks End of Command.
	cmd.concat("\n\r");
	// Write to the Serial Line.
	Rhino_Serial->print(cmd);	

	// Debugger message: (level: INFO)
	// Rhino I2C Address changed to: <Address Value>
	String msg = "Rhino I2C Address changed to: ";
	msg.concat(val/2);
	this->debugger.print(msg,INFO);
}

// Set an Encoder Reading Value to the current Position of the Rhino.
void RhinoServo :: Reset_Reference(int val=0)
{
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		// Command Variable to Set Encoder Value.
		String cmd = "P";
		// Write Address value.
		cmd.concat(val);
		// Marks End of Command.
		cmd.concat("\n\r");
		// Write to the Serial Line.
		Rhino_Serial->print(cmd);
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Begin Transmission.
		Wire.beginTransmission(Rhino_Address);
		// Write the 'Reset Encoder Value' Command Value: '3'.
		Wire.write(3);
		// "Byte" Array for the Data to be Transmitted.
		int arr[4];
		// Converts the Input Value into a "Byte" Array.
		this->toBytes(val,arr);
		// Write the Data in the Little - Endian Format.
		for(int i=0;i<4;i++)
			Wire.write((uint8_t)arr[i]); 
		// End Transmission.
		Wire.endTransmission();
		delay(500);
	}

	else
	{
		// Debugger message: (level: ERROR)
		// Connection Mode not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	if(val == 0)
	{
		// Debugger message: (level: DEBUG)
		// Encoder Value Reset.
		this->debugger.print("Encoder value Reset.",DEBUG);
		return;
	}

	String msg = "Encoder value Adjusted to: ";
	msg.concat(val);
	// Debugger message: (level: Debugger)
	// Encoder value Adjusted to: <New Value>
	this->debugger.print(msg,DEBUG);
}

// Set Proportional Gain for PID Correction.
void RhinoServo :: Set_P_Gain(int val)
{
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		// Command Variable to Change P Gain.
		String cmd = "B";
		// Write Address value.
		cmd.concat(val);
		// Marks End of Command.
		cmd.concat("\n\r");
		// Write to the Serial Line.
		Rhino_Serial->print(cmd);
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Begin Transmission.
		Wire.beginTransmission(Rhino_Address);
		// Write the 'Write P-Gain term' Command Value: '6'.
		Wire.write(6);
		// "Byte" Array for the Data to be Transmitted.
		int arr[4];
		// Converts the Input Value into a "Byte" Array.
		this->toBytes(val,arr);
		// Write the Data in the Little - Endian Format.
		for(int i=0;i<4;i++)
			Wire.write((uint8_t)arr[i]); 
		// End Transmission.
		Wire.endTransmission();
		delay(500);
	}

	else
	{
		// Debugger message: (level: ERROR)
		// Connection Mode Not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "P-Gain Changed to: ";
	msg.concat(val);
	// Debugger message: (level: DEBUG)
	// P-Gain Changed to: <New P-Gain Value>
	this->debugger.print(msg,DEBUG);	
}

// Set Integral Gain for PID Correction.
void RhinoServo :: Set_I_Gain(int val)
{
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		// Command Variable to Change I Gain.
		String cmd = "C";
		// Write Address value.
		cmd.concat(val);
		// Marks End of Command.
		cmd.concat("\n\r");
		// Write to the Serial Line.
		Rhino_Serial->print(cmd);
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Begin Transmission.
		Wire.beginTransmission(Rhino_Address);
		// Write the 'Write I-Gain term' Command Value: '7'.
		Wire.write(7);
		// "Byte" Array for the Data to be Transmitted.
		int arr[4];
		// Converts the Input Value into a "Byte" Array.
		this->toBytes(val,arr);
		// Write the Data in the Little - Endian Format.
		for(int i=0;i<4;i++)
			Wire.write((uint8_t)arr[i]); 
		// End Transmission.
		Wire.endTransmission();
		delay(500);
	}

	else
	{
		// Debugger message: (level: ERROR)
		// Connection Mode Not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "I-Gain Changed to: ";
	msg.concat(val);
	// Debugger message: (level: DEBUG)
	// I-Gain changed to: <New I-Gain Value>
	this->debugger.print(msg,DEBUG);	
}

// Goto a specific Position. 
void RhinoServo :: Moveto(int val)
{
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		// Command Variable to Move Motor to Position.
		String cmd = "G";
		// Write Address value.
		cmd.concat(val);
		// Marks End of Command.
		cmd.concat("\n\r");
		// Write to the Serial Line.
		Rhino_Serial->print(cmd);
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Begin Transmission.
		Wire.beginTransmission(Rhino_Address);
		// Write the 'Write Position/Encoder' Command Value: '4'.
		Wire.write(4);
		// "Byte" Array for the Data to be Transmitted.
		int arr[4];
		// Converts the Input Value into a "Byte" Array.
		this->toBytes(val,arr);
		// Write the Data in the Little - Endian Format.
		for(int i=0;i<4;i++)
			Wire.write((uint8_t)arr[i]); 
		// End Transmission.
		Wire.endTransmission();
		delay(500);
	}

	else
	{
		// Debugger message: (level: ERROR)
		// Connection Mode Not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "Motor Moved to Position: ";
	msg.concat(val);
	// Debugger message: (level: DEBUG)
	// Motor Moved to Position: <Position Value>
	this->debugger.print(msg,DEBUG);	
}

// Move Relative to the Current Position.
void RhinoServo :: MovetoRel(int val)
{
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		// Command Variable to Move Motor to Relative Position.
		String cmd = "R";
		// Write Address value.
		cmd.concat(val);
		// Marks End of Command.
		cmd.concat("\n\r");
		// Write to the Serial Line.
		Rhino_Serial->print(cmd);
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Begin Transmission.
		Wire.beginTransmission(Rhino_Address);
		// Write the 'Write Relative Position/Encoder' Command Value: '8'.
		Wire.write(8);
		// "Byte" Array for the Data to be Transmitted.
		int arr[4];
		// Converts the Input Value into a "Byte" Array.
		this->toBytes(val,arr);
		// Write the Data in the Little - Endian Format.
		for(int i=0;i<4;i++)
			Wire.write((uint8_t)arr[i]);
		// End Transmission.
		Wire.endTransmission();
		delay(500);
	}

	else
	{
		// Debugger message (level: ERROR)
		// Connection Mode Not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "Motor Moved Relative to the Old Position by: ";
	msg.concat((val%1800)/5);
	msg.concat(" Degrees.");
	// Debugger message (level: DEBUG)
	// Motor Moved Relative  to the Old Position by: <Angle in Degrees> Degrees.
	this->debugger.print(msg,DEBUG);	
}

// Reset the Rhino to all Defaults.
void RhinoServo :: Reset()
{
	// For Serial mode Only.
	if(connection_mode == MODE_SERIAL)
	{
		// Command to Reset the Motor.
		Rhino_Serial->print("Y\n\r");
	}
	
	else
	{
		// Debugger message (level: ERROR)
		// Serial Support Only. Connect via Serial.
		this->debugger.print("Serial Support Only. Connect via Serial.",ERROR);
		return; 
	}
	String msg = "Rhino Reset to Defaults.";
	// Debugger message: (level: DEBUG)
	this->debugger.print(msg,DEBUG);	
}

// Get Current Position.
int RhinoServo :: GetCurrent_Position()
{
	int position=0;
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		Rhino_Serial->print("G\n\r");
		while(Rhino_Serial->available()<=2);
		position = this->read();	
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Request 4 Bytes from the Rhino at the given I2C Address.
		Wire.requestFrom((uint8_t)Rhino_Address,(uint8_t)4);
		// Write the 'Read Position/Encoder' Command Value: '4'.
		Wire.write(4);
		// Read Value into the Variable.
		position = this->read();
	}

	else
	{
		// Debugger message (level: ERROR)
		// Connection Mode Not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "Position: ";
	msg.concat(position);
	// Debugger message (level: DEBUG)
	// Position: <Encoder Value>.
	this->debugger.print(msg,DEBUG);
	return position;
}

// Get Current value set for the P-Gain.
int RhinoServo :: Get_P_Gain()
{
	int value=0;
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		Rhino_Serial->print("B\n\r");
		while(Rhino_Serial->available()<=2);
		value = this->read();	
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Request 4 Bytes from the Rhino at the given I2C Address.
		Wire.requestFrom((uint8_t)Rhino_Address,(uint8_t)4);
		// Write the 'Read P-Gain' Command Value: '6'.
		Wire.write(6);
		// Read Value into the Variable.
		value = this->read();
	}

	else
	{
		// Debugger message (level: ERROR)
		// Connection Mode Not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "P-Gain Value: ";
	msg.concat(value);
	// Debugger message (level: DEBUG)
	// Position: <Encoder Value>.
	this->debugger.print(msg,DEBUG);
	return value;
}

// Get Current value set for the I-Gain.
int RhinoServo :: Get_I_Gain()
{
	int value=0;
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		Rhino_Serial->print("C\n\r");
		while(Rhino_Serial->available()<=2);
		value = this->read();	
	}

	// For I2C Mode of Connection.
	else if(connection_mode == MODE_I2C)
	{
		// Request 4 Bytes from the Rhino at the given I2C Address.
		Wire.requestFrom((uint8_t)Rhino_Address,(uint8_t)4);
		// Write the 'Read I-Gain' Command Value: '7'.
		Wire.write(7);
		// Read Value into the Variable.
		value = this->read();
	}

	else
	{
		// Debugger message (level: ERROR)
		// Connection Mode Not Defined.
		this->debugger.print("Connection Mode Not Defined.",ERROR);
		return; 
	}

	String msg = "I-Gain Value: ";
	msg.concat(value);
	// Debugger message (level: DEBUG)
	// Position: <Encoder Value>.
	this->debugger.print(msg,DEBUG);
	return value;
}

// Get the Current Set I2C Address.
int RhinoServo :: GetI2C_Address()
{
	int Address;
	// For Serial mode.
	if(connection_mode == MODE_SERIAL)
	{
		// Command to Recive the I2C Address of the Rhino.
		Rhino_Serial->print("E\n\r");
		// Reads the Value from the Serial.
		Address = this->read();
		// Address Value Written is Divided
		// to Incorporate the Right Shift while Setting Address.
		Address/=2;

		String msg = "Address Found at: ";
		msg.concat(Address);
		// Debugger message (level: INFO)
		// Rhino Address Found at: <Address>
		this->debugger.print(msg,INFO);
	}	
	else
	{
		// Debugger message (level: ERROR)
		// Serial Support Only. Connect via Serial.
		this->debugger.print("Serial Support Only. Connect via Serial.",ERROR);
		return; 
	}
}