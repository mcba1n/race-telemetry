// ****************************************************************************
// **** Author: Manash Kumar Mandal (commented by Brendon McBain)
// **** Source: https://blog.manash.me/serial-communication-with-an-arduino-using-c-on-windows-d08710186498
// ****************************************************************************

#ifndef SERIALPORT_H		// Header guard
#define SERIALPORT_H

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 1

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class SerialPort {
	private:
		HANDLE handler;					// Serial comm handler
		bool connected;					// Connection status
		COMSTAT status;					// Get various information about the connection
		DWORD errors;					// Keep track of last error
	public:
		SerialPort(char *portName);		// Initialise Serial communication with the given COM port
		~SerialPort();					// Close the connection

		int readSerialPort(char *buffer, unsigned int buf_size);	// Read data in a buffer
		bool writeSerialPort(char *buffer, unsigned int buf_size);  // Writes data from a buffer through the Serial connection
		bool isConnected();											// Check if we are actually connected
};

#endif // SERIALPORT_H